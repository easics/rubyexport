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

#ifndef ReflectionImplement_h_
#define ReflectionImplement_h_

#include "RubyPythonReference.h"
#include "ReflectionUtil.h"
#include "ReflectionClassBase.h"
#include "ScriptAccess.h"
#include "ConcretePointer.h"
#include "ReflectionClassInfo.h"
#include "ScriptLanguage.h"
#include "ScriptReferenceFactory.h"
#ifdef SCRIPT_PYTHON
#include "PythonException.h"
#endif
#include <type_traits>
#include <algorithm>
#include <deque>
#include <map>

#if !defined(SCRIPT_RUBY) && !defined(SCRIPT_PYTHON)
#error "Define SCRIPT_RUBY or SCRIPT_PYTHON or both"
#endif

union ReflectionHandle
{
#ifdef SCRIPT_RUBY
  VALUE rubyHandle;
#endif
#ifdef SCRIPT_PYTHON
  PyObject * pythonHandle;
#endif
};

class ScriptObject;

// Implemented in ScriptInterface.C
std::string niceTypename(const std::string & typeidname);

////////////////////////////
// Functions to implement //
////////////////////////////
// C++ to script conversion
ReflectionHandle ReflectionRead(char value, void * data);
ReflectionHandle ReflectionRead(short value, void * data);
ReflectionHandle ReflectionRead(unsigned short value, void * data);
ReflectionHandle ReflectionRead(int value, void * data);
ReflectionHandle ReflectionRead(unsigned int value, void * data);
ReflectionHandle ReflectionRead(long int value, void * data);
ReflectionHandle ReflectionRead(long unsigned int value, void * data);
ReflectionHandle ReflectionRead(bool value, void * data);
ReflectionHandle ReflectionRead(const std::string & value, void * data);
ReflectionHandle ReflectionRead(unsigned long long value, void * data);
ReflectionHandle ReflectionRead(long long int value, void * data);
ReflectionHandle ReflectionRead(double value, void * data);
ReflectionHandle ReflectionRead(const ScriptObject & value, void * data);
ReflectionHandle ReflectionRead(const char * value, void * data);
// Script to C++ conversion
void ReflectionWrite(ReflectionHandle handle, char & value, void * data);
void ReflectionWrite(ReflectionHandle handle, short & value, void * data);
void ReflectionWrite(ReflectionHandle handle, unsigned short & value, void * data);
void ReflectionWrite(ReflectionHandle handle, int & value, void * data);
void ReflectionWrite(ReflectionHandle handle, unsigned int & value,
                     void * data);
void ReflectionWrite(ReflectionHandle handle, long unsigned int & value,
                     void * data);
void ReflectionWrite(ReflectionHandle handle, long int & value, void * data);
void ReflectionWrite(ReflectionHandle handle, bool & value, void * data);
void ReflectionWrite(ReflectionHandle handle, std::string & value, void * data);
void ReflectionWrite(ReflectionHandle handle, unsigned long long & value,
                     void * data);
void ReflectionWrite(ReflectionHandle handle, long long int & value,
                     void * data);
void ReflectionWrite(ReflectionHandle handle, double & value,
                     void * data);
void ReflectionWrite(ReflectionHandle handle, ScriptObject & value,
                     void * data);
// vectors of something convertable
template <typename T>
ReflectionHandle ReflectionRead(std::vector<T> & value, void * data);
template <typename T>
ReflectionHandle ReflectionRead(const std::vector<T> & value, void * data);
template <typename T>
void ReflectionWrite(ReflectionHandle handle, std::vector<T> & value,
                     void * data);

// deque
template <typename T>
ReflectionHandle ReflectionRead(std::deque<T> & value, void * data);
template <typename T>
ReflectionHandle ReflectionRead(const std::deque<T> & value, void * data);
template <typename T>
void ReflectionWrite(ReflectionHandle handle, std::deque<T> & value,
                     void * data);

