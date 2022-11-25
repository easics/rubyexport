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


#include "RubyPythonReference.h"
#include "AbstractPointer.h"
#include "ScriptLanguage.h"
#ifdef SCRIPT_RUBY
#include "ScriptInterface.h"
#endif
#include <cassert>

RubyPythonReference::RubyPythonReference(AbstractPointer * cppObject)
  : ScriptReference(cppObject)
#ifdef SCRIPT_RUBY
    , rubyObject_(Qnil)
#endif
#ifdef SCRIPT_PYTHON
    , pyObject_(nullptr)
#endif
{
}

RubyPythonReference::~RubyPythonReference()
{
  delete cppObject_;
  cppObject_ = nullptr;
}

void RubyPythonReference::useInC()
{
  //if (!usedInC_)
    {
      ++usedInC_;
#ifdef SCRIPT_RUBY
      if (rubyObject_ != Qnil)
        ScriptInterface::instance().registerRubyObject(rubyObject_);
#endif
#ifdef SCRIPT_PYTHON
      if (pyObject_)
        Py_INCREF(pyObject_);
#endif
    }
}

void RubyPythonReference::deleteFromC()
{
  assert(usedInC_);
  --usedInC_;
#ifdef SCRIPT_RUBY
  if (rubyObject_ != Qnil)
    ScriptInterface::instance().unregisterRubyObject(rubyObject_);
#endif
#ifdef SCRIPT_PYTHON
  // This DECREF can trigger deleteFromScript, which will call
  // ~RubyPythonReference, which will delete the C++ object
  if (pyObject_)
    Py_XDECREF(pyObject_);
#endif
}

void RubyPythonReference::deleteFromScript(void * data
#if !defined(SCRIPT_RUBY) || !defined(SCRIPT_PYTHON)
                                           __attribute__((unused))
#endif
                                           )
{
  assert(!usedInC_);
#if defined(SCRIPT_RUBY) && !defined(SCRIPT_PYTHON)
  delete this;
#endif
#if !defined(SCRIPT_RUBY) && defined(SCRIPT_PYTHON)
  delete this;
#endif
#if defined(SCRIPT_RUBY) && defined(SCRIPT_PYTHON)
  if (data == LANGUAGE_RUBY)
    {
      rubyObject_ = Qnil;
      if (pyObject_ == nullptr)
        delete this;
    }
  else if (data == LANGUAGE_PYTHON)
    {
      pyObject_ = nullptr;
      if (rubyObject_ == Qnil)
        delete this;
    }
#endif
}
