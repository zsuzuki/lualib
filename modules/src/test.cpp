//
// 2017 Y.Suzuki
//
#include <iostream>
#include <test.hpp>
#include <vector>

namespace TEST
{

//
class Impl
{
  static std::vector<luaL_Reg> method_list;
  static std::vector<luaL_Reg> func_list;
  static constexpr const char* module_name = "TEST";

  int num = 0;

public:
  Impl() {}
  ~Impl() {}

  static bool LuaSetup(lua_State* L)
  {
    luaL_newmetatable(L, module_name);
    lua_pushstring(L, "__index");
    lua_newtable(L);
    method_list.emplace_back(luaL_Reg{"print", print_static});
    method_list.emplace_back(luaL_Reg{nullptr, nullptr});
    luaL_setfuncs(L, method_list.data(), 0);
    lua_rawset(L, -3);
    lua_pop(L, 1);
    func_list.emplace_back(luaL_Reg{"new", create});
    func_list.emplace_back(luaL_Reg{nullptr, nullptr});
    luaL_newlib(L, func_list.data());
    lua_setglobal(L, "Test");
    return true;
  }

  static int create(lua_State* L)
  {
    auto* ni = new Impl;
    ni->store(L);
    return 1;
  }

  void store(lua_State* L)
  {
    num = luaL_checkinteger(L, -1);
    lua_pushlightuserdata(L, this);
    int r = luaL_getmetatable(L, module_name);
    std::cout << "create: " << r << std::endl;
    lua_setmetatable(L, -2);
  }

  void print() { std::cout << "print: " << num << std::endl; }

  static int print_static(lua_State* L)
  {
    auto* self = reinterpret_cast<Impl*>(luaL_checkudata(L, -1, "TEST"));
    self->print();
    return 0;
  };
};
std::vector<luaL_Reg> Impl::method_list;
std::vector<luaL_Reg> Impl::func_list;

//
Test::Test() : impl(std::make_shared<Impl>()) {}

//
Test::~Test() {}

//
bool
Test::LuaSetup(lua_State* L)
{
  return Impl::LuaSetup(L);
}

} // namespace TEST