// hash of something convertable
template <typename Key, typename Value>
ReflectionHandle ReflectionRead(std::unordered_map<Key, Value> & value,
                                void * data);
template <typename Key, typename Value>
ReflectionHandle ReflectionRead(std::unordered_map<Key, Value> const & value,
                                void * data);
template <typename Key, typename Value>
void ReflectionWrite(ReflectionHandle handle,
                     std::unordered_map<Key, Value> & value,
                     void * data);

// map
template <typename Key, typename Value>
ReflectionHandle ReflectionRead(std::map<Key, Value> & value, void * data);
template <typename Key, typename Value>
ReflectionHandle ReflectionRead(std::map<Key, Value> const & value,
                                void * data);
template <typename Key, typename Value>
void ReflectionWrite(ReflectionHandle handle,
                     std::map<Key, Value> & value,
                     void * data);

// Update a script object with a new value
void ReflectionUpdate(ReflectionHandle handle, char value, void * data);
void ReflectionUpdate(ReflectionHandle handle, short value, void * data);
void ReflectionUpdate(ReflectionHandle handle, unsigned short value, void * data);
void ReflectionUpdate(ReflectionHandle handle, int value, void * data);
void ReflectionUpdate(ReflectionHandle handle, unsigned int value, void * data);
void ReflectionUpdate(ReflectionHandle handle, long int value, void * data);
void ReflectionUpdate(ReflectionHandle handle, bool value, void * data);
void ReflectionUpdate(ReflectionHandle handle, const std::string & value, void * data);
void ReflectionUpdate(ReflectionHandle handle, unsigned long long value, void * data);
void ReflectionUpdate(ReflectionHandle handle, long long int value, void * data);
void ReflectionUpdate(ReflectionHandle handle, double value, void * data);
void ReflectionUpdate(ReflectionHandle handle, const ScriptObject & value, void * data);
void ReflectionUpdate(ReflectionHandle handle, const char * value, void * data);
template <typename T>
void ReflectionUpdate(ReflectionHandle handle, std::vector<T> & value,
                      void * data);
template <typename T>
void ReflectionUpdate(ReflectionHandle handle __attribute__((unused)),
                      T & self __attribute__((unused)),
                      void * data __attribute__((unused)))
{
  throw std::runtime_error("Reference argument only supported for vector\n"
                           "This is a " + niceTypename(typeid(T).name()));
}


// Return nil, none, zero, nullptr whatever you call nothing
ReflectionHandle ReflectionNil(void * data);

// static type checking for exported classes
template <typename T>
void ReflectionCheckType();
// Add extra info to a reflection class representation
template<typename T>
ReflectionClassInfo * ReflectionMakeClassInfo();

#include "ScriptObject.h"
#include "ScriptCppArray.h"

//////////////////
// Common stuff //
//////////////////
Reflection::ClassBase * Reflection_Registry_getClass(const std::string & name);

inline void ReflectionWrite(ReflectionHandle handle __attribute__((unused)),
                            Reflection::NoClass & value __attribute__((unused)),
                            void * data __attribute__((unused))) {}
// Static type check that can be done on a reflected class
template <typename T>
void ReflectionCheckType()
{
  static_assert(std::is_base_of<ScriptAccess, T>::value,
                "Script-exported class should be derived from ScriptAccess");
}

namespace // anonymous
{

template <typename T>
void * ReflectionMakeReference(void * self)
{
  if (self == nullptr)
    throw std::runtime_error("self is 0");
  return new RubyPythonReference(new ConcretePointer<T>((T*)self, false));
}

template<typename T, bool isDerivedFromScriptObject>
struct AsScriptObjectHelper
{
  static ScriptObject * as(void * self __attribute__((unused))) { return 0; }
};

template<typename T>
struct AsScriptObjectHelper<T, true>
{
  static ScriptObject * as(void * self)
    { return dynamic_cast<ScriptObject*>((T*)self); }
};

template<typename T>
ScriptObject * asScriptObject(void * self)
{
  return
    AsScriptObjectHelper<T,std::is_base_of<ScriptObject,T>::value>::as(self);
}

}

