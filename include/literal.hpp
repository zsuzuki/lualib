//
// 2017/06/24 Y.Suzuki
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

  Literal() {}
  Literal(const char* n, Integer i) : name(n), store(i.store) { data.l_int = i.l_int; }
  Literal(const char* n, Number m) : name(n), store(m.store) { data.l_num = m.l_num; }
  Literal(const char* n, String s) : name(n), store(s.store) { data.l_str = s.str; }

  void storeLiteral(lua_State* L)
  {
    lua_pushstring(L, name);
    store(L, data);
    lua_rawset(L, -3);
  }
};

} // namespace LUA
