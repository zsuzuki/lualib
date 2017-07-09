//
// 2017 Y.Suzuki
//
#include <iostream>
#include <test.hpp>
#include <vector>

#include <args.hpp>
#include <literal.hpp>
#include <module.hpp>

namespace TEST
{

using FunctionList = std::vector<luaL_Reg>;
using LiteralList  = std::vector<LUA::Literal>;

//
class Impl
{
  static FunctionList method_list;
  static FunctionList func_list;
  static LiteralList  literal_list;

  static constexpr const char* module_name = "TEST";

  int num = 0;

public:
  Impl() { std::cout << "createImpl" << std::endl; }
  ~Impl() { std::cout << "clearImpl" << std::endl; }

  void print(int a) { std::cout << "print: " << num + a << std::endl; }

  static bool LuaSetup(lua_State* L)
  {
    // setup
    method_list.emplace_back(luaL_Reg{"print", print_static});
    literal_list.emplace_back(LUA::Literal{"num", LUA::Literal::Integer(57)});
    literal_list.emplace_back(LUA::Literal{"real", LUA::Literal::Number(6.4)});
    literal_list.emplace_back(LUA::Literal{"b", LUA::Literal::Boolean(true)});
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
    {
      lua_pushstring(L, "__gc");
      lua_pushcfunction(L, clear);
      lua_rawset(L, -3);
    }
    lua_pop(L, 1);
    // define functions
    luaL_newlib(L, func_list.data());
    lua_setglobal(L, module_name);

    return true;
  }

  static std::pair<Impl*, LUA::Args> getSelf(lua_State* L)
  {
    LUA::Args args{L};
    Impl**    self = args.getUserData<Impl*>(0, module_name);
    return std::make_pair(*self, args);
  }

  static int create(lua_State* L)
  {
    LUA::Args args(L);

    auto* ni = new Impl;
    ni->num  = args.getInteger(0);
    Impl** p = reinterpret_cast<Impl**>(lua_newuserdata(L, sizeof(Impl*)));
    if (p)
    {
      *p = ni;
      luaL_getmetatable(L, module_name);
      lua_setmetatable(L, -2);
    }
    else
    {
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

  static int print_static(lua_State* L)
  {
    auto self = getSelf(L);
    self.first->print(self.second.getInteger(1));
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

namespace
{
void
init_test(Impl*, LUA::Args&)
{
}
const char*
get_module_name()
{
  return "TEST";
}
}

class TestModule : public LUA::Module<Impl, init_test, get_module_name>
{
public:
  static void setup()
  {
    storeLiteral("num", LUA::Literal::Integer(39));
    storeLiteral("real", LUA::Literal::Number(9.3));
    storeLiteral("b", LUA::Literal::Boolean(true));
  }
  static int print_static(lua_State* L)
  {
    auto self = getSelf(L);
    self.first->print(self.second.getInteger(1));
    return 0;
  }
};

} // namespace TEST
