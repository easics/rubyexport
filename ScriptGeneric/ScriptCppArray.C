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


#include "ReflectionImplement.h"
#include "ScriptCppArray.h"
#ifdef SCRIPT_PYTHON
#include "PythonException.h"
#endif

#ifdef SCRIPT_RUBY
#include <ruby/version.h>

VALUE ScriptCppArrayBase::rubyClassType_ = 0;

namespace // anonymous
{
ScriptCppArrayBase * readPointer(VALUE self);
VALUE ScriptCppArray_push(VALUE self, VALUE arg);
VALUE ScriptCppArray_push_m(int argc, VALUE * argv, VALUE self);
VALUE ScriptCppArray_clear(VALUE self);
VALUE ScriptCppArray_concat(VALUE self, VALUE arg);
VALUE ScriptCppArray_delete(VALUE self, VALUE arg);
VALUE ScriptCppArray_delete_at(VALUE self, VALUE arg);
VALUE ScriptCppArray_delete_if(VALUE ary);
VALUE ScriptCppArray_each(VALUE self);
VALUE ScriptCppArray_length(VALUE self);
VALUE ScriptCppArray_setelement(int argc, VALUE * argv, VALUE ary);
VALUE ScriptCppArray_pop(VALUE self);
VALUE ScriptCppArray_reverse(VALUE self);
VALUE ScriptCppArray_rotate(int argc, VALUE * argv, VALUE ary);
VALUE ScriptCppArray_shift(VALUE self);
VALUE ScriptCppArray_sort(VALUE self);
VALUE ScriptCppArray_unshift(VALUE self, VALUE arg);
VALUE ScriptCppArray_unsupported(VALUE ary);
VALUE ScriptCppArray_info(VALUE self);
}
#endif

#ifdef SCRIPT_PYTHON
struct PythonScriptCppArrayInstance
{
  PyListObject list;
  ScriptCppArrayBase * cppArray;
};

namespace // anonymous
{
  PyDoc_STRVAR(append_doc,
  "L.append(object) -- append object to end");
  PyDoc_STRVAR(extend_doc,
  "L.extend(iterable) -- extend list by appending elements from the iterable");
  PyDoc_STRVAR(insert_doc,
  "L.insert(index, object) -- insert object before index");
  PyDoc_STRVAR(pop_doc,
  "L.pop([index]) -> item -- remove and return item at index (default last).\n"
  "Raises IndexError if list is empty or index is out of range.");
  PyDoc_STRVAR(remove_doc,
  "L.remove(value) -- remove first occurrence of value.\n"
  "Raises ValueError if the value is not present.");
  PyDoc_STRVAR(reverse_doc,
  "L.reverse() -- reverse *IN PLACE*");
  PyDoc_STRVAR(sort_doc,
  "L.sort(cmp=None, key=None, reverse=False) -- stable sort *IN PLACE*;\n\
  cmp(x, y) -> -1, 0, 1");

  PyObject * pylist_append(PythonScriptCppArrayInstance * self, PyObject *item);
  PyObject * pylist_extend(PythonScriptCppArrayInstance * self, PyObject *item);
  PyObject * pylist_insert(PythonScriptCppArrayInstance * self, PyObject *args);
  PyObject * pylist_pop(PythonScriptCppArrayInstance * self, PyObject * args);
  PyObject * pylist_remove(PythonScriptCppArrayInstance * self, PyObject *item);
  PyObject * pylist_reverse(PythonScriptCppArrayInstance * self);
  PyObject * pylist_sort(PythonScriptCppArrayInstance * self, PyObject * args);

  Py_ssize_t pylist_length(PythonScriptCppArrayInstance * self);
  PyObject * pylist_subscript(PythonScriptCppArrayInstance * self,
                              PyObject * item);
  int pylist_ass_subscript(PythonScriptCppArrayInstance * self,
                           PyObject * item,
                           PyObject * value);

