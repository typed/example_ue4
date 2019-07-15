// Fill out your copyright notice in the Description page of Project Settings.

/**************************************************************************
Author: levingong
Date: 2018-11-15
Description: 重用列表
**************************************************************************/

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UReuseListSp.generated.h"

/**
*
*/

class UScrollBox;
class USizeBox;
class UCanvasPanel;

UENUM(BlueprintType)
enum class EReuseListSpStyle : uint8
{
    Vertical,
    Horizontal,
};

UENUM(BlueprintType)
enum class EReuseListSpNotFullAlignStyle : uint8
{
    Start,
    Middle,
    End,
};

UCLASS()
class EXAMPLE_UE4_API UReuseListSp : public UUserWidget
{
    GENERATED_BODY()

public:

    DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnUpdateItemDelegate, UUserWidget*, Widget, int32, Idx);
    DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCreateItemDelegate, UUserWidget*, widget);

    UReuseListSp(const FObjectInitializer& ObjectInitializer);

    UPROPERTY(BlueprintAssignable)
    FOnUpdateItemDelegate OnUpdateItem;

    UPROPERTY(BlueprintAssignable)
    FOnCreateItemDelegate OnCreateItem;

    UFUNCTION(BlueprintCallable)
    void Reload(int32 __ItemCount);

    UFUNCTION(BlueprintCallable)
    void Refresh();

    UFUNCTION(BlueprintCallable)
    void RefreshOne(int32 __Idx);

    UFUNCTION(BlueprintCallable)
    void ScrollToStart();

    UFUNCTION(BlueprintCallable)
    void ScrollToEnd();

    UFUNCTION(BlueprintCallable)
    void SetScrollOffset(float NewScrollOffset);

    UFUNCTION(BlueprintCallable)
    float GetScrollOffset() const;

    UFUNCTION(BlueprintCallable)
    const FVector2D& GetViewSize() const;

    UFUNCTION(BlueprintCallable)
    const FVector2D& GetContentSize() const;

    UFUNCTION(BlueprintCallable)
    void JumpByIdx(int32 __Idx);

    UFUNCTION(BlueprintCallable)
    void Clear();

    UFUNCTION(BlueprintCallable)
    void Reset(TSubclassOf<UUserWidget> __ItemClass, EReuseListSpStyle __Style, int32 __ItemSize, int32 __ItemPadding);

    UFUNCTION(BlueprintCallable)
    void ClearSpecialSize();

    UFUNCTION(BlueprintCallable)
    void AddSpecialSize(int32 __Idx, int32 __Size);

protected:

    bool Initialize();

    UPROPERTY(EditAnywhere, Category = Property)
    FScrollBoxStyle ScrollBoxStyle;

    UPROPERTY(EditAnywhere, Category = Property)
    FScrollBarStyle ScrollBarStyle;

    UPROPERTY(EditAnywhere, Category = Property)
    ESlateVisibility ScrollBarVisibility;

    UPROPERTY(EditAnywhere, Category = Property)
    FVector2D ScrollBarThickness;

    UPROPERTY(EditAnywhere, Category = Property, meta = (ClampMin = "0"))
    int32 ItemSize;

    UPROPERTY(EditAnywhere, Category = Property, meta = (ClampMin = "0"))
    int32 ItemPadding;

    UPROPERTY(EditAnywhere, Category = Property)
    EReuseListSpStyle Style;

    UPROPERTY(EditAnywhere, Category = Property)
    TSubclassOf<UUserWidget> ItemClass;

    UPROPERTY(EditAnywhere, Category = Property, meta = (ClampMin = "0"))
    int32 PreviewCount;

    UPROPERTY(EditAnywhere, Category = Property)
    EReuseListSpNotFullAlignStyle NotFullAlignStyle;

    UPROPERTY(EditAnywhere, Category = Property)
    bool NotFullScrollBoxHitTestInvisible;

    UPROPERTY(EditAnywhere, Category = Property)
    bool AutoAdjustItemSize;

    UPROPERTY(EditAnywhere, Category = Property, meta = (ClampMin = "1"))
    int32 ItemPoolMaxNum;

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
    void DoJump();
    float GetAlignSpace();
    void ComputeScrollBoxHitTest();

    void FillArrOffset();
    int32 GetItemSize(int32 idx);

    void AdjustItem();
    void AdjustItemWidgetSize();
    void AdjustScrollOffset();

    bool IsVertical() const;
    bool IsInvalidParam() const;

    void ClearCache();

    void SyncProp();

    TWeakObjectPtr<UScrollBox> ScrollBoxList;
    TWeakObjectPtr<UCanvasPanel> CanvasPanelBg;
    TWeakObjectPtr<USizeBox> SizeBoxBg;
    TWeakObjectPtr<UCanvasPanel> CanvasPanelList;

    FVector2D ViewSize;
    FVector2D ContentSize;

    int32 ItemCount;
    int32 MaxPos;
    TMap<int32, TWeakObjectPtr<UUserWidget> > ItemMap;
    TArray<TWeakObjectPtr<UUserWidget> > ItemPool;
    TArray<int32> ArrOffset;
    TMap<int32, int32> SpecialSizeMap;
    int32 JumpIdx;
    int32 CurJumpOffsetIdx;
    int32 LastOffset;
    bool NeedJump;
    bool NeedFillArrOffset;
    bool NeedAdjustItemWidgetSize;
    bool NeedAdjustScrollOffset;
    int32 NeedAdjustItem;

};

DECLARE_LOG_CATEGORY_EXTERN(LogUReuseListSp, Verbose, All);