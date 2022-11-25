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

#ifndef ReflectionMethod_h_
#define ReflectionMethod_h_

#include "ReflectionImplement.h"
#include "ReflectionUtil.h"
#include <stdexcept>

namespace Reflection
{

template <typename T>
struct ReferenceArgument
{
  static void convert(ReflectionHandle to __attribute__((unused)),
                      T from __attribute__((unused)),
                      void * data __attribute__((unused))) {}
};

template <typename T>
struct ReferenceArgument<T const &>
{
  static void convert(ReflectionHandle to __attribute__((unused)),
                      T const & from __attribute__((unused)),
                      void * data __attribute__((unused))) {}
};

template <typename T>
struct ReferenceArgument<T &>
{
  static void convert(ReflectionHandle to, T & from, void * data)
    {
      ReflectionUpdate(to, from ,data);
    }
};

  // A method of a class
class MethodBase
{
public:
  MethodBase() : isStatic_(false) {}
  virtual ReflectionHandle call(void * self, void * data,
                                ReflectionHandle a1, ReflectionHandle a2,
                                ReflectionHandle a3, ReflectionHandle a4,
                                ReflectionHandle a5, ReflectionHandle a6,
                                ReflectionHandle a7) = 0;
  unsigned int getNumArgs() const { return numFuncArgs_%8; }
  bool isStatic() const { return isStatic_; }
  const std::vector<std::string> & signature() const { return signature_; }

protected:
  // 0-7 : non-const, 8-15 : const
  unsigned int numFuncArgs_;
  bool isStatic_;
  std::vector<std::string> signature_;
};

// Method with up to 6 arguments returning non-void
template<typename T, typename R,
         typename A1=NoClass, typename A2=NoClass, typename A3=NoClass,
         typename A4=NoClass, typename A5=NoClass, typename A6=NoClass,
         typename A7=NoClass>
struct Method : public MethodBase
{
public:
  Method(R (T::*m)())                            : m0_(m)  { numFuncArgs_ =  0;
    signature_ = MakeSignature<A1,A2,A3,A4,A5,A6,A7>::make(); }
  Method(R (T::*m)(A1))                          : m1_(m)  { numFuncArgs_ =  1;
    signature_ = MakeSignature<A1,A2,A3,A4,A5,A6,A7>::make(); }
  Method(R (T::*m)(A1, A2))                      : m2_(m)  { numFuncArgs_ =  2;
    signature_ = MakeSignature<A1,A2,A3,A4,A5,A6,A7>::make(); }
  Method(R (T::*m)(A1, A2, A3))                  : m3_(m)  { numFuncArgs_ =  3;
    signature_ = MakeSignature<A1,A2,A3,A4,A5,A6,A7>::make(); }
  Method(R (T::*m)(A1, A2, A3, A4))              : m4_(m)  { numFuncArgs_ =  4;
    signature_ = MakeSignature<A1,A2,A3,A4,A5,A6,A7>::make(); }
  Method(R (T::*m)(A1, A2, A3, A4, A5))          : m5_(m)  { numFuncArgs_ =  5;
    signature_ = MakeSignature<A1,A2,A3,A4,A5,A6,A7>::make(); }
  Method(R (T::*m)(A1, A2, A3, A4, A5, A6))      : m6_(m)  { numFuncArgs_ =  6;
    signature_ = MakeSignature<A1,A2,A3,A4,A5,A6,A7>::make(); }
  Method(R (T::*m)(A1, A2, A3, A4, A5, A6, A7))  : m7_(m)  { numFuncArgs_ =  7;
    signature_ = MakeSignature<A1,A2,A3,A4,A5,A6,A7>::make(); }

