#include "SluaFix.h"
#include "LuaObject.h"
#include "LuaCppBinding.h"

DEFINE_LOG_CATEGORY(LogSluaFix);

#define SLUA_PTR_USERTABLE_MAPPING "SLUA_PTR_USERTABLE_MAPPING"

namespace slua {

    static void dump_lua_stack(lua_State* L)
    {
        /*
        int stackTop = lua_gettop(L);
        UE_LOG(LogSluaFix, Log, TEXT("lua_stack:"));
        for (int nIdx = stackTop; nIdx > 0; --nIdx)
        {
            int nType = lua_type(L, nIdx);
            FString st = lua_typename(L, nType);
            FString sv = luaL_tolstring(L, nIdx, NULL);
            lua_pop(L, 1);
            UE_LOG(LogSluaFix, Log, TEXT("%d:%s(%s)"), nIdx, *st, *sv);
        }
        stackTop = lua_gettop(L);
        */
    }

    static int lua_tovalue(lua_State* L, int narg, int def)
    {
        return lua_gettop(L)<abs(narg) ? def : narg;
    }

    slua::LuaState g_state;

    void SluaFix::initSluaState(class UGameInstance* ginst)
    {
        g_state.init(ginst);
    }

    void SluaFix::releaseSluaState()
    {
        g_state.close();
    }

    LuaState* SluaFix::getSluaState()
    {
        return &g_state;
    }

    void SluaFix::init(lua_State* L)
    {
        lua_pushstring(L, SLUA_PTR_USERTABLE_MAPPING);
        lua_newtable(L);
        lua_rawset(L, LUA_REGISTRYINDEX);
    }

    
    
    void SluaFix::add_usertable_by_ptr(lua_State* L, int refid, int usertable)
    {
        lua_pushstring(L, SLUA_PTR_USERTABLE_MAPPING);
        dump_lua_stack(L);
        lua_rawget(L, LUA_REGISTRYINDEX);                               
        dump_lua_stack(L);
        lua_pushinteger(L, refid);                                      
        dump_lua_stack(L);
        lua_pushvalue(L, usertable);                                    
        dump_lua_stack(L);
        lua_rawset(L, -3);                                              
        dump_lua_stack(L);
        lua_pop(L, 1);                                                  
        dump_lua_stack(L);
    }
    
    int SluaFix::get_usertable_by_ptr(lua_State* L, int refid)
    {
        lua_pushstring(L, SLUA_PTR_USERTABLE_MAPPING);
        dump_lua_stack(L);
        lua_rawget(L, LUA_REGISTRYINDEX);                               
        dump_lua_stack(L);
        lua_pushinteger(L, refid);                                      
        dump_lua_stack(L);
        lua_rawget(L, -2);                                              
        dump_lua_stack(L);
        lua_remove(L, -2);
        dump_lua_stack(L);
        return 1;
    }
    
    void SluaFix::remove_usertable_by_ptr(lua_State* L, int refid)
    {
        lua_pushstring(L, SLUA_PTR_USERTABLE_MAPPING);
        dump_lua_stack(L);
        lua_rawget(L, LUA_REGISTRYINDEX);                               
        dump_lua_stack(L);
        lua_pushinteger(L, refid);                                      
        dump_lua_stack(L);
        lua_pushnil(L);                                                 
        dump_lua_stack(L);
        lua_rawset(L, -3);                                              
        dump_lua_stack(L);
        lua_pop(L, 1);                                                  
        dump_lua_stack(L);
    }

    void SluaFix::call_usertable_func_by_ptr(lua_State* L, int refid, const char* func)
    {
        if (func == nullptr) {
            return;
        }
        lua_pushstring(L, SLUA_PTR_USERTABLE_MAPPING);
        dump_lua_stack(L);
        lua_rawget(L, LUA_REGISTRYINDEX);
        dump_lua_stack(L);
        lua_pushinteger(L, refid);
        dump_lua_stack(L);
        lua_rawget(L, -2);
        dump_lua_stack(L);
        lua_remove(L, -2);
        dump_lua_stack(L);
        if (!lua_istable(L, -1)) {
            lua_pop(L, 1);
            dump_lua_stack(L);
            return;
        }
        lua_pushstring(L, func);
        dump_lua_stack(L);
        lua_gettable(L, -2);
        dump_lua_stack(L);
        if (!lua_isfunction(L, -1)) {
            lua_pop(L, 2);
            dump_lua_stack(L);
            return;
        }
        dump_lua_stack(L);
        lua_pushvalue(L, -2);
        dump_lua_stack(L);
        lua_pcall(L, 1, 0, 0);
        dump_lua_stack(L);
        lua_pop(L, 1);
        dump_lua_stack(L);
    }

}