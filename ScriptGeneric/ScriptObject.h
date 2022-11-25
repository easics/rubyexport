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

#ifndef ScriptObject_h_
#define ScriptObject_h_

#ifdef SCRIPT_PYTHON
#include <Python.h>
#endif
#ifdef SCRIPT_RUBY
#include <ruby.h>
#endif

#include <string>
#include <vector>

class ScriptObject
{
public:
  ScriptObject();
#ifdef SCRIPT_RUBY
  ScriptObject(VALUE rubyValue);
#endif
#ifdef SCRIPT_PYTHON
  ScriptObject(PyObject * pyObject);
#endif
  ScriptObject(const ScriptObject & rhs);
  ~ScriptObject();

  ScriptObject & operator=(const ScriptObject & rhs);

  std::string classname() const;

  bool hasFunction(const std::string & name, bool excludeCpp) const;

  /// If functionName is empty, assume the script object is callable
  /// Ruby : it has a .call method (e.g. a Proc object)
  ///   e.g. Making a callable object of a function
  ///        def f() ... end
  ///        def g(a,b) ... end
  ///        a = lambda { f }
  ///        b = lambda { |a,b| g(a,b) }
  /// Python : it has a __call__ method
  ///
  /// If functionName is not empty, call the specified function
  template <typename R>
  void call(const std::string & functionName,
            R & result) const;
  template <typename T1, typename R>
  void call(const std::string & functionName,
            const T1 & argument1,
            R & result) const;
  template <typename T1, typename T2, typename R>
  void call(const std::string & functionName,
            const T1 & argument1,
            const T2 & argument2,
            R & result) const;
  template <typename T1, typename T2, typename T3, typename R>
  void call(const std::string & functionName,
            const T1 & argument1,
            const T2 & argument2,
            const T3 & argument3,
            R & result) const;
  template <typename T1, typename T2, typename T3, typename T4, typename R>
  void call(const std::string & functionName,
            const T1 & argument1,
            const T2 & argument2,
            const T3 & argument3,
            const T4 & argument4,
            R & result) const;
  template <typename T1, typename T2, typename T3, typename T4, typename T5,
            typename R>
  void call(const std::string & functionName,
            const T1 & argument1,
            const T2 & argument2,
            const T3 & argument3,
            const T4 & argument4,
            const T5 & argument5,
            R & result) const;
  template <typename T1, typename T2, typename T3, typename T4, typename T5,
            typename T6, typename R>
  void call(const std::string & functionName,
            const T1 & argument1,
            const T2 & argument2,
            const T3 & argument3,
            const T4 & argument4,
            const T5 & argument5,
            const T6 & argument6,
            R & result) const;
  template <typename T1, typename T2, typename T3, typename T4, typename T5,
            typename T6, typename T7, typename R>
  void call(const std::string & functionName,
            const T1 & argument1,
            const T2 & argument2,
            const T3 & argument3,
            const T4 & argument4,
            const T5 & argument5,
            const T6 & argument6,
            const T7 & argument7,
            R & result) const;

  template <typename T>
  void setAttr(const std::string & name, const T & value);
  template <typename T>
  void getAttr(const std::string & name, T & value);

#ifdef SCRIPT_RUBY
  VALUE getRubyValue() const { return rubyValue_; }
  void setRubyValue(VALUE rubyValue);
  static std::string getRubyClassname(VALUE rubyValue);
#endif
#ifdef SCRIPT_PYTHON
  PyObject * getPyObject() const { return pyObject_; }
  void setPyObject(PyObject * pyObject);
  static std::string getPythonClassname(PyObject * pyObject);
#endif

private:
#ifdef SCRIPT_RUBY
  void call(const std::string & functionName,
            VALUE & result,
            VALUE argument1=0,
            VALUE argument2=0,
            VALUE argument3=0,
            VALUE argument4=0,
            VALUE argument5=0,
            VALUE argument6=0,
            VALUE argument7=0) const;

  VALUE rubyValue_;
#endif
#ifdef SCRIPT_PYTHON
  void call(const std::string & functionName,
            PyObject * & result,
            PyObject * argument1=0,
            PyObject * argument2=0,
            PyObject * argument3=0,
            PyObject * argument4=0,
            PyObject * argument5=0,
            PyObject * argument6=0,
            PyObject * argument7=0) const;
  PyObject * pyObject_;
#endif
  void * language_;  // LANGUAGE_RUBY or LANGUAGE_PYTHON, set in constructor
  unsigned int * referenceCount_;
};

#include "ReflectionImplement.h"