  Method(R (T::*m)() const)                      : m0c_(m) { numFuncArgs_ =  8;
    signature_ = MakeSignature<A1,A2,A3,A4,A5,A6,A7>::make(); }
  Method(R (T::*m)(A1) const)                    : m1c_(m) { numFuncArgs_ =  9;
    signature_ = MakeSignature<A1,A2,A3,A4,A5,A6,A7>::make(); }
  Method(R (T::*m)(A1, A2) const)                : m2c_(m) { numFuncArgs_ = 10;
    signature_ = MakeSignature<A1,A2,A3,A4,A5,A6,A7>::make(); }
  Method(R (T::*m)(A1, A2, A3) const)            : m3c_(m) { numFuncArgs_ = 11;
    signature_ = MakeSignature<A1,A2,A3,A4,A5,A6,A7>::make(); }
  Method(R (T::*m)(A1, A2, A3, A4) const)        : m4c_(m) { numFuncArgs_ = 12;
    signature_ = MakeSignature<A1,A2,A3,A4,A5,A6,A7>::make(); }
  Method(R (T::*m)(A1, A2, A3, A4, A5) const)    : m5c_(m) { numFuncArgs_ = 13;
    signature_ = MakeSignature<A1,A2,A3,A4,A5,A6,A7>::make(); }
  Method(R (T::*m)(A1, A2, A3, A4, A5, A6) const): m6c_(m) { numFuncArgs_ = 14;
    signature_ = MakeSignature<A1,A2,A3,A4,A5,A6,A7>::make(); }
  Method(R (T::*m)(A1, A2, A3, A4,A5,A6,A7) const): m7c_(m) { numFuncArgs_ = 15;
    signature_ = MakeSignature<A1,A2,A3,A4,A5,A6,A7>::make(); }

