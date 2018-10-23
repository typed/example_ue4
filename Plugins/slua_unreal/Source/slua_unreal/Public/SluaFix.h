#pragma once
#include "LuaState.h"

namespace slua {

    class SLUA_UNREAL_API SluaFix
    {
    public:
        static void init(lua_State* L);
        static void print_lua_stack(lua_State* L, const char* pre);
        static void add_usertable_by_ptr(lua_State* L, void* ptr, int usertable);
        static int get_usertable_by_ptr(lua_State* L, void* ptr);
        static void remove_usertable_by_ptr(lua_State* L, void* ptr);
    };
}