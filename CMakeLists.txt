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

cmake_minimum_required(VERSION 3.16.3)
project(rubyexport LANGUAGES CXX)

## Settings
option(SCRIPT_RUBY "Support Ruby scripts" OFF)
option(SCRIPT_PYTHON "Support Python scripts" OFF)

if(SCRIPT_RUBY)
  add_compile_definitions(SCRIPT_RUBY)
endif()

if(SCRIPT_PYTHON)
  add_compile_definitions(SCRIPT_PYTHON)
endif()

if (NOT (SCRIPT_RUBY OR SCRIPT_PYTHON))
  message(FATAL_ERROR "No script support option supplied. Supply either "
                      "SCRIPT_RUBY, SCRIPT_PYTHON or both")
endif()

## Target libraries
add_library(rubyexport STATIC)

## Package dependencies
find_package(Ruby REQUIRED)

if (RUBY_VERSION_MAJOR VERSION_EQUAL "2" AND
    RUBY_VERSION_MINOR VERSION_EQUAL "7")
  message(FATAL_ERROR "Ruby version 2.7.x is not supported")
endif()

find_package(Python REQUIRED COMPONENTS Interpreter Development)

target_include_directories(rubyexport
  PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/ScriptGeneric
    ${CMAKE_CURRENT_SOURCE_DIR}/Reflection/src

  PRIVATE
    ${RUBY_INCLUDE_DIRS}
    ${RUBY_INCLUDE_DIR}/ruby
    ${Python_INCLUDE_DIRS}
)

target_link_libraries(rubyexport ${RUBY_LIBRARY} ${Python_LIBRARY})

## Sources
add_subdirectory(ScriptGeneric)
add_subdirectory(Reflection/src)
