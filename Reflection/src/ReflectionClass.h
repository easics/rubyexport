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

#ifndef ReflectionClass_h_
#define ReflectionClass_h_

#include "ReflectionAttribute.h"
#include "ReflectionClassBase.h"
#include "ReflectionMethod.h"
#include "ReflectionConstructor.h"
#include "ReflectionEnum.h"
#include <map>
#include <vector>

namespace Reflection
{

template <typename A1=NoClass, typename A2=NoClass, typename A3=NoClass,
          typename A4=NoClass, typename A5=NoClass, typename A6=NoClass,
          typename A7=NoClass>
struct init
{
};

template<typename T>
class Class : public ClassBase
{
public:
  typedef Class<T> self;

  Class(std::string && name);
  virtual std::string getTypeIdName() const override;
  virtual std::string getPointerTypeIdName() const override;

  // Define access to a member
  template<typename A>
    self & def_a(const std::string & argName, A T:: * a);
  // Define access to a static method
  template<typename R>
    self & def_f(const std::string & methodName, R (*a)());
  template<typename R, typename A1>
    self & def_f(const std::string & methodName, R (*a)(A1));
  template<typename R, typename A1, typename A2>
    self & def_f(const std::string & methodName, R (*a)(A1, A2));
  template<typename R, typename A1, typename A2, typename A3>
    self & def_f(const std::string & methodName, R (*a)(A1, A2, A3));
  template<typename R, typename A1, typename A2, typename A3,
           typename A4>
    self & def_f(const std::string & methodName, R (*a)(A1, A2, A3, A4));
  template<typename R, typename A1, typename A2, typename A3,
           typename A4, typename A5>
    self & def_f(const std::string & methodName, R (*a)(A1, A2, A3, A4, A5));
  template<typename R, typename A1, typename A2, typename A3,
           typename A4, typename A5, typename A6>
    self & def_f(const std::string & methodName, R (*a)(A1, A2, A3, A4, A5,A6));

  // Define access to a method
  template<typename R>
    self & def_f(const std::string & methodName, R (T::*a)());
  template<typename R, typename A1>
    self & def_f(const std::string & methodName, R (T::*a)(A1));
  template<typename R, typename A1, typename A2>
    self & def_f(const std::string & methodName, R (T::*a)(A1, A2));
  template<typename R, typename A1, typename A2, typename A3>
    self & def_f(const std::string & methodName, R (T::*a)(A1, A2, A3));
  template<typename R, typename A1, typename A2, typename A3,
           typename A4>
    self & def_f(const std::string & methodName, R (T::*a)(A1, A2, A3, A4));
  template<typename R, typename A1, typename A2, typename A3,
           typename A4, typename A5>
    self & def_f(const std::string & methodName, R (T::*a)(A1, A2, A3, A4, A5));
  template<typename R, typename A1, typename A2, typename A3,
           typename A4, typename A5, typename A6>
    self & def_f(const std::string & methodName, R (T::*a)(A1, A2, A3, A4, A5,
                                                           A6));
  template<typename R>
    self & def_f(const std::string & methodName, R (T::*a)() const);
  template<typename R, typename A1>
    self & def_f(const std::string & methodName, R (T::*a)(A1) const);
  template<typename R, typename A1, typename A2>
    self & def_f(const std::string & methodName, R (T::*a)(A1, A2) const);
  template<typename R, typename A1, typename A2, typename A3>
    self & def_f(const std::string & methodName, R (T::*a)(A1, A2, A3) const);
  template<typename R, typename A1, typename A2, typename A3, typename A4>
    self & def_f(const std::string & methodName, R (T::*a)(A1, A2, A3,
                                                           A4) const);
  template<typename R, typename A1, typename A2, typename A3,
           typename A4, typename A5>
    self & def_f(const std::string & methodName, R (T::*a)(A1, A2, A3,
                                                           A4, A5) const);
  template<typename R, typename A1, typename A2, typename A3,
           typename A4, typename A5, typename A6>
    self & def_f(const std::string & methodName, R (T::*a)(A1, A2, A3,
                                                           A4, A5, A6) const);
  // Define a constructor
  template<typename A1, typename A2, typename A3,
           typename A4, typename A5, typename A6,
           typename A7>
  self & def_c(const init<A1,A2,A3,A4,A5,A6,A7> & i);

