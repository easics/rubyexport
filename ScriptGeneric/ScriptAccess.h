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

#ifndef ScriptAccess_h_
#define ScriptAccess_h_

class ScriptReference;

class ScriptAccess
{
public:
  ScriptAccess();
  ScriptAccess(const ScriptAccess & rhs);
  virtual ~ScriptAccess();

  // Call this instead of just delete
  void deleteFromC();

  void setReference(ScriptReference * reference);
  ScriptReference * getReference() const { return reference_; }

  bool isAlive() const { return alive_ == 0x12345678; }
protected:
  int alive_;
  ScriptReference * reference_;
};

#endif
