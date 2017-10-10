//
//
//
#include <iostream>
#include <memory>
#include <string>

#include <state.hpp>
#include <test.hpp>

//
int
main(int argc, char** argv)
{
  if (argc < 2)
  {
    std::cerr << "Error: " << argv[0] << " <input lua file>" << std::endl;
    return 1;
  }

  std::cout << "Application: start" << std::endl;
  auto state = std::make_unique<LUA::State>();
  state->start(argc, argv);

  TEST::Test::LuaSetup(state->getLua());
  state->runFile(argv[1]);

  while (state->update() == false)
  {
  }

  state->finish();
  std::cout << "Application: all done" << std::endl;
  return 0;
}

//
// End
//
