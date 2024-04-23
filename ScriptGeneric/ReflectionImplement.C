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
#include "ScriptObject.h"
#include <stdexcept>

ReflectionHandle ReflectionNil(void * data)
{
  ReflectionHandle result;
#ifdef SCRIPT_RUBY
  if (data == LANGUAGE_RUBY)
    result.rubyHandle = Qnil;
#endif
#ifdef SCRIPT_PYTHON
  if (data == LANGUAGE_PYTHON)
    {
      result.pythonHandle = Py_None;
      Py_INCREF(Py_None);
    }
#endif
  return result;
}

// C++ to Script conversion
ReflectionHandle ReflectionRead(char value, void * data)
{
  ReflectionHandle result;
#ifdef SCRIPT_RUBY
  if (data == LANGUAGE_RUBY)
    result.rubyHandle = INT2FIX(value);
#endif
#ifdef SCRIPT_PYTHON
  if (data == LANGUAGE_PYTHON)
#if PY_MAJOR_VERSION == 2    
    result.pythonHandle = PyInt_FromLong(value);
#endif
#if PY_MAJOR_VERSION == 3
    result.pythonHandle = PyLong_FromLong(value);
#endif
#endif
  return result;
}

ReflectionHandle ReflectionRead(short value, void * data)
{
  return ReflectionRead((int)value, data);
}

ReflectionHandle ReflectionRead(int value, void * data)
{
  ReflectionHandle result;
#ifdef SCRIPT_RUBY
  if (data == LANGUAGE_RUBY)
    result.rubyHandle = INT2NUM(value);
#endif
#ifdef SCRIPT_PYTHON
  if (data == LANGUAGE_PYTHON)
#if PY_MAJOR_VERSION == 2    
    result.pythonHandle = PyInt_FromLong(value);
#endif
#if PY_MAJOR_VERSION == 3
    result.pythonHandle = PyLong_FromLong(value);
#endif
#endif
  return result;
}

ReflectionHandle ReflectionRead(unsigned short value, void * data)
{
  return ReflectionRead((unsigned int)value, data);
}

ReflectionHandle ReflectionRead(unsigned int value, void * data)
{
  ReflectionHandle result;
#ifdef SCRIPT_RUBY
  if (data == LANGUAGE_RUBY)
    result.rubyHandle = UINT2NUM(value);
#endif
#ifdef SCRIPT_PYTHON
  if (data == LANGUAGE_PYTHON)
#if PY_MAJOR_VERSION == 2    
    result.pythonHandle = PyInt_FromSize_t(value);
#endif
#if PY_MAJOR_VERSION == 3
    result.pythonHandle = PyLong_FromSize_t(value);
#endif
#endif
  return result;
}

ReflectionHandle ReflectionRead(long unsigned int value, void * data)
{
  ReflectionHandle result;
#ifdef SCRIPT_RUBY
  if (data == LANGUAGE_RUBY)
    result.rubyHandle = ULONG2NUM(value);
#endif
#ifdef SCRIPT_PYTHON
  if (data == LANGUAGE_PYTHON)
#if PY_MAJOR_VERSION == 2
    result.pythonHandle = PyInt_FromSize_t(value);
#endif
#if PY_MAJOR_VERSION == 3
    result.pythonHandle = PyLong_FromSize_t(value);
#endif
#endif
  return result;
}

ReflectionHandle ReflectionRead(long int value, void * data)
{
  ReflectionHandle result;
#ifdef SCRIPT_RUBY
  if (data == LANGUAGE_RUBY)
    result.rubyHandle = LONG2NUM(value);
#endif
#ifdef SCRIPT_PYTHON
  if (data == LANGUAGE_PYTHON)
#if PY_MAJOR_VERSION == 2
    result.pythonHandle = PyInt_FromLong(value);
#endif
#if PY_MAJOR_VERSION == 3
    result.pythonHandle = PyLong_FromLong(value);
#endif
#endif
  return result;
}

