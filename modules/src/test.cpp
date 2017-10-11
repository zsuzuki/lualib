//
// 2017 Y.Suzuki
//
#include <iostream>
#include <vector>

#include <args.hpp>
#include <literal.hpp>
#include <module.hpp>

#include <test.hpp>

namespace TEST
{

//
// test class implement
//
class Test
{
  int m_n = 0;
  int m_c = 0;

public:
  Test()  = default;
  ~Test() = default;

  void add(int n) { m_n += n; }

  void print(int n)
  {
    add(n);
    std::cout << "num: " << m_n << " count: " << m_c << std::endl;
    m_c++;
  }
};

//
// local functions
//
namespace
{
void
init_test(Test* t, LUA::Args& args)
{
  t->print(args.getInteger(0));
}

const char*
test_module_name()
{
  return "TEST";
}
}

//
// module setup class
//
class TestModuleImpl : public LUA::ModuleSetup<Test, init_test, test_module_name>
{
protected:
  bool setupLocal(lua_State* L) override
  {
    storeLiteral("num", LUA::Literal::Integer(39));
    storeLiteral("real", LUA::Literal::Number(9.3));
    storeLiteral("b", LUA::Literal::Boolean(true));
    storeMethod("print", [](lua_State* L) {
      auto self = getSelf(L);
      self.first->print(self.second.getInteger(1));
      return 0;
    });
    return true;
  }

public:
  ~TestModuleImpl() override = default;
};

//
// external interface
//
namespace
{
std::shared_ptr<TestModuleImpl> impl;
}
void
Setup(lua_State* L)
{
  if (!impl)
  {
    impl = std::make_shared<TestModuleImpl>();
  }
  impl->Setup(L);
}

} // namespace TEST
