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

#ifndef ReflectionUtil_h_
#define ReflectionUtil_h_

#include <vector>
#include <string>
#include <typeinfo>

namespace Reflection
{

  // Meta-class to get the unqualified type
  // A qualifier for a type is something like const or &
  // The meta class returns the type without the qualifier

template <typename T>
struct GetUnQualifiedType
{
  typedef T BaseType;
};

template <typename T>
struct GetUnQualifiedType<T &>
{
  typedef T BaseType;
};

template <typename T>
struct GetUnQualifiedType<const T &>
{
  typedef T BaseType;
};

  // A empty class used as default template argument in some places
  struct NoClass {};

  // Get typeid(T).name() of a type or an array
template<typename T>
struct TypeIdName
{
  static std::string name() { return typeid(T).name(); }
};

template<typename T>
struct TypeIdName<std::vector<T> >
{
  static std::string name() { return std::string("array ") +
                                     TypeIdName<T>::name(); }
};

template<typename T>
struct TypeIdName<std::vector<T> const &>
{
  static std::string name() { return std::string("array ") +
                                     TypeIdName<T>::name(); }
};

template<typename T>
struct TypeIdName<std::vector<T> &>
{
  static std::string name() { return std::string("array ") +
                                     TypeIdName<T>::name(); }
};

  // Count the number of arguments
  template<typename A1, typename A2, typename A3, typename A4, typename A5,
           typename A6, typename A7>
  struct CountArguments { enum { value = 7 }; };

  template<>
  struct CountArguments<NoClass, NoClass, NoClass, NoClass, NoClass, NoClass,
                        NoClass>
    { enum { value = 0 }; };
  template<typename A1>
  struct CountArguments<A1, NoClass, NoClass, NoClass, NoClass, NoClass,
                        NoClass>
    { enum { value = 1 }; };
  template<typename A1, typename A2>
  struct CountArguments<A1, A2, NoClass, NoClass, NoClass, NoClass, NoClass>
    { enum { value = 2 }; };
  template<typename A1, typename A2, typename A3>
  struct CountArguments<A1, A2, A3, NoClass, NoClass, NoClass, NoClass>
    { enum { value = 3 }; };
  template<typename A1, typename A2, typename A3, typename A4>
  struct CountArguments<A1, A2, A3, A4, NoClass, NoClass, NoClass>
    { enum { value = 4 }; };
  template<typename A1, typename A2, typename A3, typename A4, typename A5>
  struct CountArguments<A1, A2, A3, A4, A5, NoClass, NoClass>
    { enum { value = 5 }; };
  template<typename A1, typename A2, typename A3, typename A4, typename A5,
           typename A6>
  struct CountArguments<A1, A2, A3, A4, A5, A6, NoClass>
    { enum { value = 6 }; };

  // Make a signature
  template<typename A1, typename A2, typename A3, typename A4, typename A5,
           typename A6, typename A7>
  struct MakeSignature
  {
    static std::vector<std::string> make()
      {
        std::vector<std::string> result;
        result.push_back(TypeIdName<A1>::name());
        result.push_back(TypeIdName<A2>::name());
        result.push_back(TypeIdName<A3>::name());
        result.push_back(TypeIdName<A4>::name());
        result.push_back(TypeIdName<A5>::name());
        result.push_back(TypeIdName<A6>::name());
        result.push_back(TypeIdName<A7>::name());
        return result;
      }
  };
  template<>
  struct MakeSignature<NoClass, NoClass, NoClass, NoClass, NoClass, NoClass,
                       NoClass>
  {
    static std::vector<std::string> make()
      {
        return std::vector<std::string>();
      }
  };
  template<typename A1>
  struct MakeSignature<A1, NoClass, NoClass, NoClass, NoClass, NoClass, NoClass>
  {
    static std::vector<std::string> make()
      {
        std::vector<std::string> result;
        result.push_back(TypeIdName<A1>::name());
        return result;
      }
  };
  template<typename A1, typename A2>
  struct MakeSignature<A1, A2, NoClass, NoClass, NoClass, NoClass, NoClass>
  {
    static std::vector<std::string> make()
      {
        std::vector<std::string> result;
        result.push_back(TypeIdName<A1>::name());
        result.push_back(TypeIdName<A2>::name());
        return result;
      }
  };
  template<typename A1, typename A2, typename A3>
  struct MakeSignature<A1, A2, A3, NoClass, NoClass, NoClass, NoClass>
  {
    static std::vector<std::string> make()
      {
        std::vector<std::string> result;
        result.push_back(TypeIdName<A1>::name());
        result.push_back(TypeIdName<A2>::name());
        result.push_back(TypeIdName<A3>::name());
        return result;
      }
  };
  template<typename A1, typename A2, typename A3,
           typename A4>
  struct MakeSignature<A1, A2, A3, A4, NoClass, NoClass, NoClass>
  {
    static std::vector<std::string> make()
      {
        std::vector<std::string> result;
        result.push_back(TypeIdName<A1>::name());
        result.push_back(TypeIdName<A2>::name());
        result.push_back(TypeIdName<A3>::name());
        result.push_back(TypeIdName<A4>::name());
        return result;
      }
  };
  template<typename A1, typename A2, typename A3,
           typename A4, typename A5>
  struct MakeSignature<A1, A2, A3, A4, A5, NoClass, NoClass>
  {
    static std::vector<std::string> make()
      {
        std::vector<std::string> result;
        result.push_back(TypeIdName<A1>::name());
        result.push_back(TypeIdName<A2>::name());
        result.push_back(TypeIdName<A3>::name());
        result.push_back(TypeIdName<A4>::name());
        result.push_back(TypeIdName<A5>::name());
        return result;
      }
  };
  template<typename A1, typename A2, typename A3,
           typename A4, typename A5, typename A6>
  struct MakeSignature<A1, A2, A3, A4, A5, A6, NoClass>
  {
    static std::vector<std::string> make()
      {
        std::vector<std::string> result;
        result.push_back(TypeIdName<A1>::name());
        result.push_back(TypeIdName<A2>::name());
        result.push_back(TypeIdName<A3>::name());
        result.push_back(TypeIdName<A4>::name());
        result.push_back(TypeIdName<A5>::name());
        result.push_back(TypeIdName<A6>::name());
        return result;
      }
  };
}

#endif