  // Define an enum
  template<typename E1>
  self & def_e(const Enum<E1> & e);
};

template<typename T>
Class<T>::Class(std::string && name)
  : ClassBase(std::move(name))
{
  ReflectionCheckType<T>();
  classInfo_ = ReflectionMakeClassInfo<T>();
}

template<typename T>
std::string Class<T>::getTypeIdName() const
{
  return typeid(T).name();
}

template<typename T>
std::string Class<T>::getPointerTypeIdName() const
{
  return typeid(T*).name();
}

template<typename T>
template<typename A>
Class<T> & Class<T>::def_a(const std::string & argName, A T:: * a)
{
  (*attributeMap_)[argName] = new Attribute<T, A>(a);
  return *this;
}

template<typename T>
template<typename R>
Class<T> & Class<T>::def_f(const std::string & methodName, R (*a)())
{
  (*methodMap_).insert({methodName, new Function<R>((void*)a)});
  return *this;
}

template<typename T>
template<typename R, typename A1>
Class<T> & Class<T>::def_f(const std::string & methodName, R (*a)(A1))
{
  (*methodMap_).insert({methodName, new Function<R, A1>((void*)a)});
  return *this;
}

template<typename T>
template<typename R, typename A1, typename A2>
Class<T> & Class<T>::def_f(const std::string & methodName,
                           R (*a)(A1,A2))
{
  (*methodMap_).insert({methodName, new Function<R, A1, A2>((void*)a)});
  return *this;
}

template<typename T>
template<typename R, typename A1, typename A2, typename A3>
Class<T> & Class<T>::def_f(const std::string & methodName,
                           R (*a)(A1,A2,A3))
{
  (*methodMap_).insert({methodName, new Function<R, A1, A2, A3>((void*)a)});
  return *this;
}

template<typename T>
template<typename R, typename A1, typename A2, typename A3, typename A4>
Class<T> & Class<T>::def_f(const std::string & methodName,
                           R (*a)(A1,A2,A3,A4))
{
  (*methodMap_).insert({methodName, new Function<R,A1,A2,A3,A4>((void*)a)});
  return *this;
}

template<typename T>
template<typename R, typename A1, typename A2, typename A3, typename A4,
         typename A5>
Class<T> & Class<T>::def_f(const std::string & methodName,
                           R (*a)(A1,A2,A3,A4,A5))
{
  (*methodMap_).insert({methodName, new Function<R, A1,A2,A3,A4,A5>((void*)a)});
  return *this;
}

template<typename T>
template<typename R, typename A1, typename A2, typename A3, typename A4,
         typename A5, typename A6>
Class<T> & Class<T>::def_f(const std::string & methodName,
                           R (*a)(A1,A2,A3,A4,A5,A6))
{
  (*methodMap_).insert({methodName, new Function<R, A1, A2, A3, A4,
                                                 A5, A6>((void*)a)});
  return *this;
}

template<typename T>
template<typename R>
Class<T> & Class<T>::def_f(const std::string & methodName,
                           R (T::*a)())
{
  (*methodMap_).insert({methodName, new Method<T, R>(a)});
  return *this;
}

template<typename T>
template<typename R, typename A1>
Class<T> & Class<T>::def_f(const std::string & methodName,
                           R (T::*a)(A1))
{
  (*methodMap_).insert({methodName, new Method<T, R, A1>(a)});
  return *this;
}

template<typename T>
template<typename R, typename A1, typename A2>
Class<T> & Class<T>::def_f(const std::string & methodName,
                           R (T::*a)(A1, A2))
{
  (*methodMap_).insert({methodName, new Method<T, R, A1, A2>(a)});
  return *this;
}

template<typename T>
template<typename R, typename A1, typename A2, typename A3>
Class<T> & Class<T>::def_f(const std::string & methodName,
                           R (T::*a)(A1, A2, A3))
{
  (*methodMap_).insert({methodName, new Method<T, R, A1, A2, A3>(a)});
  return *this;
}

template<typename T>
template<typename R, typename A1, typename A2, typename A3, typename A4>
Class<T> & Class<T>::def_f(const std::string & methodName,
                           R (T::*a)(A1, A2, A3, A4))
{
  (*methodMap_).insert({methodName, new Method<T, R, A1, A2, A3, A4>(a)});
  return *this;
}

template<typename T>
template<typename R, typename A1, typename A2, typename A3, typename A4,
         typename A5>
Class<T> & Class<T>::def_f(const std::string & methodName,
                           R (T::*a)(A1, A2, A3, A4, A5))
{
  (*methodMap_).insert({methodName, new Method<T, R, A1, A2, A3, A4, A5>(a)});
  return *this;
}

template<typename T>
template<typename R, typename A1, typename A2, typename A3, typename A4,
         typename A5, typename A6>
Class<T> & Class<T>::def_f(const std::string & methodName,
                           R (T::*a)(A1, A2, A3, A4, A5, A6))
{
  (*methodMap_).insert({methodName, new Method<T, R, A1, A2, A3, A4,A5,A6>(a)});
  return *this;
}

template<typename T>
template<typename R>
Class<T> & Class<T>::def_f(const std::string & methodName,
                           R (T::*a)() const)
{
  (*methodMap_).insert({methodName, new Method<T, R>(a)});
  return *this;
}

template<typename T>
template<typename R, typename A1>
Class<T> & Class<T>::def_f(const std::string & methodName,
                           R (T::*a)(A1) const)
{
  (*methodMap_).insert({methodName, new Method<T, R, A1>(a)});
  return *this;
}

template<typename T>
template<typename R, typename A1, typename A2>
Class<T> & Class<T>::def_f(const std::string & methodName,
                           R (T::*a)(A1, A2) const)
{
  (*methodMap_).insert({methodName, new Method<T, R, A1, A2>(a)});
  return *this;
}

template<typename T>
template<typename R, typename A1, typename A2, typename A3>
Class<T> & Class<T>::def_f(const std::string & methodName,
                           R (T::*a)(A1, A2, A3) const)
{
  (*methodMap_).insert({methodName, new Method<T, R, A1, A2, A3>(a)});
  return *this;
}

template<typename T>
template<typename R, typename A1, typename A2, typename A3, typename A4>
Class<T> & Class<T>::def_f(const std::string & methodName,
                           R (T::*a)(A1, A2, A3, A4) const)
{
  (*methodMap_).insert({methodName, new Method<T, R, A1, A2, A3, A4>(a)});
  return *this;
}

template<typename T>
template<typename R, typename A1, typename A2, typename A3, typename A4,
         typename A5>
Class<T> & Class<T>::def_f(const std::string & methodName,
                           R (T::*a)(A1, A2, A3, A4, A5) const)
{
  (*methodMap_).insert({methodName, new Method<T, R, A1, A2, A3, A4, A5>(a)});
  return *this;
}

template<typename T>
template<typename R, typename A1, typename A2, typename A3, typename A4,
         typename A5, typename A6>
Class<T> & Class<T>::def_f(const std::string & methodName,
                           R (T::*a)(A1, A2, A3, A4, A5, A6) const)
{
  (*methodMap_).insert({methodName, new Method<T, R, A1, A2, A3, A4,A5,A6>(a)});
  return *this;
}

template<typename T>
template<typename A1, typename A2, typename A3,
         typename A4, typename A5, typename A6,
         typename A7>
Class<T> & Class<T>::def_c(const init<A1,A2,A3,A4,A5,A6,A7> & i
                           __attribute__((unused)))
{
  Class<T>::constructorArray_->
    push_back(new Constructor<T,A1,A2,A3,A4,A5,A6,A7>);
  return *this;
}

template<typename T>
template<typename E1>
Class<T> & Class<T>::def_e(const Enum<E1> & e)
{
  Class<T>::enumArray_->push_back(new Enum<E1>(e));
  return *this;
}

}

#endif
