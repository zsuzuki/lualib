//
//
//
#include <iostream>
#include <memory>
#include <string>

#include "state.hpp"

//
int
main(int argc, char** argv)
{
  if (argc < 2)
  {
    std::cerr << "Error: " << argv[0] << " <input lua file>" << std::endl;
    return 1;
  }

  auto state = std::make_unique<LUA::State>();
  state->start();
  state->runFile(argv[1]);

  while (state->update() == false)
  {
  }

  state->finish();
  std::cout << "all done" << std::endl;
  return 0;
}

//
// End
//
