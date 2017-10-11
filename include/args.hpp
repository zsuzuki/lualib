//
// 2017/06/24 Y.Suzuki
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
