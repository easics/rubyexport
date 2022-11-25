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

#ifndef ScriptReference_h_
#define ScriptReference_h_

class AbstractPointer;

class ScriptReference
{
public:
  // This class becomes owner of cppObject !
  ScriptReference(AbstractPointer * cppObject);

  // Call this when C++ no longer needs the object this reference refers to
  // (instead of calling delete)
  virtual void deleteFromC() = 0;

  virtual void deleteFromScript(void * data) = 0;

  // Indicate that C++ holds a pointer to this object somewhere
  virtual void useInC() = 0;

  bool isUsedInC() const { return usedInC_ != 0; }

  AbstractPointer * getCppObject() const;

protected:
  // delete not allowed, you should call deleteFromC(), that will call delete
  // when appropriate
  virtual ~ScriptReference() {}

  AbstractPointer * cppObject_;
  // For ruby this is equal to VALUE being stored in the rubyHash_
  // For python this is equal to the pyObject being Py_INCREF'd
  // And it is a reference count, i.e. how many times it has been (stored in
  // rubyHash_ | Py_INCREF'd)
  unsigned int usedInC_;
};

#endif