  virtual ReflectionHandle call(void * self,
                                void * data,
                                ReflectionHandle a1,
                                ReflectionHandle a2,
                                ReflectionHandle a3,
                                ReflectionHandle a4,
                                ReflectionHandle a5,
                                ReflectionHandle a6,
                                ReflectionHandle a7) override
    {
      T * selfCasted = (T*)self;
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
      ReflectionHandle result;
      switch (numFuncArgs_)
        {
        case 0:
          return ReflectionRead((selfCasted->*m0_)(), data);
        case 1:
          result = ReflectionRead((selfCasted->*m1_)(cA1), data);
          ReferenceArgument<A1>::convert(a1, cA1, data);
          return result;
        case 2:
          result = ReflectionRead((selfCasted->*m2_)(cA1, cA2), data);
          ReferenceArgument<A1>::convert(a1, cA1, data);
          ReferenceArgument<A2>::convert(a2, cA2, data);
          return result;
        case 3:
          result = ReflectionRead((selfCasted->*m3_)(cA1, cA2, cA3), data);
          ReferenceArgument<A1>::convert(a1, cA1, data);
          ReferenceArgument<A2>::convert(a2, cA2, data);
          ReferenceArgument<A3>::convert(a3, cA3, data);
          return result;
        case 4:
          result = ReflectionRead((selfCasted->*m4_)(cA1, cA2, cA3, cA4),
                                  data);
          ReferenceArgument<A1>::convert(a1, cA1, data);
          ReferenceArgument<A2>::convert(a2, cA2, data);
          ReferenceArgument<A3>::convert(a3, cA3, data);
          ReferenceArgument<A4>::convert(a4, cA4, data);
          return result;
        case 5:
          result = ReflectionRead((selfCasted->*m5_)(cA1, cA2, cA3, cA4,
                                                     cA5), data);
          ReferenceArgument<A1>::convert(a1, cA1, data);
          ReferenceArgument<A2>::convert(a2, cA2, data);
          ReferenceArgument<A3>::convert(a3, cA3, data);
          ReferenceArgument<A4>::convert(a4, cA4, data);
          ReferenceArgument<A5>::convert(a5, cA5, data);
          return result;
        case 6:
          result = ReflectionRead((selfCasted->*m6_)(cA1, cA2, cA3, cA4,
                                                     cA5, cA6), data);
          ReferenceArgument<A1>::convert(a1, cA1, data);
          ReferenceArgument<A2>::convert(a2, cA2, data);
          ReferenceArgument<A3>::convert(a3, cA3, data);
          ReferenceArgument<A4>::convert(a4, cA4, data);
          ReferenceArgument<A5>::convert(a5, cA5, data);
          ReferenceArgument<A6>::convert(a6, cA6, data);
          return result;
        case 7:
          result = ReflectionRead((selfCasted->*m7_)(cA1, cA2, cA3, cA4,
                                                     cA5, cA6, cA7), data);
          ReferenceArgument<A1>::convert(a1, cA1, data);
          ReferenceArgument<A2>::convert(a2, cA2, data);
          ReferenceArgument<A3>::convert(a3, cA3, data);
          ReferenceArgument<A4>::convert(a4, cA4, data);
          ReferenceArgument<A5>::convert(a5, cA5, data);
          ReferenceArgument<A6>::convert(a6, cA6, data);
          ReferenceArgument<A7>::convert(a7, cA7, data);
          return result;
        case 8:
          return ReflectionRead((selfCasted->*m0c_)(), data);
        case 9:
          result = ReflectionRead((selfCasted->*m1c_)(cA1), data);
          ReferenceArgument<A1>::convert(a1, cA1, data);
          return result;
        case 10:
          result = ReflectionRead((selfCasted->*m2c_)(cA1, cA2), data);
          ReferenceArgument<A1>::convert(a1, cA1, data);
          ReferenceArgument<A2>::convert(a2, cA2, data);
          return result;
        case 11:
          result = ReflectionRead((selfCasted->*m3c_)(cA1, cA2, cA3),data);
          ReferenceArgument<A1>::convert(a1, cA1, data);
          ReferenceArgument<A2>::convert(a2, cA2, data);
          ReferenceArgument<A3>::convert(a3, cA3, data);
          return result;
        case 12:
          result = ReflectionRead((selfCasted->*m4c_)(cA1, cA2, cA3, cA4),
                                  data);
          ReferenceArgument<A1>::convert(a1, cA1, data);
          ReferenceArgument<A2>::convert(a2, cA2, data);
          ReferenceArgument<A3>::convert(a3, cA3, data);
          ReferenceArgument<A4>::convert(a4, cA4, data);
          return result;
        case 13:
          result = ReflectionRead((selfCasted->*m5c_)(cA1, cA2, cA3, cA4,
                                                      cA5), data);
          ReferenceArgument<A1>::convert(a1, cA1, data);
          ReferenceArgument<A2>::convert(a2, cA2, data);
          ReferenceArgument<A3>::convert(a3, cA3, data);
          ReferenceArgument<A4>::convert(a4, cA4, data);
          ReferenceArgument<A5>::convert(a5, cA5, data);
          return result;
        case 14:
          result = ReflectionRead((selfCasted->*m6c_)(cA1, cA2, cA3,cA4,
                                                      cA5, cA6), data);
          ReferenceArgument<A1>::convert(a1, cA1, data);
          ReferenceArgument<A2>::convert(a2, cA2, data);
          ReferenceArgument<A3>::convert(a3, cA3, data);
          ReferenceArgument<A4>::convert(a4, cA4, data);
          ReferenceArgument<A5>::convert(a5, cA5, data);
          ReferenceArgument<A6>::convert(a6, cA6, data);
          return result;
        case 15:
          result = ReflectionRead((selfCasted->*m7c_)(cA1, cA2, cA3,cA4,
                                                      cA5, cA6, cA7), data);
          ReferenceArgument<A1>::convert(a1, cA1, data);
          ReferenceArgument<A2>::convert(a2, cA2, data);
          ReferenceArgument<A3>::convert(a3, cA3, data);
          ReferenceArgument<A4>::convert(a4, cA4, data);
          ReferenceArgument<A5>::convert(a5, cA5, data);
          ReferenceArgument<A6>::convert(a6, cA6, data);
          ReferenceArgument<A7>::convert(a7, cA7, data);
          return result;
        default:
          throw std::runtime_error("Impossible");
        }
    }
private:
  union
    {
      R (T::*m0_)();
      R (T::*m1_)(A1);
      R (T::*m2_)(A1, A2);
      R (T::*m3_)(A1, A2, A3);
      R (T::*m4_)(A1, A2, A3, A4);
      R (T::*m5_)(A1, A2, A3, A4, A5);
      R (T::*m6_)(A1, A2, A3, A4, A5, A6);
      R (T::*m7_)(A1, A2, A3, A4, A5, A6, A7);
      R (T::*m0c_)() const;
      R (T::*m1c_)(A1) const;
      R (T::*m2c_)(A1, A2) const;
      R (T::*m3c_)(A1, A2, A3) const;
      R (T::*m4c_)(A1, A2, A3, A4) const;
      R (T::*m5c_)(A1, A2, A3, A4, A5) const;
      R (T::*m6c_)(A1, A2, A3, A4, A5, A6) const;
      R (T::*m7c_)(A1, A2, A3, A4, A5, A6, A7) const;
    };
};

// Specialisation for void-returning methods
template<typename T,
         typename A1, typename A2, typename A3,
         typename A4, typename A5, typename A6,
         typename A7>
struct Method<T, void, A1, A2, A3, A4, A5, A6, A7> : public MethodBase
{
public:
  Method(void (T::*m)())                         : m0_(m)  { numFuncArgs_ =  0;
    signature_ = MakeSignature<A1,A2,A3,A4,A5,A6,A7>::make(); }
  Method(void (T::*m)(A1))                       : m1_(m)  { numFuncArgs_ =  1;
    signature_ = MakeSignature<A1,A2,A3,A4,A5,A6,A7>::make(); }
  Method(void (T::*m)(A1, A2))                   : m2_(m)  { numFuncArgs_ =  2;
    signature_ = MakeSignature<A1,A2,A3,A4,A5,A6,A7>::make(); }
  Method(void (T::*m)(A1, A2, A3))               : m3_(m)  { numFuncArgs_ =  3;
    signature_ = MakeSignature<A1,A2,A3,A4,A5,A6,A7>::make(); }
  Method(void (T::*m)(A1, A2, A3, A4))           : m4_(m)  { numFuncArgs_ =  4;
    signature_ = MakeSignature<A1,A2,A3,A4,A5,A6,A7>::make(); }
  Method(void (T::*m)(A1, A2, A3, A4, A5))       : m5_(m)  { numFuncArgs_ =  5;
    signature_ = MakeSignature<A1,A2,A3,A4,A5,A6,A7>::make(); }
  Method(void (T::*m)(A1, A2, A3, A4, A5, A6))   : m6_(m)  { numFuncArgs_ =  6;
    signature_ = MakeSignature<A1,A2,A3,A4,A5,A6,A7>::make(); }
  Method(void (T::*m)(A1, A2, A3, A4, A5, A6, A7)) : m7_(m)  { numFuncArgs_ = 7;
    signature_ = MakeSignature<A1,A2,A3,A4,A5,A6,A7>::make(); }

