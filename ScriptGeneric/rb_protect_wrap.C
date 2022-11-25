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


#include "rb_protect_wrap.h"

static int recurseLevel = 0;

VALUE rb_protect_wrap(VALUE(*func)(VALUE), VALUE arg, int * status)
{
  if (recurseLevel)
    {
      *status = 0;
      return func(arg);
    }
  else
    {
      ++recurseLevel;
      VALUE result = rb_protect(func, arg, status);
      --recurseLevel;
      return result;
    }
}

void rb_load_protect_wrap(VALUE fname, int wrap, int * state)
{
  if (recurseLevel)
    {
      rb_load(fname, wrap);
      *state = 0;
    }
  else
    {
      ++recurseLevel;
      rb_load_protect(fname, wrap, state);
      --recurseLevel;
    }
}

VALUE rb_eval_string_protect_wrap(const char * str, int * state)
{
  if (recurseLevel)
    {
      *state = 0;
      return rb_eval_string(str);
    }
  else
    {
      ++recurseLevel;
      VALUE result = rb_eval_string_protect(str, state);
      --recurseLevel;
      return result;
    }
}
