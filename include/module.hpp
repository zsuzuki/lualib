//
// 2017/06/24 Y.Suzuki
//
#pragma once

#include <algorithm>
#include <vector>

#include <lua.hpp>

#include "args.hpp"
#include "literal.hpp"

namespace LUA
{

template <class Impl, void init_func(Impl*, Args&), const char* get_name()>
class ModuleSetup
{
protected:
  using FunctionList = std::vector<luaL_Reg>;
  using LiteralList  = std::vector<Literal>;
  using LuaFuncion   = int (*)(lua_State*);

  FunctionList method_list{};
  FunctionList func_list{};
  LiteralList  literal_list{};

  bool is_initialized_ = false;

  void storeLiteral(const char* name, Literal::Integer lit) { literal_list.emplace_back(Literal{name, lit}); }
  void storeLiteral(const char* name, Literal::Number lit) { literal_list.emplace_back(Literal{name, lit}); }
  void storeLiteral(const char* name, Literal::String lit) { literal_list.emplace_back(Literal{name, lit}); }
  void storeLiteral(const char* name, Literal::Boolean lit) { literal_list.emplace_back(Literal{name, lit}); }

  void storeMethod(const char* name, LuaFuncion func) { method_list.emplace_back(luaL_Reg{name, func}); }
  void storeFunction(const char* name, LuaFuncion func) { func_list.emplace_back(luaL_Reg{name, func}); }

  virtual bool setupLocal(lua_State*) = 0;

  void setup_finish(lua_State* L)
  {
    // list close
    func_list.emplace_back(luaL_Reg{"new", create});
    method_list.emplace_back(luaL_Reg{nullptr, nullptr});
    func_list.emplace_back(luaL_Reg{nullptr, nullptr});
  }

  bool setup(lua_State* L)
  {
    // define methods
    luaL_newmetatable(L, get_name());
    lua_pushstring(L, "__index");
    lua_newtable(L);
    luaL_setfuncs(L, method_list.data(), 0);
    // literals
    for (auto& l : literal_list)
    {
      l.storeLiteral(L);
    }
    lua_rawset(L, -3);
    {
      lua_pushstring(L, "__gc");
      lua_pushcfunction(L, clear);
      lua_rawset(L, -3);
    }
    lua_pop(L, 1);
    // define functions
    luaL_newlib(L, func_list.data());
    lua_setglobal(L, get_name());

    return true;
  }

  static std::pair<Impl*, LUA::Args> getSelf(lua_State* L)
  {
    LUA::Args args{L};
    Impl**    self = args.getUserData<Impl*>(0, get_name());
    return std::make_pair(*self, args);
  }

  static int create(lua_State* L)
  {
    LUA::Args args(L);

    auto* ni = new Impl;
    init_func(ni, args);
    Impl** p = reinterpret_cast<Impl**>(lua_newuserdata(L, sizeof(Impl*)));
    if (p)
    {
      *p = ni;
      luaL_getmetatable(L, get_name());
      lua_setmetatable(L, -2);
    }
    else
    {
      delete ni;
      lua_pushnil(L);
    }

    return 1;
  }
  static int clear(lua_State* L)
  {
    auto self = getSelf(L);
    delete self.first;
    return 0;
  }

public:
  virtual ~ModuleSetup() = default;

  bool Setup(lua_State* L)
  {
    if (is_initialized_ == false)
    {
      if (setupLocal(L) == false)
      {
        return false;
      }
      setup_finish(L);
      is_initialized_ = true;
    }
    setup(L);
    return true;
  }
};

} // namespace LUA