ReflectionHandle ReflectionRead(bool value, void * data)
{
  ReflectionHandle result;
#ifdef SCRIPT_RUBY
  if (data == LANGUAGE_RUBY)
    result.rubyHandle = value?Qtrue:Qfalse;
#endif
#ifdef SCRIPT_PYTHON
  if (data == LANGUAGE_PYTHON)
    {
      result.pythonHandle = value?Py_True:Py_False;
      Py_INCREF(result.pythonHandle);
    }
#endif
  return result;
}

ReflectionHandle ReflectionRead(const std::string & value, void * data)
{
  ReflectionHandle result;
#ifdef SCRIPT_RUBY
  if (data == LANGUAGE_RUBY)
    result.rubyHandle = rb_str_new2(value.c_str());
#endif
#ifdef SCRIPT_PYTHON
  if (data == LANGUAGE_PYTHON)
#if PY_MAJOR_VERSION == 2    
    result.pythonHandle = PyString_FromString(value.c_str());
#endif
#if PY_MAJOR_VERSION == 3
    result.pythonHandle = PyUnicode_FromString(value.c_str());
#endif
#endif
  return result;
}

ReflectionHandle ReflectionRead(unsigned long long value, void * data)
{
  ReflectionHandle result;
#ifdef SCRIPT_RUBY
  if (data == LANGUAGE_RUBY)
    result.rubyHandle = ULL2NUM(value);
#endif
#ifdef SCRIPT_PYTHON
  if (data == LANGUAGE_PYTHON)
    result.pythonHandle = PyLong_FromUnsignedLongLong(value);
#endif
  return result;
}

ReflectionHandle ReflectionRead(long long int value, void * data)
{
  ReflectionHandle result;
#ifdef SCRIPT_RUBY
  if (data == LANGUAGE_RUBY)
    result.rubyHandle = LL2NUM(value);
#endif
#ifdef SCRIPT_PYTHON
  if (data == LANGUAGE_PYTHON)
    result.pythonHandle = PyLong_FromLongLong(value);
#endif
  return result;
}

// Ruby 1.8 does not define DBL2NUM
#ifndef DBL2NUM
#define DBL2NUM(dbl) rb_float_new(dbl)
#endif

ReflectionHandle ReflectionRead(double value, void * data)
{
  ReflectionHandle result;
#ifdef SCRIPT_RUBY
  if (data == LANGUAGE_RUBY)
    result.rubyHandle = DBL2NUM(value);
#endif
#ifdef SCRIPT_PYTHON
  if (data == LANGUAGE_PYTHON)
    result.pythonHandle = PyFloat_FromDouble(value);
#endif
  return result;
}

ReflectionHandle ReflectionRead(const ScriptObject & value, void * data)
{
  ReflectionHandle result;
#ifdef SCRIPT_RUBY
  if (data == LANGUAGE_RUBY)
    {
      result.rubyHandle = value.getRubyValue();
    }
#endif
#ifdef SCRIPT_PYTHON
  if (data == LANGUAGE_PYTHON)
    {
      result.pythonHandle = value.getPyObject();
    }
#endif
  return result;
}

ReflectionHandle ReflectionRead(const char * value, void * data)
{
  ReflectionHandle result;
#ifdef SCRIPT_RUBY
  if (data == LANGUAGE_RUBY)
    result.rubyHandle = rb_str_new2(value);
#endif
#ifdef SCRIPT_PYTHON
  if (data == LANGUAGE_PYTHON)
#if PY_MAJOR_VERSION == 2    
    result.pythonHandle = PyString_FromString(value);
#endif
#if PY_MAJOR_VERSION == 3
    result.pythonHandle = PyUnicode_FromString(value);
#endif
#endif
  return result;
}

