# This file is part of rubyexport.
#
# rubyexport is free software: you can redistribute it and/or modify it under
# the terms of the GNU General Public License as published by the Free Software
# Foundation, either version 3 of the License, or (at your option) any later
# version.
#
# rubyexport is distributed in the hope that it will be useful, but WITHOUT ANY
# WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
# PARTICULAR PURPOSE. See the GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License along with
# rubyexport. If not, see <https://www.gnu.org/licenses/>.

#! /usr/bin/env ruby
# This script takes a C++ class header as its input,
# and creates a ScriptForward class that inherits from the given class.
#
# For each function of the base class, the ScriptForward class has:
# - function:
#   if functionScript exists in Ruby, it calls this function
#   else it calls function of the base class
# - functionScript: it calls function of the base class
#
# Note on the functions in the base class:
# - each function can have only 1 argument
# - the return type of each function can only be 1 word
# - the destructor of the base class will be ignored
# - it is assumed that the base class has no constructor
#
# The ScriptForward class also holds its own Reflection define_class,
# with all functionScript functions
#
# Note: the base class should not be accessible directly from Ruby!

$functionInfos    = []
$classesToInclude = []

def parseHeader(headerfilename)
  headerfile = open(headerfilename, 'r')
  header = File.basename(headerfilename, ".h")

  baseClasses = []
  fullLine = ""
  headerfile.readlines.each { |line|
    line.strip!
    line.sub!(/\/\/.*/, '') # remove comment
    if line.start_with?("class")
      if !line.index(";").nil?   # forward declaration
        line["class "] = ""
        line.chomp! ";"
        $classesToInclude << line
      else   # class definition, check inheritance
        line.sub!(/class #{header}[ ]*:[ ]*/, '')
        baseclassWords = line.split(" ")
        baseclassWords.each { |classword|
          if classword != "public"
            classword.chomp! ","
            if classword != "ScriptAccess"
              baseClasses << classword
            end
          end
        }
      end
    else
      if (line.start_with?("#") || line.end_with?(":"))
        fullLine = ""
      else
        fullLine += " " + line
        if line.end_with?(";", "}")
          fullLine.strip!
          if fullLine.start_with?("virtual ") and fullLine.index("~").nil?
            fullLine["virtual "] = ""
            fullLine.sub!(" override", "")
            fullLine.chomp! ";"
            fullLine.chomp! "{}"
            fullLine.strip!

            # Find returnType, functionName, argumentType and argumentName
            words = fullLine.split(" ", 2)   # Assuming return type is 1 word
            returnType   = words[0]
            functionName = words[1].sub(/([^(]+).+/, '\1') # Everything before (
            splittedfunction = words[1].split(" ")
            argumentName = splittedfunction[-1].chomp ")"
            argumentType = (words[1].chomp ")").chomp " #{argumentName}"
            argumentType[functionName + "("] = ""

            if $functionInfos.detect{|info|info.functionName==functionName}.nil?
              functioninfo = FunctionInfo.new(functionName, returnType,
                                              argumentType, argumentName)
              $functionInfos << functioninfo
            end
          end
          fullLine = ""
        end
      end
    end
  }

  baseClasses.each { |base|
    parseHeader("#{File.dirname(headerfilename)}/#{base}.h")
  }
end

def makeIncludes(classnames)
  includes = ""
  classnames.each { |classname|
    includes += "#include \"#{classname}.h\"\n"
  }
  return includes
end

def makeMethodsPrototypes(functionInfo)
  prototypes = ""
  prototypes << <<EOS
  virtual #{functionInfo.returnType} #{functionInfo.functionName
  }(#{functionInfo.argumentType} #{functionInfo.argumentName}) override;
  virtual #{functionInfo.returnType} #{functionInfo.functionName
  }Script(#{functionInfo.argumentType} #{functionInfo.argumentName});
EOS
  return prototypes
end

def makeMethodsImplementations(header, functionInfo)
  implementations = ""
  implementations << <<EOS
#{functionInfo.returnType} #{header}ScriptForward::#{functionInfo.functionName
  }(#{functionInfo.argumentType} #{functionInfo.argumentName})
{
  if (ScriptObject::hasFunction("#{functionInfo.functionName}Script", true))
    ScriptObject::call("#{functionInfo.functionName}Script", #{
    functionInfo.argumentName}, result_);
  else
    #{header}::#{functionInfo.functionName}(#{functionInfo.argumentName});
}

#{functionInfo.returnType} #{header}ScriptForward::#{functionInfo.functionName
      }Script(#{functionInfo.argumentType} #{functionInfo.argumentName})
{
  #{header}::#{functionInfo.functionName}(#{functionInfo.argumentName});
}

EOS
  return implementations
end

def makeMethodsExportDefines(functionName)
  return "\n  .DEF_F(#{functionName}Script)"
end


# Check parameter
if ARGV.size == 0
  puts "Please give the C++ header file to create the ScriptForward class for"
  exit 1
elsif ARGV.size > 1
  puts
    "Please only give 1 C++ header file to create the ScriptForward class for"
  exit 1
end

# Parse header
headerfilename = ARGV[0]

FunctionInfo =
  Struct.new(:functionName, :returnType, :argumentType, :argumentName)

parseHeader(headerfilename)

# Generate method parts
methodsPrototypes      = ""
methodsImplementations = ""
methodExportDefines    = ""
header = File.basename(headerfilename, ".h")

$functionInfos.each { |functioninfo|
  methodsPrototypes << makeMethodsPrototypes(functioninfo)
  methodsImplementations << makeMethodsImplementations(header, functioninfo)
  methodExportDefines << makeMethodsExportDefines(functioninfo.functionName)
}

# Making files for ScriptForward class
headerout = open(header + "ScriptForward.h", 'w')
cppout = open(header + "ScriptForward.C", 'w')

# Writing header
headerout << <<EOF
#ifndef #{header}ScriptForward_h_
#define #{header}ScriptForward_h_

#include "#{header}.h"
#include "ScriptObject.h"

class #{header}ScriptForward : public #{header}, public ScriptObject
{
public:
  #{header}ScriptForward();
  ~#{header}ScriptForward();

#{methodsPrototypes}
private:
  ScriptObject result_;
};

#endif
EOF
headerout.close

# Writing implementation
cppout << <<EOF

#include "#{header}ScriptForward.h"
#{makeIncludes($classesToInclude)}
#include "ReflectionRegistry.h"

#{header}ScriptForward::#{header}ScriptForward()
{
}

#{header}ScriptForward::~#{header}ScriptForward()
{
}

#{methodsImplementations}
REFLECT_CLASS_DERIVED(#{header}ScriptForward, #{header})
  .def_c(Reflection::init<>())#{methodExportDefines};
}
EOF
cppout.close