  Method(void (T::*m)() const)                   : m0c_(m) { numFuncArgs_ =  8;
    signature_ = MakeSignature<A1,A2,A3,A4,A5,A6,A7>::make(); }
  Method(void (T::*m)(A1) const)                 : m1c_(m) { numFuncArgs_ =  9;
    signature_ = MakeSignature<A1,A2,A3,A4,A5,A6,A7>::make(); }
  Method(void (T::*m)(A1, A2) const)             : m2c_(m) { numFuncArgs_ = 10;
    signature_ = MakeSignature<A1,A2,A3,A4,A5,A6,A7>::make(); }
  Method(void (T::*m)(A1, A2, A3) const)         : m3c_(m) { numFuncArgs_ = 11;
    signature_ = MakeSignature<A1,A2,A3,A4,A5,A6,A7>::make(); }
  Method(void (T::*m)(A1, A2, A3, A4) const)     : m4c_(m) { numFuncArgs_ = 12;
    signature_ = MakeSignature<A1,A2,A3,A4,A5,A6,A7>::make(); }
  Method(void (T::*m)(A1, A2, A3, A4, A5) const) : m5c_(m) { numFuncArgs_ = 13;
    signature_ = MakeSignature<A1,A2,A3,A4,A5,A6,A7>::make(); }
  Method(void (T::*m)(A1, A2, A3,A4,A5,A6) const): m6c_(m) { numFuncArgs_ = 14;
    signature_ = MakeSignature<A1,A2,A3,A4,A5,A6,A7>::make(); }
  Method(void (T::*m)(A1,A2,A3,A4,A5,A6,A7) const): m7c_(m) { numFuncArgs_ = 15;
    signature_ = MakeSignature<A1,A2,A3,A4,A5,A6,A7>::make(); }

