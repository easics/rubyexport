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

#ifndef ReflectionClassInfo_h_
#define ReflectionClassInfo_h_

// Class that contains the class specific info of a reflected C++ class.

#ifdef SCRIPT_PYTHON
#include <Python.h>
#endif
#ifdef SCRIPT_RUBY
#include <ruby.h>
#endif

class ScriptObject;

class ReflectionClassInfo
{
public:
  ReflectionClassInfo() : makeReference(nullptr)
#ifdef SCRIPT_RUBY
    , rubyClass(0)
#endif
#ifdef SCRIPT_PYTHON
    , pythonClass(nullptr)
#endif
  {}
  using MakeReferenceFunction = void*(*)(void *);
  using AsScriptObjectFunction = ScriptObject*(*)(void*);
  MakeReferenceFunction makeReference;
  AsScriptObjectFunction asScriptObject;
#ifdef SCRIPT_RUBY
  VALUE rubyClass;
#endif
#ifdef SCRIPT_PYTHON
  PyTypeObject * pythonClass;
#endif
};

#endif
