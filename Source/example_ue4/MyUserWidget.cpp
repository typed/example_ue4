// Fill out your copyright notice in the Description page of Project Settings.

#include "MyUserWidget.h"
#include "SluaFix.h"
#include "LuaObject.h"
#include "LuaCppBinding.h"

using namespace slua;

int UMyUserWidget::s_luaId = 1;

static int lua_tovalue(lua_State* L, int narg, int def)
{
    return lua_gettop(L) < abs(narg) ? def : narg;
}

UMyUserWidget::UMyUserWidget(const FObjectInitializer& ObjectInitializer)
    : UUserWidget(ObjectInitializer)
{
    m_luaId = s_luaId++;
}

void UMyUserWidget::NativeDestruct()
{
    Super::NativeDestruct();
    RemoveLuaTable();
}

void UMyUserWidget::RemoveLuaTable()
{
    auto LS = LuaState::get();
    if (LS) {
        auto L = LS->getLuaState();
        if (L) {
            SluaFix::call_usertable_func_by_ptr(L, m_luaId, "__destruct");
            SluaFix::remove_usertable_by_ptr(L, m_luaId);
        }
    }
}

void UMyUserWidget::bindLuaFunc()
{
    auto LS = LuaState::get();
    if (LS) {
        lua_State* L = LS->getLuaState();
        if (L) {
            
            //slua fix
            REG_EXTENSION_METHOD_IMP(UMyUserWidget, "SetLuaTable", {
                CheckUD(UMyUserWidget,L,1);
                if (UD == nullptr) {
                    return 0;
                }
                if (lua_istable(L, 2)) {
                    int tbl = lua_tovalue(L, 2, 0);
                    SluaFix::add_usertable_by_ptr(L, UD->m_luaId, tbl);
                }
                else if (lua_isnil(L, 2)) {
                    SluaFix::remove_usertable_by_ptr(L, UD->m_luaId);
                }
                return 0;
            });

            REG_EXTENSION_METHOD_IMP(UMyUserWidget, "GetLuaTable", {
                CheckUD(UMyUserWidget,L,1);
                if (UD == nullptr) {
                    return 0;
                }
                return SluaFix::get_usertable_by_ptr(L, UD->m_luaId);
            });

        }
    }
}

