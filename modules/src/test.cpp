//
// 2017 Y.Suzuki
//
#include <iostream>
#include <test.hpp>
#include <vector>

#include <literal.hpp>

namespace TEST
{

using FunctionList = std::vector<luaL_Reg>;
using LiteralList  = std::vector<LUA::Literal>;

struct MetaInfo
{
  FunctionList methods;
  FunctionList functions;
  LiteralList  literals;
};

//
class Impl
{
  static FunctionList method_list;
  static FunctionList func_list;
  static LiteralList  literal_list;

  static constexpr const char* module_name = "TEST";

  int num = 0;

public:
  Impl() {}
  ~Impl() {}

  static bool LuaSetup(lua_State* L)
  {
    // setup
    method_list.emplace_back(luaL_Reg{"print", print_static});
    literal_list.emplace_back(LUA::Literal{"num", LUA::Literal::Integer(57)});
    literal_list.emplace_back(LUA::Literal{"real", LUA::Literal::Number(6.4)});
    func_list.emplace_back(luaL_Reg{"new", create});

    // list close
    method_list.emplace_back(luaL_Reg{nullptr, nullptr});
    func_list.emplace_back(luaL_Reg{nullptr, nullptr});
    // define methods
    luaL_newmetatable(L, module_name);
    lua_pushstring(L, "__index");
    lua_newtable(L);
    luaL_setfuncs(L, method_list.data(), 0);
    // literals
    for (auto& l : literal_list)
    {
      l.storeLiteral(L);
    }
    lua_rawset(L, -3);
    lua_pop(L, 1);
    // define functions
    luaL_newlib(L, func_list.data());
    lua_setglobal(L, module_name);

    return true;
  }

  template <class I>
  static std::pair<I*, int> getSelf(lua_State* L)
  {
    int   num  = -lua_gettop(L);
    auto* self = reinterpret_cast<I*>(luaL_checkudata(L, num, module_name));
    return std::make_pair(self, num + 1);
  }

  static int create(lua_State* L)
  {
    auto* ni = new Impl;
    ni->store(L);
    return 1;
  }
  static int clear(lua_State* L)
  {
    auto self = getSelf<Impl>(L);
    delete self.first;
    std::cout << "clear" << std::endl;
    return 0;
  }

  void store(lua_State* L)
  {
    num = luaL_checkinteger(L, -1);
    lua_pushlightuserdata(L, this);
    int r = luaL_getmetatable(L, module_name);
    std::cout << "create: " << r << std::endl;
    lua_setmetatable(L, -2);
  }

  void print(int a) { std::cout << "print: " << num + a << std::endl; }

  static int print_static(lua_State* L)
  {
    auto self = getSelf<Impl>(L);
    int  add  = luaL_checkinteger(L, self.second);
    self.first->print(add);
    return 0;
  };
};
FunctionList Impl::method_list;
FunctionList Impl::func_list;
LiteralList  Impl::literal_list;

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