  PyMethodDef ScriptCppArrayMethods[] = {
    {"append",      (PyCFunction)pylist_append,  METH_O, append_doc},
    {"extend",      (PyCFunction)pylist_extend,  METH_O, extend_doc},
    {"insert",      (PyCFunction)pylist_insert,  METH_VARARGS, insert_doc},
    {"pop",         (PyCFunction)pylist_pop,     METH_VARARGS, pop_doc},
    {"remove",      (PyCFunction)pylist_remove,  METH_O, remove_doc},
    {"reverse",     (PyCFunction)pylist_reverse, METH_NOARGS, reverse_doc},
    {"sort",        (PyCFunction)pylist_sort,    METH_VARARGS | METH_KEYWORDS,
      sort_doc},
    { 0, 0 },
  };
  PyMappingMethods ScriptCppArrayMappingMethods = {
    (lenfunc)pylist_length,          /* mp_length */
    (binaryfunc)pylist_subscript,
    (objobjargproc)pylist_ass_subscript,
  };
  int PythonScriptCppArrayInit(PythonScriptCppArrayInstance * self,
                               PyObject * args,
                               PyObject * kwds);
#if PY_MAJOR_VERSION == 2
  PyTypeObject ScriptCppArrayType = {
    PyObject_HEAD_INIT(NULL)
    0,                         /*ob_size*/
    "ScriptCppArray",          /*tp_name*/
    sizeof(PythonScriptCppArrayInstance), /*tp_basicsize*/
    0,                         /*tp_itemsize*/
    0,                         /*tp_dealloc*/
    0,                         /*tp_print*/
    0,                         /*tp_getattr*/
    0,                         /*tp_setattr*/
    0,                         /*tp_compare*/
    0,                         /*tp_repr*/
    0,                         /*tp_as_number*/
    0,                         /*tp_as_sequence*/
    &ScriptCppArrayMappingMethods, /*tp_as_mapping*/
    0,                         /*tp_hash */
    0,                         /*tp_call*/
    0,                         /*tp_str*/
    0,                         /*tp_getattro*/
    0,                         /*tp_setattro*/
    0,                         /*tp_as_buffer*/
    Py_TPFLAGS_DEFAULT |
      Py_TPFLAGS_BASETYPE,     /*tp_flags*/
    "ScriptCppArray objects",  /*tp_doc*/
    0,                         /* tp_traverse */
    0,                         /* tp_clear */
    0,                         /* tp_richcompare */
    0,                         /* tp_weaklistoffset */
    0,                         /* tp_iter */
    0,                         /* tp_iternext */
    ScriptCppArrayMethods,     /* tp_methods */
    0,                         /* tp_members */
    0,                         /* tp_getset */
    &PyList_Type,              /* tp_base */
    0,                         /* tp_dict */
    0,                         /* tp_descr_get */
    0,                         /* tp_descr_set */
    0,                         /* tp_dictoffset */
    (initproc)PythonScriptCppArrayInit, /* tp_init */
    0,                         /* tp_alloc */
    0,                         /* tp_new */
  };
#endif
#if PY_MAJOR_VERSION == 3
  PyTypeObject ScriptCppArrayType = {
    PyObject_HEAD_INIT(NULL)
//    0,                         /*ob_size*/
    "ScriptCppArray",          /*tp_name*/
    sizeof(PythonScriptCppArrayInstance), /*tp_basicsize*/
    0,                         /*tp_itemsize*/
    0,                         /*tp_dealloc*/
    0,                         /*tp_print*/
    0,                         /*tp_getattr*/
    0,                         /*tp_setattr*/
    0,                         /*tp_compare*/
    0,                         /*tp_repr*/
    0,                         /*tp_as_number*/
    0,                         /*tp_as_sequence*/
    &ScriptCppArrayMappingMethods, /*tp_as_mapping*/
    0,                         /*tp_hash */
    0,                         /*tp_call*/
    0,                         /*tp_str*/
    0,                         /*tp_getattro*/
    0,                         /*tp_setattro*/
    0,                         /*tp_as_buffer*/
    Py_TPFLAGS_DEFAULT |
      Py_TPFLAGS_BASETYPE,     /*tp_flags*/
    "ScriptCppArray objects",  /*tp_doc*/
    0,                         /* tp_traverse */
    0,                         /* tp_clear */
    0,                         /* tp_richcompare */
    0,                         /* tp_weaklistoffset */
    0,                         /* tp_iter */
    0,                         /* tp_iternext */
    ScriptCppArrayMethods,     /* tp_methods */
    0,                         /* tp_members */
    0,                         /* tp_getset */
    &PyList_Type,              /* tp_base */
    0,                         /* tp_dict */
    0,                         /* tp_descr_get */
    0,                         /* tp_descr_set */
    0,                         /* tp_dictoffset */
    (initproc)PythonScriptCppArrayInit, /* tp_init */
    0,                         /* tp_alloc */
    0,                         /* tp_new */
  };

#endif
};
#endif

