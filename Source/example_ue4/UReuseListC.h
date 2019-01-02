// Fill out your copyright notice in the Description page of Project Settings.

/**************************************************************************
Author: levingong
Date: 2018-11-15
Description: 重用列表
**************************************************************************/

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UReuseListC.generated.h"

/**
 * 
 */

class UScrollBox;
class USizeBox;
class UCanvasPanel;

UENUM(BlueprintType)
enum class EReuseListStyle : uint8
{
    Vertical,
    Horizontal,
    VerticalGrid,
};

UENUM(BlueprintType)
enum class EReuseListJumpStyle : uint8
{
    Middle,
    Begin,
    End,
};

UCLASS()
class EXAMPLE_UE4_API UReuseListC : public UUserWidget
{
	GENERATED_BODY()
	
public:

    DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnUpdateItemDelegate, UUserWidget*, Widget, int32, Idx);
    DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnScrollItemDelegate, int32, BeginIdx, int32, EndIdx);
    DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCreateItemDelegate, UUserWidget*, widget);

    UReuseListC(const FObjectInitializer& ObjectInitializer);

    UPROPERTY(BlueprintAssignable)
    FOnUpdateItemDelegate OnUpdateItem;

    UPROPERTY(BlueprintAssignable)
    FOnScrollItemDelegate OnScrollItem;

    UPROPERTY(BlueprintAssignable)
    FOnCreateItemDelegate OnCreateItem;

    UFUNCTION(BlueprintCallable)
    void Reload(int32 __ItemCount);

    UFUNCTION(BlueprintCallable)
    void Refresh();

    UFUNCTION(BlueprintCallable)
    void RefreshOne(int32 __Idx);

    UFUNCTION(BlueprintCallable)
    void JumpByIdx(int32 __Idx) { JumpByIdxStyle(__Idx, EReuseListJumpStyle::Middle); }

    UFUNCTION(BlueprintCallable)
    void JumpByIdxStyle(int32 __Idx, EReuseListJumpStyle __Style);

    UFUNCTION(BlueprintCallable)
    void Clear();

    UFUNCTION(BlueprintCallable)
    void Reset(UClass* __ItemClass, EReuseListStyle __Style, int32 __ItemWidth, int32 __ItemHeight, int32 __PaddingX, int32 __PaddingY);

protected:

    virtual bool Initialize();

    //UVisual interface
    virtual void ReleaseSlateResources(bool bReleaseChildren);
    //~ End UVisual Interface

    //~ Begin UWidget Interface
    virtual void SynchronizeProperties();
    //~ End UWidget Interface

    UPROPERTY(EditAnywhere, Category = Property)
    FScrollBoxStyle ScrollBoxStyle;

    UPROPERTY(EditAnywhere, Category = Property)
    FScrollBarStyle ScrollBarStyle;

	UPROPERTY(EditAnywhere, Category = Property)
	ESlateVisibility ScrollBarVisibility;

	UPROPERTY(EditAnywhere, Category = Property)
	FVector2D ScrollBarThickness;

    //UPROPERTY(EditAnywhere, Category = Property, meta = (ClampMin = "0"))
    int32 ItemCacheNum;

    UPROPERTY(EditAnywhere, Category = Property, meta = (ClampMin = "0"))
    int32 ItemWidth;

    UPROPERTY(EditAnywhere, Category = Property, meta = (ClampMin = "0"))
    int32 ItemHeight;

    UPROPERTY(EditAnywhere, Category = Property, meta = (ClampMin = "0"))
    int32 PaddingX;

    UPROPERTY(EditAnywhere, Category = Property, meta = (ClampMin = "0"))
    int32 PaddingY;

    UPROPERTY(EditAnywhere, Category = Property)
    EReuseListStyle Style;

    UPROPERTY(EditAnywhere, Category = Property, meta = (BlueprintBaseOnly = ""))
    UClass* ItemClass;

    UPROPERTY(EditAnywhere, Category = Property, meta = (ClampMin = "0"))
    int32 PreviewCount;
    FTimerHandle tmhOnPreviewTick;

    void NativeConstruct();
    void NativeTick(const FGeometry& MyGeometry, float InDeltaTime);

    void InitWidgetPtr();
    void ScrollUpdate(float __Offset);
    void UpdateContentSize(UWidget* widget);
    void RemoveNotUsed();
    void DoReload();
    UUserWidget* NewItem();
    void ReleaseItem(UUserWidget* __Item);
    void Update();
    void DoJump();

    void OnPreviewTick();

    bool IsVertical() const;
    bool IsInvalidParam() const;

    void ClearCache();

    UScrollBox* ScrollBoxList;
    UCanvasPanel* CanvasPanelBg;
    USizeBox* SizeBoxBg;
    UCanvasPanel* CanvasPanelList;
    
    FVector2D ViewSize;
    FVector2D ContentSize;
    
    int32 ItemCount;
    int32 MaxPos;
    TMap<int32, UUserWidget*> ItemMap;
    TArray<UUserWidget*> ItemPool;
    int32 BIdx;
    int32 EIdx;
    int32 ColNum;
    int32 RowNum;
    int32 CurLine;
    int32 JumpIdx;
    EReuseListJumpStyle JumpStyle;
    bool NeedJump;

};

DECLARE_LOG_CATEGORY_EXTERN(LogUReuseListC, Verbose, All);