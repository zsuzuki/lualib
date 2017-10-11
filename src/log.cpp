//
// Debug log print utility
//
#include <map>
#include <stdlib.h>

#include <lua.hpp>

#include "log.hpp"

namespace LUA
{

namespace
{

auto output = stdout; // log output file

LOG::Level s_level = LOG::Level::Normal; // log level

std::map<LOG::Level, const char*> level_string = {{LOG::Level::Check, "CHECK"},
                                                  {LOG::Level::Normal, "NORMAL"},
                                                  {LOG::Level::Warning, "WARNING"},
                                                  {LOG::Level::Important, "IMPORTANT"}};

//
void
print_start(lua_State* L, LOG::Level l)
{
  fprintf(output, "[%s]:", level_string[l]);
  lua_Debug ar;
  if (lua_getstack(L, 1, &ar))
  {
    lua_getinfo(L, "Sl", &ar);
    fprintf(output, "%s(%d): ", ar.source, ar.currentline);
  }
  else
  {
    fprintf(output, "** not information **: ");
  }
}

//
void
print_end(lua_State*)
{
  fprintf(output, "\n");
}

//
void
print_body(const char* format, ...)
{
  va_list args;
  va_start(args, format);
  vfprintf(output, format, args);
  va_end(args);
}

//
void
log_put(lua_State* L, LOG::Level lv)
{
  if (lv < s_level)
  {
    return;
  }

  print_start(L, lv);

  int stack_size = lua_gettop(L);
  for (int i = 0; i < stack_size; i++)
  {
    int index = i + 1;
    if (lua_isboolean(L, index))
    {
      bool b = lua_toboolean(L, index);
      print_body("%s", b ? "true" : "false");
    }
    else
    {
      size_t len  = 0;
      auto*  path = lua_tolstring(L, index, &len);
      print_body("%s", path);
    }
  }

  print_end(L);
}

//
// Interface
//

//
int
log_check(lua_State* L)
{
  log_put(L, LOG::Level::Check);
  return 0;
}

//
int
log_normal(lua_State* L)
{
  log_put(L, LOG::Level::Normal);
  return 0;
}

//
int
log_warning(lua_State* L)
{
  log_put(L, LOG::Level::Warning);
  return 0;
}

//
int
log_fatal(lua_State* L)
{
  log_put(L, LOG::Level::Important);
  return 0;
}

//
int
log_level(lua_State* L)
{
  int level = lua_tointeger(L, -1);
  print_start(L, s_level);
  print_body("Change Log Level to: %s", level_string[static_cast<LOG::Level>(level)]);
  print_end(L);
  s_level = static_cast<LOG::Level>(level);
  return 0;
}

//
const struct luaL_Reg log_funcs[] = {
    {"CHECK", log_check},    {"NORMAL", log_normal}, {"WARNING", log_warning},
    {"IMPOTANT", log_fatal}, {"LEVEL", log_level},   {nullptr, nullptr},
};
} // namespae

namespace LOG
{
//
void
Setup(lua_State* L)
{
  luaL_newlib(L, log_funcs);
  lua_setglobal(L, "LOG");
}

//
void
LogPut(Level lv, const char* file, int line, const char* fmt, ...)
{
  if (lv < s_level)
  {
    return;
  }

  char new_fmt[1024];
  snprintf(new_fmt, sizeof(new_fmt), "%s(%d): %s\n", file, line, fmt);
  va_list args;
  va_start(args, fmt);
  vfprintf(output, new_fmt, args);
  va_end(args);
}

} // LOG
} // LUA
// End
