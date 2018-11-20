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
}

void USluaUserWidget::RemoveLuaTable()
{
}