  virtual ReflectionHandle call(void * self,
                                void * data,
                                ReflectionHandle a1,
                                ReflectionHandle a2,
                                ReflectionHandle a3,
                                ReflectionHandle a4,
                                ReflectionHandle a5,
                                ReflectionHandle a6,
                                ReflectionHandle a7) override
    {
      T * selfCasted = (T*)self;
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
      switch (numFuncArgs_)
        {
        case 0:
          (selfCasted->*m0_)();
          break;
        case 1:
          (selfCasted->*m1_)(cA1);
          ReferenceArgument<A1>::convert(a1, cA1, data);
          break;
        case 2:
          (selfCasted->*m2_)(cA1, cA2);
          ReferenceArgument<A1>::convert(a1, cA1, data);
          ReferenceArgument<A2>::convert(a2, cA2, data);
          break;
        case 3:
          (selfCasted->*m3_)(cA1, cA2, cA3);
          ReferenceArgument<A1>::convert(a1, cA1, data);
          ReferenceArgument<A2>::convert(a2, cA2, data);
          ReferenceArgument<A3>::convert(a3, cA3, data);
          break;
        case 4:
          (selfCasted->*m4_)(cA1, cA2, cA3, cA4);
          ReferenceArgument<A1>::convert(a1, cA1, data);
          ReferenceArgument<A2>::convert(a2, cA2, data);
          ReferenceArgument<A3>::convert(a3, cA3, data);
          ReferenceArgument<A4>::convert(a4, cA4, data);
          break;
        case 5:
          (selfCasted->*m5_)(cA1, cA2, cA3, cA4, cA5);
          ReferenceArgument<A1>::convert(a1, cA1, data);
          ReferenceArgument<A2>::convert(a2, cA2, data);
          ReferenceArgument<A3>::convert(a3, cA3, data);
          ReferenceArgument<A4>::convert(a4, cA4, data);
          ReferenceArgument<A5>::convert(a5, cA5, data);
          break;
        case 6:
          (selfCasted->*m6_)(cA1, cA2, cA3, cA4,cA5, cA6);
          ReferenceArgument<A1>::convert(a1, cA1, data);
          ReferenceArgument<A2>::convert(a2, cA2, data);
          ReferenceArgument<A3>::convert(a3, cA3, data);
          ReferenceArgument<A4>::convert(a4, cA4, data);
          ReferenceArgument<A5>::convert(a5, cA5, data);
          ReferenceArgument<A6>::convert(a6, cA6, data);
          break;
        case 7:
          (selfCasted->*m7_)(cA1, cA2, cA3, cA4,cA5, cA6, cA7);
          ReferenceArgument<A1>::convert(a1, cA1, data);
          ReferenceArgument<A2>::convert(a2, cA2, data);
          ReferenceArgument<A3>::convert(a3, cA3, data);
          ReferenceArgument<A4>::convert(a4, cA4, data);
          ReferenceArgument<A5>::convert(a5, cA5, data);
          ReferenceArgument<A6>::convert(a6, cA6, data);
          ReferenceArgument<A7>::convert(a7, cA7, data);
          break;
        case 8:
          (selfCasted->*m0c_)();
          ReferenceArgument<A1>::convert(a1, cA1, data);
          break;
        case 9:
          (selfCasted->*m1c_)(cA1);
          ReferenceArgument<A1>::convert(a1, cA1, data);
          ReferenceArgument<A2>::convert(a2, cA2, data);
          break;
        case 10:
          (selfCasted->*m2c_)(cA1, cA2);
          ReferenceArgument<A1>::convert(a1, cA1, data);
          ReferenceArgument<A2>::convert(a2, cA2, data);
          break;
        case 11:
          (selfCasted->*m3c_)(cA1, cA2, cA3);
          ReferenceArgument<A1>::convert(a1, cA1, data);
          ReferenceArgument<A2>::convert(a2, cA2, data);
          ReferenceArgument<A3>::convert(a3, cA3, data);
          break;
        case 12:
          (selfCasted->*m4c_)(cA1, cA2, cA3, cA4);
          ReferenceArgument<A1>::convert(a1, cA1, data);
          ReferenceArgument<A2>::convert(a2, cA2, data);
          ReferenceArgument<A3>::convert(a3, cA3, data);
          ReferenceArgument<A4>::convert(a4, cA4, data);
          break;
        case 13:
          (selfCasted->*m5c_)(cA1, cA2, cA3, cA4, cA5);
          ReferenceArgument<A1>::convert(a1, cA1, data);
          ReferenceArgument<A2>::convert(a2, cA2, data);
          ReferenceArgument<A3>::convert(a3, cA3, data);
          ReferenceArgument<A4>::convert(a4, cA4, data);
          ReferenceArgument<A5>::convert(a5, cA5, data);
          break;
        case 14:
          (selfCasted->*m6c_)(cA1, cA2, cA3,cA4,cA5,cA6);
          ReferenceArgument<A1>::convert(a1, cA1, data);
          ReferenceArgument<A2>::convert(a2, cA2, data);
          ReferenceArgument<A3>::convert(a3, cA3, data);
          ReferenceArgument<A4>::convert(a4, cA4, data);
          ReferenceArgument<A5>::convert(a5, cA5, data);
          ReferenceArgument<A6>::convert(a6, cA6, data);
          break;
        case 15:
          (selfCasted->*m7c_)(cA1, cA2, cA3,cA4,cA5,cA6,cA7);
          ReferenceArgument<A1>::convert(a1, cA1, data);
          ReferenceArgument<A2>::convert(a2, cA2, data);
          ReferenceArgument<A3>::convert(a3, cA3, data);
          ReferenceArgument<A4>::convert(a4, cA4, data);
          ReferenceArgument<A5>::convert(a5, cA5, data);
          ReferenceArgument<A6>::convert(a6, cA6, data);
          ReferenceArgument<A7>::convert(a7, cA7, data);
          break;
        default:
          throw std::runtime_error("Impossible");
        }
      return ReflectionNil(data);
    }
private:
  union
    {
      void (T::*m0_)();
      void (T::*m1_)(A1);
      void (T::*m2_)(A1, A2);
      void (T::*m3_)(A1, A2, A3);
      void (T::*m4_)(A1, A2, A3, A4);
      void (T::*m5_)(A1, A2, A3, A4, A5);
      void (T::*m6_)(A1, A2, A3, A4, A5, A6);
      void (T::*m7_)(A1, A2, A3, A4, A5, A6, A7);
      void (T::*m0c_)() const;
      void (T::*m1c_)(A1) const;
      void (T::*m2c_)(A1, A2) const;
      void (T::*m3c_)(A1, A2, A3) const;
      void (T::*m4c_)(A1, A2, A3, A4) const;
      void (T::*m5c_)(A1, A2, A3, A4, A5) const;
      void (T::*m6c_)(A1, A2, A3, A4, A5, A6) const;
      void (T::*m7c_)(A1, A2, A3, A4, A5, A6, A7) const;
    };
};

// Static method with up to 7 arguments returning non-void
template<typename R,
         typename A1=NoClass, typename A2=NoClass, typename A3=NoClass,
         typename A4=NoClass, typename A5=NoClass, typename A6=NoClass,
         typename A7=NoClass>
struct Function : public MethodBase
{
public:
  Function(void * m) : functionPointer_((void*)m)
  {
    isStatic_ = true;
    numFuncArgs_ = CountArguments<A1,A2,A3,A4,A5,A6,A7>::value;
    signature_ = MakeSignature<A1,A2,A3,A4,A5,A6,A7>::make();
  }

