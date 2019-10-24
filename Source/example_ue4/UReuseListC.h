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
class UNamedSlot;

UENUM(BlueprintType)
enum class EReuseListStyle : uint8
{
    Vertical,
    Horizontal,
    VerticalGrid,
    HorizontalGrid,
};

UENUM(BlueprintType)
enum class EReuseListJumpStyle : uint8
{
    Middle,
    Begin,
    End,
};

UENUM(BlueprintType)
enum class EReuseListNotFullAlignStyle : uint8
{
    Start,
    Middle,
    End,
};

UCLASS()
class EXAMPLE_UE4_API UReuseListC : public UUserWidget
{
	GENERATED_BODY()
	
public:

    DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnUpdateItemParamDelegate, UUserWidget*, Widget, int32, Idx, FString, Param);
    DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnUpdateItemDelegate, UUserWidget*, Widget, int32, Idx);
    DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnScrollItemDelegate, int32, BeginIdx, int32, EndIdx);
    DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCreateItemDelegate, UUserWidget*, widget);

    UReuseListC(const FObjectInitializer& ObjectInitializer);

    UPROPERTY(BlueprintAssignable)
    FOnUpdateItemParamDelegate OnUpdateItemParam;
    
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
    void RefreshParam(const FString& _Param);

    UFUNCTION(BlueprintCallable)
    void RefreshOneParam(int32 __Idx, const FString& _Param);

    UFUNCTION(BlueprintCallable)
    void ScrollToStart();

    UFUNCTION(BlueprintCallable)
    void ScrollToEnd();

    UFUNCTION(BlueprintCallable)
    void SetScrollOffset(float NewScrollOffset);

    UFUNCTION(BlueprintCallable)
    float GetScrollOffset() const;

    UFUNCTION(BlueprintCallable)
    const FVector2D& GetViewSize() const { return ViewSize; }

    UFUNCTION(BlueprintCallable)
    const FVector2D& GetContentSize() const { return ContentSize; }

    UFUNCTION(BlueprintCallable)
    int32 GetItemWidth() const { return ItemWidth; }

    UFUNCTION(BlueprintCallable)
    int32 GetItemHeight() const { return ItemHeight; }

    UFUNCTION(BlueprintCallable)
    int32 GetPaddingX() const { return PaddingX; }

    UFUNCTION(BlueprintCallable)
    int32 GetPaddingY() const { return PaddingY; }

    UFUNCTION(BlueprintCallable)
    int32 GetItemWidthAndPaddingX() const { return ItemWidth + PaddingX; }

    UFUNCTION(BlueprintCallable)
    int32 GetItemHeightAndPaddingY() const { return ItemHeight + PaddingY; }

    UFUNCTION(BlueprintCallable)
    void JumpByIdx(int32 __Idx) { JumpByIdxStyle(__Idx, EReuseListJumpStyle::Middle); }

    UFUNCTION(BlueprintCallable)
    void JumpByIdxStyle(int32 __Idx, EReuseListJumpStyle __Style);

    UFUNCTION(BlueprintCallable)
    void SetTitleSize(int32 sz);

    UFUNCTION(BlueprintCallable)
    void SetTitleSlotAutoSize(bool as);

    UFUNCTION(BlueprintCallable)
    void Clear();

    UFUNCTION(BlueprintCallable)
    void Reset(TSubclassOf<UUserWidget> __ItemClass, EReuseListStyle __Style, int32 __ItemWidth, int32 __ItemHeight, int32 __PaddingX, int32 __PaddingY);

