// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MyUserWidget.generated.h"

/**
 * 
 */
UCLASS()
class EXAMPLE_UE4_API UMyUserWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
    UMyUserWidget(const FObjectInitializer& ObjectInitializer);

    int m_luaId;

    static void bindLuaFunc();

protected:

    static int s_luaId;

    virtual void NativeDestruct();

    void RemoveLuaTable();
	
};