ScriptCppArrayBase::ScriptCppArrayBase(void * language)
{
#ifdef SCRIPT_RUBY
  rubyHandle_ = 0;
  if (language == LANGUAGE_RUBY)
    {
      rubyHandle_ = rb_class_new_instance(0, 0, rubyClassType_);
      rb_iv_set(rubyHandle_, "@c++pointer", rb_uint2big((long)this));
    }
#endif
#ifdef SCRIPT_PYTHON
  pythonHandle_ = nullptr;
  if (language == LANGUAGE_PYTHON)
    {
      pythonHandle_ = (PythonScriptCppArrayInstance*)
        PyObject_CallObject((PyObject*)&ScriptCppArrayType, 0);
      pythonHandle_->cppArray = this;
    }
#endif
}

void ScriptCppArrayBase::init()
{
#ifdef SCRIPT_RUBY
  rubyClassType_ = rb_define_class("ScriptCppArray", rb_cArray);
  using RubyCallback = VALUE(*)(...);
  rb_define_method(rubyClassType_, "[]=",
                   (RubyCallback)ScriptCppArray_setelement, -1);
  rb_define_method(rubyClassType_, "<<", (RubyCallback)ScriptCppArray_push, 1);
  rb_define_method(rubyClassType_, "push", (RubyCallback)ScriptCppArray_push_m,
                   -1);
  rb_define_method(rubyClassType_, "clear",
                   (RubyCallback)ScriptCppArray_clear, 0);
  rb_define_method(rubyClassType_, "collect!",
                   (RubyCallback)ScriptCppArray_unsupported, 0);
  rb_define_method(rubyClassType_, "compact!",
                   (RubyCallback)ScriptCppArray_unsupported, 0);
  rb_define_method(rubyClassType_, "concat",
                   (RubyCallback)ScriptCppArray_concat, 1);
  rb_define_method(rubyClassType_, "delete",
                   (RubyCallback)ScriptCppArray_delete, 1);
  rb_define_method(rubyClassType_, "delete_at",
                   (RubyCallback)ScriptCppArray_delete_at, 1);
  rb_define_method(rubyClassType_, "delete_if",
                   (RubyCallback)ScriptCppArray_delete_if, 0);
  rb_define_method(rubyClassType_, "each",
                   (RubyCallback)ScriptCppArray_each, 0);
  rb_define_method(rubyClassType_, "length",
                   (RubyCallback)ScriptCppArray_length, 0);
  rb_define_method(rubyClassType_, "fill",
                   (RubyCallback)ScriptCppArray_unsupported, -1);
  rb_define_method(rubyClassType_, "flatten!",
                   (RubyCallback)ScriptCppArray_unsupported, -1);
  rb_define_method(rubyClassType_, "replace",
                   (RubyCallback)ScriptCppArray_unsupported, 1);
  rb_define_method(rubyClassType_, "insert",
                   (RubyCallback)ScriptCppArray_unsupported, -1);
  rb_define_method(rubyClassType_, "keep_if",
                   (RubyCallback)ScriptCppArray_unsupported, 0);
  rb_define_method(rubyClassType_, "map!",
                   (RubyCallback)ScriptCppArray_unsupported, 0);
  rb_define_method(rubyClassType_, "pop",
                   (RubyCallback)ScriptCppArray_pop, 0);
  rb_define_method(rubyClassType_, "reject!",
                   (RubyCallback)ScriptCppArray_unsupported, 0);
  rb_define_method(rubyClassType_, "reverse!",
                   (RubyCallback)ScriptCppArray_reverse, 0);
  rb_define_method(rubyClassType_, "rotate!",
                   (RubyCallback)ScriptCppArray_rotate, -1);
  rb_define_method(rubyClassType_, "select!",
                   (RubyCallback)ScriptCppArray_unsupported, 0);
  rb_define_method(rubyClassType_, "shift",
                   (RubyCallback)ScriptCppArray_shift, 0);
  rb_define_method(rubyClassType_, "shuffle!",
                   (RubyCallback)ScriptCppArray_unsupported, -1);
  rb_define_method(rubyClassType_, "slice!",
                   (RubyCallback)ScriptCppArray_unsupported, -1);
  rb_define_method(rubyClassType_, "sort!",
                   (RubyCallback)ScriptCppArray_sort, 0);
  rb_define_method(rubyClassType_, "sort_by!",
                   (RubyCallback)ScriptCppArray_unsupported, 0);
  rb_define_method(rubyClassType_, "uniq!",
                   (RubyCallback)ScriptCppArray_unsupported, 0);
  rb_define_method(rubyClassType_, "unshift",
                   (RubyCallback)ScriptCppArray_unshift, 1);

  rb_define_method(rubyClassType_, "info",
                   (RubyCallback)ScriptCppArray_info, 0);
#endif
#ifdef SCRIPT_PYTHON
  if (PyType_Ready(&ScriptCppArrayType) < 0)
    {
      PythonException::checkPythonException();
      throw std::runtime_error("PyType_Ready failed");
    }
#endif
}

