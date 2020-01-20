// Fill out your copyright notice in the Description page of Project Settings.

/**************************************************************************
Author: levingong
Date: 2019-6-13
Description: 换皮缓存组件
**************************************************************************/

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UCommonSkinRoot.generated.h"

/**
*
*/

class UCanvasPanel;

UCLASS()
class EXAMPLE_UE4_API UCommonSkinRoot : public UUserWidget
{
    GENERATED_BODY()

public:

    UCommonSkinRoot(const FObjectInitializer& ObjectInitializer);

    UFUNCTION(BlueprintCallable)
    UUserWidget* LoadSkin(FString __WidgetPath);

protected:

    bool Initialize();

    void NativeConstruct();

    void OnWidgetRebuilt();

    void InitWidgetPtr();

    TWeakObjectPtr<UUserWidget> GetAndCreateWidgetFromPool(const FString& __WidgetPath);

    TWeakObjectPtr<UCanvasPanel> CanvasPanelRoot;

    TMap<FString, TWeakObjectPtr<UUserWidget> > SkinPool;
};

DECLARE_LOG_CATEGORY_EXTERN(LogUCommonSkinRoot, Verbose, All);