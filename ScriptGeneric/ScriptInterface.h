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

#ifndef ScriptInterface_h_
#define ScriptInterface_h_

#include "Singleton.h"
#include "ReflectionImplement.h"
#include <string>
#include <vector>

class ScriptInterface : public Singleton<ScriptInterface>
{
public:
  /// Initialize scripting engine.  This should be called at the start of the
  /// program.
  void init(const char * modulename);

  /// Define a global variable in the scripting language.
  /// The leading $ (for Ruby global variables) should NOT be in the name, it is
  /// added when required.
  template <typename T>
    void makeGlobalVariable(const std::string & name, T * variable);
  template <typename T>
    void makeGlobalVariable(const std::string & name,std::vector<T> & variable);
  /// Add a global function in the scripting language
  /// The function should be a instance of Reflection::Function with a return
  /// value, i.e.  not returning void.
  /// This class becomes the owner of function.
  void makeGlobalFunction(const std::string & name,
                          Reflection::MethodBase * function);
  template <typename T>
    void makeGlobalConstant(const std::string & name,
                            const T & value);

#ifdef SCRIPT_RUBY
  // Run a ruby script.
  // Raised Ruby exceptions are converted to C++ exceptions
  // Note that the current directory in NOT in INC by default, you can add
  // it with addRubyScriptPath(".")
  void runRubyScript(const std::string & filename);
  // Run ruby code in string
  void runRubyString(const std::string & code);
  void addRubyScriptPath(const std::string & path);

  // Call a global function in Ruby.
  //
  // Puting return value as argument, so template deduction works
  template <typename R>
  void callRuby(const std::string & functionName,
                R & returnValue) const;
  template <typename R, typename A1>
  void callRuby(const std::string & functionName,
                const A1 & a1,
                R & returnValue) const;
  template <typename R, typename A1, typename A2>
  void callRuby(const std::string & functionName,
                const A1 & a1,
                const A2 & a2,
                R & returnValue) const;
  template <typename R, typename A1, typename A2, typename A3>
  void callRuby(const std::string & functionName,
                const A1 & a1,
                const A2 & a2,
                const A3 & a3,
                R & returnValue) const;
  template <typename R, typename A1, typename A2, typename A3, typename A4>
  void callRuby(const std::string & functionName,
                const A1 & a1,
                const A2 & a2,
                const A3 & a3,
                const A4 & a4,
                R & returnValue) const;
  template <typename R, typename A1, typename A2, typename A3, typename A4,
            typename A5>
  void callRuby(const std::string & functionName,
                const A1 & a1,
                const A2 & a2,
                const A3 & a3,
                const A4 & a4,
                const A5 & a5,
                R & returnValue) const;
  template <typename R, typename A1, typename A2, typename A3, typename A4,
            typename A5, typename A6>
  void callRuby(const std::string & functionName,
                const A1 & a1,
                const A2 & a2,
                const A3 & a3,
                const A4 & a4,
                const A5 & a5,
                const A6 & a6,
                R & returnValue) const;
  void callRuby(const std::string & functionName) const;
  template <typename A1>
  void callRuby(const std::string & functionName,
                const A1 & a1) const;
  template <typename A1, typename A2>
  void callRuby(const std::string & functionName,
                const A1 & a1,
                const A2 & a2) const;
  template <typename A1, typename A2, typename A3>
  void callRuby(const std::string & functionName,
                const A1 & a1,
                const A2 & a2,
                const A3 & a3) const;
  template <typename A1, typename A2, typename A3, typename A4>
  void callRuby(const std::string & functionName,
                const A1 & a1,
                const A2 & a2,
                const A3 & a3,
                const A4 & a4) const;
  template <typename A1, typename A2, typename A3, typename A4, typename A5>
  void callRuby(const std::string & functionName,
                const A1 & a1,
                const A2 & a2,
                const A3 & a3,
                const A4 & a4,
                const A5 & a5) const;
  template <typename A1, typename A2, typename A3, typename A4, typename A5,
            typename A6>
  void callRuby(const std::string & functionName,
                const A1 & a1,
                const A2 & a2,
                const A3 & a3,
                const A4 & a4,
                const A5 & a5,
                const A6 & a6) const;
#endif
#ifdef SCRIPT_PYTHON
  // Run a python script.
  // Returns a reference to the python module just loaded.
  // This can be used by callPython to call module functions.
  // Note that the current directory in NOT in sys.path by default, you can add
  // it with addPythonScriptPath(".")
  ReflectionHandle runPythonScript(const std::string & filename);
  // Run python code in string
  void runPythonString(const std::string & code);
  void addPythonScriptPath(const std::string & path);

