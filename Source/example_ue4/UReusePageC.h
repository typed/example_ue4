// Fill out your copyright notice in the Description page of Project Settings.

/**************************************************************************
Author: levingong
Date: 2018-11-15
Description: ÷ÿ”√“≥«©
**************************************************************************/

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UReusePageC.generated.h"

/**
 * 
 */

UCLASS()
class EXAMPLE_UE4_API UReusePageC : public UUserWidget
{
	GENERATED_BODY()
	
public:

    DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnUpdateItemDelegate, UUserWidget*, widget, int32, idx);
    DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPageChangedDelegate, int32, PageIdx);
    DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCreateItemDelegate, UUserWidget*, widget);

    UReusePageC(const FObjectInitializer& ObjectInitializer);

    UPROPERTY(BlueprintAssignable)
    FOnUpdateItemDelegate OnUpdateItem;

    UPROPERTY(BlueprintAssignable)
    FOnPageChangedDelegate OnPageChanged;

    UPROPERTY(BlueprintAssignable)
    FOnCreateItemDelegate OnCreateItem;

    UFUNCTION(BlueprintCallable)
    virtual bool Reload(int32 __Count);

    UFUNCTION(BlueprintCallable)
    virtual void MoveNextPage();

    UFUNCTION(BlueprintCallable)
    virtual void SetPage(int32 __Idx);

    UFUNCTION(BlueprintCallable)
    virtual int32 GetPage() const { return Page; }

    virtual bool Initialize();

    UFUNCTION(BlueprintCallable)
    virtual void ClearCache();

    //UVisual interface
    virtual void ReleaseSlateResources(bool bReleaseChildren) override;
    //~ End UVisual Interface

    //~ Begin UWidget Interface
    virtual void SynchronizeProperties() override;
    //~ End UWidget Interface

protected:

    UPROPERTY(EditAnywhere, Category = Property, meta = (BlueprintBaseOnly = ""))
    class UWidgetBlueprintGeneratedClass* ItemClass;

    UPROPERTY(EditAnywhere, Category = Property)
    bool Loop;

    UPROPERTY(EditAnywhere, Category = Property)
    bool StyleUpDown;

    UPROPERTY(EditAnywhere, Category = Property, meta = (ClampMin = "0"))
    int32 PreviewCount;
    FTimerHandle tmhOnPreviewTick;

    UPROPERTY(EditAnywhere, Category = Property, meta = (ClampMin = "1"))
    float DChgPageParam;

    UPROPERTY(EditAnywhere, Category = Property, meta = (ClampMin = "1"))
    float SlipParam;

    virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime);

    virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent);

    virtual void NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent, UDragDropOperation*& OutOperation);

    virtual void NativeOnDragCancelled(const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation);

    virtual void NativeOnMouseCaptureLost();

    UUserWidget* NewItem();
    void ReleaseItem(UUserWidget* widget);
    void RemoveNotUsed(TMap<int32, int32>& ItemIdxMap);
    void SetOffset(float __Offset);
    void StopSlip();
    FVector2D GetMousePoint();
    void Drop();

    void OffsetToPage();
    int32 BlockToPage(int32 __Block);
    int32 OffsetToBlock(float __Offset);
    float BlockToOffset(int32 __Block);

    void Update();
    void UpdateReload();
    void UpdateDrag();
    void UpdateOffset();
    void UpdateSlip();
    void UpdatePage();

    void OnPreviewTick();

    bool IsValidClass() const;

    class UCanvasPanel* CanvasPanelRoot;
    FVector2D ViewSize;
    TMap<int32, UUserWidget* > ItemMap;
    TArray< UUserWidget* > ItemPool;
    int32 Count;
    int32 Page;
    int32 BeginBlock;
    int32 EndBlock;
    int32 SelectPage;
    float Offset;
    float ViewLen;
    float DStart;
    float DOffset;
    float DDelta;
    float Slip;
    bool IsDrag;
    bool NeedReload;
    bool NeedUpdateOffset;
    bool NeedUpdateSlip;
    bool NeedUpdatePage;

	
};

DECLARE_LOG_CATEGORY_EXTERN(LogUReusePageC, Verbose, All);