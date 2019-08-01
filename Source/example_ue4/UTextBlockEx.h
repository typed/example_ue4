// Fill out your copyright notice in the Description page of Project Settings.

/**************************************************************************
Author: levingong
Date: 2019-7-25
Description: 文本
**************************************************************************/

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UTextBlockEx.generated.h"

class UTextBlock;

/**
* 字符截断处理...
*
* * No Children
*/
UCLASS()
class EXAMPLE_UE4_API UTextBlockEx : public UUserWidget
{
	GENERATED_BODY()
	
public:
    UTextBlockEx(const FObjectInitializer& ObjectInitializer);

    UFUNCTION(BlueprintCallable)
    virtual void SetString(FString InText);

    UFUNCTION(BlueprintCallable)
    virtual FString GetString() const;

    UPROPERTY(EditAnywhere, Category = Property)
    FString Content;

    //UVisual interface
    virtual void ReleaseSlateResources(bool bReleaseChildren) override;
    //~ End UVisual Interface

    //~ Begin UWidget Interface
    virtual void SynchronizeProperties() override;
    //~ End UWidget Interface

protected:

    bool Initialize();

    void BeginDestroy();

    void NativeConstruct();
    void NativeDestruct();

    void NativeTick(const FGeometry& MyGeometry, float InDeltaTime);

    void OnMyTick();

#if WITH_EDITOR
    void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent);
    FTimerHandle OnTickTimerHandle;
#endif
    void OnWidgetRebuilt();

    void InitWidgetPtr();

    void BuildString();

    void SyncProp();

    
    TWeakObjectPtr<UTextBlock> TextBlockMain;
    FVector2D ViewSize;
    bool NeedBuildString;
    bool Ticked;

};

DECLARE_LOG_CATEGORY_EXTERN(LogUTextBlockEx, Verbose, All);