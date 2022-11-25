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


#include "RubyException.h"
#include <sstream>
#include <ruby.h>
#include <version.h>

RubyException::RubyException(const std::string & rubyExceptionName,
                             const std::string & backtrace,
                             const std::string & infoString)
  : std::runtime_error(backtrace + "\n" + rubyExceptionName + " : " +
                       infoString)
{
}

void RubyException::checkRubyException(int state) // static
{
  switch (state)
    {
    case 0:
      return;
    case 6:
        {
#if RUBY_VERSION_MAJOR == 1 && RUBY_VERSION_MINOR == 8
          VALUE error = ruby_errinfo;
#else
          VALUE error = rb_errinfo();
#endif
          VALUE eclass = CLASS_OF(error);
          VALUE einfo = rb_obj_as_string(error);
          if (eclass == rb_eRuntimeError && RSTRING_LEN(einfo) == 0)
            {
              throw RubyException("RuntimeError", "?", "unhandled exception");
            }
          else
            {
              VALUE epath = rb_class_path(eclass);
              VALUE info = rb_funcall(error, rb_intern("backtrace"), 0);
              VALUE joined = rb_ary_join(info, rb_str_new2("\n"));

              throw RubyException(RSTRING_PTR(epath),
                                  RSTRING_PTR(joined),
                                  RSTRING_PTR(einfo));
            }
        }
    default:
      throw RubyException("UnknownError", "?", "?");
    }
}
