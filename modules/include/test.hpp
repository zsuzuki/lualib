//
// 2017 Y.Suzuki
//
#pragma once

#include <lua.hpp>
#include <memory>

namespace TEST
{

class Impl;

struct Test
{
  std::shared_ptr<Impl> impl;

public:
  Test();
  ~Test();

  static bool LuaSetup(lua_State* L);
};

} // namespace TEST
