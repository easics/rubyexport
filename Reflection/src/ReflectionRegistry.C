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


#include "ReflectionRegistry.h"
#include <algorithm>
#include <unordered_map>

Reflection::Registry * Reflection::Registry::instance_ = nullptr;

Reflection::Registry & Reflection::Registry::instance()
{
  if (!instance_)
    instance_ = new Registry;
  return *instance_;
}

Reflection::Registry::Registry()
{
}

Reflection::Registry::~Registry()
{
}

void Reflection::Registry::registerClass(InitFunction f,
                                         const std::string & className,
                                         const std::string & classTypeIdName,
                                         const std::string & pointerClassTypeIdName)
{
  initFunctions_[classTypeIdName] = f;
  pointerToObjectMap_[pointerClassTypeIdName] = classTypeIdName;
  typeidToClassname_[classTypeIdName] = className;
}

void Reflection::Registry::registerClass(InitFunction f,
                                         const std::string & className,
                                         const std::string & classTypeIdName,
                                         const std::string & pointerClassTypeIdName,
                                         const std::string & parent)
{
  initFunctions_[classTypeIdName] = f;
  inheritanceMap_.insert(std::make_pair(classTypeIdName, parent));
  pointerToObjectMap_[pointerClassTypeIdName] = classTypeIdName;
  typeidToClassname_[classTypeIdName] = className;
}

namespace // anonymous
{
  // Helper stuff for topological sort
typedef std::vector<std::string> StringArray;
struct Node
{
  Node() : incomingConnections(0), visited(false) {}
  unsigned int incomingConnections;
  StringArray outgoingConnections;
  bool visited;
};

typedef std::map<std::string, Node> NodeMap;
NodeMap nodes;
StringArray L;
StringArray S;

void visit(const std::string & className)
{
  Node n = nodes[className];
  if (!n.visited)
    {
      n.visited = true;
      for (auto m : n.outgoingConnections)
        visit(m);
      L.push_back(className);
    }
}

}

void Reflection::Registry::init()
{
  // First to topological sort of classes, so the base classes are initialized
  // before the derived classes.

  // Topological sort algorithm (depth-first version)
  // L = Empty list that will contain the sorted nodes
  // S = Set of all nodes with no incoming edges
  // for each node n in S do
  //     visit(n)
  // function visit(node n)
  //     if n has not been visited yet then
  //         mark n as visited
  //         for each node m with an edge from n to m do
  //             visit(m)
  //         add n to L

  // First need to get the data in the proper format
  if (!inheritanceMap_.empty())
    {
      for (auto classPair : inheritanceMap_)
        {
          Node derived = nodes[classPair.first];
          derived.incomingConnections++;
          nodes[classPair.first] = derived;
          Node base = nodes[classPair.second];
          base.outgoingConnections.push_back(classPair.first);
          nodes[classPair.second] = base;
        }

      for (auto node : nodes)
        {
          if (node.second.incomingConnections == 0)
            S.push_back(node.first);
        }

      for (auto n : S)
        visit(n);
    }

  for (auto klass : initFunctions_)
    {
      if (nodes.find(klass.first) == nodes.end())
        L.push_back(klass.first);
    }

  // Reverse L now
  std::reverse(L.begin(), L.end());

  // remove duplicates from multiple inheritance
  for (auto className=L.begin(); className!=L.end();)
    {
      bool removedOne = false;
      for (auto className2=className; className2!=L.end(); ++className2)
        {
          if (className == className2)
            {
              continue;
            }
          if (*className == *className2)
            {
              className = L.erase(className);
              removedOne = true;
              break;
            }
        }
      if (!removedOne)
        ++className;
    }

  // Call init functions
  for (auto className : L)
    {
      auto f = initFunctions_.find(className);
      if (f == initFunctions_.end())
        throw std::runtime_error("Class " + className + " is not reflected,\n"
                                 "but is used as a base clase for a reflected "
                                 "class\n"
                                 "You have to reflect " + className);
      auto klass = f->second();
      classMap_[className] = klass;
      classes_.push_back(klass);
    }

  // Set parent pointers in ClassBase
  for (auto classPair : inheritanceMap_)
    {
      auto derived = classPair.first;
      auto base = classPair.second;
      classMap_[derived]->addParent(classMap_[base]);
    }
}

Reflection::Registry::ClassArray Reflection::Registry::getClasses() const
{
  return classes_;
}

bool Reflection::Registry::isInheritedFrom(const std::string & derived,
                                           const std::string & base) const
{
  if (derived == base)
    return true;
  auto needles = inheritanceMap_.equal_range(derived);
  if (needles.first == inheritanceMap_.end())
    return false;
  for (auto needle = needles.first; needle != needles.second; ++needle)
    {
      bool isInherited = isInheritedFrom(needle->second, base);
      if (isInherited)
        return true;
    }
  return false;
}

std::string
Reflection::Registry::typeidNameToClassname(const std::string & typeidName)const
{
  auto needle = typeidToClassname_.find(typeidName);
  if (needle != typeidToClassname_.end())
    return needle->second;
  return std::string();
}

std::string
Reflection::Registry::pointerToClassname(const std::string & pointerName) const
{
  auto needle = pointerToObjectMap_.find(pointerName);
  if (needle != pointerToObjectMap_.end())
    return needle->second;
  return std::string();
}

Reflection::ClassBase *
Reflection::Registry::getClass(const std::string & name) const
{
  auto needle = classMap_.find(name);
  if (needle == classMap_.end())
    return nullptr;
  else
    return needle->second;
}

Reflection::ClassBase * Reflection_Registry_getClass(const std::string & name)
{
  return Reflection::Registry::instance().getClass(name);
}
