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

#ifndef AbstractPointer_h_
#define AbstractPointer_h_

#include <string>

// A wrapper for a virtual destructor
class AbstractPointer
{
public:
  AbstractPointer() {}
  virtual ~AbstractPointer() {}

  virtual void * get() const = 0;
  virtual std::string typeidName() const = 0;
};

#endif
