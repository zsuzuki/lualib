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

public:
  Args(lua_State* lua) : L(lua) { num = -lua_gettop(L); }

  lua_Integer getInteger(int n) const { return luaL_checkinteger(L, num + n); }
  lua_Number getNumber(int n) const { return luaL_checknumber(L, num + n); }
  bool getBoolean(int n) const { return lua_toboolean(L, num + n); }
  const char* getString(int n) const { return luaL_checkstring(L, num + n); }
  template <typename Ty>
  Ty* getUserData(int n, const char* name) const
  {
    return reinterpret_cast<Ty*>(luaL_checkudata(L, num + n, name));
  }
};

} // namespace LUA
