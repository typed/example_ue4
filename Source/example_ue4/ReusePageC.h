// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Runtime/UMG/Public/UMG.h"
#include "ReusePageC.generated.h"

/**
 * 
 */
UCLASS()
class EXAMPLE_UE4_API UReusePageC : public UUserWidget
{
	GENERATED_BODY()
	
public:

    UReusePageC(const FObjectInitializer& ObjectInitializer);

    UFUNCTION(BlueprintCallable)
    virtual void Reload(UClass* __Class, int32 __ItemCount, bool __Loop = false, bool __StyleUpDown = false, int32 __DefaultPage = 0);

    UFUNCTION(BlueprintCallable)
    virtual void MoveNextPage();

    UFUNCTION(BlueprintCallable)
    virtual void SetPageByIdx(int32 __Idx);


protected:

    virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime);

    virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent);

    virtual void NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent, UDragDropOperation*& OutOperation);

    virtual void NativeOnDragCancelled(const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation);

    virtual void NativeOnMouseCaptureLost();

private:

    TWeakObjectPtr<UCanvasPanel> CanvasPanelRoot;
    TWeakObjectPtr<UClass> ItemClass;
    FVector2D ViewSize;
    TMap<int32, TWeakObjectPtr<UUserWidget> > ItemMap;
    TArray< TWeakObjectPtr<UUserWidget> > ItemPool;
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