protected:

    bool Initialize();

    UPROPERTY(EditAnywhere, Category = Property)
    FScrollBoxStyle ScrollBoxStyle;

    UPROPERTY(EditAnywhere, Category = Property)
    EWidgetClipping ScrollBoxClipping;

    UPROPERTY(EditAnywhere, Category = Property)
    ESlateVisibility ScrollBoxVisibility;

    UPROPERTY(EditAnywhere, Category = Property)
    FScrollBarStyle ScrollBarStyle;

	UPROPERTY(EditAnywhere, Category = Property)
	ESlateVisibility ScrollBarVisibility;

	UPROPERTY(EditAnywhere, Category = Property)
	FVector2D ScrollBarThickness;

    UPROPERTY(EditAnywhere, Category = Property, meta = (ClampMin = "0"))
    int32 ItemWidth;

    UPROPERTY(EditAnywhere, Category = Property, meta = (ClampMin = "0"))
    int32 ItemHeight;

    UPROPERTY(EditAnywhere, Category = Property, meta = (ClampMin = "0"))
    int32 PaddingX;

    UPROPERTY(EditAnywhere, Category = Property, meta = (ClampMin = "0"))
    int32 PaddingY;

    UPROPERTY(EditAnywhere, Category = Property, meta = (ClampMin = "0"))
    int32 TitleSize;

    UPROPERTY(EditAnywhere, Category = Property, meta = (ClampMin = "0"))
    int32 TitlePadding;

    UPROPERTY(EditAnywhere, Category = Property)
    bool AutoTitleSize;

    UPROPERTY(EditAnywhere, Category = Property)
    EReuseListStyle Style;

    UPROPERTY(EditAnywhere, Category = Property)
    TSubclassOf<UUserWidget> ItemClass;

    //预览个数
    UPROPERTY(EditAnywhere, Category = Property, meta = (ClampMin = "0"))
    int32 PreviewCount;

    //未满状态下Item的对齐样式
    UPROPERTY(EditAnywhere, Category = Property)
    EReuseListNotFullAlignStyle NotFullAlignStyle;

    //未满状态下是否点击穿透
    UPROPERTY(EditAnywhere, Category = Property)
    bool NotFullScrollBoxHitTestInvisible;

    //更新强制布局计算
    UPROPERTY(EditAnywhere, Category = Property)
    bool UpdateForceLayoutPrepass;

    //上下多绘的个数，Grid此参数无效
    UPROPERTY(EditAnywhere, Category = Optimization, meta = (ClampMin = "0"))
    int32 ItemCacheNum;

    //1帧最多花费多少时间处理（毫秒）
    UPROPERTY(EditAnywhere, Category = Optimization, meta = (ClampMin = "0"))
    int32 DelayUpdateTimeLimitMS;

    void NativeConstruct();
    void NativeTick(const FGeometry& MyGeometry, float InDeltaTime);

#if WITH_EDITOR
    void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent);
#endif
    void OnWidgetRebuilt();

    void InitWidgetPtr();
    void ScrollUpdate(float __Offset);
    void UpdateContentSize(TWeakObjectPtr<UWidget> widget);
    void RemoveNotUsed(int32 BIdx, int32 EIdx);
    void DoReload();
    TWeakObjectPtr<UUserWidget> NewItem();
    void ReleaseItem(TWeakObjectPtr<UUserWidget> __Item);
    void ReleaseAllItem();
    void Update();
    void DoJump();
    void ComputeAlignSpace();
    void ComputeScrollBoxHitTest();

    float GetTitleSize();
    void UpdateNamedSlotTitleAnchors();

    int32 GetDelayUpdateTimeLimitMS();
    void AddDelayUpdate(int32 idx);
    void DoDelayUpdate();

    bool IsVertical() const;
    bool IsInvalidParam() const;

    float GetViewSpan() const {
        return (IsVertical() ? ViewSize.Y : ViewSize.X);
    }
    float GetContentSpan() const {
        return (IsVertical() ? ContentSize.Y : ContentSize.X);
    }

    void OnEditReload();

    void ClearCache();

    void SyncProp();

    TWeakObjectPtr<UScrollBox> ScrollBoxList;
    TWeakObjectPtr<UCanvasPanel> CanvasPanelBg;
    TWeakObjectPtr<USizeBox> SizeBoxBg;
    TWeakObjectPtr<UCanvasPanel> CanvasPanelList;
    TWeakObjectPtr<UNamedSlot> NamedSlotTitle;
    
    FVector2D ViewSize;
    FVector2D ContentSize;
    
    int32 ItemCount;
    int32 MaxPos;
    TMap<int32, TWeakObjectPtr<UUserWidget> > ItemMap;
    TArray<TWeakObjectPtr<UUserWidget> > ItemPool;
    TArray<int32> DelayUpdateList;
    int32 ColNum;
    int32 RowNum;
    int32 CurLine;
    int32 JumpIdx;
    EReuseListJumpStyle JumpStyle;
    float AlignSpace;
    int32 LastOffset;
    float CacheTitleSize;
    bool NeedJump;

};

DECLARE_LOG_CATEGORY_EXTERN(LogUReuseListC, Verbose, All);