#ifdef SCRIPT_RUBY
namespace // anonymous
{
ScriptCppArrayBase * readPointer(VALUE self)
{
  VALUE rubyPointer = rb_iv_get(self, "@c++pointer");
  if (rubyPointer == Qnil)
    throw std::runtime_error("@c++pointer member not found in ScriptCppArray");
  return reinterpret_cast<ScriptCppArrayBase*>(rb_big2ulong(rubyPointer));
}

VALUE ScriptCppArray_push(VALUE self, VALUE arg)
{
  auto cppSelf = readPointer(self);
  rb_ary_push(self, arg);
  cppSelf->ruby_push(arg);
  return self;
}

VALUE ScriptCppArray_push_m(int argc, VALUE * argv, VALUE self)
{
  auto cppSelf = readPointer(self);
  while (argc--)
    {
      rb_ary_push(self, *argv);
      cppSelf->ruby_push(*argv++);
    }
  return self;
}

VALUE ScriptCppArray_clear(VALUE self)
{
  auto cppSelf = readPointer(self);
  rb_ary_clear(self);
  cppSelf->clear();
  return self;
}

// Ruby 1.8
#ifndef OBJ_UNTRUSTED
#define OBJ_UNTRUSTED(o) OBJ_TAINTED(o)
#endif

// Some code copied from Ruby source code
static inline void
rb_ary_modify_check(VALUE ary)
{
    rb_check_frozen(ary);
#if RUBY_API_VERSION_MAJOR < 3
    if (!RB_OBJ_TAINTED(ary) && rb_safe_level() >= 4)
      rb_raise(rb_eSecurityError, "Insecure: can't modify array");
#endif
}

VALUE
rb_ary_aset(int argc, VALUE *argv, VALUE ary, long & offset)
{
  long beg, len;

  if (argc != 2) {
    rb_raise(rb_eArgError, "wrong number of arguments (%d for 2)", argc);
  }
  rb_ary_modify_check(ary);
  if (FIXNUM_P(argv[0])) {
    offset = FIX2LONG(argv[0]);
    goto fixnum;
  }
  if (rb_range_beg_len(argv[0], &beg, &len, RARRAY_LEN(ary), 1)) {
    /* check if idx is Range */
    rb_raise(rb_eArgError, "unsupported type of arguments(range i.s.o. index)");
    return argv[1];
  }

  offset = NUM2LONG(argv[0]);
fixnum:
  rb_ary_store(ary, offset, argv[1]);
  return argv[1];
}

VALUE ScriptCppArray_setelement(int argc, VALUE * argv, VALUE ary)
{
  auto cppSelf = readPointer(ary);
  long index = 0;
  auto result = rb_ary_aset(argc, argv, ary, index);
  cppSelf->ruby_setelement(index, result);

  return result;
}

VALUE ScriptCppArray_unsupported(VALUE ary __attribute__((unused)))
{
  rb_raise(rb_eArgError, "unsupported method for ScriptCppArray");
}

VALUE to_ary(VALUE ary)
{
  return rb_convert_type(ary, T_ARRAY, "Array", "to_ary");
}

VALUE ScriptCppArray_concat(VALUE self, VALUE arg)
{
  auto cppSelf = readPointer(self);
  auto result = rb_ary_concat(self, arg);
  arg = to_ary(arg);
  for (unsigned int i=0; i<RARRAY_LEN(arg); ++i)
    {
      cppSelf->ruby_push(RARRAY_PTR(arg)[i]);
    }
  return result;
}

VALUE ScriptCppArray_delete(VALUE self, VALUE arg)
{
  auto cppSelf = readPointer(self);
  auto result = rb_ary_delete(self, arg);
  if (result != Qnil)
    cppSelf->ruby_delete(arg);
  return result;
}

VALUE ScriptCppArray_delete_at(VALUE self, VALUE arg)
{
  auto cppSelf = readPointer(self);
  long pos = NUM2LONG(arg);
  auto result = rb_ary_delete_at(self, pos);
  if (result != Qnil)
    cppSelf->ruby_delete_at(pos);
  return result;
}

VALUE ScriptCppArray_delete_if(VALUE self)
{
  if (!rb_block_given_p())
    throw std::runtime_error("delete_if without a block is not supported");
  auto cppSelf = readPointer(self);
  VALUE result = Qnil;
  for (int i=(RARRAY_LEN(self) - 1); i>= 0 ;)
    {
      VALUE v = RARRAY_PTR(self)[i];
      if (RTEST(rb_yield(v)))
        {
          rb_ary_delete_at(self, i);
          cppSelf->ruby_delete_at(i);
          result = self;
        }
      i--;
    }
  return result;
}

VALUE ScriptCppArray_each(VALUE self)
{
  if (!rb_block_given_p())
    throw std::runtime_error("each without a block is not supported");
  auto cppSelf = readPointer(self);
  VALUE result = Qnil;
  for (unsigned int i=0; i<RARRAY_LEN(self); ++i)
    {
      VALUE v = RARRAY_PTR(self)[i];
      rb_yield(v);
      result = self;
      cppSelf->ruby_setelement(i, v);
      rb_ary_store(self, i, v);
    }
  return result;
}

VALUE ScriptCppArray_length(VALUE self)
{
  auto cppSelf = readPointer(self);
  if (RARRAY_LEN(self) != cppSelf->length())
    throw std::runtime_error("Ruby length and C++ length don't match");
  return INT2FIX(cppSelf->length());
}

VALUE ScriptCppArray_pop(VALUE self)
{
  auto cppSelf = readPointer(self);
  auto result = rb_ary_pop(self);
  if (result != Qnil)
    cppSelf->ruby_pop();
  return result;
}

VALUE ScriptCppArray_reverse(VALUE self)
{
  auto cppSelf = readPointer(self);
  auto result = rb_ary_reverse(self);
  cppSelf->ruby_reverse();
  return result;
}

#if RUBY_VERSION_MAJOR != 1 || RUBY_VERSION_MINOR != 8
void ary_reverse(VALUE * p1, VALUE * p2)
{
    while (p1 < p2) {
        VALUE tmp = *p1;
        *p1++ = *p2;
        *p2-- = tmp;
    }
}

inline long rotate_count(long cnt, long len)
{
    return (cnt < 0) ? (len - (~cnt % len) - 1) : (cnt % len);
}

VALUE rb_ary_rotate(VALUE ary, long cnt)
{
    rb_ary_modify(ary);

    if (cnt != 0) {
        VALUE *ptr = RARRAY_PTR(ary);
        long len = RARRAY_LEN(ary);

        if (len > 0 && (cnt = rotate_count(cnt, len)) > 0) {
            --len;
            if (cnt < len) ary_reverse(ptr + cnt, ptr + len);
            if (--cnt > 0) ary_reverse(ptr, ptr + cnt);
            if (len > 0) ary_reverse(ptr, ptr + len);
            return ary;
        }
    }

    return Qnil;
}

VALUE ScriptCppArray_rotate(int argc, VALUE * argv, VALUE self)
{
  auto cppSelf = readPointer(self);
  long n = 1;
  switch (argc)
    {
    case 1: n = NUM2LONG(argv[0]);
    case 0: break;
    default: rb_scan_args(argc, argv, "01", NULL);
    }
  rb_ary_rotate(self, n);
  cppSelf->ruby_rotate(n);
  return self;
}
#else
VALUE ScriptCppArray_rotate(int argc, VALUE * argv, VALUE self)
{
  throw std::runtime_error("array rotate not implemented for ruby 1.8");
}
#endif

VALUE ScriptCppArray_shift(VALUE self)
{
  auto cppSelf = readPointer(self);
  auto result = rb_ary_shift(self);
  if (result != Qnil)
    {
      cppSelf->ruby_shift();
    }
  return result;
}

VALUE ScriptCppArray_sort(VALUE self)
{
  auto cppSelf = readPointer(self);
  auto result = rb_ary_sort_bang(self);
  cppSelf->clear();
  for (unsigned int i=0; i<RARRAY_LEN(self); ++i)
    {
      cppSelf->ruby_push(RARRAY_PTR(self)[i]);
    }
  return result;
}

VALUE ScriptCppArray_unshift(VALUE self, VALUE arg)
{
  auto cppSelf = readPointer(self);
  auto result = rb_ary_shift(self);
  cppSelf->ruby_unshift(arg);
  return result;
}

VALUE ScriptCppArray_info(VALUE self)
{
  auto cppSelf = readPointer(self);
  return rb_str_new2(cppSelf->info().c_str());
}

}
#endif

