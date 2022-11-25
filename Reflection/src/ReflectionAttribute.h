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

#ifndef ReflectionAttribute_h_
#define ReflectionAttribute_h_

#include "ReflectionImplement.h"

namespace Reflection
{

  // An attribute (member) of a class

class AttributeBase
{
public:
  virtual ReflectionHandle getter(void * self, void * data) = 0;
  virtual ReflectionHandle setter(void * self, void * data,
                                  ReflectionHandle value) = 0;
};

template<typename T, typename A>
class Attribute : public AttributeBase
{
public:
  Attribute(A T:: * a) : a_(a) {}

  virtual ReflectionHandle getter(void * self, void * data) override
    {
      T * selfCasted = (T*)self;
      return ReflectionRead(*selfCasted.*a_, data);
    }
  virtual ReflectionHandle setter(void * self, void * data,
                                  ReflectionHandle value) override
    {
      T * selfCasted = (T*)self;
      ReflectionWrite(value, *selfCasted.*a_, data);
      return value;
    }

private:
  A T::* a_;
};

}

#endif