template <typename R>
void ScriptObject::call(const std::string & functionName,
                      R & result) const
{
  ReflectionHandle rubyResult;
#ifdef SCRIPT_RUBY
  if (rubyValue_)
    call(functionName, rubyResult.rubyHandle);
#endif
#ifdef SCRIPT_PYTHON
  if (pyObject_)
    call(functionName, rubyResult.pythonHandle);
#endif
  try
    {
      ReflectionWrite(rubyResult, result, language_);
    }
  catch (std::exception & e)
    {
      throw std::runtime_error("When converting return value for script "
                               "function " + classname() + "::" + functionName +
                               " :\n" + e.what());
    }
#ifdef SCRIPT_PYTHON
  if (pyObject_)
    Py_DECREF(rubyResult.pythonHandle);
#endif
}

template <typename T, typename R>
void ScriptObject::call(const std::string & functionName, const T & argument1,
                       R & result) const
{
  ReflectionHandle rubyResult;
  ReflectionHandle rubyArgument1 = ReflectionRead(argument1, language_);
#ifdef SCRIPT_RUBY
  if (rubyValue_)
    call(functionName, rubyResult.rubyHandle, rubyArgument1.rubyHandle);
#endif
#ifdef SCRIPT_PYTHON
  if (pyObject_)
    call(functionName, rubyResult.pythonHandle, rubyArgument1.pythonHandle);
#endif
  try
    {
      ReflectionWrite(rubyResult, result, language_);
    }
  catch (std::exception & e)
    {
      throw std::runtime_error("When converting return value for script "
                               "function " + classname() + "::" + functionName +
                               " :\n" + e.what());
    }
#ifdef SCRIPT_PYTHON
  if (pyObject_)
    Py_DECREF(rubyResult.pythonHandle);
#endif
}

template <typename T1, typename T2, typename R>
void ScriptObject::call(const std::string & functionName,
                      const T1 & argument1,
                      const T2 & argument2,
                      R & result) const
{
  ReflectionHandle rubyResult;
  ReflectionHandle rubyArgument1 = ReflectionRead(argument1, language_);
  ReflectionHandle rubyArgument2 = ReflectionRead(argument2, language_);
#ifdef SCRIPT_RUBY
  if (rubyValue_)
    call(functionName, rubyResult.rubyHandle, rubyArgument1.rubyHandle,
         rubyArgument2.rubyHandle);
#endif
#ifdef SCRIPT_PYTHON
  if (pyObject_)
    call(functionName, rubyResult.pythonHandle, rubyArgument1.pythonHandle,
         rubyArgument2.pythonHandle);
#endif
  try
    {
      ReflectionWrite(rubyResult, result, language_);
    }
  catch (std::exception & e)
    {
      throw std::runtime_error("When converting return value for script "
                               "function " + classname() + "::" + functionName +
                               " :\n" + e.what());
    }
#ifdef SCRIPT_PYTHON
  if (pyObject_)
    Py_DECREF(rubyResult.pythonHandle);
#endif
}

template <typename T1, typename T2, typename T3, typename R>
void ScriptObject::call(const std::string & functionName,
                      const T1 & argument1,
                      const T2 & argument2,
                      const T3 & argument3,
                      R & result) const
{
  ReflectionHandle rubyResult;
  ReflectionHandle rubyArgument1 = ReflectionRead(argument1, language_);
  ReflectionHandle rubyArgument2 = ReflectionRead(argument2, language_);
  ReflectionHandle rubyArgument3 = ReflectionRead(argument3, language_);
#ifdef SCRIPT_RUBY
  if (rubyValue_)
    call(functionName, rubyResult.rubyHandle, rubyArgument1.rubyHandle,
         rubyArgument2.rubyHandle, rubyArgument3.rubyHandle);
#endif
#ifdef SCRIPT_PYTHON
  if (pyObject_)
    call(functionName, rubyResult.pythonHandle, rubyArgument1.pythonHandle,
         rubyArgument2.pythonHandle, rubyArgument3.pythonHandle);
#endif
  ReflectionWrite(rubyResult, result, language_);
#ifdef SCRIPT_PYTHON
  if (pyObject_)
    Py_DECREF(rubyResult.pythonHandle);
#endif
}

