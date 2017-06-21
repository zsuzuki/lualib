#pragma once

#include <lua.hpp>

#include <array>
#include <initializer_list>
#include <list>
#include <memory>
#include <string>

#include "fileinfo.hpp"

namespace LUA
{
struct FileInfo;

class Arg
{
  enum class Type : int8_t
  {
    Number,
    String,
    Bool
  };
  Type type_;
  union {
    lua_Number  n_;
    const char* s_;
    bool        b_;
  } u_;

public:
  Arg(lua_Number n) : type_(Type::Number) { u_.n_ = n; }
  Arg(const char* s) : type_(Type::String) { u_.s_ = s; }
  ~Arg()                                           = default;

  void push(lua_State* L) const
  {
    switch (type_)
    {
    case Type::Number:
      lua_pushinteger(L, u_.n_);
      break;
    case Type::String:
      lua_pushstring(L, u_.s_);
      break;
    case Type::Bool:
      lua_pushboolean(L, u_.b_);
      break;
    }
  }
};

class State
{
  using ModuleList  = std::list<std::string>;
  using FileInfoPtr = std::unique_ptr<FileInfo>;

  lua_State*  L       = nullptr;
  bool        clone   = false;
  bool        standby = false;
  bool        running = false;
  FileInfoPtr fi;
  ModuleList  ml;

  void setup();
  bool check_fileinfo();

public:
  State() = default;
  State(const State& s)
  {
    L     = s.L;
    clone = true;
  }
  ~State() { finish(); }

  void start();
  void finish();

  bool loadBuffer(const char* buffer, size_t sz, const char* bname)
  {
    if (isRunning() == false)
    {
      if (luaL_loadbuffer(L, buffer, sz, bname) == LUA_OK)
      {
        standby = true;
        return true;
      }
    }
    return false;
  }
  void loadModule(std::string path) { ml.emplace_back(path); }
  bool runFile(std::string path);

  bool callFunction(std::string fname, std::initializer_list<Arg> al); // success = true

  bool update(); // execute complete = true

  bool isRunning() const { return standby || running; }
  operator lua_State*() { return L; }
  bool operator()() { return L != nullptr; }
};
} // namespace LUA
