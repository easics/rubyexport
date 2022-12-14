// This file is part of rubyexport.
//
// rubyexport is free software: you can redistribute it and/or modify it under
// the terms of the GNU General Public License as published by the Free Software
// Foundation, either version 3 of the License, or (at your option) any later
// version.
//
// rubyexport is distributed in the hope that it will be useful, but WITHOUT ANY
// WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
// PARTICULAR PURPOSE. See the GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License along with
// rubyexport. If not, see <https://www.gnu.org/licenses/>.


#ifdef SCRIPT_PYTHON
#include <Python.h>
#include <structmember.h>
#endif
#include "ScriptInterface.h"
#include "ReflectionRegistry.h"
#include "ScriptAccess.h"
#include "ScriptObject.h"
#include <cctype>
#include <cassert>
#include <sstream>
#include <cstring>
#include <unistd.h>
#include <iostream>
#include <stdexcept>
#include <sstream>
#ifdef SCRIPT_RUBY
#include "rb_protect_wrap.h"
#include "RubyException.h"
#include <ruby.h>
#include <ruby/version.h>
#if __has_include(<ruby/vm.h>)
#include <ruby/vm.h>
#endif
#endif
#ifdef SCRIPT_PYTHON
#include "PythonException.h"
#include <unordered_set>
#include <frameobject.h>
#include <set>
#endif

template<>
ScriptInterface * Singleton<ScriptInterface>::instance_ = nullptr;

std::string niceTypename(const std::string & typeidname);

namespace // anonymous
{
#ifdef SCRIPT_RUBY
  bool ruby_vm_exiting = false;
  void script_interface_ruby_vm_exiting()
    {
      ruby_vm_exiting = true;
    }
#endif
#ifdef SCRIPT_PYTHON
  PyMethodDef module_methods[] = {
      {nullptr}
  };
#if PY_MAJOR_VERSION == 3
  char const * scriptInterfaceModuleName = 0;
#endif
#endif

  std::unordered_map<std::string, std::string> typeEqualities_;
}

#ifdef SCRIPT_PYTHON
#if PY_MAJOR_VERSION == 3
PyObject * PyInit_ScriptInterface()
{
  static struct PyModuleDef moduledef = {
    PyModuleDef_HEAD_INIT,
    scriptInterfaceModuleName,          /* m_name */
    scriptInterfaceModuleName,          /* m_doc */
    -1,                  /* m_size */
    module_methods,      /* m_methods */
    NULL,                /* m_reload */
    NULL,                /* m_traverse */
    NULL,                /* m_clear */
    NULL,                /* m_free */
  };
  return ScriptInterface::instance().pymodule_ = PyModule_Create(&moduledef);
}
#endif
#endif

void ScriptInterface::init(const char * modulename)
{
#ifdef SCRIPT_RUBY
#if RUBY_API_VERSION_MAJOR == 3
  VALUE * debug_ptr = 0;
#elif RUBY_VERSION_MAJOR != 1 || RUBY_VERSION_MINOR != 8
  VALUE * debug_ptr = rb_ruby_debug_ptr();
#else
  VALUE * debug_ptr = 0;
#endif
  if ((long)debug_ptr < 4096)
    // This means rb_current_vm is probably still 0 -> not yet initialized
    {
      // Ruby interpreter embedded in C++ program
      ruby_init();
      ruby_init_loadpath();
      ruby_script("plugin");
      handleRUBYOPT();
#if RUBY_VERSION_MAJOR != 1 || RUBY_VERSION_MINOR != 8
      rb_require("enc/encdb");
      rb_require("enc/trans/transdb");
#endif
    }
  else
    {
#if RUBY_VERSION_MAJOR != 1 || RUBY_VERSION_MINOR != 8
      // C++ program is a Ruby module
      // -> Ruby kernel is already initialized, just check version
      if (ruby_api_version[0] != RUBY_API_VERSION_MAJOR ||
          ruby_api_version[1] != RUBY_API_VERSION_MINOR)
        {
          std::cerr << "ruby plugin was compiled with ruby " <<
            RUBY_API_VERSION_MAJOR << "." << RUBY_API_VERSION_MINOR << "\n";
          std::cerr << "It is executed with ruby " <<
            ruby_api_version[0] << "." << ruby_api_version[1] << "\n";
          exit(1);
        }
#endif
    }
#endif
#ifdef SCRIPT_PYTHON
#if PY_MAJOR_VERSION == 3
  scriptInterfaceModuleName = modulename;
  PyImport_AppendInittab(modulename, PyInit_ScriptInterface);
#endif

  Py_Initialize();
#endif
  Reflection::Registry::instance().init();
#ifdef SCRIPT_RUBY
  rbmodule_ = rb_define_module(modulename);
  rbObjectHash_ = rb_hash_new();
  rb_gc_register_address(&rbObjectHash_);
  ScriptCppArrayBase::init();
  using RubyCallback = VALUE(*)(...);
  rb_define_global_function("script_interface_ruby_vm_exiting",
                            (RubyCallback)&script_interface_ruby_vm_exiting, 0);
  runRubyString("at_exit { script_interface_ruby_vm_exiting }");
#endif
#ifdef SCRIPT_PYTHON
#if PY_MAJOR_VERSION == 2
  pymodule_ = Py_InitModule(modulename, module_methods);
#endif
#if PY_MAJOR_VERSION == 3
  PyImport_ImportModule(modulename);
#endif
#endif
  makeClasses();
}

#ifdef SCRIPT_RUBY
namespace // anonymous
{

void proc_options(int argc __attribute__((unused)), char * argv[])
{
  switch (argv[1][1])
    {
    case 'I':
      ruby_incpush(argv[1]+2);
      break;
    case 'd':
      ruby_debug = Qtrue;
      ruby_verbose = Qtrue;
      break;
    case 'v':
      ruby_show_version();
      ruby_verbose = Qtrue;
      break;
    case 'w':
      ruby_verbose = Qtrue;
      break;
    case 'W':
        {
          int v = 2; /* -W as -W2 */
          if (argv[1][2])
            {
              v = argv[1][2] - '0';
              if (v < 0 || v > 2)
                v = 1;
            }
          switch (v)
            {
            case 0:
              ruby_verbose = Qnil;
              break;
            case 1:
              ruby_verbose = Qfalse;
              break;
            default:
              ruby_verbose = Qtrue;
              break;
            }
        }
    case 'r':
      // Don't know how to load libraries
      break;
    }
}

char * moreswitches(char *s)
{
  int argc; char *argv[3];
  char *p = s;

  argc = 2; argv[0] = argv[2] = 0;
  while (*s && !isspace(*s))
    s++;
  argv[1] = (char*)malloc(s-p+2);
  argv[1][0] = '-';
  strncpy(argv[1]+1, p, s-p);
  argv[1][s-p+1] = '\0';
  proc_options(argc, argv);
  //free(argv[1]);
  while (*s && isspace(*s))
    s++;
  return s;
}

}

void ScriptInterface::handleRUBYOPT()
{
  char * rubyopt = getenv("RUBYOPT");
  if (!rubyopt)
    return;

  while (isspace(*rubyopt)) rubyopt++;
    while (rubyopt && *rubyopt) {
      if (*rubyopt == '-') {
        rubyopt++;
        if (ISSPACE(*rubyopt)) {
          do {rubyopt++;} while (ISSPACE(*rubyopt));
          continue;
        }
      }
      if (!*rubyopt) break;
      if (!strchr("IdvwWr", *rubyopt))
        throw std::runtime_error(std::string("Illegal switch in RUBYOPT : -") +
                                 *rubyopt);
      rubyopt = moreswitches(rubyopt);
    }
}

void ScriptInterface::registerRubyObject(VALUE object)
{
  // Ruby objects can be put multiple times in the hash
  // e.g. 2 exported classes A and B, B is instantiated in A twice
  // class A { B * b_0; B * b_1; };
  // Then in Ruby
  // boo = B.new
  // aaa = A.new
  // aaa.b_0 = boo
  // aaa.b_1 = boo
  // Then the deletion of aaa will trigger the destructor of A which will call
  // boo->deleteFromC() twice
  // => Use reference counting in the hash
  //    each hash item is a pair for the Ruby VALUE and a counter
  VALUE existing = rb_hash_aref(rbObjectHash_, LONG2FIX(object));
  if (existing == Qnil)
    {
      VALUE pair = rb_ary_new();
      rb_ary_push(pair, object);
      rb_ary_push(pair, INT2FIX(1));
      rb_hash_aset(rbObjectHash_, LONG2FIX(object), pair);
    }
  else
    {
      int count = FIX2INT(rb_ary_entry(existing, 1));
      rb_ary_store(existing, 1, INT2FIX(count + 1));
      rb_hash_aset(rbObjectHash_, LONG2FIX(object), existing);
    }
}

void ScriptInterface::unregisterRubyObject(VALUE object)
{
  if (ruby_vm_exiting)
    return;
  VALUE existing = rb_hash_aref(rbObjectHash_, LONG2FIX(object));
  if (existing == Qnil)
    std::cerr << "key missing\n";
  else
    {
      int count = FIX2INT(rb_ary_entry(existing, 1));
      if (count == 1)
        rb_hash_delete(rbObjectHash_, LONG2FIX(object));
      else
        {
          rb_ary_store(existing, 1, INT2FIX(count - 1));
          rb_hash_aset(rbObjectHash_, LONG2FIX(object), existing);
        }
    }
}

void ScriptInterface::runRubyScript(const std::string & filename)
{
  int state = 0;
  rb_load_protect_wrap(rb_str_new2(filename.c_str()), 0, &state);
  RubyException::checkRubyException(state);
}

void ScriptInterface::runRubyString(const std::string & code)
{
  int state = 0;
  rb_eval_string_protect_wrap(code.c_str(), &state);
  RubyException::checkRubyException(state);
}

void ScriptInterface::addRubyScriptPath(const std::string & path)
{
  ruby_incpush(path.c_str());
}
#endif