// Extra info to be stored in a reflected class representation
template<typename T>
ReflectionClassInfo * ReflectionMakeClassInfo()
{
  auto info = new ReflectionClassInfo;
  info->makeReference = ReflectionMakeReference<T>;
  info->asScriptObject = asScriptObject<T>;
  return info;
}

///////////////////////////
// Python specific stuff //
///////////////////////////
#ifdef SCRIPT_PYTHON
// The struct that will be used for each instance
struct PythonReflectionInstance
{
  PyObject_HEAD
  RubyPythonReference * reference;
};

// The struct that will be used for each class
struct PythonClassBase
{
  PyTypeObject pyClass;
  Reflection::ClassBase * cppClass;
};

// Implemented in ScriptInterface.C
PythonClassBase * isPythonClassBase(PyTypeObject * arg);
#endif

//////////////////////////////////////////////////////
// Implementation of reflection for Ruby and Python //
//////////////////////////////////////////////////////
// vectors of something convertable
template <typename T>
ReflectionHandle ReflectionRead(std::vector<T> & value, void * data)
{
  ReflectionHandle result;
#ifdef SCRIPT_RUBY
  if (data == LANGUAGE_RUBY)
    {
      result.rubyHandle = (new ScriptCppArray<T>(&value, data))->rubyHandle_;
      for (auto element : value)
        {
          rb_ary_push(result.rubyHandle,
                      ReflectionRead(element, LANGUAGE_RUBY).rubyHandle);
        }
    }
#endif
#ifdef SCRIPT_PYTHON
  if (data == LANGUAGE_PYTHON)
    {
      result.pythonHandle =
        (PyObject*)(new ScriptCppArray<T>(&value, data))->pythonHandle_;
      for (auto element : value)
        {
          if (PyList_Append(result.pythonHandle,
                            ReflectionRead(element, data).pythonHandle) < 0)
            PythonException::checkPythonException();
        }
    }
#endif
  return result;
}

template <typename T>
ReflectionHandle ReflectionRead(const std::vector<T> & value, void * data)
{
  ReflectionHandle result;
#ifdef SCRIPT_RUBY
  if (data == LANGUAGE_RUBY)
    {
      result.rubyHandle = (new ScriptCppArray<T>(nullptr, data))->rubyHandle_;
      for (auto element : value)
        {
          rb_ary_push(result.rubyHandle,
                      ReflectionRead(element, LANGUAGE_RUBY).rubyHandle);
        }
    }
#endif
#ifdef SCRIPT_PYTHON
  if (data == LANGUAGE_PYTHON)
    {
      result.pythonHandle =
        (PyObject*)(new ScriptCppArray<T>(nullptr, data))->pythonHandle_;
      for (auto element : value)
        {
          if (PyList_Append(result.pythonHandle,
                            ReflectionRead(element, data).pythonHandle) < 0)
            PythonException::checkPythonException();
        }
    }
#endif
  return result;
}

template <typename T>
void ReflectionUpdate(ReflectionHandle handle, std::vector<T> & value,
                      void * data)
{
#ifdef SCRIPT_RUBY
  // TODO check script type of handle is array
  // TODO maybe also check that it is a ScriptCppArray ?
  if (data == LANGUAGE_RUBY)
    {
      rb_ary_clear(handle.rubyHandle);
      for (auto element : value)
        {
          rb_ary_push(handle.rubyHandle,
                      ReflectionRead(element, LANGUAGE_RUBY).rubyHandle);
        }
    }
#endif
}

