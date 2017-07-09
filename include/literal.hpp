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