#ifdef SCRIPT_PYTHON
ReflectionHandle ScriptInterface::runPythonScript(const std::string & filename)
{
  ReflectionHandle result;
  try
    {
      if (!(result.pythonHandle = PyImport_ImportModule(filename.c_str())))
        {
          PythonException::checkPythonException();
        }
      return result;
    }
  catch (std::exception & e)
    {
      std::ostringstream message;
      message << e.what() << "\n";
      if (PyErr_Occurred())
        {
      PyThreadState * tstate = PyThreadState_GET();
      if (tstate != 0 && tstate->frame != 0)
        {
          PyFrameObject * frame = tstate->frame;
          message << "Python stack trace:\n";
          while (frame != 0)
            {
              int line = frame->f_lineno;
#if PY_MAJOR_VERSION == 2              
              const char *filename = PyString_AsString(frame->f_code->co_filename);
#endif
#if PY_MAJOR_VERSION == 3
              const char *filename = PyUnicode_AsUTF8(frame->f_code->co_filename);
#endif
#if PY_MAJOR_VERSION == 2              
              const char *funcname = PyString_AsString(frame->f_code->co_name);
#endif
#if PY_MAJOR_VERSION == 3
              const char *funcname = PyUnicode_AsUTF8(frame->f_code->co_name);
#endif
              message << "  " << filename << "(" << line << "): " << funcname <<
                "\n";
              frame = frame->f_back;
            }
        }
        }
      throw std::runtime_error(message.str());
    }
}

void ScriptInterface::runPythonString(const std::string & code)
{
  if (PyRun_SimpleString(code.c_str()) < 0)
    throw std::runtime_error("Python error");
}

void ScriptInterface::addPythonScriptPath(const std::string & path)
{
  PyObject * sys_path;
  PyObject * pypath;

  sys_path = PySys_GetObject((char*)"path");
  if (sys_path == 0)
    throw std::runtime_error("sys.path not found ?");
#if PY_MAJOR_VERSION == 2  
  pypath = PyString_FromString(path.c_str());
#endif
#if PY_MAJOR_VERSION == 3
  pypath = PyUnicode_FromString(path.c_str());
#endif
  if (pypath == 0)
    throw std::runtime_error("Can't create string");
  if (PyList_Append(sys_path, pypath) < 0)
    PythonException::checkPythonException();
}

void ScriptInterface::callPython(ReflectionHandle pythonModule,
                                 const std::string & functionName) const
{
  PyObject * result = callPythonPrivate(pythonModule.pythonHandle,
                                        functionName);
  Py_XDECREF(result);
}

PyObject * ScriptInterface::callPythonPrivate(PyObject * pythonModule,
                                              const std::string & functionName,
                                              PyObject * argument1,
                                              PyObject * argument2,
                                              PyObject * argument3,
                                              PyObject * argument4,
                                              PyObject * argument5,
                                              PyObject * argument6) const
{
  if (!pythonModule) // user forgot to check result of runPythonScript ?
    throw std::runtime_error("pythonModule = 0");

  PyObject * func = PyObject_GetAttrString(pythonModule,
                                           functionName.c_str());
  if (func && PyCallable_Check(func))
    {
      PyObject * result;
      result = PyObject_CallFunctionObjArgs(func, argument1, argument2,
                                            argument3, argument4,
                                            argument5, argument6, 0);
      Py_DECREF(func);
      PythonException::checkPythonException();
      return result;
    }
  else
    {
      Py_XDECREF(func);
      PythonException::checkPythonException();
      return nullptr;
    }
}

#endif

void ScriptInterface::addTypeEquality(const std::string & scriptType,
                                      const std::string & cppType)
{
  typeEqualities_[scriptType] = cppType;
}

/////////////////////////////////////////////
// ScriptInterface implementation for Ruby //
/////////////////////////////////////////////
// Class types get an instance variable @c++class which is a pointer to the
// Reflection::ClassBase in C++
//
// Variables in the C-Ruby API have a generic pointer that can be used.
// Here that pointer is used to store a pointer to a RubyPythonReference
// It can be read with Data_Get_Struct and set with DATA_PTR(self) =
//
class ScriptInterface::Anonymous
{
public:
#ifdef SCRIPT_RUBY
// Ruby 'new'
static VALUE RubyClassBaseAlloc(VALUE self);
// Call a global function
static VALUE RubyCallGlobalFunction(int argc, VALUE * argv,
                                    VALUE rubyObjectClass);
#endif
#ifdef SCRIPT_PYTHON
static PyObject * PythonGlobalFunction(PyObject * functionName,
                                       PyObject * args);
#endif
};

namespace // anonymous
{
// Print the typename as written in C++, based on typeid(X).name()
void printNiceTypename(std::ostream & str, const std::string & typeidname);
// Print human readable signature
void printNiceSignature(std::ostream & str,
                        const std::vector<std::string> & signature);

// Make a nice error message when a function is not found
std::string signatureMismatch(const std::vector<std::string> & scriptSig,
                              const std::string & prefix,
                              const std::string & functionKind,
                              Reflection::ClassBase * klass,
                              const std::string & methodName);
#ifdef SCRIPT_RUBY
// Ruby 'free'
void RubyClassBaseFree(long * reference);
// Ruby 'garbage collect mark'
void RubyClassBaseMark(long * reference);
// Ruby constructor
VALUE RubyInitialize(int argc, VALUE * argv, VALUE self);
// Read an attribute
VALUE RubyGetAttr(VALUE self);
// Write an attribute
VALUE RubySetAttr(VALUE self, VALUE value);
// Call a C++ method from ruby
VALUE RubyCallMethod(int argc, VALUE * argv, VALUE self);
// Call a C++ static method from ruby
VALUE RubyCallFunction(int argc, VALUE * argv, VALUE self);
// Compare C++ classes from Ruby
VALUE RubyEqual(VALUE self, VALUE arg);

std::vector<std::string> makeRubySignature(int argc, VALUE * argv);
#endif
bool equalSignature(const std::vector<std::string> & sig1,
                    const std::vector<std::string> & sig2);
#ifdef SCRIPT_PYTHON
// Python 'malloc'
PyObject * PythonClassBaseAlloc(PyTypeObject * type,
                                PyObject * args,
                                PyObject * kwds);
// Python delete
void PythonClassBaseFree(PythonReflectionInstance * self);

// Python constructor
int PythonInitialize(PythonReflectionInstance * self,
                     PyObject * args,
                     PyObject * kwds);
#if PY_MAJOR_VERSION == 2
// Compare
int PythonClassBaseCompare(PyObject * o1, PyObject * o2);
#endif

// Get attribute
PyObject * PythonGetAttr(PythonReflectionInstance * self, void * closure);

// Set attribute
int PythonSetAttr(PythonReflectionInstance * self, PyObject * value,
                  void * closure);

// Get method
PyObject * PythonGetMethod(PythonReflectionInstance * self, void * closure);

// Set method
int PythonSetMethod(PythonReflectionInstance * self, PyObject * value,
                  void * closure);

// Call method
PyObject * PythonMethod(PyObject * selfCapsule, PyObject * args);

PyObject * PythonStaticMethod(PyObject * self, PyObject * args);

std::vector<std::string> makePythonSignature(PyObject * args);

// A dummy PyObject we can copy everytime we make a new class
// Need this because there is no function to create a new PyTypeObject, only
// macros
#if PY_MAJOR_VERSION == 2
PyTypeObject dummyType = {
    PyObject_HEAD_INIT(NULL)
    0,                         /*ob_size*/
    "dummy",                   /*tp_name*/
    0,                         /*tp_basicsize*/
    0,                         /*tp_itemsize*/
    0,                         /*tp_dealloc*/
    0,                         /*tp_print*/
    0,                         /*tp_getattr*/
    0,                         /*tp_setattr*/
    0,                         /*tp_compare*/
    0,                         /*tp_repr*/
    0,                         /*tp_as_number*/
    0,                         /*tp_as_sequence*/
    0,                         /*tp_as_mapping*/
    0,                         /*tp_hash */
    0,                         /*tp_call*/
    0,                         /*tp_str*/
    0,                         /*tp_getattro*/
    0,                         /*tp_setattro*/
    0,                         /*tp_as_buffer*/
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE, /*tp_flags*/
    "Dummy objects",           /*tp_doc*/
};

#endif
#if PY_MAJOR_VERSION == 3
PyTypeObject dummyType = {
    PyObject_HEAD_INIT(NULL)
    "dummy",                   /*tp_name*/
    0,                         /*tp_basicsize*/
    0,                         /*tp_itemsize*/
    0,                         /*tp_dealloc*/
    0,                         /*tp_print*/
    0,                         /*tp_getattr*/
    0,                         /*tp_setattr*/
    0,                         /*tp_compare*/
    0,                         /*tp_repr*/
    0,                         /*tp_as_number*/
    0,                         /*tp_as_sequence*/
    0,                         /*tp_as_mapping*/
    0,                         /*tp_hash */
    0,                         /*tp_call*/
    0,                         /*tp_str*/
    0,                         /*tp_getattro*/
    0,                         /*tp_setattro*/
    0,                         /*tp_as_buffer*/
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE, /*tp_flags*/
    "Dummy objects",           /*tp_doc*/
};

#endif

// For class methods, code taken from Python source
#if PY_MAJOR_VERSION  == 2
typedef struct {
    PyDescr_COMMON;
} PyDescrObject;
#endif

typedef struct {
    PyDescr_COMMON;
#if PY_MAJOR_VERSION == 3
    PyMethodDef * d_method;
#endif
    Reflection::ClassBase * klass;
    std::string * methodName;
} ScriptMethodDescrObject;
void descr_dealloc(PyDescrObject *descr);
#if PY_MAJOR_VERSION == 2
char * descr_name(PyDescrObject *descr);
PyObject * descr_repr(PyDescrObject *descr, char *format);
#endif
#if PY_MAJOR_VERSION == 3
PyObject * descr_name(PyDescrObject *descr);
#endif
PyDescrObject * descr_new(PyTypeObject *descrtype,
                          PyTypeObject *type, const char *name);
PyObject * method_repr(ScriptMethodDescrObject *descr);
PyObject * classmethod_get(ScriptMethodDescrObject *descr,
                           PyObject *obj, PyObject *type);
int descr_traverse(PyObject *self, visitproc visit, void *arg);
PyMemberDef descr_members[] = {
#if PY_MAJOR_VERSION == 2
    {(char*)"__objclass__", T_OBJECT, offsetof(PyDescrObject, d_type),READONLY},
    {(char*)"__name__", T_OBJECT, offsetof(PyDescrObject, d_name), READONLY},
    {0}
#endif
#if PY_MAJOR_VERSION == 3
    {(char*)"__objclass__", T_OBJECT, offsetof(PyDescrObject, d_type ),READONLY},
    {(char*)"__name__", T_OBJECT, offsetof(PyDescrObject, d_name), READONLY},
    {0}
#endif
};

#if PY_MAJOR_VERSION == 2
PyTypeObject MyPyClassMethodDescr_Type = {
    PyVarObject_HEAD_INIT(&PyType_Type, 0)
    "classmethod_descriptor",
    sizeof(ScriptMethodDescrObject),
    0,
    (destructor)descr_dealloc,                  /* tp_dealloc */
    0,                                          /* tp_print */
    0,                                          /* tp_getattr */
    0,                                          /* tp_setattr */
    0,                                          /* tp_compare */
    (reprfunc)method_repr,                      /* tp_repr */
    0,                                          /* tp_as_number */
    0,                                          /* tp_as_sequence */
    0,                                          /* tp_as_mapping */
    0,                                          /* tp_hash */
    0,                                          /* tp_call */
    0,                                          /* tp_str */
    PyObject_GenericGetAttr,                    /* tp_getattro */
    0,                                          /* tp_setattro */
    0,                                          /* tp_as_buffer */
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_HAVE_GC, /* tp_flags */
    0,                                          /* tp_doc */
    descr_traverse,                             /* tp_traverse */
    0,                                          /* tp_clear */
    0,                                          /* tp_richcompare */
    0,                                          /* tp_weaklistoffset */
    0,                                          /* tp_iter */
    0,                                          /* tp_iternext */
    0,                                          /* tp_methods */
    descr_members,                              /* tp_members */
    0            ,                              /* tp_getset */
    0,                                          /* tp_base */
    0,                                          /* tp_dict */
    (descrgetfunc)classmethod_get,              /* tp_descr_get */
    0,                                          /* tp_descr_set */
};
#endif
#if PY_MAJOR_VERSION == 3
PyTypeObject MyPyClassMethodDescr_Type = {
    PyVarObject_HEAD_INIT(&PyType_Type, 0)
    "classmethod_descriptor",
    sizeof(ScriptMethodDescrObject),
    0,
    (destructor)descr_dealloc,                  /* tp_dealloc */
    0,                                          /* tp_print */
    0,                                          /* tp_getattr */
    0,                                          /* tp_setattr */
    0,                                          /* tp_compare */
    (reprfunc)method_repr,                      /* tp_repr */
    0,                                          /* tp_as_number */
    0,                                          /* tp_as_sequence */
    0,                                          /* tp_as_mapping */
    0,                                          /* tp_hash */
    0,                                          /* tp_call */
    0,                                          /* tp_str */
    PyObject_GenericGetAttr,                    /* tp_getattro */
    0,                                          /* tp_setattro */
    0,                                          /* tp_as_buffer */
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_HAVE_GC, /* tp_flags */
    0,                                          /* tp_doc */
    descr_traverse,                             /* tp_traverse */
    0,                                          /* tp_clear */
    0,                                          /* tp_richcompare */
    0,                                          /* tp_weaklistoffset */
    0,                                          /* tp_iter */
    0,                                          /* tp_iternext */
    0,                                          /* tp_methods */
    descr_members,                              /* tp_members */
    0            ,                              /* tp_getset */
    0,                                          /* tp_base */
    0,                                          /* tp_dict */
    (descrgetfunc)classmethod_get,              /* tp_descr_get */
    0,                                          /* tp_descr_set */
    0,                                          /* d_method */
};
#endif

// Needed to verify if a PyObject is a C++ class
std::unordered_set<PyTypeObject*> allPythonClasses;

struct PythonMethodClosure
{
  Reflection::ClassBase * klass;
  std::string methodName;
  PythonReflectionInstance * self; // Filled in on function 'get'
};
#endif

// Translate reserved names into something not reserved
std::string translateName(const std::string & name);
std::string untranslateName(const std::string & name);
}

