//
// Copyright 2017 Yoshinori Suzuki<wave.suzuki.z@gmail.com>
//
// Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated
// documentation files (the "Software"), to deal in the Software without restriction, including without limitation the
// rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to
// permit persons to whom the Software is furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all copies or substantial portions of the
// Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
// WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
// COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR
// OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.//
//
// ---
// lua in stack argument utility
//
#pragma once

#include <algorithm>
#include <lua.hpp>

namespace LUA
{

class Args
{
  lua_State* L;
  int        num;

  int geti(int n) const { return (n >= num) ? 1 : n + 1; }

public:
  Args(lua_State* lua) : L(lua) { num = lua_gettop(L); }

  lua_Integer getInteger(int n) const { return luaL_checkinteger(L, geti(n)); }
  lua_Number getNumber(int n) const { return luaL_checknumber(L, geti(n)); }
  bool getBoolean(int n) const { return lua_toboolean(L, geti(n)); }
  const char* getString(int n) const { return luaL_checkstring(L, geti(n)); }
  template <typename Ty>
  Ty* getUserData(int n, const char* name) const
  {
    return reinterpret_cast<Ty*>(luaL_checkudata(L, geti(n), name));
  }
};

} // namespace LUA
