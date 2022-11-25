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

#ifndef RubyPythonReference_h_
#define RubyPythonReference_h_

#ifdef SCRIPT_PYTHON
#include <Python.h>
#endif
#include "ScriptReference.h"
#ifdef SCRIPT_RUBY
#include <ruby.h>
#endif

class RubyPythonReference : public ScriptReference
{
public:
  RubyPythonReference(AbstractPointer * cppObject);

  virtual void useInC() override;

  virtual void deleteFromC() override;

  virtual void deleteFromScript(void * data) override;

#ifdef SCRIPT_RUBY
  VALUE getRubyObject() const { return rubyObject_; }
  void setRubyObject(VALUE rubyObject) { rubyObject_ = rubyObject; }
#endif
#ifdef SCRIPT_PYTHON
  PyObject * getPyObject() const { return pyObject_; }
  void setPyObject(PyObject * pyObject) { pyObject_ = pyObject; }
#endif

private:
  ~RubyPythonReference();

#ifdef SCRIPT_RUBY
  VALUE rubyObject_;
#endif
#ifdef SCRIPT_PYTHON
  PyObject * pyObject_;
#endif
};

#endif
