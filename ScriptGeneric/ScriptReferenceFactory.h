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

#ifndef ScriptReferenceFactory_h_
#define ScriptReferenceFactory_h_

#include "Singleton.h"
#include "ConcretePointer.h"
#include <typeinfo>

class ScriptReference;

class ScriptReferenceFactory : public Singleton<ScriptReferenceFactory>
{
public:
  template<typename T> ScriptReference * make(T * cppObject,
                                              bool fromSharedPtr);

private:
  ScriptReference * makeGeneric(AbstractPointer * cppObject,
                                const char * typeid_name);
};

template<typename T>
ScriptReference * ScriptReferenceFactory::make(T * cppObject,
                                               bool fromSharedPtr)
{
  return makeGeneric(new ConcretePointer<T>(cppObject, fromSharedPtr),
                     typeid(T).name());
}

#endif