template <typename T1, typename T2, typename T3, typename T4, typename R>
void ScriptObject::call(const std::string & functionName,
                      const T1 & argument1,
                      const T2 & argument2,
                      const T3 & argument3,
                      const T4 & argument4,
                      R & result) const
{
  ReflectionHandle rubyResult;
  ReflectionHandle rubyArgument1 = ReflectionRead(argument1, language_);
  ReflectionHandle rubyArgument2 = ReflectionRead(argument2, language_);
  ReflectionHandle rubyArgument3 = ReflectionRead(argument3, language_);
  ReflectionHandle rubyArgument4 = ReflectionRead(argument4, language_);
#ifdef SCRIPT_RUBY
  if (rubyValue_)
    call(functionName, rubyResult.rubyHandle, rubyArgument1.rubyHandle,
         rubyArgument2.rubyHandle, rubyArgument3.rubyHandle,
         rubyArgument4.rubyHandle);
#endif
#ifdef SCRIPT_PYTHON
  if (pyObject_)
    call(functionName, rubyResult.pythonHandle, rubyArgument1.pythonHandle,
         rubyArgument2.pythonHandle, rubyArgument3.pythonHandle,
         rubyArgument4.pythonHandle);
#endif
  try
    {
      ReflectionWrite(rubyResult, result, language_);
    }
  catch (std::exception & e)
    {
      throw std::runtime_error("When converting return value for script "
                               "function " + classname() + "::" + functionName +
                               " :\n" + e.what());
    }
#ifdef SCRIPT_PYTHON
  if (pyObject_)
    Py_DECREF(rubyResult.pythonHandle);
#endif
}

template <typename T1, typename T2, typename T3, typename T4, typename T5,
          typename R>
void ScriptObject::call(const std::string & functionName,
                      const T1 & argument1,
                      const T2 & argument2,
                      const T3 & argument3,
                      const T4 & argument4,
                      const T5 & argument5,
                      R & result) const
{
  ReflectionHandle rubyResult;
  ReflectionHandle rubyArgument1 = ReflectionRead(argument1, language_);
  ReflectionHandle rubyArgument2 = ReflectionRead(argument2, language_);
  ReflectionHandle rubyArgument3 = ReflectionRead(argument3, language_);
  ReflectionHandle rubyArgument4 = ReflectionRead(argument4, language_);
  ReflectionHandle rubyArgument5 = ReflectionRead(argument5, language_);
#ifdef SCRIPT_RUBY
  if (rubyValue_)
    call(functionName, rubyResult.rubyHandle, rubyArgument1.rubyHandle,
         rubyArgument2.rubyHandle, rubyArgument3.rubyHandle,
         rubyArgument4.rubyHandle, rubyArgument5.rubyHandle);
#endif
#ifdef SCRIPT_PYTHON
  if (pyObject_)
    call(functionName, rubyResult.pythonHandle, rubyArgument1.pythonHandle,
         rubyArgument2.pythonHandle, rubyArgument3.pythonHandle,
         rubyArgument4.pythonHandle, rubyArgument5.pythonHandle);
#endif
  try
    {
      ReflectionWrite(rubyResult, result, language_);
    }
  catch (std::exception & e)
    {
      throw std::runtime_error("When converting return value for script "
                               "function " + classname() + "::" + functionName +
                               " :\n" + e.what());
    }
#ifdef SCRIPT_PYTHON
  if (pyObject_)
    Py_DECREF(rubyResult.pythonHandle);
#endif
}

template <typename T1, typename T2, typename T3, typename T4, typename T5,
          typename T6, typename R>
void ScriptObject::call(const std::string & functionName,
                      const T1 & argument1,
                      const T2 & argument2,
                      const T3 & argument3,
                      const T4 & argument4,
                      const T5 & argument5,
                      const T6 & argument6,
                      R & result) const
{
  ReflectionHandle rubyResult;
  ReflectionHandle rubyArgument1 = ReflectionRead(argument1, language_);
  ReflectionHandle rubyArgument2 = ReflectionRead(argument2, language_);
  ReflectionHandle rubyArgument3 = ReflectionRead(argument3, language_);
  ReflectionHandle rubyArgument4 = ReflectionRead(argument4, language_);
  ReflectionHandle rubyArgument5 = ReflectionRead(argument5, language_);
  ReflectionHandle rubyArgument6 = ReflectionRead(argument6, language_);
#ifdef SCRIPT_RUBY
  if (rubyValue_)
    call(functionName, rubyResult.rubyHandle, rubyArgument1.rubyHandle,
         rubyArgument2.rubyHandle, rubyArgument3.rubyHandle,
         rubyArgument4.rubyHandle, rubyArgument5.rubyHandle,
         rubyArgument6.rubyHandle);
#endif
#ifdef SCRIPT_PYTHON
  if (pyObject_)
    call(functionName, rubyResult.pythonHandle, rubyArgument1.pythonHandle,
         rubyArgument2.pythonHandle, rubyArgument3.pythonHandle,
         rubyArgument4.pythonHandle, rubyArgument5.pythonHandle,
         rubyArgument6.pythonHandle);
#endif
  try
    {
      ReflectionWrite(rubyResult, result, language_);
    }
  catch (std::exception & e)
    {
      throw std::runtime_error("When converting return value for script "
                               "function " + classname() + "::" + functionName +
                               " :\n" + e.what());
    }
#ifdef SCRIPT_PYTHON
  if (pyObject_)
    Py_DECREF(rubyResult.pythonHandle);
#endif
}

