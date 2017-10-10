//
// log output control
//
#pragma once

struct lua_State;

namespace LUA
{
namespace LOG
{

//
enum class Level : int
{
  Check,
  Normal,
  Warning,
  Important
};

void Setup(lua_State* L);

void LogPut(Level lv, const char* file, int line, const char* fmt, ...);
#define LOG_PUT(lv, ...) LogPut(lv, __FILE__, __LINE__, __VA_ARGS__)
} // LOG
} // LUA
// End
