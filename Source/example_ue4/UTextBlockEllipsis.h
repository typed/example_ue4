// Fill out your copyright notice in the Description page of Project Settings.

/**************************************************************************
Author: levingong
Date: 2019-7-25
Description: 文本省略号组件
**************************************************************************/

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UTextBlockEllipsis.generated.h"

class UTextBlock;
class UUTRichTextBlock;

/**
* 字符截断处理...
*
* * No Children
*/

UENUM(BlueprintType)
enum class ETextBlockEllipsisType : uint8
{
    Invalid,
    Normal,
    Rich,
};

class FSeqementItem
{
public:
    int32 Begin;
    int32 End;
    int32 Count;
    FString Content;
    bool IsLabel;
};

UCLASS()
class EXAMPLE_UE4_API UTextBlockEllipsis : public UUserWidget
{
	GENERATED_BODY()
	
public:
    UTextBlockEllipsis(const FObjectInitializer& ObjectInitializer);

    UFUNCTION(BlueprintCallable)
    virtual void SetString(FString InText);

    UFUNCTION(BlueprintCallable)
    virtual FString GetString() const;

    UPROPERTY(EditAnywhere, Category = Property)
    FString Content;

    UFUNCTION(BlueprintCallable)
    void Test();

    //UVisual interface
    virtual void ReleaseSlateResources(bool bReleaseChildren) override;
    //~ End UVisual Interface

    //~ Begin UWidget Interface
    virtual void SynchronizeProperties() override;
    //~ End UWidget Interface

protected:

    virtual bool Initialize();

    virtual void NativeConstruct();

    virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime);

    void OnMyTick();

#if WITH_EDITOR
    virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent);
    FTimerHandle OnTickTimerHandle;
#endif
    virtual void OnWidgetRebuilt();

    void InitWidgetPtr();

    void BuildString();

    void SyncProp();

    void BuildString_Normal();
    void BuildString_Rich();
    int32 Len_Rich();
    FString Mid_Rich(int32 count);
    void DetachTextAndLabel();
    FString GetString_Rich();
    FString Content_Rich;
    TArray<FSeqementItem> ArraySeqement;

    ETextBlockEllipsisType TextType;
    TWeakObjectPtr<UTextBlock> TextBlockMain;
    TWeakObjectPtr<UUTRichTextBlock> RichTextBlockMain;
    FVector2D ViewSize;
    bool NeedBuildString;
    bool Ticked;

};

DECLARE_LOG_CATEGORY_EXTERN(LogUTextBlockEllipsis, Verbose, All);