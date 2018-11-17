// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SluaUserWidget.generated.h"

/**
 * 
 */
UCLASS()
class SLUA_UNREAL_API USluaUserWidget : public UUserWidget
{
    GENERATED_BODY()

public:

    DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnNativeDestructDelegate, UUserWidget*, widget);

    UPROPERTY(BlueprintAssignable)
    FOnNativeDestructDelegate OnNativeDestruct;

    virtual void RemoveFromParent();

protected:

    virtual void NativeDestruct();

private:

    void RemoveLuaTable();
	
};

DECLARE_LOG_CATEGORY_EXTERN(LogSluaUserWidget, Verbose, All);