template <typename T>
void ReflectionWrite(ReflectionHandle handle, std::vector<T> & value,
                     void * data)
{
#ifdef SCRIPT_RUBY
  if (data == LANGUAGE_RUBY)
    {
      if (TYPE(handle.rubyHandle) != T_ARRAY)
        {
          throw std::runtime_error("expected type array, got type " +
                                   ScriptObject::
                                   getRubyClassname(handle.rubyHandle));
        }
      value.clear();
      for (int item=0; item<RARRAY_LEN(handle.rubyHandle); ++item)
        {
          T element;
          ReflectionHandle rubyElement;
          rubyElement.rubyHandle = RARRAY_PTR(handle.rubyHandle)[item];
          try
            {
              ReflectionWrite(rubyElement, element, data);
            }
          catch (std::exception & e)
            {
              char number[8];
              snprintf(number, 8, "%d", item);
              throw std::runtime_error("When converting element " +
                                       std::string(number) + " to a C++ "
                                       "array of " +
                                       niceTypename(typeid(T).name()) + "\n" +
                                       e.what());
            }
          value.push_back(element);
        }
    }
#endif
#ifdef SCRIPT_PYTHON
  if (data == LANGUAGE_PYTHON)
    {
      if (!PySequence_Check(handle.pythonHandle))
        {
          throw std::runtime_error("argument is not a sequence");
        }
      value.clear();
      unsigned int length = PySequence_Length(handle.pythonHandle);
      for (unsigned int item=0; item<length; ++item)
        {
          T element;
          ReflectionHandle pyItem;
          pyItem.pythonHandle = PySequence_GetItem(handle.pythonHandle, item);
          if (pyItem.pythonHandle == nullptr)
            {
              throw std::runtime_error("Can't get sequence item");
            }
          try
            {
              ReflectionWrite(pyItem, element, data);
            }
          catch (std::exception & e)
            {
              throw std::runtime_error("When converting elements to a C++ "
                                       "array of " +
                                       niceTypename(typeid(T).name()) + "\n" +
                                       e.what());
            }
          Py_DECREF(pyItem.pythonHandle);
          value.push_back(element);
        }
    }
#endif
}

// Deques
template <typename T>
ReflectionHandle ReflectionRead(std::deque<T> & value, void * data)
{
  ReflectionHandle result;
#ifdef SCRIPT_RUBY
  if (data == LANGUAGE_RUBY)
    {
      result.rubyHandle = (new ScriptCppArray<T>(&value, data))->rubyHandle_;
      for (auto element : value)
        {
          rb_ary_push(result.rubyHandle,
                      ReflectionRead(element, LANGUAGE_RUBY).rubyHandle);
        }
    }
#endif
#ifdef SCRIPT_PYTHON
  if (data == LANGUAGE_PYTHON)
    {
      result.pythonHandle =
        (PyObject*)(new ScriptCppArray<T>(&value, data))->pythonHandle_;
      for (auto element : value)
        {
          if (PyList_Append(result.pythonHandle,
                            ReflectionRead(element, data).pythonHandle) < 0)
            PythonException::checkPythonException();
        }
    }
#endif
  return result;
}

template <typename T>
ReflectionHandle ReflectionRead(const std::deque<T> & value, void * data)
{
  ReflectionHandle result;
#ifdef SCRIPT_RUBY
  if (data == LANGUAGE_RUBY)
    {
      result.rubyHandle = (new ScriptCppArray<T>(nullptr, data))->rubyHandle_;
      for (auto element : value)
        {
          rb_ary_push(result.rubyHandle,
                      ReflectionRead(element, LANGUAGE_RUBY).rubyHandle);
        }
    }
#endif
#ifdef SCRIPT_PYTHON
  if (data == LANGUAGE_PYTHON)
    {
      result.pythonHandle =
        (PyObject*)(new ScriptCppArray<T>(nullptr, data))->pythonHandle_;
      for (auto element : value)
        {
          if (PyList_Append(result.pythonHandle,
                            ReflectionRead(element, data).pythonHandle) < 0)
            PythonException::checkPythonException();
        }
    }
#endif
  return result;
}