// Script to C++ conversion
void ReflectionWrite(ReflectionHandle handle, char & value, void * data)
{
#ifdef SCRIPT_RUBY
  if (data == LANGUAGE_RUBY)
    value = NUM2CHR(handle.rubyHandle);
#endif
#ifdef SCRIPT_PYTHON
  if (data == LANGUAGE_PYTHON)
    {
      if (!PyNumber_Check(handle.pythonHandle))
        throw std::runtime_error("argument is not of numeric type");
#if PY_MAJOR_VERSION == 2      
      if (PyInt_Check(handle.pythonHandle))
#endif
#if PY_MAJOR_VERSION == 3
      if (PyLong_Check(handle.pythonHandle))
#endif
        {
#if PY_MAJOR_VERSION == 2          
          value = PyInt_AsLong(handle.pythonHandle);
#endif
#if PY_MAJOR_VERSION == 3
          value = PyLong_AsLong(handle.pythonHandle);
#endif
          if (PyErr_Occurred())
            throw std::runtime_error("integer conversion failed");
        }
      else if (PyLong_Check(handle.pythonHandle))
        {
          value = PyLong_AsLong(handle.pythonHandle);
          if (PyErr_Occurred())
            throw std::runtime_error("integer conversion failed");
        }
      else
        throw std::runtime_error("unknown numeric type");
    }
#endif
}

void ReflectionWrite(ReflectionHandle handle, short & value, void * data)
{
  int bigValue;
  ReflectionWrite(handle, bigValue, data);
  value = bigValue;
}

void ReflectionWrite(ReflectionHandle handle, unsigned short & value,void * data)
{
  unsigned int bigValue;
  ReflectionWrite(handle, bigValue, data);
  value = bigValue;
}

void ReflectionWrite(ReflectionHandle handle, int & value, void * data)
{
#ifdef SCRIPT_RUBY
  if (data == LANGUAGE_RUBY)
    value = NUM2INT(handle.rubyHandle);
#endif
#ifdef SCRIPT_PYTHON
  if (data == LANGUAGE_PYTHON)
    {
      if (!PyNumber_Check(handle.pythonHandle))
        throw std::runtime_error("argument is not of numeric type");
#if PY_MAJOR_VERSION == 2      
      if (PyInt_Check(handle.pythonHandle))
#endif
#if PY_MAJOR_VERSION == 3
      if (PyLong_Check(handle.pythonHandle))
#endif
        {
#if PY_MAJOR_VERSION == 2          
          value = PyInt_AsLong(handle.pythonHandle);
#endif
#if PY_MAJOR_VERSION == 3
          value = PyLong_AsLong(handle.pythonHandle);
#endif
          if (PyErr_Occurred())
            throw std::runtime_error("integer conversion failed");
        }
      else if (PyLong_Check(handle.pythonHandle))
        {
          value = PyLong_AsLong(handle.pythonHandle);
          if (PyErr_Occurred())
            throw std::runtime_error("integer conversion failed");
        }
      else
        throw std::runtime_error("unknown numeric type");
    }
#endif
}

void ReflectionWrite(ReflectionHandle handle, unsigned int & value, void * data)
{
#ifdef SCRIPT_RUBY
  if (data == LANGUAGE_RUBY)
    value = NUM2UINT(handle.rubyHandle);
#endif
#ifdef SCRIPT_PYTHON
  if (data == LANGUAGE_PYTHON)
    {
      if (!PyNumber_Check(handle.pythonHandle))
        throw std::runtime_error("argument is not of numeric type");
#if PY_MAJOR_VERSION == 2      
      if (PyInt_Check(handle.pythonHandle))
#endif
#if PY_MAJOR_VERSION == 3
      if (PyLong_Check(handle.pythonHandle))
#endif
        {
#if PY_MAJOR_VERSION == 2          
          value = PyInt_AsLong(handle.pythonHandle);
#endif
#if PY_MAJOR_VERSION == 3
          value = PyLong_AsLong(handle.pythonHandle);
#endif
          if (PyErr_Occurred())
            throw std::runtime_error("integer conversion failed");
        }
      else if (PyLong_Check(handle.pythonHandle))
        {
          value = PyLong_AsUnsignedLong(handle.pythonHandle);
          if (PyErr_Occurred())
            throw std::runtime_error("integer conversion failed");
        }
      else
        throw std::runtime_error("unknown numeric type");
    }
#endif
}

