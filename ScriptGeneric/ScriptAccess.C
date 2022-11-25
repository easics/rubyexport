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


#include "ScriptAccess.h"
#include "ScriptReference.h"
#include <cassert>

ScriptAccess::ScriptAccess()
  : alive_(0x12345678), reference_(nullptr)
{
}

ScriptAccess::ScriptAccess(const ScriptAccess & rhs)
  : alive_(0x12345678), reference_(nullptr)
{
}

ScriptAccess::~ScriptAccess()
{
  assert(alive_ == 0x12345678);
  alive_ = 0;
  // Don't delete reference_, reference_ is deleting us
  reference_ = 0;
}

void ScriptAccess::deleteFromC()
{
  if (reference_)
    {
      ScriptReference * ref = reference_;
      // reference_ can be set to 0 by our destructor, which can be called from
      // deleteFromC()
      ref->deleteFromC();
    }
  else
    // Not exported to scripting language, not reference counted, delete it
    // directly
    delete this;
}

void ScriptAccess::setReference(ScriptReference * reference)
{
  reference_ = reference;
}