  template <typename R>
  void callPython(ReflectionHandle pythonModule,
                  const std::string & functionName,
                  R & returnValue) const;
  template <typename R, typename A1>
  void callPython(ReflectionHandle pythonModule,
                  const std::string & functionName,
                  const A1 & a1,
                  R & returnValue) const;
  template <typename R, typename A1, typename A2>
  void callPython(ReflectionHandle pythonModule,
                  const std::string & functionName,
                  const A1 & a1,
                  const A2 & a2,
                  R & returnValue) const;
  template <typename R, typename A1, typename A2, typename A3>
  void callPython(ReflectionHandle pythonModule,
                  const std::string & functionName,
                  const A1 & a1,
                  const A2 & a2,
                  const A3 & a3,
                  R & returnValue) const;
  template <typename R, typename A1, typename A2, typename A3, typename A4>
  void callPython(ReflectionHandle pythonModule,
                  const std::string & functionName,
                  const A1 & a1,
                  const A2 & a2,
                  const A3 & a3,
                  const A4 & a4,
                  R & returnValue) const;
  template <typename R, typename A1, typename A2, typename A3, typename A4,
            typename A5>
  void callPython(ReflectionHandle pythonModule,
                  const std::string & functionName,
                  const A1 & a1,
                  const A2 & a2,
                  const A3 & a3,
                  const A4 & a4,
                  const A5 & a5,
                  R & returnValue) const;
  template <typename R, typename A1, typename A2, typename A3, typename A4,
            typename A5, typename A6>
  void callPython(ReflectionHandle pythonModule,
                  const std::string & functionName,
                  const A1 & a1,
                  const A2 & a2,
                  const A3 & a3,
                  const A4 & a4,
                  const A5 & a5,
                  const A6 & a6,
                  R & returnValue) const;
  void callPython(ReflectionHandle pythonModule,
                  const std::string & functionName) const;
  template <typename A1>
  void callPython(ReflectionHandle pythonModule,
                  const std::string & functionName,
                  const A1 & a1) const;
  template <typename A1, typename A2>
  void callPython(ReflectionHandle pythonModule,
                  const std::string & functionName,
                  const A1 & a1,
                  const A2 & a2) const;
  template <typename A1, typename A2, typename A3>
  void callPython(ReflectionHandle pythonModule,
                  const std::string & functionName,
                  const A1 & a1,
                  const A2 & a2,
                  const A3 & a3) const;
  template <typename A1, typename A2, typename A3, typename A4>
  void callPython(ReflectionHandle pythonModule,
                  const std::string & functionName,
                  const A1 & a1,
                  const A2 & a2,
                  const A3 & a3,
                  const A4 & a4) const;
  template <typename A1, typename A2, typename A3, typename A4, typename A5>
  void callPython(ReflectionHandle pythonModule,
                  const std::string & functionName,
                  const A1 & a1,
                  const A2 & a2,
                  const A3 & a3,
                  const A4 & a4,
                  const A5 & a5) const;
  template <typename A1, typename A2, typename A3, typename A4, typename A5,
            typename A6>
  void callPython(ReflectionHandle pythonModule,
                  const std::string & functionName,
                  const A1 & a1,
                  const A2 & a2,
                  const A3 & a3,
                  const A4 & a4,
                  const A5 & a5,
                  const A6 & a6) const;
#endif

  // Treat the \arg scriptType (in C++ typeid().name style) equal to \arg
  // cppType (also in typeid().name style).
  // e.g. ariadne has a case-aware implementation of a string and all exported
  // methods have that custom class as argument, but in scripting, you just want
  // to use a regular string.
  // Note that you still have to write ReflectionRead and ReflectionWrite for
  // your custom class.
  void addTypeEquality(const std::string & scriptType, const std::string & cppType);

private:
  class Anonymous; // friend in anonymous namespace trick : holds functions
                   // which should have access to our private members

  friend class RubyPythonReference;
#ifdef SCRIPT_PYTHON
  friend PyObject * PyInit_ScriptInterface();
#endif

  void makeClasses();
  void defineGlobalVariable(const std::string & name, ReflectionHandle variable,
                            void * data);
  void defineGlobalConstant(const std::string & name, ReflectionHandle constant,
                            void * data);
  Reflection::MethodBase *
    getGlobalFunction(const std::string & name,
                      const std::vector<std::string> & signature) const;

