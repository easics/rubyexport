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

#ifndef ReflectionClassBase_h_
#define ReflectionClassBase_h_

#include <vector>
#include <unordered_map>
#include <string>

class ReflectionClassInfo;

namespace Reflection
{

class AttributeBase;
class MethodBase;
class ConstructorBase;
class EnumBase;

class ClassBase
{
public:
  typedef std::unordered_map<std::string, AttributeBase*> AttributeMap;
  typedef std::unordered_multimap<std::string, MethodBase*> MethodMap;
  typedef std::vector<ConstructorBase*> ConstructorArray;
  typedef std::vector<EnumBase*> EnumArray;

  ClassBase() = delete;
  ClassBase(std::string && name);
  ClassBase(ClassBase && rhs);
  ClassBase(const ClassBase & rhs) = delete;
  ~ClassBase();
  ClassBase & operator=(const ClassBase & rhs) = delete;

  std::string getName() const;
  virtual std::string getTypeIdName() const = 0;
  virtual std::string getPointerTypeIdName() const = 0;
  ClassBase * getParent1() const;
  ClassBase * getParent2() const;
  ReflectionClassInfo * getClassInfo() const;

  AttributeMap * getAttributeMap() const { return attributeMap_; }
  MethodMap * getMethodMap() const { return methodMap_; }
  ConstructorArray * getConstructorArray() const { return constructorArray_; }
  EnumArray * getEnumArray() const { return enumArray_; }

protected:
  std::string name_;
  ClassBase * parent1_;
  ClassBase * parent2_;
  ReflectionClassInfo * classInfo_;
  AttributeMap * attributeMap_;
  MethodMap * methodMap_;
  ConstructorArray * constructorArray_;
  EnumArray * enumArray_;

private:
  friend class Registry;

  void addParent(ClassBase * parent);
};

}

#endif
