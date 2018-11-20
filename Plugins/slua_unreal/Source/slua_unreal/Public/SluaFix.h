#pragma once
#include "LuaState.h"

namespace slua {

    class SLUA_UNREAL_API SluaFix
    {
    public:
        static void initSluaState(class UGameInstance* ginst);
        static void releaseSluaState();
        static LuaState* getSluaState();

        static void init(lua_State* L);
        static void add_usertable_by_ptr(lua_State* L, int refid, int usertable);
        static int get_usertable_by_ptr(lua_State* L, int refid);
        static void remove_usertable_by_ptr(lua_State* L, int refid);
        static void call_usertable_func_by_ptr(lua_State* L, int refid, const char* func);
    };
}

DECLARE_LOG_CATEGORY_EXTERN(LogSluaFix, Verbose, All);