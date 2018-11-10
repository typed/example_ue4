// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Runtime/UMG/Public/UMG.h"
#include "UReusePageC.generated.h"

/**
 * 
 */
UCLASS()
class EXAMPLE_UE4_API UReusePageC : public UUserWidget
{
	GENERATED_BODY()
	
public:

    DECLARE_DYNAMIC_MULTICAST_DELEGATE(FEventDropDelegate);
    DECLARE_DYNAMIC_MULTICAST_DELEGATE(FEventDropStartDelegate);
    DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FEventUpdateItemDelegate, UUserWidget*, widget, int32, idx);
    DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FEventPageChangedDelegate, int32, PageIdx);
    DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FEventCreateItemDelegate, UUserWidget*, widget);
    DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FEventDestroyItemDelegate, UUserWidget*, widget);

    UReusePageC(const FObjectInitializer& ObjectInitializer);

    UPROPERTY(BlueprintAssignable)
    FEventDropDelegate EventDrop;

    UPROPERTY(BlueprintAssignable)
    FEventDropStartDelegate EventDropStart;

    UPROPERTY(BlueprintAssignable)
    FEventUpdateItemDelegate EventUpdateItem;

    UPROPERTY(BlueprintAssignable)
    FEventPageChangedDelegate EventPageChanged;

    UPROPERTY(BlueprintAssignable)
    FEventCreateItemDelegate EventCreateItem;

    UPROPERTY(BlueprintAssignable)
    FEventDestroyItemDelegate EventDestroyItem;

    UFUNCTION(BlueprintCallable)
    virtual void Reload(UClass* __ItemClass, int32 __Count, bool __Loop = false, bool __StyleUpDown = false, int32 __DefaultPage = 0);

    UFUNCTION(BlueprintCallable)
    virtual void MoveNextPage();

    UFUNCTION(BlueprintCallable)
    virtual void SetPageByIdx(int32 __Idx);

    virtual bool Initialize();

    UFUNCTION(BlueprintCallable)
    virtual void ClearCache();

protected:

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

private:

    UCanvasPanel* CanvasPanelRoot;
    UClass* ItemClass;
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
    float DChgPageParam;
    float Slip;
    float SlipParam;
    bool IsDrag;
    bool StyleUpDown;
    bool Loop;
    bool NeedReload;
    bool NeedUpdateOffset;
    bool NeedUpdateSlip;
    bool NeedUpdatePage;

	
};