  typedef std::unordered_multimap<std::string, Reflection::MethodBase*>
    GlobalFunctionMap;
  GlobalFunctionMap globalFunctions_;
#ifdef SCRIPT_RUBY
  void handleRUBYOPT();
  void registerRubyObject(VALUE object);
  void unregisterRubyObject(VALUE object);
  VALUE callRubyPrivate(const std::string & functionName,
                        VALUE argument1=0,
                        VALUE argument2=0,
                        VALUE argument3=0,
                        VALUE argument4=0,
                        VALUE argument5=0,
                        VALUE argument6=0) const;

  VALUE rbmodule_;
  // All ruby objects which have a counterpart in C
  // This is needed for garbage collection to work.
  // The mark based thing doesn't seem to work.  It is only for VALUE member of
  // a C wrapped struct.  You still need some method to 'mark' the C wrapper
  // struct itself.
  // We could use rb_gc_register_address but some people say it is not as
  // efficient as a hash (especially to find and delete elements from it)
  // The hash contains all ruby VALUEs for all C++ objects which have usedInC_
  // set to true, i.e. objects that are also used in C++.
  VALUE rbObjectHash_;
#endif
#ifdef SCRIPT_PYTHON
  PyObject * pymodule_;
  PyObject * callPythonPrivate(PyObject * pythonModule,
                               const std::string & functionName,
                               PyObject * argument1=0,
                               PyObject * argument2=0,
                               PyObject * argument3=0,
                               PyObject * argument4=0,
                               PyObject * argument5=0,
                               PyObject * argument6=0) const;
#endif
};

template <typename T>
void ScriptInterface::makeGlobalVariable(const std::string & name,
                                         T * variable)
{
#ifdef SCRIPT_RUBY
  {
    ReflectionHandle scriptVar = ReflectionRead(variable, LANGUAGE_RUBY);
    defineGlobalVariable(name, scriptVar, LANGUAGE_RUBY);
  }
#endif
#ifdef SCRIPT_PYTHON
  {
    ReflectionHandle scriptVar = ReflectionRead(variable, LANGUAGE_PYTHON);
    defineGlobalVariable(name, scriptVar, LANGUAGE_PYTHON);
  }
#endif
}

template <typename T>
void ScriptInterface::makeGlobalVariable(const std::string & name,
                                         std::vector<T> & variable)
{
#ifdef SCRIPT_RUBY
  {
    ReflectionHandle scriptVar = ReflectionRead(variable, LANGUAGE_RUBY);
    defineGlobalVariable(name, scriptVar, LANGUAGE_RUBY);
  }
#endif
#ifdef SCRIPT_PYTHON
  {
    ReflectionHandle scriptVar = ReflectionRead(variable, LANGUAGE_PYTHON);
    defineGlobalVariable(name, scriptVar, LANGUAGE_PYTHON);
  }
#endif
}

template <typename T>
void ScriptInterface::makeGlobalConstant(const std::string & name,
                                         const T & value)
{
#ifdef SCRIPT_RUBY
  {
    ReflectionHandle scriptVar = ReflectionRead(value, LANGUAGE_RUBY);
    defineGlobalConstant(name, scriptVar, LANGUAGE_RUBY);
  }
#endif
#ifdef SCRIPT_PYTHON
  {
    ReflectionHandle scriptVar = ReflectionRead(value, LANGUAGE_PYTHON);
    defineGlobalConstant(name, scriptVar, LANGUAGE_PYTHON);
  }
#endif
}

#ifdef SCRIPT_RUBY
template <typename R>
void ScriptInterface::callRuby(const std::string & functionName,
                               R & returnValue) const
{
  ReflectionHandle rubyResult;
  rubyResult.rubyHandle = callRubyPrivate(functionName);
  ReflectionWrite(rubyResult, returnValue, LANGUAGE_RUBY);
}

template <typename R, typename A1>
void ScriptInterface::callRuby(const std::string & functionName,
                               const A1 & a1,
                               R & returnValue) const
{
  ReflectionHandle rubyArgument1 = ReflectionRead(a1, LANGUAGE_RUBY);
  ReflectionHandle rubyResult;
  rubyResult.rubyHandle =
    callRubyPrivate(functionName, rubyArgument1.rubyHandle);
  ReflectionWrite(rubyResult, returnValue, LANGUAGE_RUBY);
}

template <typename R, typename A1, typename A2>
void ScriptInterface::callRuby(const std::string & functionName,
                               const A1 & a1,
                               const A2 & a2,
                               R & returnValue) const
{
  ReflectionHandle rubyArgument1 = ReflectionRead(a1, LANGUAGE_RUBY);
  ReflectionHandle rubyArgument2 = ReflectionRead(a2, LANGUAGE_RUBY);
  ReflectionHandle rubyResult;
  rubyResult.rubyHandle =
    callRubyPrivate(functionName,
                    rubyArgument1.rubyHandle,
                    rubyArgument2.rubyHandle);
  ReflectionWrite(rubyResult, returnValue, LANGUAGE_RUBY);
}

