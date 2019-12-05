// Fill out your copyright notice in the Description page of Project Settings.

/**************************************************************************
Author: levingong
Date: 2019-6-15
Description: 支持item任意大小的重用列表
**************************************************************************/

#pragma once


#include "CoreMinimal.h"
#include "GenericQuadTree.h"
#include "Blueprint/UserWidget.h"
#include "Runtime/SlateCore/Public/Styling/SlateTypes.h"
#include "UReuseListS.generated.h"

/**
*
*/

class UScrollBox;
class USizeBox;
class UCanvasPanel;

UENUM(BlueprintType)
enum class EReuseListSStyle : uint8
{
    Vertical,
    Horizontal,
};

enum EReuseListSMsgType
{
    AdjustItem,
    FillArrOffset,
    AdjustItemWidgetSize,
    DoReload,
    Num,
};

UCLASS()
class EXAMPLE_UE4_API UReuseListS : public UUserWidget
{
    GENERATED_BODY()

public:

    DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnUpdateItemDelegate, UUserWidget*, Widget, int32, Idx);
    DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCreateItemDelegate, UUserWidget*, Widget);

    UReuseListS(const FObjectInitializer& ObjectInitializer);

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
    const FVector2D& GetViewSize() const { return ViewSize; }

    UFUNCTION(BlueprintCallable)
    const FVector2D& GetContentSize() const { return ContentSize; }

    UFUNCTION(BlueprintCallable)
    void Clear() { Reload(0); }

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
    int32 ItemWidth;

    UPROPERTY(EditAnywhere, Category = Property, meta = (ClampMin = "0"))
    int32 ItemHeight;

    UPROPERTY(EditAnywhere, Category = Property, meta = (ClampMin = "0"))
    int32 ItemPaddingX;

    UPROPERTY(EditAnywhere, Category = Property, meta = (ClampMin = "0"))
    int32 ItemPaddingY;

    UPROPERTY(EditAnywhere, Category = Property)
    EReuseListSStyle Style1;

    UPROPERTY(EditAnywhere, Category = Property)
    TSubclassOf<UUserWidget> ItemClass;

    UPROPERTY(EditAnywhere, Category = Property, meta = (ClampMin = "0"))
    int32 PreviewCount;

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
    void RemoveNotUsed(const TArray<int32>& items);
    void DoReload();
    TWeakObjectPtr<UUserWidget> NewItem();
    void ReleaseItem(TWeakObjectPtr<UUserWidget> __Item);
    void ReleaseAllItem();
    void DoJump();

    void ClearSpecialSize();
    void AddSpecialSize(int32 __Idx, const FVector2D& __Size);

    float GetViewSpan() const {
        return (IsVertical() ? ViewSize.Y : ViewSize.X);
    }
    float GetContentSpan() const {
        return (IsVertical() ? ContentSize.Y : ContentSize.X);
    }

    void FillArrOffset();
    FVector2D GetItemSize(int32 idx);

    bool IsVertical() const;
    bool IsInvalidParam() const;

    void ClearCache();

    void SyncProp();

    void OnEditReload();

    void AdjustItem();
    void AdjustItemWidgetSize();

    TWeakObjectPtr<UScrollBox> ScrollBoxList;
    TWeakObjectPtr<UCanvasPanel> CanvasPanelBg;
    TWeakObjectPtr<USizeBox> SizeBoxBg;
    TWeakObjectPtr<UCanvasPanel> CanvasPanelList;

    FVector2D ViewSize;
    FVector2D ContentSize;

    int32 ItemCount;
    int32 MaxPos;
    TMap<int32, TWeakObjectPtr<UUserWidget>> ItemMap;
    TArray<TWeakObjectPtr<UUserWidget>> ItemPool;
    TQuadTree<int32> ArrOffset;
    TMap<int32, FBox2D> TransMap;
    TMap<int32, FVector2D> SpecialSizeMap;
    float LastOffset;
    int8 MsgMap[EReuseListSMsgType::Num];
};

DECLARE_LOG_CATEGORY_EXTERN(LogUReuseListS, Verbose, All);