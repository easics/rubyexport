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


#include "ScriptObject.h"
#ifdef SCRIPT_RUBY
#include "rb_protect_wrap.h"
#include "RubyException.h"
#endif
#ifdef SCRIPT_PYTHON
#include <frameobject.h>
#endif
#include <iostream>
#include <sstream>
#include <dlfcn.h>
#include <execinfo.h>
#include <cxxabi.h>

ScriptObject::ScriptObject()
  :
#ifdef SCRIPT_RUBY
    rubyValue_(0),
#endif
#ifdef SCRIPT_PYTHON
    pyObject_(0),
#endif
    language_(LANGUAGE_RUBY),
    referenceCount_(nullptr)
{
}

#ifdef SCRIPT_RUBY
ScriptObject::ScriptObject(VALUE rubyValue)
  : rubyValue_(rubyValue),
#ifdef SCRIPT_PYTHON
    pyObject_(nullptr),
#endif
    language_(LANGUAGE_RUBY),
    referenceCount_(new unsigned int(1))
{
  rb_gc_register_address(&rubyValue_);
}
#endif

#ifdef SCRIPT_PYTHON
ScriptObject::ScriptObject(PyObject * pyObject)
  :
#ifdef SCRIPT_RUBY
    rubyValue_(0),
#endif
    pyObject_(pyObject),
    language_(LANGUAGE_PYTHON),
    referenceCount_(new unsigned int(1))
{
  Py_INCREF(pyObject);
}
#endif

ScriptObject::ScriptObject(const ScriptObject & rhs)
{
#ifdef SCRIPT_RUBY
  rubyValue_ = rhs.rubyValue_;
#endif
#ifdef SCRIPT_PYTHON
  pyObject_ = rhs.pyObject_;
#endif
  language_ = rhs.language_;
  referenceCount_ = rhs.referenceCount_;
  if (referenceCount_)
    *referenceCount_ += 1;
}

ScriptObject::~ScriptObject()
{
  if (referenceCount_)
    if (--*referenceCount_ == 0)
      {
        delete referenceCount_;
#ifdef SCRIPT_RUBY
        if (rubyValue_)
          rb_gc_unregister_address(&rubyValue_);
#endif
#ifdef SCRIPT_PYTHON
        if (pyObject_)
          Py_DECREF(pyObject_);
#endif
      }
  referenceCount_ = 0;
}

ScriptObject & ScriptObject::operator=(const ScriptObject & rhs)
{
  if (&rhs == this)
    return *this;

  if (referenceCount_)
    if (--*referenceCount_ == 0)
      {
        delete referenceCount_;
#ifdef SCRIPT_RUBY
        if (rubyValue_)
          rb_gc_unregister_address(&rubyValue_);
#endif
#ifdef SCRIPT_PYTHON
        if (pyObject_)
          Py_DECREF(pyObject_);
#endif
      }
#ifdef SCRIPT_RUBY
  rubyValue_ = rhs.rubyValue_;
#endif
#ifdef SCRIPT_PYTHON
  pyObject_ = rhs.pyObject_;
#endif
  language_ = rhs.language_;
  referenceCount_ = rhs.referenceCount_;
  if (referenceCount_)
    *referenceCount_ += 1;
  return *this;
}

std::string ScriptObject::classname() const
{
  if (referenceCount_)
    {
#ifdef SCRIPT_RUBY
      if (rubyValue_)
        return rb_class2name(rb_obj_class(rubyValue_));
#endif
#ifdef SCRIPT_PYTHON
      if (pyObject_)
        {
          return getPythonClassname(pyObject_);
        }
#endif
    }
  return "Uninitialized ScriptObject";
}

bool ScriptObject::hasFunction(const std::string & name, bool excludeCpp) const
{
#ifdef SCRIPT_RUBY
  if (rubyValue_)
    {
      if (!excludeCpp)
        return rb_respond_to(rubyValue_, rb_intern(name.c_str()));

      VALUE noParents = Qfalse;
      VALUE rbClass = CLASS_OF(rubyValue_);
      while (rbClass != Qnil)
        {
          if (rb_iv_get(rbClass, "@c++class") != Qnil)
            return false;

          auto result = rb_class_instance_methods(1, &noParents, rbClass);
          for (unsigned int i=0; i<RARRAY_LEN(result); ++i)
            {
              if (rb_id2name(SYM2ID(RARRAY_PTR(result)[i])) == name)
                return true;
            }
          rbClass = RCLASS_SUPER(rbClass);
        }
    }
#endif
#ifdef SCRIPT_PYTHON
  if (pyObject_)
    {
      PyTypeObject * pyClass = pyObject_->ob_type;
      while (pyClass)
        {
          if (isPythonClassBase(pyClass) && excludeCpp)
            return false;
          if (PyMapping_HasKeyString(pyClass->tp_dict, (char*)name.c_str()))
            return true;
          pyClass = pyClass->tp_base;
        }
    }
#endif
  return false;
}