template <typename R, typename A1, typename A2, typename A3>
void ScriptInterface::callRuby(const std::string & functionName,
                               const A1 & a1,
                               const A2 & a2,
                               const A3 & a3,
                               R & returnValue) const
{
  ReflectionHandle rubyArgument1 = ReflectionRead(a1, LANGUAGE_RUBY);
  ReflectionHandle rubyArgument2 = ReflectionRead(a2, LANGUAGE_RUBY);
  ReflectionHandle rubyArgument3 = ReflectionRead(a3, LANGUAGE_RUBY);
  ReflectionHandle rubyResult;
  rubyResult.rubyHandle =
    callRubyPrivate(functionName,
                    rubyArgument1.rubyHandle,
                    rubyArgument2.rubyHandle,
                    rubyArgument3.rubyHandle);
  ReflectionWrite(rubyResult, returnValue, LANGUAGE_RUBY);
}

template <typename R, typename A1, typename A2, typename A3, typename A4>
void ScriptInterface::callRuby(const std::string & functionName,
                               const A1 & a1,
                               const A2 & a2,
                               const A3 & a3,
                               const A4 & a4,
                               R & returnValue) const
{
  ReflectionHandle rubyArgument1 = ReflectionRead(a1, LANGUAGE_RUBY);
  ReflectionHandle rubyArgument2 = ReflectionRead(a2, LANGUAGE_RUBY);
  ReflectionHandle rubyArgument3 = ReflectionRead(a3, LANGUAGE_RUBY);
  ReflectionHandle rubyArgument4 = ReflectionRead(a4, LANGUAGE_RUBY);
  ReflectionHandle rubyResult;
  rubyResult.rubyHandle =
    callRubyPrivate(functionName,
                    rubyArgument1.rubyHandle,
                    rubyArgument2.rubyHandle,
                    rubyArgument3.rubyHandle,
                    rubyArgument4.rubyHandle);
  ReflectionWrite(rubyResult, returnValue, LANGUAGE_RUBY);
}

template <typename R, typename A1, typename A2, typename A3, typename A4,
          typename A5>
void ScriptInterface::callRuby(const std::string & functionName,
                               const A1 & a1,
                               const A2 & a2,
                               const A3 & a3,
                               const A4 & a4,
                               const A5 & a5,
                               R & returnValue) const
{
  ReflectionHandle rubyArgument1 = ReflectionRead(a1, LANGUAGE_RUBY);
  ReflectionHandle rubyArgument2 = ReflectionRead(a2, LANGUAGE_RUBY);
  ReflectionHandle rubyArgument3 = ReflectionRead(a3, LANGUAGE_RUBY);
  ReflectionHandle rubyArgument4 = ReflectionRead(a4, LANGUAGE_RUBY);
  ReflectionHandle rubyArgument5 = ReflectionRead(a5, LANGUAGE_RUBY);
  ReflectionHandle rubyResult;
  rubyResult.rubyHandle =
    callRubyPrivate(functionName,
                    rubyArgument1.rubyHandle,
                    rubyArgument2.rubyHandle,
                    rubyArgument3.rubyHandle,
                    rubyArgument4.rubyHandle,
                    rubyArgument5.rubyHandle);
  ReflectionWrite(rubyResult, returnValue, LANGUAGE_RUBY);
}

template <typename R, typename A1, typename A2, typename A3, typename A4,
          typename A5, typename A6>
void ScriptInterface::callRuby(const std::string & functionName,
                               const A1 & a1,
                               const A2 & a2,
                               const A3 & a3,
                               const A4 & a4,
                               const A5 & a5,
                               const A6 & a6,
                               R & returnValue) const
{
  ReflectionHandle rubyArgument1 = ReflectionRead(a1, LANGUAGE_RUBY);
  ReflectionHandle rubyArgument2 = ReflectionRead(a2, LANGUAGE_RUBY);
  ReflectionHandle rubyArgument3 = ReflectionRead(a3, LANGUAGE_RUBY);
  ReflectionHandle rubyArgument4 = ReflectionRead(a4, LANGUAGE_RUBY);
  ReflectionHandle rubyArgument5 = ReflectionRead(a5, LANGUAGE_RUBY);
  ReflectionHandle rubyArgument6 = ReflectionRead(a6, LANGUAGE_RUBY);
  ReflectionHandle rubyResult;
  rubyResult.rubyHandle =
    callRubyPrivate(functionName,
                    rubyArgument1.rubyHandle,
                    rubyArgument2.rubyHandle,
                    rubyArgument3.rubyHandle,
                    rubyArgument4.rubyHandle,
                    rubyArgument5.rubyHandle,
                    rubyArgument6.rubyHandle);
  ReflectionWrite(rubyResult, returnValue, LANGUAGE_RUBY);
}