template <typename T>
void ReflectionWrite(ReflectionHandle handle, std::deque<T> & value,
                     void * data)
{
#ifdef SCRIPT_RUBY
  if (data == LANGUAGE_RUBY)
    {
      if (TYPE(handle.rubyHandle) != T_ARRAY)
        {
          throw std::runtime_error("expected type array, got type " +
                                   ScriptObject::
                                   getRubyClassname(handle.rubyHandle));
        }
      value.clear();
      for (int item=0; item<RARRAY_LEN(handle.rubyHandle); ++item)
        {
          T element;
          ReflectionHandle rubyElement;
          rubyElement.rubyHandle = RARRAY_PTR(handle.rubyHandle)[item];
          try
            {
              ReflectionWrite(rubyElement, element, data);
            }
          catch (std::exception & e)
            {
              char number[8];
              snprintf(number, 8, "%d", item);
              throw std::runtime_error("When converting element " +
                                       std::string(number) + " to a C++ "
                                       "array of " +
                                       niceTypename(typeid(T).name()) + "\n" +
                                       e.what());
            }
          value.push_back(element);
        }
    }
#endif
#ifdef SCRIPT_PYTHON
  if (data == LANGUAGE_PYTHON)
    {
      if (!PySequence_Check(handle.pythonHandle))
        {
          throw std::runtime_error("argument is not a sequence");
        }
      value.clear();
      unsigned int length = PySequence_Length(handle.pythonHandle);
      for (unsigned int item=0; item<length; ++item)
        {
          T element;
          ReflectionHandle pyItem;
          pyItem.pythonHandle = PySequence_GetItem(handle.pythonHandle, item);
          if (pyItem.pythonHandle == nullptr)
            {
              throw std::runtime_error("Can't get sequence item");
            }
          try
            {
              ReflectionWrite(pyItem, element, data);
            }
          catch (std::exception & e)
            {
              throw std::runtime_error("When converting elements to a C++ "
                                       "array of " +
                                       niceTypename(typeid(T).name()) + "\n" +
                                       e.what());
            }
          Py_DECREF(pyItem.pythonHandle);
          value.push_back(element);
        }
    }
#endif
}

// Hashes
template <typename Key, typename Value>
ReflectionHandle ReflectionRead(std::unordered_map<Key, Value> & value,
                                void * data)
{
  ReflectionHandle result;
#ifdef SCRIPT_RUBY
  if (data == LANGUAGE_RUBY)
    {
      result.rubyHandle = rb_hash_new();
      for (auto element : value)
        {
          rb_hash_aset(result.rubyHandle,
                       ReflectionRead(element.first, LANGUAGE_RUBY).rubyHandle,
                       ReflectionRead(element.second, LANGUAGE_RUBY).rubyHandle);
        }
    }
#endif
#ifdef SCRIPT_PYTHON
  throw std::runtime_error("Not yet implemented");
#endif
  return result;
}

template <typename Key, typename Value>
ReflectionHandle ReflectionRead(std::unordered_map<Key, Value> const & value,
                                void * data)
{
  ReflectionHandle result;
#ifdef SCRIPT_RUBY
  if (data == LANGUAGE_RUBY)
    {
      result.rubyHandle = rb_hash_new();
      for (auto element : value)
        {
          rb_hash_aset(result.rubyHandle,
                       ReflectionRead(element.first, LANGUAGE_RUBY).rubyHandle,
                       ReflectionRead(element.second, LANGUAGE_RUBY).rubyHandle);
        }
    }
#endif
#ifdef SCRIPT_PYTHON
  throw std::runtime_error("Not yet implemented");
#endif
  return result;
}

template <typename Key, typename Value>
void ReflectionWrite(ReflectionHandle handle,
                     std::unordered_map<Key, Value> & value,
                     void * data)
{
  // rb_hash_foreach
  throw std::runtime_error("Not yet implemented");
}

