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

#ifndef rb_protect_wrap_h_
#define rb_protect_wrap_h_

#include <ruby.h>

// Recursive rb_protect calls don't work.  This wrapper makes sure rb_protect is
// called only once. So yes, we are protecting rb_protect against itself.
// recursive rb_protect can happen if C++ calls a ruby function (inside
// rb_protect) and the ruby calls another C++ function which in turn calls
// another ruby function (inside rb_protect)
VALUE rb_protect_wrap(VALUE(*func)(VALUE), VALUE arg, int * status);

void rb_load_protect_wrap(VALUE fname, int wrap, int * state);

VALUE rb_eval_string_protect_wrap(const char * str, int * state);

#endif