template <typename A1>
void ScriptInterface::callRuby(const std::string & functionName,
                               const A1 & a1) const
{
  ReflectionHandle rubyArgument1 = ReflectionRead(a1, LANGUAGE_RUBY);
  callRubyPrivate(functionName, rubyArgument1.rubyHandle);
}

template <typename A1, typename A2>
void ScriptInterface::callRuby(const std::string & functionName,
                               const A1 & a1,
                               const A2 & a2) const
{
  ReflectionHandle rubyArgument1 = ReflectionRead(a1, LANGUAGE_RUBY);
  ReflectionHandle rubyArgument2 = ReflectionRead(a2, LANGUAGE_RUBY);
  callRubyPrivate(functionName,
                  rubyArgument1.rubyHandle,
                  rubyArgument2.rubyHandle);
}

template <typename A1, typename A2, typename A3>
void ScriptInterface::callRuby(const std::string & functionName,
                               const A1 & a1,
                               const A2 & a2,
                               const A3 & a3) const
{
  ReflectionHandle rubyArgument1 = ReflectionRead(a1, LANGUAGE_RUBY);
  ReflectionHandle rubyArgument2 = ReflectionRead(a2, LANGUAGE_RUBY);
  ReflectionHandle rubyArgument3 = ReflectionRead(a3, LANGUAGE_RUBY);
  callRubyPrivate(functionName,
                  rubyArgument1.rubyHandle,
                  rubyArgument2.rubyHandle,
                  rubyArgument3.rubyHandle);
}

template <typename A1, typename A2, typename A3, typename A4>
void ScriptInterface::callRuby(const std::string & functionName,
                               const A1 & a1,
                               const A2 & a2,
                               const A3 & a3,
                               const A4 & a4) const
{
  ReflectionHandle rubyArgument1 = ReflectionRead(a1, LANGUAGE_RUBY);
  ReflectionHandle rubyArgument2 = ReflectionRead(a2, LANGUAGE_RUBY);
  ReflectionHandle rubyArgument3 = ReflectionRead(a3, LANGUAGE_RUBY);
  ReflectionHandle rubyArgument4 = ReflectionRead(a4, LANGUAGE_RUBY);
  callRubyPrivate(functionName,
                  rubyArgument1.rubyHandle,
                  rubyArgument2.rubyHandle,
                  rubyArgument3.rubyHandle,
                  rubyArgument4.rubyHandle);
}

template <typename A1, typename A2, typename A3, typename A4, typename A5>
void ScriptInterface::callRuby(const std::string & functionName,
                               const A1 & a1,
                               const A2 & a2,
                               const A3 & a3,
                               const A4 & a4,
                               const A5 & a5) const
{
  ReflectionHandle rubyArgument1 = ReflectionRead(a1, LANGUAGE_RUBY);
  ReflectionHandle rubyArgument2 = ReflectionRead(a2, LANGUAGE_RUBY);
  ReflectionHandle rubyArgument3 = ReflectionRead(a3, LANGUAGE_RUBY);
  ReflectionHandle rubyArgument4 = ReflectionRead(a4, LANGUAGE_RUBY);
  ReflectionHandle rubyArgument5 = ReflectionRead(a5, LANGUAGE_RUBY);
  callRubyPrivate(functionName,
                  rubyArgument1.rubyHandle,
                  rubyArgument2.rubyHandle,
                  rubyArgument3.rubyHandle,
                  rubyArgument4.rubyHandle,
                  rubyArgument5.rubyHandle);
}

template <typename A1, typename A2, typename A3, typename A4, typename A5,
          typename A6>
