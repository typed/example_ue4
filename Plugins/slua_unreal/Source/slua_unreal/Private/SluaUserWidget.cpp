// Fill out your copyright notice in the Description page of Project Settings.

#include "SluaUserWidget.h"
#include "LuaState.h"
#include "SluaFix.h"

DEFINE_LOG_CATEGORY(LogSluaUserWidget);

using namespace slua;

void USluaUserWidget::RemoveFromParent()
{
    Super::RemoveFromParent();
    //RemoveLuaTable();
}

void USluaUserWidget::NativeDestruct()
{
    Super::NativeDestruct();
    OnNativeDestruct.Broadcast(this);
    RemoveLuaTable();
    //UE_LOG(LogSluaUserWidget, Log, TEXT("USluaUserWidget::NativeDestruct World:%x Name: %s"), this, *GetName());
}

void USluaUserWidget::RemoveLuaTable()
{
    if (LuaState::get() && LuaState::get()->getLuaState()) {
        SluaFix::remove_usertable_by_ptr(LuaState::get()->getLuaState(), this);
    }
}