// Fill out your copyright notice in the Description page of Project Settings.

/**************************************************************************
Author: levingong
Date: 2019-11-30
Description: 封装WrapBox
**************************************************************************/

#pragma once

#include "CoreMinimal.h"
#include "Runtime/UMG/Public/Components/WrapBox.h"
#include "UGroupBox.generated.h"

/**
*
*/

UCLASS()
class EXAMPLE_UE4_API UGroupBox : public UWrapBox
{
    GENERATED_BODY()

public:

    DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnUpdateItemDelegate, UUserWidget*, Widget, int32, Idx);

    UGroupBox(const FObjectInitializer& ObjectInitializer);

    UPROPERTY(BlueprintAssignable)
    FOnUpdateItemDelegate OnUpdateItem;

    UFUNCTION(BlueprintCallable)
    void Reload(int32 __ItemCount);

    UFUNCTION(BlueprintCallable)
    void Reset(TSubclassOf<UUserWidget> __ItemClass);

protected:

    UPROPERTY(EditAnywhere, Category = Property)
    TSubclassOf<UUserWidget> ItemClass;

    UPROPERTY(EditAnywhere, Category = Property, meta = (ClampMin = "0"))
    int32 PreviewCount;

    // UWidget interface
    virtual void SynchronizeProperties() override;
    // End of UWidget interface

    TSubclassOf<UUserWidget> LastItemClass;

};

DECLARE_LOG_CATEGORY_EXTERN(LogUGroupBox, Verbose, All);