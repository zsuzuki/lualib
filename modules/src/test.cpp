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

namespace
{
void
init_test(Test* t, LUA::Args&)
{
  t->print(0);
}

const char*
get_module_name()
{
  return "TEST";
}
}

class TestModuleImpl : public LUA::ModuleSetup<Test, init_test, get_module_name>
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
//
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
