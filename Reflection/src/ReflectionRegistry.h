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

#ifndef ReflectionRegistry_h_
#define ReflectionRegistry_h_

#include "ReflectionClass.h"
#include <map>
#include <unordered_map>

#ifdef __GNUC__
#define NOT_USED __attribute__ ((unused))
#else
#define NOT_USED
#endif

#define REFLECT_CLASS_PREAMBLE(klass) \
  static Reflection::ClassBase * init_reflection_##klass(); \
  static int init_reflection_wrapper_##klass(); \
  static NOT_USED int dummy_int_for_##klass = init_reflection_wrapper_##klass(); \
  static int init_reflection_wrapper_##klass() \
  {

#define REFLECT_CLASS(klass) \
  REFLECT_CLASS_PREAMBLE(klass) \
    Reflection::Registry::instance().registerClass(&init_reflection_##klass, \
                                                   #klass, \
                                                   typeid(klass).name(), \
                                                   typeid(klass*).name()); \
    return 0; \
  } \
  static Reflection::ClassBase * init_reflection_##klass() \
  { \
    typedef NOT_USED klass WrappedClass; \
    return &(*new Reflection::Class<klass>(#klass))

#define REFLECT_CLASS_DERIVED(klass, parent) \
  REFLECT_CLASS_PREAMBLE(klass) \
    Reflection::Registry::instance().registerClass(&init_reflection_##klass, \
                                                   #klass, \
                                                   typeid(klass).name(), \
                                                   typeid(klass*).name(), \
                                                   typeid(parent).name()); \
    return 0; \
  } \
  static Reflection::ClassBase * init_reflection_##klass() \
  { \
    typedef NOT_USED klass WrappedClass; \
    return &(*new Reflection::Class<klass>(#klass))

#define DEF_A(a) \
  def_a(#a, &WrappedClass::a)

#define DEF_F(f) \
  def_f(#f, &WrappedClass::f)


namespace Reflection
{

class Registry
{
public:
  using InitFunction = Reflection::ClassBase*(*)();
  typedef std::vector<Reflection::ClassBase*> ClassArray;

  static Registry & instance();

  void registerClass(InitFunction f,
                     const std::string & className,
                     const std::string & classTypeIdName,
                     const std::string & pointerClassTypeIdName);
  void registerClass(InitFunction f,
                     const std::string & className,
                     const std::string & classTypeIdName,
                     const std::string & pointerClassTypeIdName,
                     const std::string & parent);
  void init();

  // Array is sorted by inheritance
  // i.e. base classes come before derived classes
  ClassArray getClasses() const;

  bool isInheritedFrom(const std::string & derived,
                       const std::string & base) const;
  // Return empty string if no match was found
  std::string pointerToClassname(const std::string & pointerName) const;
  // Return empty string if no match was found
  std::string typeidNameToClassname(const std::string & typeidName) const;

  ClassBase * getClass(const std::string & name) const;

private:
  typedef std::map<std::string , InitFunction> InitFunctionMap;
  typedef std::multimap<std::string, std::string> InheritanceMap;
  typedef std::unordered_map<std::string, std::string> StringMap;
  typedef std::unordered_map<std::string, ClassBase *> ClassMap;

  Registry();
  ~Registry();

  static Registry * instance_;
  InitFunctionMap initFunctions_;
  InheritanceMap inheritanceMap_;
  ClassArray classes_;
  ClassMap classMap_;
  StringMap pointerToObjectMap_;
  StringMap typeidToClassname_;
};

}

#endif