#ifdef SCRIPT_RUBY
void ScriptObject::setRubyValue(VALUE rubyValue)
{
  rubyValue_ = rubyValue;
  language_ = LANGUAGE_RUBY;
}
std::string ScriptObject::getRubyClassname(VALUE rubyValue) // static
{
  return rb_class2name(rb_obj_class(rubyValue));
}
#endif
#ifdef SCRIPT_PYTHON
void ScriptObject::setPyObject(PyObject * pyObject)
{
  pyObject_ = pyObject;
  language_ = LANGUAGE_PYTHON;
}

std::string ScriptObject::getPythonClassname(PyObject * pyObject) // static
{
  // One might be tempted to write
  // return pyObject_->ob_type->tp_name;
  // But that only works for 'new' style classes (python 3.1+)
  // For 'old' style classes, this returns "Instance"
  auto klass = PyObject_GetAttrString(pyObject, "__class__");
  if (!klass)
    return "Python object without __class__ attribute ?";
  auto name = PyObject_GetAttrString(klass, "__name__");
  if (!name)
    return "Python class without __name__ attribute ?";
#if PY_MAJOR_VERSION == 2  
  if (!PyString_Check(name))
#endif
#if PY_MAJOR_VERSION == 3
  if (!PyUnicode_Check(name))
#endif
    return "Python class name which is not a string ?";
#if PY_MAJOR_VERSION == 2  
  return PyString_AsString(name);
#endif
#if PY_MAJOR_VERSION == 3
  return PyUnicode_AsUTF8(name);
#endif
}
#endif

#ifdef SCRIPT_RUBY
static VALUE RubyFunctionCall(VALUE args)
{
  VALUE instance = rb_ary_shift(args);
  VALUE name = rb_ary_shift(args);
  std::string functionName = rb_string_value_cstr(&name);
  return rb_apply(instance, rb_intern(functionName.c_str()),
                  rb_ary_shift(args));
}

void ScriptObject::call(const std::string & functionName,
                      VALUE & result,
                      VALUE argument1,
                      VALUE argument2,
                      VALUE argument3,
                      VALUE argument4,
                      VALUE argument5,
                      VALUE argument6,
                      VALUE argument7) const
{
  VALUE functionArgs = rb_ary_new();
  if (argument1)
    rb_ary_push(functionArgs, argument1);
  if (argument2)
    rb_ary_push(functionArgs, argument2);
  if (argument3)
    rb_ary_push(functionArgs, argument3);
  if (argument4)
    rb_ary_push(functionArgs, argument4);
  if (argument5)
    rb_ary_push(functionArgs, argument5);
  if (argument6)
    rb_ary_push(functionArgs, argument6);
  if (argument7)
    rb_ary_push(functionArgs, argument7);
  VALUE args = rb_ary_new();
  rb_ary_push(args, rubyValue_);
  if (!functionName.empty())
    rb_ary_push(args, rb_str_new2(functionName.c_str()));
  else
    rb_ary_push(args, rb_str_new2("call"));
  rb_ary_push(args, functionArgs);
  int state = 0;
  result = rb_protect_wrap(RubyFunctionCall, args, &state);
  RubyException::checkRubyException(state);
}
#endif

#ifdef SCRIPT_PYTHON
static std::string invoke_addr2line_on_so(void * address)
{
  Dl_info info;
  char buf[1024];

  dladdr(address, &info);
  sprintf(buf, "addr2line -e %s %p\n", info.dli_fname,
          //(void *) ((char *) address - (char *)info.dli_fbase));
          (void *) ((char *) address ));
  FILE * pipe = popen(buf, "r");
  std::string result;
  if (fgets(buf, 1024, pipe) != 0)
    {
      result = buf;
    }
  pclose(pipe);
  return result;
}