void ScriptInterface::makeClasses()
{
  auto classes = Reflection::Registry::instance().getClasses();
  for (auto klass : classes)
    {
      std::string name = klass->getName();
      auto & classInfo = *klass->getClassInfo();
#ifdef SCRIPT_RUBY
      {
      std::string nameUpperFirst = name;
      nameUpperFirst[0] = std::toupper(nameUpperFirst[0]);

      VALUE rubyParent;
      if (auto parent = klass->getParent1())
        rubyParent = parent->getClassInfo()->rubyClass;
      else
        rubyParent = rb_cObject;
      classInfo.rubyClass =
        rb_define_class(nameUpperFirst.c_str(), rubyParent);
      using RubyCallback = VALUE(*)(...);
      rb_define_alloc_func(classInfo.rubyClass, Anonymous::RubyClassBaseAlloc);
      rb_define_method(classInfo.rubyClass, "initialize",
                       (RubyCallback)RubyInitialize, -1);
      rb_iv_set(classInfo.rubyClass, "@c++class", rb_uint2big((long)klass));
      rb_define_method(classInfo.rubyClass, "==",
                       (RubyCallback)RubyEqual, 1);
      rb_define_method(classInfo.rubyClass, "eql?",
                       (RubyCallback)RubyEqual, 1);
      rb_define_method(classInfo.rubyClass, "equal?",
                       (RubyCallback)RubyEqual, 1);

      auto attributes = klass->getAttributeMap();
      for (auto attribute : *attributes)
        {
          rb_define_method(classInfo.rubyClass,
                           translateName(attribute.first).c_str(),
                           (RubyCallback)RubyGetAttr, 0);
          rb_define_method(classInfo.rubyClass,
                           (translateName(attribute.first) + "=").c_str(),
                           (RubyCallback)RubySetAttr, 1);
        }
      auto methods = klass->getMethodMap();
      for (auto method : *methods)
        {
          if (method.second->isStatic())
            {
              rb_define_singleton_method(classInfo.rubyClass,
                                         translateName(method.first).c_str(),
                                         (RubyCallback)RubyCallFunction, -1);
            }
          else
            {
              rb_define_method(classInfo.rubyClass,
                               translateName(method.first).c_str(),
                               (RubyCallback)RubyCallMethod, -1);
            }
        }
      auto enums = klass->getEnumArray();
      for (auto anEnum : *enums)
        {
          for (auto value : anEnum->enumValues_)
            rb_define_const(classInfo.rubyClass,
                            value.first.c_str(), INT2FIX(value.second));
        }
      }
#endif
#ifdef SCRIPT_PYTHON
      {
      PyTypeObject * pythonParent;
      if (auto parent = klass->getParent1())
        pythonParent = parent->getClassInfo()->pythonClass;
      else
        pythonParent = nullptr;
      classInfo.pythonClass = (PyTypeObject*)malloc(sizeof(PythonClassBase));
      memcpy(classInfo.pythonClass, &dummyType, sizeof(PyTypeObject));
      classInfo.pythonClass->tp_name = strdup(name.c_str());
      classInfo.pythonClass->tp_basicsize = sizeof(PythonReflectionInstance);
      classInfo.pythonClass->tp_doc = "C++ class instances";
      classInfo.pythonClass->tp_base = pythonParent;
      classInfo.pythonClass->tp_new = PythonClassBaseAlloc;
      classInfo.pythonClass->tp_dealloc = (destructor)PythonClassBaseFree;
      classInfo.pythonClass->tp_init = (initproc)PythonInitialize;
#if PY_MAJOR_VERSION == 2
      classInfo.pythonClass->tp_compare = PythonClassBaseCompare;
#endif
#if PY_MAJOR_VERSION == 3
#endif

      ((PythonClassBase*)classInfo.pythonClass)->cppClass = klass;

      auto methods = klass->getMethodMap();
      auto attributes = klass->getAttributeMap();
      PyGetSetDef * getsetters =
        (PyGetSetDef*)calloc(attributes->size()+methods->size()+1,
                             sizeof(PyGetSetDef));
      int getset = 0;
      std::set<std::string> uniqueMethodNames;
      std::set<std::string> uniqueStaticMethodNames;
      for (auto method : *methods)
        {
          if (method.second->isStatic() &&
              uniqueStaticMethodNames.count(method.first) == 0)
            {
              ScriptMethodDescrObject *descr;

              const char * name = strdup(translateName(method.first).c_str());
              descr = (ScriptMethodDescrObject *)
                descr_new(&MyPyClassMethodDescr_Type,
                          classInfo.pythonClass,
                          name);
              if (descr != NULL)
                {
                  descr->klass = klass;
                  descr->methodName =
                    new std::string(translateName(method.first));
                }
              auto dict = classInfo.pythonClass->tp_dict;
              if (dict == nullptr)
                {
                  dict = PyDict_New();
                  if (dict == nullptr)
                    throw std::runtime_error("Unable to make a new dictionary");
                  classInfo.pythonClass->tp_dict = dict;
                }
              int err = PyDict_SetItemString(dict, name, (PyObject*)descr);
              if (err < 0)
                throw std::runtime_error("Unable to add item to dict");
              Py_DECREF(descr);
              uniqueStaticMethodNames.insert(method.first);
            }
          if (!method.second->isStatic() &&
              uniqueMethodNames.count(method.first) == 0)
            {
              // delay name lookup until call to support polymorphism (multiple
              // function with same name)
              getsetters[getset].name =
                strdup(translateName(method.first).c_str());
              getsetters[getset].get  = (getter)PythonGetMethod;
              getsetters[getset].set  = (setter)PythonSetMethod;
              getsetters[getset].doc  = (char*)"C++ method";
              auto closure = new PythonMethodClosure;
              closure->methodName = translateName(method.first);
              closure->klass = klass;
              closure->self = nullptr;
              getsetters[getset].closure = closure;
              ++getset;
              uniqueMethodNames.insert(method.first);
            }
        }

      for (auto attribute : *attributes)
        {
          getsetters[getset].name =
            strdup(translateName(attribute.first).c_str());
          getsetters[getset].get  = (getter)PythonGetAttr;
          getsetters[getset].set  = (setter)PythonSetAttr;
          getsetters[getset].doc  = (char*)"C++ attribute";
          getsetters[getset].closure = attribute.second;
          ++getset;
        }

      auto enums = klass->getEnumArray();
      for (auto anEnum : *enums)
        {
          auto dict = classInfo.pythonClass->tp_dict;
          if (dict == nullptr)
            {
              dict = PyDict_New();
              if (dict == nullptr)
                throw std::runtime_error("Unable to make a new dictionary");
              classInfo.pythonClass->tp_dict = dict;
            }
          for (auto value : anEnum->enumValues_)
            {
              const char * name =
                strdup(translateName(value.first).c_str());
#if PY_MAJOR_VERSION == 2
              int err = PyDict_SetItemString(dict, name,
                                             PyInt_FromLong(value.second));
#endif
#if PY_MAJOR_VERSION == 3
              int err = PyDict_SetItemString(dict, name,
                                             PyLong_FromLong(value.second));
#endif
              if (err < 0)
                throw std::runtime_error("Unable to add item to dict");
            }
        }

      getsetters[getset].name = nullptr; // Sentinel
      classInfo.pythonClass->tp_getset = getsetters;

      if (PyType_Ready(classInfo.pythonClass) == -1)
        throw std::runtime_error("PyType_Ready failed");

      allPythonClasses.insert(classInfo.pythonClass);

      PyModule_AddObject(pymodule_, name.c_str(),
                         (PyObject*)classInfo.pythonClass);
      }
#endif
    }
}