template <typename T1, typename T2, typename T3, typename T4, typename T5,
          typename T6, typename T7, typename R>
void ScriptObject::call(const std::string & functionName,
                      const T1 & argument1,
                      const T2 & argument2,
                      const T3 & argument3,
                      const T4 & argument4,
                      const T5 & argument5,
                      const T6 & argument6,
                      const T7 & argument7,
                      R & result) const
{
  ReflectionHandle rubyResult;
  ReflectionHandle rubyArgument1 = ReflectionRead(argument1, language_);
  ReflectionHandle rubyArgument2 = ReflectionRead(argument2, language_);
  ReflectionHandle rubyArgument3 = ReflectionRead(argument3, language_);
  ReflectionHandle rubyArgument4 = ReflectionRead(argument4, language_);
  ReflectionHandle rubyArgument5 = ReflectionRead(argument5, language_);
  ReflectionHandle rubyArgument6 = ReflectionRead(argument6, language_);
  ReflectionHandle rubyArgument7 = ReflectionRead(argument7, language_);
#ifdef SCRIPT_RUBY
  if (rubyValue_)
    call(functionName, rubyResult.rubyHandle, rubyArgument1.rubyHandle,
         rubyArgument2.rubyHandle, rubyArgument3.rubyHandle,
         rubyArgument4.rubyHandle, rubyArgument5.rubyHandle,
         rubyArgument6.rubyHandle, rubyArgument7.rubyHandle);
#endif
#ifdef SCRIPT_PYTHON
  if (pyObject_)
    call(functionName, rubyResult.pythonHandle, rubyArgument1.pythonHandle,
         rubyArgument2.pythonHandle, rubyArgument3.pythonHandle,
         rubyArgument4.pythonHandle, rubyArgument5.pythonHandle,
         rubyArgument6.pythonHandle, rubyArgument7.pythonHandle);
#endif
  try
    {
      ReflectionWrite(rubyResult, result, language_);
    }
  catch (std::exception & e)
    {
      throw std::runtime_error("When converting return value for script "
                               "function " + classname() + "::" + functionName +
                               " :\n" + e.what());
    }
#ifdef SCRIPT_PYTHON
  if (pyObject_)
    Py_DECREF(rubyResult.pythonHandle);
#endif
}

template <typename T>
void ScriptObject::setAttr(const std::string & name, const T & value)
{
#ifdef SCRIPT_RUBY
  if (rubyValue_)
    rb_iv_set(rubyValue_, name.c_str(),
              ReflectionRead(value, LANGUAGE_RUBY).rubyHandle);
#endif
#ifdef SCRIPT_PYTHON
  if (pyObject_)
#if PY_MAJOR_VERSION == 3
    PyObject_SetAttr(pyObject_, PyUnicode_FromString(name.c_str()),
                     ReflectionRead(value, LANGUAGE_PYTHON).pythonHandle);
#endif
#if PY_MAJOR_VERSION == 2
    PyObject_SetAttr(pyObject_, PyString_FromString(name.c_str()),
                     ReflectionRead(value, LANGUAGE_PYTHON).pythonHandle);
#endif
#endif
}

template <typename T>
void ScriptObject::getAttr(const std::string & name, T & value)
{
  ReflectionHandle result;
#ifdef SCRIPT_RUBY
  if (rubyValue_)
    result.rubyHandle = rb_iv_get(rubyValue_, name.c_str());
#endif
#ifdef SCRIPT_PYTHON
  if (pyObject_)
    result.pythonHandle = PyObject_GetAttrString(pyObject_, name.c_str());
#endif
  try
    {
      ReflectionWrite(result, value, language_);
    }
  catch (std::exception & e)
    {
      throw std::runtime_error("When converting value for script attribute " +
                               classname() + "::" + name + " :\n" + e.what());
    }
}

#endif
