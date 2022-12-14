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

## File list
target_sources(rubyexport
  PRIVATE
    ReflectionImplement.C
    ScriptAccess.C
    ScriptReference.C
    RubyPythonReference.C
    ScriptInterface.C
    ScriptObject.C
    ScriptReferenceFactory.C
    ScriptCppArray.C
)

# Only needed for Ruby support
if (SCRIPT_RUBY)
  target_sources(rubyexport
    PRIVATE
      rb_protect_wrap.C
      RubyException.C
  )
endif()

# Only needed for Python support
if (SCRIPT_PYTHON)
  target_sources(rubyexport
    PRIVATE
      PythonException.C
  )
endif()