void ScriptInterface::defineGlobalVariable(const std::string & name,
                                           ReflectionHandle variable,
                                           void * data)
{
  // Ruby stores the pointer to the ReflectionHandle, need to make a copy
  // This is a memory leak, but hopefully nobody is defining millions of
  // global variables
#ifdef SCRIPT_RUBY
  if (data == LANGUAGE_RUBY)
    {
      VALUE * varCopy = (VALUE*)malloc(sizeof(VALUE));
      *varCopy = variable.rubyHandle;
      rb_define_variable(("$" + name).c_str(), varCopy);
    }
#endif
#ifdef SCRIPT_PYTHON
  if (data == LANGUAGE_PYTHON)
    {
      if (PyObject_SetAttrString(pymodule_, name.c_str(),
                                 variable.pythonHandle) == -1)
        {
          PyErr_Print();
        }
    }
#endif
}

void ScriptInterface::defineGlobalConstant(const std::string & name,
                                           ReflectionHandle variable,
                                           void * data)
{
#ifdef SCRIPT_RUBY
  if (data == LANGUAGE_RUBY)
    {
      rb_define_global_const(name.c_str(), variable.rubyHandle);
    }
#endif
#ifdef SCRIPT_PYTHON
  if (data == LANGUAGE_PYTHON)
    {
      if (PyObject_SetAttrString(pymodule_, name.c_str(),
                                 variable.pythonHandle) == -1)
        {
          PyErr_Print();
        }
    }
#endif
}

#ifdef SCRIPT_PYTHON
PyMethodDef pythonGlobalFunctionCaller = {
  "globalfunctioncpp",
  nullptr,
  METH_VARARGS,
  "global function in a C++ module"
};
#endif

void ScriptInterface::makeGlobalFunction(const std::string & name,
                                         Reflection::MethodBase * function)
{
#ifdef SCRIPT_RUBY
  {
    using RubyCallback = VALUE(*)(...);
    rb_define_global_function(translateName(name).c_str(),
                              (RubyCallback)Anonymous::RubyCallGlobalFunction,
                              -1);
  }
#endif
#ifdef SCRIPT_PYTHON
  {
    // TODO check if attr already exists;
    if (globalFunctions_.count(name) == 0)
      {
        pythonGlobalFunctionCaller.ml_meth = Anonymous::PythonGlobalFunction;
#if PY_MAJOR_VERSION == 2
        PyObject * callable = PyCFunction_New(&pythonGlobalFunctionCaller,
                                              PyString_FromString(name.c_str()));
#endif
#if PY_MAJOR_VERSION == 3
        PyObject * callable = PyCFunction_New(&pythonGlobalFunctionCaller,
                                              PyUnicode_FromString(name.c_str()));
#endif
        if (PyObject_SetAttrString(pymodule_, name.c_str(),
                                   callable) == -1)
          {
            PyErr_Print();
          }
      }
  }
#endif
  globalFunctions_.insert({name, function});
}

Reflection::MethodBase *
ScriptInterface::getGlobalFunction(const std::string & name,
                                   const std::vector<std::string> & signature)
  const
{
  const auto functionRange= globalFunctions_.equal_range(name);
  for (auto function = functionRange.first; function!=functionRange.second;
       ++function)
    {
      if (equalSignature(signature, function->second->signature()))
        return function->second;
    }
  return nullptr;
}

#ifdef SCRIPT_RUBY
namespace // anonymous
{
struct RubyGlobalFunctionCallInfo
{
  VALUE args[6];
  unsigned int numArgs;
  ID id;
};
VALUE RubyGlobalFunctionCall(VALUE args)
{
  RubyGlobalFunctionCallInfo * info = (RubyGlobalFunctionCallInfo*)args;
  // self refers to the global main object where global function are put in
  // It used to be accessible via rb_vm_top_self() but that is hidden now.
  return rb_funcall2(rb_eval_string("self"), info->id, info->numArgs,
                     info->args);
}
}

void ScriptInterface::callRuby(const std::string & functionName) const
{
  callRubyPrivate(functionName);
}

VALUE ScriptInterface::callRubyPrivate(const std::string & functionName,
                                       VALUE argument1,
                                       VALUE argument2,
                                       VALUE argument3,
                                       VALUE argument4,
                                       VALUE argument5,
                                       VALUE argument6) const
{
  RubyGlobalFunctionCallInfo info;
  info.id = rb_intern(functionName.c_str());
  info.numArgs = 0;
  if (argument1)
    info.args[info.numArgs++] = argument1;
  if (argument2)
    info.args[info.numArgs++] = argument2;
  if (argument3)
    info.args[info.numArgs++] = argument3;
  if (argument4)
    info.args[info.numArgs++] = argument4;
  if (argument5)
    info.args[info.numArgs++] = argument5;
  if (argument6)
    info.args[info.numArgs++] = argument6;
  int state = 0;
  VALUE result = rb_protect_wrap(RubyGlobalFunctionCall, (VALUE)&info, &state);
  RubyException::checkRubyException(state);
  return result;
}

// Ruby 'new'
VALUE ScriptInterface::Anonymous::RubyClassBaseAlloc(VALUE self)
{
  //std::cerr << "alloc " << rb_class2name(self) << "\n";
  VALUE result = Data_Wrap_Struct(self, RubyClassBaseMark, RubyClassBaseFree, nullptr);
  ScriptInterface::instance().registerRubyObject(result);
  return result;
}

// Call a global function
VALUE ScriptInterface::Anonymous::
RubyCallGlobalFunction(int argc, VALUE * argv,
                       VALUE rubyObjectClass __attribute__((unused)))
{
  std::string callingFunction =
    untranslateName(rb_id2name(rb_frame_this_func()));

  auto rubySig = makeRubySignature(argc, argv);
  auto method = ScriptInterface::instance().
    getGlobalFunction(callingFunction, rubySig);
  if (method == nullptr)
    {
      const auto & globalFunctions = ScriptInterface::instance().globalFunctions_;
      if (globalFunctions.count(callingFunction) == 0)
        {
          // This normally cannot happen, this function only gets called after
          // defining the function in scripting
          rb_exc_raise(rb_exc_new2(rb_eNoMethodError,
                                   ("Internal error: Undefined global function "
                                    + callingFunction).c_str()));
          return Qnil;
        }
      std::ostringstream message;
      message
        << "'Argument mismatch for function " << callingFunction
        << "\nCaller signature :\n";
      printNiceSignature(message, rubySig);
      message << "\nAvailable signatures :\n";
      const auto functionRange = globalFunctions.equal_range(callingFunction);
      for (auto function = functionRange.first; function!=functionRange.second;
           ++function)
        {
          printNiceSignature(message, function->second->signature());
          message << "\n";
        }
      rb_exc_raise(rb_exc_new2(rb_eArgError, message.str().c_str()));
      return Qnil;
    }
  try
    {
      ReflectionHandle rubyArgs[7] = {};
      if (argc >= 1)
        rubyArgs[0].rubyHandle = argv[0];
      if (argc >= 2)
        rubyArgs[1].rubyHandle = argv[1];
      if (argc >= 3)
        rubyArgs[2].rubyHandle = argv[2];
      if (argc >= 4)
        rubyArgs[3].rubyHandle = argv[3];
      if (argc >= 5)
        rubyArgs[4].rubyHandle = argv[4];
      if (argc >= 6)
        rubyArgs[5].rubyHandle = argv[5];
      if (argc >= 7)
        rubyArgs[6].rubyHandle = argv[6];
      return method->call(nullptr,
                          LANGUAGE_RUBY,
                          rubyArgs[0], rubyArgs[1], rubyArgs[2],
                          rubyArgs[3], rubyArgs[4], rubyArgs[5],
                          rubyArgs[6]).
        rubyHandle;
    }
  catch (std::exception & e)
    {
      rb_exc_raise(rb_exc_new2(rb_eArgError, e.what()));
      return Qnil;
    }
}
#endif
#ifdef SCRIPT_PYTHON
PyObject * ScriptInterface::Anonymous::
PythonGlobalFunction(PyObject * functionName, PyObject * args)
{
#if PY_MAJOR_VERSION == 2
  const char * functionNameC = PyString_AsString(functionName);
#endif
#if PY_MAJOR_VERSION == 3
  const char * functionNameC = PyUnicode_AsUTF8(functionName);
#endif
  auto pySig = makePythonSignature(args);
  auto method = ScriptInterface::instance().
    getGlobalFunction(functionNameC, pySig);
  if (method == nullptr)
    {
      const auto & globalFunctions = ScriptInterface::instance().globalFunctions_;
      if (globalFunctions.count(functionNameC) == 0)
        {
          // This normally cannot happen, this function only gets called after
          // defining the function in scripting
          PyErr_SetString(PyExc_TypeError,
                          (std::string("Internal error: ") +
                           "Undefined global function "+functionNameC).c_str());
          return nullptr;
        }
      std::ostringstream message;
      message
        << "'Argument mismatch for function " << functionNameC
        << "\nCaller signature :\n";
      printNiceSignature(message, pySig);
      message << "\nAvailable signatures :\n";
      const auto functionRange = globalFunctions.equal_range(functionNameC);
      for (auto function = functionRange.first; function!=functionRange.second;
           ++function)
        {
          printNiceSignature(message, function->second->signature());
          message << "\n";
        }
      PyErr_SetString(PyExc_TypeError, message.str().c_str());
      return nullptr;
    }
  ReflectionHandle pyArgs[] = { 0, 0, 0, 0, 0, 0 };
  Py_ssize_t argc = PyTuple_Size(args);
  for (unsigned int i=0; i<argc; ++i)
    {
      pyArgs[i].pythonHandle = PyTuple_GetItem(args, i);
    }
  return method->call(nullptr,
                      LANGUAGE_PYTHON,
                      pyArgs[0], pyArgs[1], pyArgs[2],
                      pyArgs[3], pyArgs[4], pyArgs[5],
                      pyArgs[6]).pythonHandle;
}