void ScriptInterface::callRuby(const std::string & functionName,
                               const A1 & a1,
                               const A2 & a2,
                               const A3 & a3,
                               const A4 & a4,
                               const A5 & a5,
                               const A6 & a6) const
{
  ReflectionHandle rubyArgument1 = ReflectionRead(a1, LANGUAGE_RUBY);
  ReflectionHandle rubyArgument2 = ReflectionRead(a2, LANGUAGE_RUBY);
  ReflectionHandle rubyArgument3 = ReflectionRead(a3, LANGUAGE_RUBY);
  ReflectionHandle rubyArgument4 = ReflectionRead(a4, LANGUAGE_RUBY);
  ReflectionHandle rubyArgument5 = ReflectionRead(a5, LANGUAGE_RUBY);
  ReflectionHandle rubyArgument6 = ReflectionRead(a6, LANGUAGE_RUBY);
  callRubyPrivate(functionName,
                  rubyArgument1.rubyHandle,
                  rubyArgument2.rubyHandle,
                  rubyArgument3.rubyHandle,
                  rubyArgument4.rubyHandle,
                  rubyArgument5.rubyHandle,
                  rubyArgument6.rubyHandle);
}
#endif

#ifdef SCRIPT_PYTHON
template <typename R>
void ScriptInterface::callPython(ReflectionHandle pythonModule,
                                 const std::string & functionName,
                                 R & returnValue) const
{
  ReflectionHandle pyResult;
  pyResult.pythonHandle = callPythonPrivate(pythonModule.pythonHandle,
                                            functionName);
  ReflectionWrite(pyResult, returnValue, LANGUAGE_PYTHON);
  Py_XDECREF(pyResult.pythonHandle);
}

template <typename R, typename A1>
void ScriptInterface::callPython(ReflectionHandle pythonModule,
                                 const std::string & functionName,
                                 const A1 & a1,
                                 R & returnValue) const
{
  ReflectionHandle pyArgument1 = ReflectionRead(a1, LANGUAGE_PYTHON);
  ReflectionHandle pyResult;
  pyResult.pythonHandle = callPythonPrivate(pythonModule.pythonHandle,
                                            functionName,
                                            pyArgument1.pythonHandle);
  ReflectionWrite(pyResult, returnValue, LANGUAGE_PYTHON);
  Py_XDECREF(pyResult.pythonHandle);
}

template <typename R, typename A1, typename A2>
void ScriptInterface::callPython(ReflectionHandle pythonModule,
                                 const std::string & functionName,
                                 const A1 & a1,
                                 const A2 & a2,
                                 R & returnValue) const
{
  ReflectionHandle pyArgument1 = ReflectionRead(a1, LANGUAGE_PYTHON);
  ReflectionHandle pyArgument2 = ReflectionRead(a2, LANGUAGE_PYTHON);
  ReflectionHandle pyResult;
  pyResult.pythonHandle = callPythonPrivate(pythonModule.pythonHandle,
                                            functionName,
                                            pyArgument1.pythonHandle,
                                            pyArgument2.pythonHandle);
  ReflectionWrite(pyResult, returnValue, LANGUAGE_PYTHON);
  Py_XDECREF(pyResult.pythonHandle);
}

template <typename R, typename A1, typename A2, typename A3>
void ScriptInterface::callPython(ReflectionHandle pythonModule,
                                 const std::string & functionName,
                                 const A1 & a1,
                                 const A2 & a2,
                                 const A3 & a3,
                                 R & returnValue) const
{
  ReflectionHandle pyArgument1 = ReflectionRead(a1, LANGUAGE_PYTHON);
  ReflectionHandle pyArgument2 = ReflectionRead(a2, LANGUAGE_PYTHON);
  ReflectionHandle pyArgument3 = ReflectionRead(a3, LANGUAGE_PYTHON);
  ReflectionHandle pyResult;
  pyResult.pythonHandle = callPythonPrivate(pythonModule.pythonHandle,
                                            functionName,
                                            pyArgument1.pythonHandle,
                                            pyArgument2.pythonHandle,
                                            pyArgument3.pythonHandle);
  ReflectionWrite(pyResult, returnValue, LANGUAGE_PYTHON);
  Py_XDECREF(pyResult.pythonHandle);
}

template <typename R, typename A1, typename A2, typename A3, typename A4>
void ScriptInterface::callPython(ReflectionHandle pythonModule,
                                 const std::string & functionName,
                                 const A1 & a1,
                                 const A2 & a2,
                                 const A3 & a3,
                                 const A4 & a4,
                                 R & returnValue) const
{
  ReflectionHandle pyArgument1 = ReflectionRead(a1, LANGUAGE_PYTHON);
  ReflectionHandle pyArgument2 = ReflectionRead(a2, LANGUAGE_PYTHON);
  ReflectionHandle pyArgument3 = ReflectionRead(a3, LANGUAGE_PYTHON);
  ReflectionHandle pyArgument4 = ReflectionRead(a4, LANGUAGE_PYTHON);
  ReflectionHandle pyResult;
  pyResult.pythonHandle = callPythonPrivate(pythonModule.pythonHandle,
                                            functionName,
                                            pyArgument1.pythonHandle,
                                            pyArgument2.pythonHandle,
                                            pyArgument3.pythonHandle,
                                            pyArgument4.pythonHandle);
  ReflectionWrite(pyResult, returnValue, LANGUAGE_PYTHON);
  Py_XDECREF(pyResult.pythonHandle);
}

