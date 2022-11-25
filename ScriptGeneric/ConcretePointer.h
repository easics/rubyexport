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

#ifndef ConcretePointer_h_
#define ConcretePointer_h_

#include "AbstractPointer.h"
#include <memory>
#include <typeinfo>

/// Pointer to an object with an optional shared_ptr to prevent deletion in C++
/// when the object is still in use in the scripting language.
template<typename T>
class ConcretePointer : public AbstractPointer
{
public:
  ConcretePointer(T * object, bool fromSharedPtr);
  ~ConcretePointer();

  T * operator->() const { return object_; }
  T * operator*() const { return object_; }

  virtual void * get() const override { return object_; }
  virtual std::string typeidName() const { return typeid(T).name(); }

private:
  // When the shared pointer is in use, both of these point to the same thing.
  T * object_;
  std::shared_ptr<T> sharedPtr_;
};

#include "ConcretePointer.C"

#endif