  virtual ReflectionHandle call(void * self, void * data,
                                ReflectionHandle a1, ReflectionHandle a2,
                                ReflectionHandle a3, ReflectionHandle a4,
                                ReflectionHandle a5, ReflectionHandle a6,
                                ReflectionHandle a7)
    {
      typename GetUnQualifiedType<A1>::BaseType cA1;
      typename GetUnQualifiedType<A2>::BaseType cA2;
      typename GetUnQualifiedType<A3>::BaseType cA3;
      typename GetUnQualifiedType<A4>::BaseType cA4;
      typename GetUnQualifiedType<A5>::BaseType cA5;
      typename GetUnQualifiedType<A6>::BaseType cA6;
      typename GetUnQualifiedType<A6>::BaseType cA7;
      ReflectionWrite(a1, cA1, data);
      ReflectionWrite(a2, cA2, data);
      ReflectionWrite(a3, cA3, data);
      ReflectionWrite(a4, cA4, data);
      ReflectionWrite(a5, cA5, data);
      ReflectionWrite(a6, cA6, data);
      ReflectionWrite(a7, cA7, data);
      ReflectionHandle result;
      switch ((int)CountArguments<A1,A2,A3,A4,A5,A6,A7>::value)
        {
        case 0 :
          return ReflectionRead(((R(*)())functionPointer_)(), data);
        case 1 :
          result = ReflectionRead(((R(*)(A1))functionPointer_)(cA1), data);
          ReferenceArgument<A1>::convert(a1, cA1, data);
          return result;
        case 2 :
          result = ReflectionRead(((R(*)(A1,A2))functionPointer_)(cA1,cA2),
                                  data);
          ReferenceArgument<A1>::convert(a1, cA1, data);
          ReferenceArgument<A2>::convert(a2, cA2, data);
          return result;
        case 3 :
          result = ReflectionRead(((R(*)(A1,A2,A3))functionPointer_)
                                  (cA1,cA2,cA3), data);
          ReferenceArgument<A1>::convert(a1, cA1, data);
          ReferenceArgument<A2>::convert(a2, cA2, data);
          ReferenceArgument<A3>::convert(a3, cA3, data);
          return result;
        case 4 :
          result = ReflectionRead(((R(*)(A1,A2,A3,A4))functionPointer_)
                                  (cA1,cA2,cA3,cA4), data);
          ReferenceArgument<A1>::convert(a1, cA1, data);
          ReferenceArgument<A2>::convert(a2, cA2, data);
          ReferenceArgument<A3>::convert(a3, cA3, data);
          ReferenceArgument<A4>::convert(a4, cA4, data);
          return result;
        case 5 :
          result = ReflectionRead(((R(*)(A1,A2,A3,A4,A5))functionPointer_)
                                  (cA1,cA2,cA3,cA4,cA5), data);
          ReferenceArgument<A1>::convert(a1, cA1, data);
          ReferenceArgument<A2>::convert(a2, cA2, data);
          ReferenceArgument<A3>::convert(a3, cA3, data);
          ReferenceArgument<A4>::convert(a4, cA4, data);
          ReferenceArgument<A5>::convert(a5, cA5, data);
          return result;
        case 6 :
          result = ReflectionRead(((R(*)(A1,A2,A3,A4,A5,A6))functionPointer_)
                                  (cA1,cA2,cA3,cA4,cA5,cA6), data);
          ReferenceArgument<A1>::convert(a1, cA1, data);
          ReferenceArgument<A2>::convert(a2, cA2, data);
          ReferenceArgument<A3>::convert(a3, cA3, data);
          ReferenceArgument<A4>::convert(a4, cA4, data);
          ReferenceArgument<A5>::convert(a5, cA5, data);
          ReferenceArgument<A6>::convert(a6, cA6, data);
          return result;
        case 7 :
          result = ReflectionRead(((R(*)(A1,A2,A3,A4,A5,A6,A7))functionPointer_)
                                  (cA1,cA2,cA3,cA4,cA5,cA6,cA7), data);
          ReferenceArgument<A1>::convert(a1, cA1, data);
          ReferenceArgument<A2>::convert(a2, cA2, data);
          ReferenceArgument<A3>::convert(a3, cA3, data);
          ReferenceArgument<A4>::convert(a4, cA4, data);
          ReferenceArgument<A5>::convert(a5, cA5, data);
          ReferenceArgument<A6>::convert(a6, cA6, data);
          ReferenceArgument<A7>::convert(a7, cA7, data);
        }
      return ReflectionNil(data);
    }
private:
  void * functionPointer_;
};

// Static method with up to 7 arguments returning void
template<typename A1, typename A2, typename A3,
         typename A4, typename A5, typename A6,
         typename A7>
struct Function<void,A1,A2,A3,A4,A5,A6,A7> : public MethodBase
{
public:
  Function(void * m) : functionPointer_((void*)m)
  {
    isStatic_ = true;
    numFuncArgs_ = CountArguments<A1,A2,A3,A4,A5,A6,A7>::value;
    signature_ = MakeSignature<A1,A2,A3,A4,A5,A6,A7>::make();
  }

