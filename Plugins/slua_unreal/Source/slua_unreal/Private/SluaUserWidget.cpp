// Fill out your copyright notice in the Description page of Project Settings.

#include "SluaUserWidget.h"
#include "LuaState.h"
#include "SluaFix.h"

using namespace slua;

int USluaUserWidget::s_luaID = 1;

USluaUserWidget::USluaUserWidget(const FObjectInitializer& ObjectInitializer)
    :UUserWidget(ObjectInitializer)
{
    m_luaID = s_luaID++;
}

USluaUserWidget::~USluaUserWidget()
{
    if (LuaState::get() && LuaState::get()->getLuaState()) {
        SluaFix::remove_usertable_by_refid(LuaState::get()->getLuaState(), m_luaID);
    }
}