template <typename R, typename A1, typename A2, typename A3, typename A4,
          typename A5>
void ScriptInterface::callPython(ReflectionHandle pythonModule,
                                 const std::string & functionName,
                                 const A1 & a1,
                                 const A2 & a2,
                                 const A3 & a3,
                                 const A4 & a4,
                                 const A5 & a5,
                                 R & returnValue) const
{
  ReflectionHandle pyArgument1 = ReflectionRead(a1, LANGUAGE_PYTHON);
  ReflectionHandle pyArgument2 = ReflectionRead(a2, LANGUAGE_PYTHON);
  ReflectionHandle pyArgument3 = ReflectionRead(a3, LANGUAGE_PYTHON);
  ReflectionHandle pyArgument4 = ReflectionRead(a4, LANGUAGE_PYTHON);
  ReflectionHandle pyArgument5 = ReflectionRead(a5, LANGUAGE_PYTHON);
  ReflectionHandle pyResult;
  pyResult.pythonHandle = callPythonPrivate(pythonModule.pythonHandle,
                                            functionName,
                                            pyArgument1.pythonHandle,
                                            pyArgument2.pythonHandle,
                                            pyArgument3.pythonHandle,
                                            pyArgument4.pythonHandle,
                                            pyArgument5.pythonHandle);
  ReflectionWrite(pyResult, returnValue, LANGUAGE_PYTHON);
  Py_XDECREF(pyResult.pythonHandle);
}

template <typename R, typename A1, typename A2, typename A3, typename A4,
          typename A5, typename A6>
void ScriptInterface::callPython(ReflectionHandle pythonModule,
                                 const std::string & functionName,
                                 const A1 & a1,
                                 const A2 & a2,
                                 const A3 & a3,
                                 const A4 & a4,
                                 const A5 & a5,
                                 const A6 & a6,
                                 R & returnValue) const
{
  ReflectionHandle pyArgument1 = ReflectionRead(a1, LANGUAGE_PYTHON);
  ReflectionHandle pyArgument2 = ReflectionRead(a2, LANGUAGE_PYTHON);
  ReflectionHandle pyArgument3 = ReflectionRead(a3, LANGUAGE_PYTHON);
  ReflectionHandle pyArgument4 = ReflectionRead(a4, LANGUAGE_PYTHON);
  ReflectionHandle pyArgument5 = ReflectionRead(a5, LANGUAGE_PYTHON);
  ReflectionHandle pyArgument6 = ReflectionRead(a6, LANGUAGE_PYTHON);
  ReflectionHandle pyResult;
  pyResult.pythonHandle = callPythonPrivate(pythonModule.pythonHandle,
                                            functionName,
                                            pyArgument1.pythonHandle,
                                            pyArgument2.pythonHandle,
                                            pyArgument3.pythonHandle,
                                            pyArgument4.pythonHandle,
                                            pyArgument5.pythonHandle,
                                            pyArgument6.pythonHandle);
  ReflectionWrite(pyResult, returnValue, LANGUAGE_PYTHON);
  Py_XDECREF(pyResult.pythonHandle);
}

template <typename A1>
void ScriptInterface::callPython(ReflectionHandle pythonModule,
                                 const std::string & functionName,
                                 const A1 & a1) const
{
  ReflectionHandle pyArgument1 = ReflectionRead(a1, LANGUAGE_PYTHON);
  PyObject * result = callPythonPrivate(pythonModule.pythonHandle,
                                        functionName,
                                        pyArgument1.pythonHandle);
  Py_XDECREF(result);
}

template <typename A1, typename A2>
void ScriptInterface::callPython(ReflectionHandle pythonModule,
                                 const std::string & functionName,
                                 const A1 & a1,
                                 const A2 & a2) const
{
  ReflectionHandle pyArgument1 = ReflectionRead(a1, LANGUAGE_PYTHON);
  ReflectionHandle pyArgument2 = ReflectionRead(a2, LANGUAGE_PYTHON);
  PyObject * result = callPythonPrivate(pythonModule.pythonHandle,
                                        functionName,
                                        pyArgument1.pythonHandle,
                                        pyArgument2.pythonHandle);
  Py_XDECREF(result);
}

