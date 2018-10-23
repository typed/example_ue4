// Fill out your copyright notice in the Description page of Project Settings.

#include "SluaUserWidget.h"
#include "LuaState.h"
#include "SluaFix.h"

using namespace slua;

USluaUserWidget::~USluaUserWidget()
{
    if (LuaState::get() && LuaState::get()->getLuaState()) {
        SluaFix::remove_usertable_by_ptr(LuaState::get()->getLuaState(), this);
    }
}