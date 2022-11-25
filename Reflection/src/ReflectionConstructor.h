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

#ifndef ReflectionConstructor_h_
#define ReflectionConstructor_h_

#include "ReflectionUtil.h"

namespace Reflection
{

class ConstructorBase
{
public:
  virtual void * call(void * data,
                      ReflectionHandle a1, ReflectionHandle a2,
                      ReflectionHandle a3, ReflectionHandle a4,
                      ReflectionHandle a5, ReflectionHandle a6,
                      ReflectionHandle a7) = 0;

  const std::vector<std::string> & signature() const { return signature_; }

protected:
  std::vector<std::string> signature_;
};

template<typename T,
         typename A1, typename A2, typename A3,
         typename A4, typename A5, typename A6,
         typename A7>
class ConstructHelper
{
public:
  static T * create(A1 a1, A2 a2, A3 a3,
             A4 a4, A5 a5, A6 a6, A7 a7) { return new T(a1, a2, a3, a4, a5, a6,
                                                        a7); }
};

template<typename T>
class ConstructHelper<T, NoClass, NoClass, NoClass, NoClass, NoClass, NoClass,
      NoClass>
{
public:
  static T * create(NoClass a1 __attribute__((unused)),
                    NoClass a2 __attribute__((unused)),
                    NoClass a3 __attribute__((unused)),
                    NoClass a4 __attribute__((unused)),
                    NoClass a5 __attribute__((unused)),
                    NoClass a6 __attribute__((unused)),
                    NoClass a7 __attribute__((unused))) { return new T; }
};

template<typename T, typename A1>
class ConstructHelper<T, A1, NoClass, NoClass, NoClass, NoClass, NoClass,
      NoClass>
{
public:
  static T * create(A1 a1,
                    NoClass a2 __attribute((unused)),
                    NoClass a3 __attribute((unused)),
                    NoClass a4 __attribute((unused)),
                    NoClass a5 __attribute((unused)),
                    NoClass a6 __attribute((unused)),
                    NoClass a7 __attribute((unused))) { return new T(a1); }
};

template<typename T, typename A1, typename A2>
class ConstructHelper<T, A1, A2, NoClass, NoClass, NoClass, NoClass, NoClass>
{
public:
  static T * create(A1 a1,
                    A2 a2,
                    NoClass a3 __attribute__((unused)),
                    NoClass a4 __attribute__((unused)),
                    NoClass a5 __attribute__((unused)),
                    NoClass a6 __attribute__((unused)),
                    NoClass a7 __attribute__((unused))) { return new T(a1, a2);}
};

template<typename T, typename A1, typename A2, typename A3>
class ConstructHelper<T, A1, A2, A3, NoClass, NoClass, NoClass, NoClass>
{
public:
  static T * create(A1 a1,
                    A2 a2,
                    A3 a3,
                    NoClass a4 __attribute__((unused)),
                    NoClass a5 __attribute__((unused)),
                    NoClass a6 __attribute__((unused)),
                    NoClass a7 __attribute__((unused)))
    { return new T(a1, a2, a3); }
};

template<typename T, typename A1, typename A2, typename A3, typename A4>
class ConstructHelper<T, A1, A2, A3, A4, NoClass, NoClass, NoClass>
{
public:
  static T * create(A1 a1,
                    A2 a2,
                    A3 a3,
                    A4 a4,
                    NoClass a5 __attribute__((unused)),
                    NoClass a6 __attribute__((unused)),
                    NoClass a7 __attribute__((unused)))
    { return new T(a1, a2, a3, a4); }
};

template<typename T, typename A1, typename A2, typename A3, typename A4,
         typename A5>
class ConstructHelper<T, A1, A2, A3, A4, A5, NoClass, NoClass>
{
public:
  static T * create(A1 a1,
                    A2 a2,
                    A3 a3,
                    A4 a4,
                    A5 a5,
                    NoClass a6 __attribute__((unused)),
                    NoClass a7 __attribute__((unused)))
    { return new T(a1, a2, a3, a4, a5); }
};

template<typename T, typename A1, typename A2, typename A3, typename A4,
         typename A5, typename A6>
class ConstructHelper<T, A1, A2, A3, A4, A5, A6, NoClass>
{
public:
  static T * create(A1 a1,
                    A2 a2,
                    A3 a3,
                    A4 a4,
                    A5 a5,
                    A6 a6,
                    NoClass a7 __attribute__((unused)))
    { return new T(a1, a2, a3, a4, a5, a6); }
};

// Constructor with up to 7 arguments
template<typename T,
         typename A1=NoClass, typename A2=NoClass, typename A3=NoClass,
         typename A4=NoClass, typename A5=NoClass, typename A6=NoClass,
         typename A7=NoClass>
struct Constructor : public ConstructorBase
{
  Constructor()
    {
      int numArgs = CountArguments<A1,A2,A3,A4,A5,A6,A7>::value;
      if (numArgs > 0)
        signature_.push_back(TypeIdName<A1>::name());
      if (numArgs > 1)
        signature_.push_back(TypeIdName<A2>::name());
      if (numArgs > 2)
        signature_.push_back(TypeIdName<A3>::name());
      if (numArgs > 3)
        signature_.push_back(TypeIdName<A4>::name());
      if (numArgs > 4)
        signature_.push_back(TypeIdName<A5>::name());
      if (numArgs > 5)
        signature_.push_back(TypeIdName<A6>::name());
      if (numArgs > 6)
        signature_.push_back(TypeIdName<A7>::name());
    }
  virtual void * call(void * data,
                      ReflectionHandle a1, ReflectionHandle a2,
                      ReflectionHandle a3, ReflectionHandle a4,
                      ReflectionHandle a5, ReflectionHandle a6,
                      ReflectionHandle a7) override
    {
      typename GetUnQualifiedType<A1>::BaseType cA1;
      typename GetUnQualifiedType<A2>::BaseType cA2;
      typename GetUnQualifiedType<A3>::BaseType cA3;
      typename GetUnQualifiedType<A4>::BaseType cA4;
      typename GetUnQualifiedType<A5>::BaseType cA5;
      typename GetUnQualifiedType<A6>::BaseType cA6;
      typename GetUnQualifiedType<A7>::BaseType cA7;
      ReflectionWrite(a1, cA1, data);
      ReflectionWrite(a2, cA2, data);
      ReflectionWrite(a3, cA3, data);
      ReflectionWrite(a4, cA4, data);
      ReflectionWrite(a5, cA5, data);
      ReflectionWrite(a6, cA6, data);
      ReflectionWrite(a7, cA7, data);
      return (void*)
        ConstructHelper<T, A1, A2, A3, A4, A5, A6, A7>::create(cA1, cA2, cA3,
                                                           cA4, cA5, cA6, cA7);
    }
};

}

#endif
