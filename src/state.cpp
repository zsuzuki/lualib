//
// Copyright 2017 Yoshinori Suzuki<wave.suzuki.z@gmail.com>
//
// Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated
// documentation files (the "Software"), to deal in the Software without restriction, including without limitation the
// rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to
// permit persons to whom the Software is furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all copies or substantial portions of the
// Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
// WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
// COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR
// OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.//
//
//
#include <atomic>
#include <chrono>
#include <fstream>
#include <future>
#include <iostream>
#include <random>
#include <string>
#include <thread>

#include <state.hpp>

#include "fileinfo.hpp"
#include "log.hpp"

namespace LUA
{
namespace
{
std::random_device seed_device;
std::mt19937       global_random(seed_device());

//
// base functions
//

//
int
load_module_wait(lua_State* L, int, lua_KContext)
{
  if (lua_pcallk(L, 0, 1, 0, 0, load_module_wait) != LUA_OK)
  {
    std::cerr << "loadModule failed: " << lua_tostring(L, -1) << std::endl;
    lua_pushnil(L);
  }
  else
  {
    lua_pushboolean(L, true);
  }
  return 1;
}

int
load_module_body(lua_State* L, int, lua_KContext ctx)
{
  auto* fi = reinterpret_cast<FileInfo*>(ctx);
  if (fi->isDone() == false)
  {
    return lua_yieldk(L, 0, ctx, load_module_body);
  }

  auto* buff = reinterpret_cast<const char*>(fi->getBuffer());
  luaL_loadbuffer(L, buff, fi->getSize(), fi->getPath());
  delete fi;

  if (lua_pcallk(L, 0, 1, 0, 0, load_module_wait) != LUA_OK)
  {
    std::cerr << "loadModule failed: " << lua_tostring(L, -1) << std::endl;
    lua_pushnil(L);
  }
  else
  {
    lua_pushboolean(L, true);
  }
  return 1;
}

int
load_module_entry(lua_State* L)
{
  size_t       len = 0;
  const char*  p   = lua_tolstring(L, 1, &len);
  auto         fi  = new FileInfo(p);
  lua_KContext ctx = reinterpret_cast<lua_KContext>(fi);
  return lua_yieldk(L, 0, ctx, load_module_body);
}

// yield
int
yield_continue(lua_State* L, int, lua_KContext)
{
  LOG_PUT(LOG::Level::Check, "system yield");
  return 0;
}
int
yield(lua_State* L)
{
  return lua_yieldk(L, 0, 0, yield_continue);
}

//
// Random
//
int
random_get_number(lua_State* L)
{
  std::uniform_real_distribution<> dist(0.0, 1.0);
  lua_pushnumber(L, dist(global_random));
  return 1;
}
int
random_get_int(lua_State* L)
{
  std::uniform_int_distribution<> dist(0, INT_MAX);
  lua_pushinteger(L, dist(global_random));
  return 1;
}
int
random_set_seed(lua_State* L)
{
  return 0;
}
const struct luaL_Reg random_funcs[] = {
    {"getNumber", random_get_number}, {"getInt", random_get_int}, {"setSeed", random_set_seed}, {nullptr, nullptr},
};

//
// Timer
//
int
timer_get_now(lua_State* L)
{
  std::chrono::system_clock::time_point t = std::chrono::system_clock::now();
  lua_pushinteger(L, std::chrono::duration_cast<std::chrono::microseconds>(t.time_since_epoch()).count());
  return 1;
}
const struct luaL_Reg timer_funcs[] = {{"Now", timer_get_now}, {nullptr, nullptr}};

} // namespace

//
// state
//

//
bool
State::callFunction(std::string fname, std::initializer_list<Arg> al)
{
  if (isRunning())
  {
    std::cerr << "not execute, scripts execute now" << std::endl;
    return false;
  }

  lua_getglobal(L, fname.c_str());
  for (const auto& a : al)
  {
    a.push(L);
  }
  int r = lua_resume(L, 0, al.size());
  if (r == LUA_OK)
  {
    return true;
  }
  if (r == LUA_YIELD)
  {
    running = true;
    return true;
  }
  std::cerr << "Error: " << lua_tostring(L, -1) << std::endl;
  return false;
}

// フレーム単位の更新処理
bool
State::update()
{
  if (L)
  {
    if (standby || running)
    {
      if (standby)
      {
        running = true;
        standby = false;
      }
      int r = lua_resume(L, 0, 0);
      if (r != LUA_OK)
      {
        if (r != LUA_YIELD)
        {
          std::cerr << "LuaError: " << lua_tostring(L, -1) << std::endl;
          running = false;
        }
        return false;
      }
      running = false;
    }
    if (ml.empty() == false)
    {
      auto& mn = ml.front();
      std::cout << "Module: " << mn.data() << std::endl;
      char buff[1024];
      snprintf(buff, sizeof(buff), "loadModule(\"%s\")", mn.data());
      luaL_loadstring(L, buff);
      standby = true;
      ml.pop_front();
    }
    else
    {
      // runFileで指定されているファイルがあれば、それを呼んで実行開始する
      return !check_fileinfo();
    }
  }
  return true;
}

//
bool
State::runFile(const std::string path)
{
  if (fi)
  {
    std::cerr << "runFile: already loading" << std::endl;
    return false;
  }

  fi = std::make_unique<FileInfo>(path);

  return true;
}

bool
State::check_fileinfo()
{
  if (isRunning())
  {
    return true;
  }
  if (!fi)
  {
    return false;
  }

  if (fi->isDone() == false)
  {
    return true;
  }

  auto* buff = reinterpret_cast<const char*>(fi->getBuffer());
  luaL_loadbuffer(L, buff, fi->getSize(), fi->getPath());
  fi.reset();
  standby = true;
  return true;
}

//
// setup interface for application
//

//
void
State::setup()
{
  LOG::Setup(L);
  lua_register(L, "loadModule", load_module_entry);
  lua_register(L, "Yield", yield);

  luaL_newlib(L, random_funcs);
  lua_setglobal(L, "Random");

  luaL_newlib(L, timer_funcs);
  lua_setglobal(L, "Time");
}

//
void
State::setup_argument(int argc, char** argv)
{
  lua_newtable(L);
  for (int ai = 2; ai < argc; ai++)
  {
    std::string arg = argv[ai];
    if (arg == "=")
    {
      continue;
    }

    auto fpos = arg.find("=");
    if (fpos == std::string::npos)
    {
      lua_pushstring(L, arg.c_str());
      lua_pushboolean(L, true);
    }
    else
    {
      auto value = arg.substr(fpos + 1);
      lua_pushstring(L, arg.substr(0, fpos).c_str());
      lua_pushstring(L, value.c_str());
    }
    lua_settable(L, -3);
  }

  lua_setglobal(L, "ARGS");
}

//
void
State::start(int argc, char** argv)
{
  if (L == nullptr)
  {
    L = luaL_newstate();
    luaL_openlibs(L);
    setup();
    setup_argument(argc, argv);
  }
}

//
void
State::finish()
{
  if (L == nullptr)
  {
    return;
  }

  while (check_fileinfo())
  {
  }

  if (clone == false)
  {
    lua_close(L);
  }
  ml.clear();
  clone = false;
  L     = nullptr;
}

//
State::State() = default;
State::State(const State& s)
{
  L     = s.L;
  clone = true;
}
State::~State() { finish(); }

} // namespace LUA