  virtual ReflectionHandle call(void * self, void * data,
                                ReflectionHandle a1, ReflectionHandle a2,
                                ReflectionHandle a3, ReflectionHandle a4,
                                ReflectionHandle a5, ReflectionHandle a6,
                                ReflectionHandle a7)
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
      switch ((int)CountArguments<A1,A2,A3,A4,A5,A6,A7>::value)
        {
        case 0 :
          ((void(*)())functionPointer_)();
          break;
        case 1 :
          ((void(*)(A1))functionPointer_)(cA1);
          ReferenceArgument<A1>::convert(a1, cA1, data);
          break;
        case 2 :
          ((void(*)(A1,A2))functionPointer_)(cA1,cA2);
          ReferenceArgument<A1>::convert(a1, cA1, data);
          ReferenceArgument<A2>::convert(a2, cA2, data);
          break;
        case 3 :
          ((void(*)(A1,A2,A3))functionPointer_)(cA1,cA2,cA3);
          ReferenceArgument<A1>::convert(a1, cA1, data);
          ReferenceArgument<A2>::convert(a2, cA2, data);
          ReferenceArgument<A3>::convert(a3, cA3, data);
          break;
        case 4 :
          ((void(*)(A1,A2,A3,A4))functionPointer_)(cA1,cA2,cA3,cA4);
          ReferenceArgument<A1>::convert(a1, cA1, data);
          ReferenceArgument<A2>::convert(a2, cA2, data);
          ReferenceArgument<A3>::convert(a3, cA3, data);
          ReferenceArgument<A4>::convert(a4, cA4, data);
          break;
        case 5 :
          ((void(*)(A1,A2,A3,A4,A5))functionPointer_)(cA1,cA2,cA3,cA4,cA5);
          ReferenceArgument<A1>::convert(a1, cA1, data);
          ReferenceArgument<A2>::convert(a2, cA2, data);
          ReferenceArgument<A3>::convert(a3, cA3, data);
          ReferenceArgument<A4>::convert(a4, cA4, data);
          ReferenceArgument<A5>::convert(a5, cA5, data);
          break;
        case 6 :
          ((void(*)(A1,A2,A3,A4,A5,A6))functionPointer_)
            (cA1,cA2,cA3,cA4,cA5,cA6);
          ReferenceArgument<A1>::convert(a1, cA1, data);
          ReferenceArgument<A2>::convert(a2, cA2, data);
          ReferenceArgument<A3>::convert(a3, cA3, data);
          ReferenceArgument<A4>::convert(a4, cA4, data);
          ReferenceArgument<A5>::convert(a5, cA5, data);
          ReferenceArgument<A6>::convert(a6, cA6, data);
          break;
        case 7 :
          ((void(*)(A1,A2,A3,A4,A5,A6,A7))functionPointer_)
            (cA1,cA2,cA3,cA4,cA5,cA6,cA7);
          ReferenceArgument<A1>::convert(a1, cA1, data);
          ReferenceArgument<A2>::convert(a2, cA2, data);
          ReferenceArgument<A3>::convert(a3, cA3, data);
          ReferenceArgument<A4>::convert(a4, cA4, data);
          ReferenceArgument<A5>::convert(a5, cA5, data);
          ReferenceArgument<A6>::convert(a6, cA6, data);
          ReferenceArgument<A7>::convert(a7, cA7, data);
          break;
        }
      return ReflectionNil(data);
    }
private:
  void * functionPointer_;
};
}

#endif