void ReflectionWrite(ReflectionHandle handle, long unsigned int & value,
                     void * data)
{
#ifdef SCRIPT_RUBY
  if (data == LANGUAGE_RUBY)
    value = NUM2ULONG(handle.rubyHandle);
#endif
#ifdef SCRIPT_PYTHON
  if (data == LANGUAGE_PYTHON)
    {
      if (!PyNumber_Check(handle.pythonHandle))
        throw std::runtime_error("argument is not of numeric type");
#if PY_MAJOR_VERSION == 2
      if (PyInt_Check(handle.pythonHandle))
        {
          value = PyInt_AsLong(handle.pythonHandle);
#endif
#if PY_MAJOR_VERSION == 3
      if (PyLong_Check(handle.pythonHandle))
        {
          value = PyLong_AsLong(handle.pythonHandle);
#endif
          if (PyErr_Occurred())
            throw std::runtime_error("integer conversion failed");
        }
      else if (PyLong_Check(handle.pythonHandle))
        {
          value = PyLong_AsUnsignedLong(handle.pythonHandle);
          if (PyErr_Occurred())
            throw std::runtime_error("integer conversion failed");
        }
      else
        throw std::runtime_error("unknown numeric type");
    }
#endif
}

void ReflectionWrite(ReflectionHandle handle, long int & value, void * data)
{
#ifdef SCRIPT_RUBY
  if (data == LANGUAGE_RUBY)
    value = NUM2LONG(handle.rubyHandle);
#endif
#ifdef SCRIPT_PYTHON
  if (data == LANGUAGE_PYTHON)
    {
      if (!PyNumber_Check(handle.pythonHandle))
        throw std::runtime_error("argument is not of numeric type");
#if PY_MAJOR_VERSION == 2
      if (PyInt_Check(handle.pythonHandle))
        {
          value = PyInt_AsLong(handle.pythonHandle);
#endif
#if PY_MAJOR_VERSION == 3
      if (PyLong_Check(handle.pythonHandle))
        {
          value = PyLong_AsLong(handle.pythonHandle);
#endif
          if (PyErr_Occurred())
            throw std::runtime_error("integer conversion failed");
        }
      else if (PyLong_Check(handle.pythonHandle))
        {
          value = PyLong_AsLong(handle.pythonHandle);
          if (PyErr_Occurred())
            throw std::runtime_error("integer conversion failed");
        }
      else
        throw std::runtime_error("unknown numeric type");
    }
#endif
}

void ReflectionWrite(ReflectionHandle handle, bool & value, void * data)
{
#ifdef SCRIPT_RUBY
  if (data == LANGUAGE_RUBY)
    {
      if (TYPE(handle.rubyHandle) != T_TRUE &&
          TYPE(handle.rubyHandle) != T_FALSE)
        {
          throw std::runtime_error("argument is not of type true or false, "
                                   "class is " +
                                   ScriptObject::
                                   getRubyClassname(handle.rubyHandle));
        }
      value = TYPE(handle.rubyHandle) == T_TRUE;
    }
#endif
#ifdef SCRIPT_PYTHON
  if (data == LANGUAGE_PYTHON)
    {
      if (!PyBool_Check(handle.pythonHandle))
        {
          throw std::runtime_error("argument is not of type boolean");
        }
      if (handle.pythonHandle == Py_False)
        value = false;
      else if (handle.pythonHandle == Py_True)
        value = true;
      else
        throw std::runtime_error("bool conversion error");
    }
#endif
}

void ReflectionWrite(ReflectionHandle handle, std::string & value, void * data)
{
#ifdef SCRIPT_RUBY
  if (data == LANGUAGE_RUBY)
    {
      if (TYPE(handle.rubyHandle) != T_STRING)
        {
          throw std::runtime_error("argument is not of type String, class is " +
                                   ScriptObject::
                                   getRubyClassname(handle.rubyHandle));
        }
      value = std::string(RSTRING_PTR(handle.rubyHandle),
                          RSTRING_LEN(handle.rubyHandle));
    }
#endif
#ifdef SCRIPT_PYTHON
  if (data == LANGUAGE_PYTHON)
    {
#if PY_MAJOR_VERSION == 2      
      if (!PyString_Check(handle.pythonHandle))
#endif
#if PY_MAJOR_VERSION == 3
      if (!PyUnicode_Check(handle.pythonHandle))
#endif
        {
          throw std::runtime_error("argument is not of type String");
        }
#if PY_MAJOR_VERSION == 2      
      value = PyString_AsString(handle.pythonHandle);
#endif
#if PY_MAJOR_VERSION == 3
      value = PyUnicode_AsUTF8(handle.pythonHandle);
#endif
    }
#endif
}

void ReflectionWrite(ReflectionHandle handle, unsigned long long & value,
                     void * data)
{
#ifdef SCRIPT_RUBY
  if (data == LANGUAGE_RUBY)
    value = NUM2ULL(handle.rubyHandle);
#endif
#ifdef SCRIPT_PYTHON
  if (data == LANGUAGE_PYTHON)
    {
      if (!PyNumber_Check(handle.pythonHandle))
        throw std::runtime_error("argument is not of numeric type");
#if PY_MAJOR_VERSION == 2
      if (PyInt_Check(handle.pythonHandle))
#endif
#if PY_MAJOR_VERSION == 3
      if (PyLong_Check(handle.pythonHandle))
#endif
        {
#if PY_MAJOR_VERSION == 2
          value = PyInt_AsUnsignedLongLong(handle.pythonHandle);
#endif
#if PY_MAJOR_VERSION == 3
          value = PyLong_AsUnsignedLongLong(handle.pythonHandle);
#endif
          if (PyErr_Occurred())
            throw std::runtime_error("integer conversion failed");
        }
      else if (PyLong_Check(handle.pythonHandle))
        {
          value = PyLong_AsUnsignedLongLong(handle.pythonHandle);
          if (PyErr_Occurred())
            throw std::runtime_error("integer conversion failed");
        }
      else
        throw std::runtime_error("unknown numeric type");
    }
#endif
}

void ReflectionWrite(ReflectionHandle handle, long long int & value,
                     void * data)
{
#ifdef SCRIPT_RUBY
  if (data == LANGUAGE_RUBY)
    value = NUM2LL(handle.rubyHandle);
#endif
#ifdef SCRIPT_PYTHON
  if (data == LANGUAGE_PYTHON)
    {
      if (!PyNumber_Check(handle.pythonHandle))
        throw std::runtime_error("argument is not of numeric type");
#if PY_MAJOR_VERSION == 2
      if (PyInt_Check(handle.pythonHandle))
#endif
#if PY_MAJOR_VERSION == 3
      if (PyLong_Check(handle.pythonHandle))
#endif
        {
#if PY_MAJOR_VERSION == 2
          value = PyInt_AsLongLong(handle.pythonHandle);
#endif
#if PY_MAJOR_VERSION == 3
          value = PyLong_AsLongLong(handle.pythonHandle);
#endif
          if (PyErr_Occurred())
            throw std::runtime_error("integer conversion failed");
        }
      else if (PyLong_Check(handle.pythonHandle))
        {
          value = PyLong_AsUnsignedLongLong(handle.pythonHandle);
          if (PyErr_Occurred())
            throw std::runtime_error("integer conversion failed");
        }
      else
        throw std::runtime_error("unknown numeric type");
    }
#endif
}

void ReflectionWrite(ReflectionHandle handle, double & value,
                     void * data)
{
#ifdef SCRIPT_RUBY
  if (data == LANGUAGE_RUBY)
    {
      if (TYPE(handle.rubyHandle) != T_FLOAT)
        {
          throw std::runtime_error("argument is not of type float, class is " +
                                   ScriptObject::
                                   getRubyClassname(handle.rubyHandle));
        }
      value = RFLOAT_VALUE(handle.rubyHandle);
    }
#endif
#ifdef SCRIPT_PYTHON
  if (data == LANGUAGE_PYTHON)
    {
      if (!PyFloat_Check(handle.pythonHandle))
        throw std::runtime_error("argument is not of type float");
      value = PyFloat_AsDouble(handle.pythonHandle);
    }
#endif
}

void ReflectionWrite(ReflectionHandle handle, ScriptObject & value,
                     void * data)
{
#ifdef SCRIPT_RUBY
  if (data == LANGUAGE_RUBY)
    {
      value = ScriptObject(handle.rubyHandle);
    }
#endif
#ifdef SCRIPT_PYTHON
  if (data == LANGUAGE_PYTHON)
    {
      value = ScriptObject(handle.pythonHandle);
    }
#endif
}