void ScriptObject::call(const std::string & functionName,
                      PyObject * & result,
                      PyObject * argument1,
                      PyObject * argument2,
                      PyObject * argument3,
                      PyObject * argument4,
                      PyObject * argument5,
                      PyObject * argument6,
                      PyObject * argument7) const
{
#if PY_MAJOR_VERSION == 2
  PyObject * name = PyString_FromString(functionName.c_str());
#endif
#if PY_MAJOR_VERSION == 3
  PyObject * name = PyUnicode_FromString(functionName.c_str());
#endif
  if (functionName.empty())
    {
      unsigned int numArgs = 0;
      if (argument1) ++numArgs;
      if (argument2) ++numArgs;
      if (argument3) ++numArgs;
      if (argument4) ++numArgs;
      if (argument5) ++numArgs;
      if (argument6) ++numArgs;
      if (argument7) ++numArgs;
      PyObject * args = PyTuple_New(numArgs);
      std::vector<PyObject*> cArgs = { argument1, argument2, argument3,
                                       argument4, argument5, argument6,
                                       argument7 };
      for (unsigned int i=0; i<numArgs; ++i)
        {
          PyTuple_SET_ITEM(args, i, cArgs[i]);
          Py_INCREF(cArgs[i]);
        }
      result = PyObject_Call(pyObject_, args, nullptr);
      Py_DECREF(args);
    }
  else
    {
      result = PyObject_CallMethodObjArgs(pyObject_, name,
                                          argument1, argument2,
                                          argument3, argument4,
                                          argument5, argument6,
                                          argument7, 0);
    }
  if (!result)
    {
      //PyThreadState * tstate = PyThreadState_GET();
      //if (tstate != 0 && tstate->frame != 0)
      //  {
      //    PyFrameObject * frame = tstate->frame;
      //    std::ostringstream message;
      //    message << "Python stack trace:\n";
      //    while (frame != 0)
      //      {
      //        int line = frame->f_lineno;
      //        const char *filename = PyString_AsString(frame->f_code->co_filename);
      //        const char *funcname = PyString_AsString(frame->f_code->co_name);
      //        message << "  " << filename << "(" << line << "): " << funcname <<
      //          "\n";
      //        frame = frame->f_back;
      //      }
      //    std::cerr << message.str() << "\n";
      //  }
      if (PyErr_Occurred())
        {
          PyObject * type, * value, *traceback;
          PyErr_Fetch(&type, &value, &traceback);
          PyObject * typerepr = PyObject_Repr(type);
          PyObject * valuerepr = PyObject_Repr(value);
          PyObject * tracerepr = PyObject_Repr(traceback);
#if PY_MAJOR_VERSION == 2          
          const char * typestring = PyString_AsString(typerepr);
#endif
#if PY_MAJOR_VERSION == 3
          const char * typestring = PyUnicode_AsUTF8(typerepr);
#endif
#if PY_MAJOR_VERSION == 2          
          const char * valuestring = PyString_AsString(valuerepr);
#endif
#if PY_MAJOR_VERSION == 3
          const char * valuestring = PyUnicode_AsUTF8(valuerepr);
#endif
#if PY_MAJOR_VERSION == 2          
          const char * tracestring = PyString_AsString(tracerepr);
#endif
#if PY_MAJOR_VERSION == 3
          const char * tracestring = PyUnicode_AsUTF8(tracerepr);
#endif
          std::ostringstream message;
          message << typestring << "\n";
          message << valuestring << "\n";
          if (traceback)
            message << tracestring << "\n";

          {
            void * frames[32];
            int n = backtrace(frames, 32);
            char ** symbols = backtrace_symbols(frames, n);
            for (int i=1; i<n; ++i)
              {
                //std::string traceback = std::string(symbols[i]) + "\n";
                std::string traceback = invoke_addr2line_on_so(frames[i]);
                char * begin_name = 0;
                char * begin_offset = 0;
                char * end_offset = 0;
                for (char * p = symbols[i]; *p; ++p)
                  {
                    if (*p == '(')
                      begin_name = p;
                    else if (*p == '+')
                      begin_offset = p;
                    else if (*p == ')' && begin_offset)
                      {
                        end_offset = p;
                        break;
                      }
                  }
                if (begin_name && begin_offset && end_offset &&
                    begin_name < begin_offset)
                  {
                    *begin_name++ = 0;
                    *begin_offset++ = 0;
                    *end_offset++ = 0;
                    int status;
                    char * ret = abi::__cxa_demangle(begin_name, 0, 0, &status);
                    if (status == 0)
                      {
                        message << "function " << ret << "\n";
                      }
                    else
                      {
                        message << "function " << begin_name << "\n";
                      }
                    free(ret);
                  }
                else
                  {
                    message << symbols[i] << "\n";
                  }
                if (traceback.substr(0,2) != "??")
                  message << traceback;
              }
            free(symbols);
          }
          Py_XDECREF(typerepr);
          Py_XDECREF(valuerepr);
          Py_XDECREF(tracerepr);
          throw std::runtime_error(std::string("method call failed\n") +
                                   message.str());
        }
      else
        throw std::runtime_error("method call failed, unknown reason");
    }
  Py_DECREF(name);
}
#endif
