// Fill out your copyright notice in the Description page of Project Settings.

#include "SluaUserWidget.h"
#include "LuaState.h"
#include "SluaFix.h"

DEFINE_LOG_CATEGORY(LogSluaUserWidget);

using namespace slua;

void USluaUserWidget::NativeDestruct()
{
    Super::NativeDestruct();
    if (LuaState::get() && LuaState::get()->getLuaState()) {
        SluaFix::remove_usertable_by_ptr(LuaState::get()->getLuaState(), this);
    }
    UE_LOG(LogSluaUserWidget, Log, TEXT("USluaUserWidget::NativeDestruct World:%x Name: %s"), this, *GetName());
}