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
// lua literal utility
//
#pragma once

#include <lua.hpp>

namespace LUA
{

class Literal
{
  union Data {
    lua_Integer l_int;
    lua_Number  l_num;
    const char* l_str;
  };
  using StoreFunc = void (*)(lua_State* L, Data& d);

  const char* name;
  Data        data;
  StoreFunc   store;

public:
  struct Integer
  {
    lua_Integer l_int;
    Integer(lua_Integer li) : l_int(li) {}
    static void store(lua_State* L, Data& d) { lua_pushinteger(L, d.l_int); }
  };
  struct Number
  {
    lua_Number l_num;
    Number(lua_Number ln) : l_num(ln) {}
    static void store(lua_State* L, Data& d) { lua_pushnumber(L, d.l_num); }
  };
  struct String
  {
    const char* str;
    String(const char* s) : str(s) {}
    static void store(lua_State* L, Data& d) { lua_pushstring(L, d.l_str); }
  };
  struct Boolean
  {
    bool l_bool;
    Boolean(bool b) : l_bool(b) {}
    static void store(lua_State* L, Data& d) { lua_pushboolean(L, d.l_int); }
  };

  Literal() {}
  Literal(const char* n, Integer i) : name(n), store(i.store) { data.l_int = i.l_int; }
  Literal(const char* n, Number m) : name(n), store(m.store) { data.l_num = m.l_num; }
  Literal(const char* n, String s) : name(n), store(s.store) { data.l_str = s.str; }
  Literal(const char* n, Boolean b) : name(n), store(b.store) { data.l_int = b.l_bool ? 1 : 0; }

  void storeLiteral(lua_State* L)
  {
    lua_pushstring(L, name);
    store(L, data);
    lua_rawset(L, -3);
  }
};

} // namespace LUA