// Map
template <typename Key, typename Value>
ReflectionHandle ReflectionRead(std::map<Key, Value> & value, void * data)
{
  ReflectionHandle result;
#ifdef SCRIPT_RUBY
  if (data == LANGUAGE_RUBY)
    {
      result.rubyHandle = rb_hash_new();
      for (auto element : value)
        {
          rb_hash_aset(result.rubyHandle,
                       ReflectionRead(element.first, LANGUAGE_RUBY).rubyHandle,
                       ReflectionRead(element.second,
                                      LANGUAGE_RUBY).rubyHandle);
        }
    }
#endif
#ifdef SCRIPT_PYTHON
  throw std::runtime_error("Not yet implemented");
#endif
  return result;
}

template <typename Key, typename Value>
ReflectionHandle ReflectionRead(std::map<Key, Value> const & value,
                                void * data)
{
  ReflectionHandle result;
#ifdef SCRIPT_RUBY
  if (data == LANGUAGE_RUBY)
    {
      result.rubyHandle = rb_hash_new();
      for (auto element : value)
        {
          rb_hash_aset(result.rubyHandle,
                       ReflectionRead(element.first, LANGUAGE_RUBY).rubyHandle,
                       ReflectionRead(element.second,
                                      LANGUAGE_RUBY).rubyHandle);
        }
    }
#endif
#ifdef SCRIPT_PYTHON
  throw std::runtime_error("Not yet implemented");
#endif
  return result;
}

template <typename Key, typename Value>
void ReflectionWrite(ReflectionHandle handle,
                     std::map<Key, Value> & value,
                     void * data)
{
  throw std::runtime_error("Not yet implemented");
}

// Exported classes conversion
template <typename T>
ReflectionHandle ReflectionRead(T * self, void * data)
{
  if (self == nullptr)
    return ReflectionNil(data);
  ReflectionCheckType<T>();
  ScriptAccess * selfBase = self;
  ScriptReference * selfRef = selfBase->getReference();
  if (!selfRef)
    {
      selfRef = ScriptReferenceFactory::instance().make<T>(self, false);
      selfBase->setReference(selfRef);
    }
  RubyPythonReference * ref =
   static_cast<RubyPythonReference*>(selfRef);
  ReflectionHandle result;
#ifdef SCRIPT_RUBY
  if (data == LANGUAGE_RUBY)
    {
      VALUE instance = ref->getRubyObject();
      if (instance == Qnil)
        {
          // Not yet exported to Ruby
          std::string typeidName = typeid(*self).name();
          Reflection::ClassBase * klass =
            Reflection_Registry_getClass(typeidName);
          if (!klass)
            throw std::runtime_error("C++ class  " + niceTypename(typeidName) +
                                     " is not a script-exported class");

          instance = rb_obj_alloc(klass->getClassInfo()->rubyClass);
          DATA_PTR(instance) = ref;
          ref->setRubyObject(instance);
        }
      // else reuse previously created instance
      result.rubyHandle = instance;
    }
#endif
#ifdef SCRIPT_PYTHON
  if (data == LANGUAGE_PYTHON)
    {
      PyObject * instance = ref->getPyObject();
      if (instance == nullptr)
        {
          // Not yet exported to Python
          std::string typeidName = typeid(*self).name();
          Reflection::ClassBase * klass =
            Reflection_Registry_getClass(typeidName);
          if (!klass)
            throw std::runtime_error("C++ class  " + niceTypename(typeidName) +
                                     " is not a script-exported class");

          PythonReflectionInstance * pyInstance =
            PyObject_New(PythonReflectionInstance,
                         klass->getClassInfo()->pythonClass);
          pyInstance->reference = ref;
          instance = (PyObject*)pyInstance;
          ref->setPyObject(instance);
          Py_INCREF(instance); // for C++ version
        }
      else
        {
          // else reuse previously created instance
          Py_INCREF(instance);
        }
      result.pythonHandle = instance;
    }
#endif
  return result;
}