#endif

namespace // anonymous
{

bool typeIdMatches(const std::string & id1, const std::string & id2)
{
  if (id1 == id2)
    return true;
  if (id1 == typeid(int).name())
    {
      if (id2 == typeid(short).name())
        return true;
      if (id2 == typeid(unsigned short).name())
        return true;
      if (id2 == typeid(unsigned int).name())
        return true;
      if (id2 == typeid(char).name())
        return true;
      if (id2 == typeid(unsigned char).name())
        return true;
    }
  // Maybe id1 (Ruby type) is nil and id2 is a pointer
  std::string className =
    Reflection::Registry::instance().pointerToClassname(id2);
  if (!className.empty() && id1 == "0")
    return true;
  // Maybe id2(C++ constructor argument) is a pointer and id1 is a type derived
  // from id2
  if (!className.empty())
    return Reflection::Registry::instance().isInheritedFrom(id1, className);
  // Or maybe id2 is an array of pointers and id1 is an array of pointers
  if ((id1.substr(0, 6) == "array ") && (id2.substr(0, 6) == "array "))
    {
      className =
        Reflection::Registry::instance().pointerToClassname(id2.substr(6));
      if (!className.empty())
        {
          if (Reflection::Registry::instance().isInheritedFrom(id1.substr(6),
                                                               className))
            return true;
        }

      // Or maybe they are both integer types (or something else compatible)
      return typeIdMatches(id1.substr(6), id2.substr(6));
    }
  // Or maybe id1 (Ruby type) is an empty array and id2 is also an array
  if (id1 == "empty array" && id2.substr(0, 6) == "array ")
    return true;

  auto equality = typeEqualities_.find(id1);
  if (equality != typeEqualities_.end())
    {
      return equality->second == id2;
    }
  return false;
}

void printNiceTypename(std::ostream & str, const std::string & typeidname)
{
  str << niceTypename(typeidname);
}

void printNiceSignature(std::ostream & str,
                        const std::vector<std::string> & signature)
{
  if (signature.empty())
    {
      str << "no arguments";
    }
  else
    {
      for (unsigned int arg=0; ; )
        {
          printNiceTypename(str, signature[arg]);
          if (++arg != signature.size())
            str << ", ";
          else
            break;
        }
    }
}

std::string signatureMismatch(const std::vector<std::string> & scriptSig,
                              const std::string & prefix,
                              const std::string & functionKind,
                              Reflection::ClassBase * klass,
                              const std::string & methodName)
{
  std::ostringstream message;
  unsigned int count = klass->getMethodMap()->count(methodName);
  if (count == 0)
    {
      if (klass->getParent1() == nullptr)
        message << prefix << "No " << functionKind << " `" << methodName << "'";
      else
        return signatureMismatch(scriptSig, prefix, functionKind,
                                 klass->getParent1(), methodName);
    }
  else
    {
      message << prefix
        << "No " << functionKind << " `" << methodName << "' found with";
      if (scriptSig.size() == 0)
        message << "out arguments\n";
      else
        {
          message << " following " << scriptSig.size() << " argument types :\n";
          printNiceSignature(message, scriptSig);
        }
      message << "\n";
      auto methodRange = klass->getMethodMap()->equal_range(methodName);
      assert(methodRange.first != klass->getMethodMap()->end());
      if (count == 1)
        message << "There is 1 " << functionKind << " ";
      else
        message << "There are " << count << " " << functionKind << "s ";
      message << "called `" << methodName << "' available :\n";
      for (auto method = methodRange.first; method!=methodRange.second;++method)
        {
          printNiceSignature(message, method->second->signature());
          message << "\n";
        }
    }
  return message.str();
}

Reflection::MethodBase *
findMethodInClass(const std::string & name,
                  Reflection::ClassBase * klass,
                  const std::vector<std::string> & scriptSig)
{
  auto methodMap = klass->getMethodMap();
  const auto methodRange= methodMap->equal_range(name);
  for (auto method = methodRange.first; method!=methodRange.second; ++method)
    {
      if (equalSignature(scriptSig, method->second->signature()))
        return method->second;
    }
  if (auto parent1 = klass->getParent1())
    {
      return findMethodInClass(name, parent1, scriptSig);
    }
  return nullptr;
}

#ifdef SCRIPT_RUBY
Reflection::ClassBase * getCppKlassPointer(VALUE rbClass)
{
  VALUE rbClassOrig = rbClass;
  VALUE rbcppKlass = Qnil;
  while (rbClass != Qnil)
    {
      rbcppKlass = rb_iv_get(rbClass, "@c++class");
      if (rbcppKlass != Qnil)
        break;
      rbClass = RCLASS_SUPER(rbClass);
    }
  if (rbcppKlass == Qnil)
    throw std::runtime_error("No attribute called @c++class for ruby class"
                             + std::string(rb_class2name(rbClassOrig)));
  return reinterpret_cast<Reflection::ClassBase*>(rb_big2ulong(rbcppKlass));
}

// Ruby 'free'
void RubyClassBaseFree(long * reference)
{
  auto ref = reinterpret_cast<RubyPythonReference*>(reference);
  if (!ruby_vm_exiting)
    {
      assert(!ref->isUsedInC());
      ref->deleteFromScript(LANGUAGE_RUBY);
    }
}

// Ruby 'garbage collect mark'
// This prevents Ruby GC from deleting the object while still in use by C++
// But it only works for VALUE objects that are children of the thing \arg
// reference came from.  We want to mark the VALUE object holding reference
// itself.  That is not possible -> rbObjectHash_ solution.
void RubyClassBaseMark(long * reference __attribute__((unused)))
{
  return;
}

std::string rubyTypeToTypeid(VALUE arg)
{
  int type = TYPE(arg);
  switch (type)
    {
    case T_FIXNUM : return typeid(int).name();
    case T_BIGNUM : return typeid(int).name();
    case T_TRUE   : return typeid(bool).name();
    case T_FALSE  : return typeid(bool).name();
    case T_STRING : return typeid(std::string).name();
    case T_DATA :
      {
        // Maybe a C++ exported class ?
        VALUE cppKlass = rb_iv_get(CLASS_OF(arg), "@c++class");
        if (CLASS_OF(arg) == rb_cProc)
          return typeid(ScriptObject).name();
        else if (cppKlass == Qnil) // Nope
          {
            // Maybe a parent class is a C++ class
            VALUE rbParent = RCLASS_SUPER(CLASS_OF(arg));
            while (rbParent != Qnil)
              {
                cppKlass = rb_iv_get(rbParent, "@c++class");
                if (cppKlass != Qnil)
                  {
                    auto klass =
                      reinterpret_cast<Reflection::ClassBase*>(rb_big2ulong(cppKlass));
                    return klass->getTypeIdName();
                  }
                rbParent = RCLASS_SUPER(rbParent);
              }
            // No C++ parent found, just return something special
            return "<Ruby>" + std::string(rb_class2name(CLASS_OF(arg)));
          }
        auto klass =
          reinterpret_cast<Reflection::ClassBase*>(rb_big2ulong(cppKlass));
        return klass->getTypeIdName();
      }
    case T_ARRAY :
      {
        if (RARRAY_LEN(arg))
          return "array " + rubyTypeToTypeid(RARRAY_PTR(arg)[0]);
        else
          return "empty array";
      }
    case T_NIL :
      {
        return "0";
        //rb_exc_raise(rb_exc_new2(rb_eTypeError, "Qnil passed to C++"));
      }
    case T_OBJECT :
    case T_CLASS :
      {
        return typeid(ScriptObject).name();
      }
    }
  std::cout << "type = " << type << "\n";
  throw std::runtime_error("Unknown ruby type");
  assert(false);
}

std::vector<std::string> makeRubySignature(int argc, VALUE * argv)
{
  std::vector<std::string> result;
  for (int i=0; i<argc; ++i)
    {
      if (i < argc)
        result.push_back(rubyTypeToTypeid(argv[i]));
      else
        result.push_back("0");
    }
  return result;
}

// Ruby constructor
VALUE RubyInitialize(int argc, VALUE * argv, VALUE self)
{
  auto cppKlass = getCppKlassPointer(CLASS_OF(self));
  auto classInfo = *cppKlass->getClassInfo();
  auto constructorArray = cppKlass->getConstructorArray();

  if (argc > 7)
    {
      std::ostringstream message;
      message << "C++ Class " << rb_class2name(CLASS_OF(self))
        << " : Too many arguments to initialize";
      rb_exc_raise(rb_exc_new2(rb_eTypeError, message.str().c_str()));
      return Qnil;
    }
  // Find a constructor with the same signature
  if (constructorArray->empty())
    {
      std::ostringstream message;
      message << "C++ Class " << rb_class2name(CLASS_OF(self))
        << " : No constructors available";
      rb_exc_raise(rb_exc_new2(rb_eTypeError, message.str().c_str()));
      return Qnil;
    }
  bool correct;
  ReflectionHandle reflectionArgv[7];
  for (int i=0; i<7; ++i)
    {
      if (i < argc)
        reflectionArgv[i].rubyHandle = argv[i];
      else
        reflectionArgv[i].rubyHandle = 0;
    }

  for (auto constructor : *constructorArray)
    {
      // All arguments should have the correct type
      correct = equalSignature(makeRubySignature(argc, argv),
                               constructor->signature());
      if (correct)
        {
          void * thls;
          try
            {
              thls = constructor->call(LANGUAGE_RUBY,
                                       reflectionArgv[0], reflectionArgv[1],
                                       reflectionArgv[2], reflectionArgv[3],
                                       reflectionArgv[4], reflectionArgv[5],
                                       reflectionArgv[6]);
              auto scriptThis = reinterpret_cast<ScriptAccess*>(thls);
              auto rubyRef =
                reinterpret_cast
                <RubyPythonReference*>(classInfo.makeReference(thls));
              scriptThis->setReference(rubyRef);
              rubyRef->setRubyObject(self);
              DATA_PTR(self) = rubyRef;
              // Default reference constructor assumes object is stored in C++.
              // This is not the case when it is created from Ruby (this
              // function).
              rubyRef->deleteFromC();
              if (auto scriptObject = classInfo.asScriptObject(thls))
                {
                  scriptObject->setRubyValue(self);
                }
              return self;
            }
          catch (std::exception & e)
            {
              rb_exc_raise(rb_exc_new2(rb_eArgError, e.what()));
              return Qnil;
            }
        }
    }
  if (!correct)
    {
      std::ostringstream message;
      message << "C++ Class " << rb_class2name(CLASS_OF(self))
        << " :\nNo constructor found with following " << argc
        << " argument types :\n";
      for (int arg=0; arg<argc; ++arg)
        {
          message << niceTypename(rubyTypeToTypeid(argv[arg])) << ", ";
        }
      message << "\nThere are " << constructorArray->size()
        << " constructors available :\n";
      for (auto constructor : *constructorArray)
        {
          if (constructor->signature().empty())
            message << "no arguments";
          else
            printNiceSignature(message, constructor->signature());
          message << "\n";
        }
      rb_exc_raise(rb_exc_new2(rb_eTypeError, message.str().c_str()));
      return Qnil;
    }

  return Qnil;

}

Reflection::AttributeBase *
findAttributeInClass(const std::string & name,
                     Reflection::ClassBase * klass)
{
  auto attributeMap = klass->getAttributeMap();
  const auto attribute = attributeMap->find(name);
  if (attribute == attributeMap->end())
    {
      if (auto parent1 = klass->getParent1())
        {
          auto attribute = findAttributeInClass(name, parent1);
          if (attribute)
            return attribute;
          if (auto parent2 = klass->getParent2())
            {
              attribute = findAttributeInClass(name, parent2);
              if (attribute)
                return attribute;
            }
        }
      return nullptr;
    }
  else
    return attribute->second;
}

// Read an attribute
VALUE RubyGetAttr(VALUE self)
{
  auto klass = getCppKlassPointer(CLASS_OF(self));

  std::string callingFunction =
    untranslateName(rb_id2name(rb_frame_this_func()));

  auto attribute = findAttributeInClass(callingFunction, klass);
  if (attribute == nullptr)
    {
      rb_exc_raise(rb_exc_new2(rb_eNoMethodError, ("Undefined attribute " +
                               callingFunction + " for " + klass->getName())
                               .c_str()));
    }
  try
    {
      RubyPythonReference * reference;
      Data_Get_Struct(self, RubyPythonReference, reference);
      if (reference->getCppObject()->get() == nullptr)
        rb_exc_raise(rb_exc_new2(rb_eNoMethodError, ("Undefined attribute " +
                                 callingFunction).c_str()));

      return attribute->getter(reference->getCppObject()->get(),
                               LANGUAGE_RUBY).rubyHandle;
    }
  catch (std::exception & e)
    {
      rb_exc_raise(rb_exc_new2(rb_eArgError, e.what()));
      return Qnil;
    }
}
// Write an attribute
VALUE RubySetAttr(VALUE self, VALUE value)
{
  auto klass = getCppKlassPointer(CLASS_OF(self));

  std::string callingFunction = rb_id2name(rb_frame_this_func());
  // Strip of '='
  callingFunction =
    untranslateName(callingFunction.substr(0, callingFunction.size()-1));

  auto attribute = findAttributeInClass(callingFunction, klass);
  if (attribute == nullptr)
    {
      rb_exc_raise(rb_exc_new2(rb_eNoMethodError, ("Undefined attribute " +
                               callingFunction + " for " + klass->getName())
                               .c_str()));
    }
  try
    {
      RubyPythonReference * reference;
      Data_Get_Struct(self, RubyPythonReference, reference);
      ReflectionHandle rValue;
      rValue.rubyHandle = value;
      return attribute->setter(reference->getCppObject()->get(),
                               LANGUAGE_RUBY, rValue).rubyHandle;
    }
  catch (std::exception & e)
    {
      rb_exc_raise(rb_exc_new2(rb_eArgError, e.what()));
      return Qnil;
    }
}

// Call a method
VALUE RubyCallMethod(int argc, VALUE * argv, VALUE self)
{
  auto klass = getCppKlassPointer(CLASS_OF(self));

  std::string callingFunction =
    untranslateName(rb_id2name(rb_frame_this_func()));

  auto method = findMethodInClass(callingFunction, klass,
                                  makeRubySignature(argc, argv));
  if (method == nullptr)
    {
      std::string message = signatureMismatch(makeRubySignature(argc, argv),
                                              "C++ Class " + klass->getName()
                                              + "\n",
                                              "method",
                                              klass,
                                              callingFunction);
      rb_exc_raise(rb_exc_new2(rb_eNoMethodError, message.c_str()));
    }
  try
    {
      RubyPythonReference * reference;
      Data_Get_Struct(self, RubyPythonReference, reference);
      ReflectionHandle rubyArgs[7] = {};
      if (argc >= 1)
        rubyArgs[0].rubyHandle = argv[0];
      if (argc >= 2)
        rubyArgs[1].rubyHandle = argv[1];
      if (argc >= 3)
        rubyArgs[2].rubyHandle = argv[2];
      if (argc >= 4)
        rubyArgs[3].rubyHandle = argv[3];
      if (argc >= 5)
        rubyArgs[4].rubyHandle = argv[4];
      if (argc >= 6)
        rubyArgs[5].rubyHandle = argv[5];
      if (argc >= 7)
        rubyArgs[6].rubyHandle = argv[6];
      return method->call(reference->getCppObject()->get(),
                          LANGUAGE_RUBY,
                          rubyArgs[0], rubyArgs[1], rubyArgs[2],
                          rubyArgs[3], rubyArgs[4], rubyArgs[5],
                          rubyArgs[6]).
        rubyHandle;
    }
  catch (std::exception & e)
    {
      rb_exc_raise(rb_exc_new2(rb_eArgError, e.what()));
      return Qnil;
    }
}

// Call a static method
VALUE RubyCallFunction(int argc, VALUE * argv, VALUE self)
{
  // This function is for static methods
  // self points to the class type here, not a instance of the class
  auto klass = getCppKlassPointer(self);

  std::string callingFunction =
    untranslateName(rb_id2name(rb_frame_this_func()));

  auto method = findMethodInClass(callingFunction, klass,
                                  makeRubySignature(argc, argv));
  if (method == nullptr)
    {
      rb_exc_raise(rb_exc_new2(rb_eNoMethodError,
                               signatureMismatch(makeRubySignature(argc, argv),
                                                 "C++ Class " + klass->getName()
                                                 + "\n",
                                                 "static/class method",
                                                 klass,
                                                 callingFunction).c_str()));
    }
  try
    {
      ReflectionHandle rubyArgs[7] = {};
      if (argc >= 1)
        rubyArgs[0].rubyHandle = argv[0];
      if (argc >= 2)
        rubyArgs[1].rubyHandle = argv[1];
      if (argc >= 3)
        rubyArgs[2].rubyHandle = argv[2];
      if (argc >= 4)
        rubyArgs[3].rubyHandle = argv[3];
      if (argc >= 5)
        rubyArgs[4].rubyHandle = argv[4];
      if (argc >= 6)
        rubyArgs[5].rubyHandle = argv[5];
      if (argc >= 7)
        rubyArgs[6].rubyHandle = argv[6];
      return method->call(nullptr,
                          LANGUAGE_RUBY,
                          rubyArgs[0], rubyArgs[1], rubyArgs[2],
                          rubyArgs[3], rubyArgs[4], rubyArgs[5],
                          rubyArgs[6]).
        rubyHandle;
    }
  catch (std::exception & e)
    {
      rb_exc_raise(rb_exc_new2(rb_eArgError, e.what()));
      return Qnil;
    }
}

#if RUBY_VERSION_MAJOR == 1 && RUBY_VERSION_MINOR == 8
#define RUBY_T_DATA T_DATA
#endif

// Compare C++ classes from Ruby
VALUE RubyEqual(VALUE self, VALUE arg)
{
  // TODO : this is ok for eql? and equal? but a bit too strict for ==
  RubyPythonReference * selfReference = nullptr;
  RubyPythonReference * argReference = nullptr;
  if (TYPE(self) != RUBY_T_DATA)
    throw std::runtime_error("C++ == called for something that not from C++");
  if (TYPE(arg) != RUBY_T_DATA)
    return Qfalse;
  Data_Get_Struct(self, RubyPythonReference, selfReference);
  Data_Get_Struct(arg, RubyPythonReference, argReference);
  if (selfReference && argReference)
    return selfReference == argReference;
  else
    return Qfalse;
}
#endif

bool equalSignature(const std::vector<std::string> & sig1,
                    const std::vector<std::string> & sig2)
{
  if (sig1.size() != sig2.size())
    return false;
  for (unsigned int i=0; i<sig1.size(); ++i)
    {
      if (!typeIdMatches(sig1[i], sig2[i]))
        return false;
    }
  return true;
}

// Translate ruby reserved names into something not reserved
std::string translateName(const std::string & name)
{
  if (name == "clone")
    return "cclone"; // C clone
  else
    return name;
}

std::string untranslateName(const std::string & name)
{
  if (name == "cclone")
    return "clone";
  else
    return name;
}

///////////////////////////////////////////////
// ScriptInterface implementation for Python //
///////////////////////////////////////////////
// Note about methods:
// Python C API doesn't expose the name of the current C function being called
// from Python.  In Ruby we use it to call the corresponding C++ method.  In
// Python we have to do it differently.
// Chosen method is : Don't define a class method but define a class attribute
// with the same name that return a callable object.  A class attribute can be
// given an extra void* argument.  We put the method pointer in there.  Then in
// the 'get' function (the one that returns the callable object), we store the
// method pointer in 'self' (which we extended to contain a method pointer), and
// return a callable object that calls PythonMethod.  In PythonMethod we use the
// extra info in 'self' to find out which method to call.

#ifdef SCRIPT_PYTHON
// For class methods, same problem as for normal methods : don't know the name
// of the function to call.
// Solution : make a descriptor that store the method to call in the class type.
// Some code taken from Python source
#if PY_MAJOR_VERSION == 2
void descr_dealloc(PyDescrObject *descr)
{
    _PyObject_GC_UNTRACK(descr);
    Py_XDECREF(descr->d_type);
    Py_XDECREF(descr->d_name);
    PyObject_GC_Del(descr);
}

char * descr_name(PyDescrObject *descr)
{
    if (descr->d_name != NULL && PyString_Check(descr->d_name))
        return PyString_AS_STRING(descr->d_name);
    else
        return (char*)"?";
}

PyDescrObject * descr_new(PyTypeObject *descrtype,
                          PyTypeObject *type, const char *name)
{
    PyDescrObject *descr;

    descr = (PyDescrObject *)PyType_GenericAlloc(descrtype, 0);
    if (descr != NULL) {
        Py_XINCREF(type);
        descr->d_type = type;
        descr->d_name = PyString_InternFromString(name);
        if (descr->d_name == NULL) {
            Py_DECREF(descr);
            descr = NULL;
        }
    }
    return descr;
}

PyObject * descr_repr(PyDescrObject *descr, char *format)
{
    return PyString_FromFormat(format, descr_name(descr),
                               descr->d_type->tp_name);
}

PyObject * method_repr(ScriptMethodDescrObject *descr)
{
    return descr_repr((PyDescrObject *)descr,
                      (char*)"<method '%s' of '%s' objects>");
}
#endif
#if PY_MAJOR_VERSION == 3
void descr_dealloc(PyDescrObject *descr)
{
  PyObject_GC_UnTrack(descr);
  Py_XDECREF(descr->d_type);
  Py_XDECREF(descr->d_name);
  Py_XDECREF(descr->d_qualname);
  PyObject_GC_Del(descr);
}

PyObject * descr_name(PyDescrObject *descr)
{
  if (descr->d_name != NULL && PyUnicode_Check(descr->d_name))
    return descr->d_name;
  return NULL;
}

PyDescrObject *
descr_new(PyTypeObject *descrtype, PyTypeObject *type, const char *name)
{
    PyDescrObject *descr;

    descr = (PyDescrObject *)PyType_GenericAlloc(descrtype, 0);
    if (descr != NULL) {
        Py_XINCREF(type);
        descr->d_type = type;
        descr->d_name = PyUnicode_InternFromString(name);
        if (descr->d_name == NULL) {
            Py_DECREF(descr);
            descr = NULL;
        }
        else {
            descr->d_qualname = NULL;
        }
    }
    return descr;
}

PyObject * descr_repr(PyDescrObject *descr, const char *format)
{
  PyObject *name = NULL;
  if (descr->d_name != NULL && PyUnicode_Check(descr->d_name))
    name = descr->d_name;

  return PyUnicode_FromFormat(format, name, "?", descr->d_type->tp_name);
}

PyObject * method_repr(ScriptMethodDescrObject *descr)
{
  return descr_repr((PyDescrObject *)descr,
                    "<method '%V' of '%s' objects>");
}
#endif

PyMethodDef pythonStaticMethodCaller = {
  "staticmethodcpp",
  (PyCFunction)PythonStaticMethod,
  METH_CLASS,
  "C++ static method"
};

// This gets called when the descriptor is read
// It returns a callable object, that will call the correct method based on the
// name stored in the descriptor
PyObject * classmethod_get(ScriptMethodDescrObject *descr,
                           PyObject *obj, PyObject *type)
{
    /* Ensure a valid type.  Class methods ignore obj. */
    if (type == NULL) {
        if (obj != NULL)
            type = (PyObject *)obj->ob_type;
        else {
            /* Wot - no type?! */
            PyErr_Format(PyExc_TypeError,
                         "descriptor '%s' for type '%s' "
                         "needs either an object or a type",
                         descr_name((PyDescrObject *)descr),
#if PY_MAJOR_VERSION == 2
                         descr->d_type->tp_name);
#endif
#if PY_MAJOR_VERSION == 3
                         descr->d_common.d_type->tp_name);
#endif
            return NULL;
        }
    }
    if (!PyType_Check(type)) {
        PyErr_Format(PyExc_TypeError,
                     "descriptor '%s' for type '%s' "
                     "needs a type, not a '%s' as arg 2",
                     descr_name((PyDescrObject *)descr),
#if PY_MAJOR_VERSION == 2
                         descr->d_type->tp_name,
#endif
#if PY_MAJOR_VERSION == 3
                         descr->d_common.d_type->tp_name,
#endif
                     type->ob_type->tp_name);
        return NULL;
    }
#if PY_MAJOR_VERSION == 2
    if (!PyType_IsSubtype((PyTypeObject *)type, descr->d_type)) {
#endif
#if PY_MAJOR_VERSION == 3
    if (!PyType_IsSubtype((PyTypeObject *)type, descr->d_common.d_type)) {
#endif
        PyErr_Format(PyExc_TypeError,
                     "descriptor '%s' for type '%s' "
                     "doesn't apply to type '%s'",
                     descr_name((PyDescrObject *)descr),
#if PY_MAJOR_VERSION == 2
                         descr->d_type->tp_name,
#endif
#if PY_MAJOR_VERSION == 3
                         descr->d_common.d_type->tp_name,
#endif
                     ((PyTypeObject *)type)->tp_name);
        return NULL;
    }
    // Don't actually need 'type', I need descr which contains the klass and
    // method name to call
    return PyCFunction_New(&pythonStaticMethodCaller, (PyObject*)descr);
}


//// This gets called when the descriptor is read
//// It returns a callable object, that will call the correct function based on
//// the name stored in the descriptor
//PyObject * globalfunction_get(ScriptMethodDescrObject *descr,
//                              PyObject *obj, PyObject *type)
//{
//  return PyCFunction_New(&pythonGlobalFunctionCaller, descr->c_method);
//}

int descr_traverse(PyObject *self, visitproc visit, void *arg)
{
  PyDescrObject *descr = (PyDescrObject *)self;
  Py_VISIT(descr->d_type);
  return 0;
}

PyObject * PythonClassBaseAlloc(PyTypeObject * type,
                                PyObject * args,
                                PyObject * kwds)
{
  PythonReflectionInstance * self;

  self = (PythonReflectionInstance*)type->tp_alloc(type, 0);
  if (self)
    {
      self->reference = nullptr;
    }
  return (PyObject*)self;
}

void PythonClassBaseFree(PythonReflectionInstance * self)
{
  if (self->reference) // Can be 0 on constructor failure (PythonInitialize)
    self->reference->deleteFromScript(LANGUAGE_PYTHON);
#if PY_MAJOR_VERSION == 2
  self->ob_type->tp_free((PyObject*)self);
#endif
#if PY_MAJOR_VERSION == 3
  Py_TYPE(self)->tp_free((PyObject*)self);
#endif
}

std::string pythonTypeToTypeid(PyObject * arg)
{
#if PY_MAJOR_VERSION == 2
  if (PyInt_Check(arg)) return typeid(int).name();
#endif
#if PY_MAJOR_VERSION == 3
  if (PyLong_Check(arg)) return typeid(int).name();
#endif
  if (PyBool_Check(arg)) return typeid(bool).name();
#if PY_MAJOR_VERSION == 2
  if (PyString_Check(arg)) return typeid(std::string).name();
#endif
#if PY_MAJOR_VERSION == 3
  if (PyUnicode_Check(arg)) return typeid(std::string).name();
#endif
  if (PySequence_Check(arg))
    {
      if (PySequence_Length(arg) > 0)
        {
          std::string elementType;
          PyObject * element = PySequence_GetItem(arg, 0);
          elementType = pythonTypeToTypeid(element);
          Py_DECREF(element);
          return "array " + elementType;
        }
      else
        return "empty array";
    }
  if (arg == Py_None)
    {
      throw std::runtime_error("NONE passed to C++");
    }
  if (PyType_Check(arg->ob_type))
    {
      PyTypeObject * typeArg = arg->ob_type;
      while (typeArg && typeArg != &PyBaseObject_Type)
        {
          if (auto pythonClass = isPythonClassBase(typeArg))
            return pythonClass->cppClass->getTypeIdName();
          typeArg = typeArg->tp_base;
        }
    }
  return typeid(ScriptObject).name();
}

int PythonInitialize(PythonReflectionInstance * self,
                     PyObject * args,
                     PyObject * kwds)
{
#if PY_MAJOR_VERSION == 2
  PyTypeObject * pyClass = self->ob_type;
#endif
#if PY_MAJOR_VERSION == 3
  PyTypeObject * pyClass = Py_TYPE(self);
#endif
  PythonClassBase * pyCppKlass = nullptr;
  while (pyClass != nullptr)
    {
      if ((pyCppKlass = isPythonClassBase(pyClass)) != nullptr)
        break;
      pyClass = pyClass->tp_base;
    }
  auto cppKlass = pyCppKlass->cppClass;

  auto classInfo = *cppKlass->getClassInfo();
  auto constructorArray = cppKlass->getConstructorArray();

  int argc = PySequence_Length(args);

  if (argc == -1)
    {
      PyErr_SetString(PyExc_RuntimeError, "Can't get argc");
      return -1;
    }

  if (argc > 6)
    {
      std::ostringstream message;
#if PY_MAJOR_VERSION == 2
      message << "C++ Class " << self->ob_type->tp_name
#endif
#if PY_MAJOR_VERSION == 3
      message << "C++ Class " << Py_TYPE(self)->tp_name
#endif
        << " : Too many arguments to initialize";
      PyErr_SetString(PyExc_RuntimeError, message.str().c_str());
      return -1;
    }

  // Find a constructor with the same signature
  if (constructorArray->empty())
    {
      std::ostringstream message;
#if PY_MAJOR_VERSION == 2
      message << "C++ Class " << self->ob_type->tp_name
#endif
#if PY_MAJOR_VERSION == 3
      message << "C++ Class " << Py_TYPE(self)->tp_name
#endif
        << " : No constructors available";
      PyErr_SetString(PyExc_RuntimeError, message.str().c_str());
      return -1;
    }

  bool correct;
  std::vector<ReflectionHandle> argv;
  for (int arg=0; arg<6; ++arg)
    {
      ReflectionHandle item;
      if (arg < argc)
        item.pythonHandle = PySequence_GetItem(args, arg);
      else
        item.pythonHandle = 0;
      argv.push_back(item);
    }
  for (auto constructor : *constructorArray)
    {
      if ((int)constructor->signature().size() != argc)
        continue;
      correct = true;
      // All arguments should have the correct type
      for (int arg=0; arg<argc; ++arg)
        {
          if (!typeIdMatches(pythonTypeToTypeid(argv[arg].pythonHandle),
                             constructor->signature()[arg]))
            {
              correct = false;
              break;
            }
        }
      if (correct)
        {
          void * thls;
          thls = constructor->call(LANGUAGE_PYTHON,
                                   argv[0], argv[1], argv[2], argv[3],
                                   argv[4], argv[5], argv[6]);
          auto scriptThis = reinterpret_cast<ScriptAccess*>(thls);
          auto pythonRef =
            reinterpret_cast
            <RubyPythonReference*>(classInfo.makeReference(thls));
          // Default reference constructor assumes object is stored in C++.
          // This is not the case when it is created from Python(here).
          pythonRef->deleteFromC();
          scriptThis->setReference(pythonRef);
          pythonRef->setPyObject((PyObject*)self);
          self->reference = pythonRef;
          if (auto scriptObject = classInfo.asScriptObject(thls))
            {
              scriptObject->setPyObject((PyObject*)self);
            }
          for (auto arg : argv)
            Py_XDECREF(arg.pythonHandle);
          return 0;
        }
    }
  if (!correct)
    {
      for (auto arg : argv)
        Py_XDECREF(arg.pythonHandle);
      std::ostringstream message;
#if PY_MAJOR_VERSION == 2
      message << "C++ Class " << self->ob_type->tp_name
#endif
#if PY_MAJOR_VERSION == 3
      message << "C++ Class " << Py_TYPE(self)->tp_name
#endif
        << " :\nNo constructor found with following " << argc
        << " argument types :\n";
      for (int arg=0; arg<argc; ++arg)
        {
          message << pythonTypeToTypeid(argv[arg].pythonHandle) << ", ";
        }
      message << "\nThere are " << constructorArray->size()
        << " constructors available :\n";
      for (auto constructor : *constructorArray)
        {
          if (constructor->signature().empty())
            message << "no arguments";
          else
            for (std::vector<std::string>::const_iterator
                 sig=constructor->signature().begin();
                 sig!=constructor->signature().end(); ++sig)
              {
                message << *sig << ", ";
              }
          message << "\n";
        }
      PyErr_SetString(PyExc_RuntimeError, message.str().c_str());
      // dealloc will be called -> set reference to nullptr so it won't delete
      // a random pointer
      self->reference = nullptr;
      return -1;
    }

  return -1;
}

#if PY_MAJOR_VERSION == 2
int PythonClassBaseCompare(PyObject * o1, PyObject * o2)
{
  if (!isPythonClassBase(o1->ob_type))
    {
      PyErr_SetString(PyExc_TypeError, "o1 is not a C++ class");
      return -1;
    }
  if (!isPythonClassBase(o2->ob_type))
    {
      PyErr_SetString(PyExc_TypeError, "o2 is not a C++ class");
      return -1;
    }
  // TODO : a bit too strict
  return o1 == o2;
}
#endif

PyObject * PythonGetAttr(PythonReflectionInstance * self, void * closure)
{
  auto attribute = reinterpret_cast<Reflection::AttributeBase*>(closure);
  return attribute->getter(self->reference->getCppObject()->get(),
                           LANGUAGE_PYTHON).pythonHandle;
}

// Set attribute
int PythonSetAttr(PythonReflectionInstance * self, PyObject * value,
                  void * closure)
{
  auto attribute = reinterpret_cast<Reflection::AttributeBase*>(closure);
  ReflectionHandle rValue;
  rValue.pythonHandle = value;
  attribute->setter(self->reference->getCppObject()->get(), LANGUAGE_PYTHON,
                    rValue);
  auto err = PyErr_Occurred();
  if (err)
    return -1;
  else
    return 0;
}

static PyMethodDef pythonMethodCaller = {
  "methodcpp",
  (PyCFunction)PythonMethod,
  METH_VARARGS,
  "C++ method"
};

PyObject * PythonGetMethod(PythonReflectionInstance * self, void * closure)
{
  auto pythonClosure = reinterpret_cast<PythonMethodClosure*>(closure);
  pythonClosure->self = self;
  return PyCFunction_New(&pythonMethodCaller, PyCapsule_New(pythonClosure,0,0));
}

int PythonSetMethod(PythonReflectionInstance * self, PyObject * value,
                    void * closure)
{
  PyErr_SetString(PyExc_TypeError, "Assiging to a C++ method is not allowed");
  return -1; // not allowed to assign to a method
}

std::vector<std::string> makePythonSignature(PyObject * args)
{
  std::vector<std::string> result;
  if (args == nullptr)
    return result;
  if (PyTuple_Check(args))
    {
      Py_ssize_t argc = PyTuple_Size(args);
      for (int i=0; i<argc; ++i)
        result.push_back(pythonTypeToTypeid(PyTuple_GetItem(args, i)));
    }
  else
    result.push_back(pythonTypeToTypeid(args));
  return result;
}

PyObject * PythonMethod(PyObject * selfCapsule, PyObject * args)
{
  auto * closure = reinterpret_cast<PythonMethodClosure*>
    (PyCapsule_GetPointer(selfCapsule, 0));
  if (!closure)
    return nullptr;
  auto pySig = makePythonSignature(args);
  auto method = findMethodInClass(closure->methodName, closure->klass, pySig);
  if (method == nullptr)
    {
      PyErr_SetString(PyExc_TypeError,
                      signatureMismatch(pySig,
                                        "C++ Class " +
                                        closure->klass->getName() + "\n",
                                        "method",
                                        closure->klass,
                                        closure->methodName).c_str());
      return nullptr;
    }
  ReflectionHandle pyArgs[] = { 0, 0, 0, 0, 0, 0 };
  Py_ssize_t argc = PyTuple_Size(args);
  for (unsigned int i=0; i<argc; ++i)
    {
      pyArgs[i].pythonHandle = PyTuple_GetItem(args, i);
    }
  return method->call(closure->self->reference->getCppObject()->get(),
                      LANGUAGE_PYTHON,
                      pyArgs[0], pyArgs[1], pyArgs[2],
                      pyArgs[3], pyArgs[4], pyArgs[5],
                      pyArgs[6]).
    pythonHandle;
}

PyObject * PythonStaticMethod(PyObject * self, PyObject * args)
{
  auto descr = reinterpret_cast<ScriptMethodDescrObject*>(self);
  auto pySig = makePythonSignature(args);
  auto method = findMethodInClass(*descr->methodName,
                                  descr->klass,
                                  pySig);
  if (method == nullptr)
    {
      PyErr_SetString(PyExc_TypeError,
                      signatureMismatch(pySig,
                                        "C++ Class " +
                                        descr->klass->getName() + "\n",
                                        "method",
                                        descr->klass,
                                        *descr->methodName).c_str());
      return nullptr;
    }

  ReflectionHandle pyArgs[] = { 0, 0, 0, 0, 0, 0 };
  if (args == nullptr)
    {
      // nada
    }
  else if (PyTuple_Check(args))
    {
      Py_ssize_t argsSize = PyTuple_Size(args);
      for (unsigned int i=0; i<argsSize; ++i)
        {
          pyArgs[i].pythonHandle = PyTuple_GetItem(args, i);
        }
    }
  else
    {
      pyArgs[0].pythonHandle = args;
    }
  return method->call(nullptr,
                      LANGUAGE_PYTHON,
                      pyArgs[0], pyArgs[1], pyArgs[2],
                      pyArgs[3], pyArgs[4], pyArgs[5],
                      pyArgs[6]).
    pythonHandle;
}

#endif

}

#ifdef SCRIPT_PYTHON
PythonClassBase * isPythonClassBase(PyTypeObject * arg)
{
  auto pythonClass = allPythonClasses.find(arg);
  if (pythonClass != allPythonClasses.end())
    {
      return (PythonClassBase*)*pythonClass;
    }
  return nullptr;
}
#endif

std::string niceTypename(const std::string & typeidname)
{
  typedef std::unordered_map<std::string,std::string> BuiltinTypes;
  static BuiltinTypes builtins =
    { { typeid(std::string).name(),    "string" },
      { typeid(short).name(),          "short" },
      { typeid(unsigned short).name(), "unsigned short" },
      { typeid(int).name(),            "int" },
      { typeid(unsigned int).name(),   "unsigned int" },
    };
  auto name = builtins.find(typeidname);
  if (name != builtins.end())
    return name->second;
  else if (typeidname.substr(0, 6) == "array ")
    {
      return "array " + niceTypename(typeidname.substr(6));
    }
  else
    {
      auto name =
        Reflection::Registry::instance().pointerToClassname(typeidname);
      if (name.empty())
        name = typeidname;
      name =
        Reflection::Registry::instance().typeidNameToClassname(name);
      if (name.empty())
        name = typeidname;
      return name;
    }
}
