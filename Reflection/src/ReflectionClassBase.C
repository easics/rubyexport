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


#include "ReflectionClassBase.h"
#include <stdexcept>

namespace Reflection
{

ClassBase::ClassBase(std::string && name)
  : name_(std::move(name)), parent1_(nullptr), parent2_(nullptr)
{
  attributeMap_ = new AttributeMap;
  methodMap_ = new MethodMap;
  constructorArray_ = new ConstructorArray;
  enumArray_ = new EnumArray;
}

ClassBase::ClassBase(ClassBase && rhs)
  : name_(std::move(rhs.name_)),
    parent1_(rhs.parent1_),
    parent2_(rhs.parent2_),
    attributeMap_(rhs.attributeMap_),
    methodMap_(rhs.methodMap_),
    constructorArray_(rhs.constructorArray_),
    enumArray_(rhs.enumArray_)
{
  rhs.attributeMap_ = nullptr;
  rhs.methodMap_ = nullptr;
  rhs.constructorArray_ = nullptr;
  rhs.enumArray_ = nullptr;
}

ClassBase::~ClassBase()
{
  delete attributeMap_;
  delete methodMap_;
  delete constructorArray_;
  delete enumArray_;
}

std::string ClassBase::getName() const
{
  return name_;
}

ClassBase * ClassBase::getParent1() const
{
  return parent1_;
}

ClassBase * ClassBase::getParent2() const
{
  return parent2_;
}

ReflectionClassInfo * ClassBase::getClassInfo() const
{
  return classInfo_;
}

void ClassBase::addParent(ClassBase * parent)
{
  if (!parent1_)
    parent1_ = parent;
  else if (!parent2_)
    parent2_ = parent;
  else
    throw std::runtime_error("Too many parents");
}

}