template <typename A1, typename A2, typename A3>
void ScriptInterface::callPython(ReflectionHandle pythonModule,
                                 const std::string & functionName,
                                 const A1 & a1,
                                 const A2 & a2,
                                 const A3 & a3) const
{
  ReflectionHandle pyArgument1 = ReflectionRead(a1, LANGUAGE_PYTHON);
  ReflectionHandle pyArgument2 = ReflectionRead(a2, LANGUAGE_PYTHON);
  ReflectionHandle pyArgument3 = ReflectionRead(a3, LANGUAGE_PYTHON);
  PyObject * result = callPythonPrivate(pythonModule.pythonHandle,
                                        functionName,
                                        pyArgument1.pythonHandle,
                                        pyArgument2.pythonHandle,
                                        pyArgument3.pythonHandle);
  Py_XDECREF(result);
}

template <typename A1, typename A2, typename A3, typename A4>
void ScriptInterface::callPython(ReflectionHandle pythonModule,
                                 const std::string & functionName,
                                 const A1 & a1,
                                 const A2 & a2,
                                 const A3 & a3,
                                 const A4 & a4) const
{
  ReflectionHandle pyArgument1 = ReflectionRead(a1, LANGUAGE_PYTHON);
  ReflectionHandle pyArgument2 = ReflectionRead(a2, LANGUAGE_PYTHON);
  ReflectionHandle pyArgument3 = ReflectionRead(a3, LANGUAGE_PYTHON);
  ReflectionHandle pyArgument4 = ReflectionRead(a4, LANGUAGE_PYTHON);
  PyObject * result = callPythonPrivate(pythonModule.pythonHandle,
                                        functionName,
                                        pyArgument1.pythonHandle,
                                        pyArgument2.pythonHandle,
                                        pyArgument3.pythonHandle,
                                        pyArgument4.pythonHandle);
  Py_XDECREF(result);
}

template <typename A1, typename A2, typename A3, typename A4, typename A5>
void ScriptInterface::callPython(ReflectionHandle pythonModule,
                                 const std::string & functionName,
                                 const A1 & a1,
                                 const A2 & a2,
                                 const A3 & a3,
                                 const A4 & a4,
                                 const A5 & a5) const
{
  ReflectionHandle pyArgument1 = ReflectionRead(a1, LANGUAGE_PYTHON);
  ReflectionHandle pyArgument2 = ReflectionRead(a2, LANGUAGE_PYTHON);
  ReflectionHandle pyArgument3 = ReflectionRead(a3, LANGUAGE_PYTHON);
  ReflectionHandle pyArgument4 = ReflectionRead(a4, LANGUAGE_PYTHON);
  ReflectionHandle pyArgument5 = ReflectionRead(a5, LANGUAGE_PYTHON);
  PyObject * result = callPythonPrivate(pythonModule.pythonHandle,
                                        functionName,
                                        pyArgument1.pythonHandle,
                                        pyArgument2.pythonHandle,
                                        pyArgument3.pythonHandle,
                                        pyArgument4.pythonHandle,
                                        pyArgument5.pythonHandle);
  Py_XDECREF(result);
}

template <typename A1, typename A2, typename A3, typename A4, typename A5,
          typename A6>
void ScriptInterface::callPython(ReflectionHandle pythonModule,
                                 const std::string & functionName,
                                 const A1 & a1,
                                 const A2 & a2,
                                 const A3 & a3,
                                 const A4 & a4,
                                 const A5 & a5,
                                 const A6 & a6) const
{
  ReflectionHandle pyArgument1 = ReflectionRead(a1, LANGUAGE_PYTHON);
  ReflectionHandle pyArgument2 = ReflectionRead(a2, LANGUAGE_PYTHON);
  ReflectionHandle pyArgument3 = ReflectionRead(a3, LANGUAGE_PYTHON);
  ReflectionHandle pyArgument4 = ReflectionRead(a4, LANGUAGE_PYTHON);
  ReflectionHandle pyArgument5 = ReflectionRead(a5, LANGUAGE_PYTHON);
  ReflectionHandle pyArgument6 = ReflectionRead(a6, LANGUAGE_PYTHON);
  PyObject * result = callPythonPrivate(pythonModule.pythonHandle,
                                        functionName,
                                        pyArgument1.pythonHandle,
                                        pyArgument2.pythonHandle,
                                        pyArgument3.pythonHandle,
                                        pyArgument4.pythonHandle,
                                        pyArgument5.pythonHandle,
                                        pyArgument6.pythonHandle);
  Py_XDECREF(result);
}
#endif

#endif