#ifdef SCRIPT_PYTHON
namespace // anonymous
{
int PythonScriptCppArrayInit(PythonScriptCppArrayInstance * self,
                             PyObject * args,
                             PyObject * kwds)
{
  if (PyList_Type.tp_init((PyObject*)self, args, kwds) < 0)
    return -1;
  self->cppArray = nullptr;
  return 0;
}

PyObject * pylist_append(PythonScriptCppArrayInstance * self, PyObject *item)
{
  if (PyList_Append((PyObject*)self, item) == 0)
    {
      self->cppArray->python_append(item);
      Py_RETURN_NONE;
    }
  return nullptr;
}

PyObject * pylist_extend(PythonScriptCppArrayInstance * self, PyObject *item)
{
  PyObject * iterator;
  PyObject * (*iterator_next)(PyObject*);
  iterator = PyObject_GetIter(item);
  if (iterator == 0)
    return 0;
  iterator_next = iterator->ob_type->tp_iternext;
  while (true)
    {
      PyObject * item = iterator_next(iterator);
      if (item == 0)
        {
          if (PyErr_Occurred())
            {
              if (PyErr_ExceptionMatches(PyExc_StopIteration))
                PyErr_Clear();
              else
                {
                  Py_DECREF(iterator);
                  return nullptr;
                }
            }
          break;
        }
      pylist_append(self, item);
    }

  Py_DECREF(iterator);
  Py_RETURN_NONE;
}

PyObject * pylist_insert(PythonScriptCppArrayInstance * self, PyObject *args)
{
  Py_ssize_t i;
  PyObject * v;
  if (!PyArg_ParseTuple(args, "nO:insert", &i, &v))
    return nullptr;
  if (PyList_Insert((PyObject*)self, i, v) == 0)
    {
      self->cppArray->python_insert(i, v);
      Py_RETURN_NONE;
    }
  else
    {
      return nullptr;
    }
}

PyObject * pylist_pop(PythonScriptCppArrayInstance * self, PyObject * args)
{
  Py_ssize_t n = -1;
  PyObject *v;

  if (!PyArg_ParseTuple(args, "|n:pop", &n))
    return nullptr;

  if (Py_SIZE(self) == 0)
    {
      PyErr_SetString(PyExc_IndexError, "pop from empty list");
      return nullptr;
    }
  if (n < 0)
    n += Py_SIZE(self);
  if (n < 0 || n > Py_SIZE(self))
    {
      PyErr_SetString(PyExc_IndexError, "pop index out of range");
      return nullptr;
    }
  v = self->list.ob_item[n];
  Py_INCREF(v);
  if (PySequence_DelItem((PyObject*)self, n)==0)
    self->cppArray->python_pop(n);
  else
    {
      // ? Py_DECREF(v);
      return nullptr;
    }
  return v;
}

PyObject * pylist_remove(PythonScriptCppArrayInstance * self, PyObject *item)
{
  Py_ssize_t i;

  for (i = 0; i < Py_SIZE(self); ++i)
    {
      int cmp = PyObject_RichCompareBool(self->list.ob_item[i], item, Py_EQ);
      if (cmp > 0)
        {
          if (PySequence_DelItem((PyObject*)self, i)==0)
            {
              self->cppArray->python_remove(item);
              Py_RETURN_NONE;
            }
          return nullptr;
        }
      else if (cmp < 0)
        return nullptr;
    }
  PyErr_SetString(PyExc_ValueError, "list.remove(x): x not in list");
  return nullptr;
}

PyObject * pylist_reverse(PythonScriptCppArrayInstance * self)
{
  if (PyList_Reverse((PyObject*)self) == 0)
    self->cppArray->python_reverse();
  Py_RETURN_NONE;
}

PyObject * pylist_sort(PythonScriptCppArrayInstance * self, PyObject * args)
{
  if (PyList_Sort((PyObject*)self) == 0)
    {
      self->cppArray->clear();
      for (int i = 0; i < Py_SIZE(self); ++i)
        {
          self->cppArray->python_append(self->list.ob_item[i]);
        }
      Py_RETURN_NONE;
    }
  return nullptr;
}

Py_ssize_t pylist_length(PythonScriptCppArrayInstance * self)
{
  return Py_SIZE(&self->list);
}

PyObject * pylist_subscript(PythonScriptCppArrayInstance * self,
                            PyObject * item)
{
  if (PyIndex_Check(item))
    {
      Py_ssize_t i = PyNumber_AsSsize_t(item, PyExc_IndexError);
      if (i == -1 && PyErr_Occurred())
        return nullptr;
      return PyList_GetItem((PyObject*)self, i);
    }
  else
    {
      PyErr_Format(PyExc_TypeError,
                   "list index must be integer, not %.200s",
                   item->ob_type->tp_name);
      return nullptr;
    }
}

int pylist_ass_subscript(PythonScriptCppArrayInstance * self,
                         PyObject * item,
                         PyObject * value)
{
  if (PyIndex_Check(item))
    {
      Py_ssize_t i = PyNumber_AsSsize_t(item, PyExc_IndexError);
      if (i == -1 && PyErr_Occurred())
        return -1;
      if (PyList_SetItem((PyObject*)self, i, value)==0)
        self->cppArray->python_setelement(i, value);
      else
        return -1;
      return 0;
    }
  else
    {
      PyErr_Format(PyExc_TypeError,
                   "list index must be integer, not %.200s",
                   item->ob_type->tp_name);
      return -1;
    }
}

}
#endif