template <typename T>
ReflectionHandle ReflectionRead(const T & self, void * data)
{
  // self is possibly a temporary, need to make a copy
  T * selfCopy = new T(self);

  ReflectionHandle result = ReflectionRead(selfCopy, data);
  selfCopy->deleteFromC(); // So the scripting language can delete it
  return result;
}

template <typename T>
void ReflectionUpdate(ReflectionHandle handle __attribute__((unused)),
                      T * self __attribute__((unused)),
                      void * data __attribute__((unused)))
{
  throw std::runtime_error("Not yet implemented");
}

template <typename T>
void ReflectionWrite(ReflectionHandle handle, T * & value, void * data)
{
#ifdef SCRIPT_RUBY
  if (data == LANGUAGE_RUBY)
    {
      if (TYPE(handle.rubyHandle) != T_DATA)
        {
          value = nullptr;
          if (handle.rubyHandle == Qnil)
            { // allow ruby nil (null pointer)
              return;
            }
          // Not a data wrapped object
          throw std::runtime_error("Conversion from script to C++ failed\n"
                                   "Expected type " +
                                   niceTypename(typeid(T).name()) + "\n"
                                   "Got " +
                                   ScriptObject::
                                   getRubyClassname(handle.rubyHandle));
        }
      VALUE rbParent = CLASS_OF(handle.rubyHandle);
      VALUE cppKlass;
      while (rbParent != Qnil)
        {
          cppKlass = rb_iv_get(rbParent, "@c++class");
          if (cppKlass != Qnil)
            {
              break;
            }
          rbParent = RCLASS_SUPER(rbParent);
        }
      if (cppKlass == Qnil)
        {
          // Not one of ours
          value = 0;
          throw std::runtime_error("Conversion from script to C++ failed\n"
                                   "Got a C++ wrapped class NOT created by me");
        }

      RubyPythonReference * reference;
      Data_Get_Struct(handle.rubyHandle, RubyPythonReference, reference);
      if (!reference)
        {
          // Impossible ?
          value = 0;
          throw std::runtime_error("Conversion from script to C++ failed\n"
                                   "The impossible has happened");
        }
      value = reinterpret_cast<T*>(reference->getCppObject()->get());
      reference->useInC();
    }
#endif
#ifdef SCRIPT_PYTHON
  if (data == LANGUAGE_PYTHON)
    {
      PyTypeObject * pyParent = handle.pythonHandle->ob_type;
      PythonClassBase * pythonClass = nullptr;
      while (pyParent != nullptr)
        {
          pythonClass = isPythonClassBase(pyParent);
          if (pythonClass != nullptr)
            {
              break;
            }
          pyParent = pyParent->tp_base;
        }
      if (pythonClass == nullptr)
        {
          // Not a C++ object
          value = nullptr;
          throw std::runtime_error("Conversion from script to C++ failed\n"
                                   "Expected type " +
                                   niceTypename(typeid(T).name()) + "\n"
                                   "Got " +
                                   ScriptObject::
                                   getPythonClassname(handle.pythonHandle));
        }
      PythonReflectionInstance * instance =
        (PythonReflectionInstance*)handle.pythonHandle;
      value = reinterpret_cast<T*>(instance->reference->getCppObject()->get());
      instance->reference->useInC();
    }
#endif
}

template <typename T>
void ReflectionWrite(ReflectionHandle handle, T & value, void * data)
{
  T * valuePtr = nullptr;
  ReflectionWrite(handle, valuePtr, data);
  if (valuePtr == nullptr)
    throw std::runtime_error("Conversion from script to C++ failed\n"
                             "Got nil from script for a reference type in C++");
  value = *valuePtr;
}

#endif
