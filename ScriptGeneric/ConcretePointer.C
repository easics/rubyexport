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


#include "ConcretePointer.h"
#include <type_traits>
#include <stdexcept>

template<typename T, bool isDerivedFromEnableSharedPtr>
struct MakeSharedPointerHelper {};

template<typename T>
struct MakeSharedPointerHelper<T,false>
{
  static std::shared_ptr<T> make(T * object __attribute__((unused)))
    {
      throw std::runtime_error(
        "Class is not derived from std::enable_shared_from_this,\n"
        "so you can't use it with shared_ptr");
    }
};

template<typename T>
struct MakeSharedPointerHelper<T,true>
{
  static std::shared_ptr<T> make(T * object)
    {
      return object->shared_from_this();
    }
};

template<typename T>
ConcretePointer<T>::ConcretePointer(T * object, bool fromSharedPtr)
  : object_(object), sharedPtr_(0)
{
  if (fromSharedPtr)
    sharedPtr_ =
      MakeSharedPointerHelper<T,
       std::is_base_of<std::enable_shared_from_this<T>,T>::value>::make(object);
}

template<typename T>
ConcretePointer<T>::~ConcretePointer()
{
  if (sharedPtr_)
    sharedPtr_.reset();
  else
    delete object_;
}

