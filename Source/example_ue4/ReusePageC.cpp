// Fill out your copyright notice in the Description page of Project Settings.

#include "ReusePageC.h"
#include "Runtime/UMG/Public/Blueprint/WidgetBlueprintLibrary.h"

UReusePageC::UReusePageC(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer)
    , CanvasPanelRoot(nullptr)
    , ViewSize(FVector2D::ZeroVector)
    , Count(0)
    , Page(0)
    , BeginBlock(0)
    , EndBlock(0)
    , SelectPage(0)
    , Offset(0)
    , ViewLen(0)
    , DStart(0)
    , DOffset(0)
    , DDelta(0)
    , DChgPageParam(0)
    , Slip(0)
    , SlipParam(0)
    , IsDrag(false)
    , StyleUpDown(false)
    , Loop(false)
    , NeedReload(false)
    , NeedUpdateOffset(false)
    , NeedUpdateSlip(false)
    , NeedUpdatePage(false)
{

}

void UReusePageC::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{

}

FReply UReusePageC::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
    Super::NativeOnMouseButtonDown(InGeometry, InMouseEvent);
    FEventReply reply = UWidgetBlueprintLibrary::DetectDragIfPressed(InMouseEvent, this, EKeys::LeftMouseButton);
    return reply.NativeReply;
}

void UReusePageC::NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent, UDragDropOperation*& OutOperation)
{
    Super::NativeOnDragDetected(InGeometry, InMouseEvent, OutOperation);
}

void UReusePageC::NativeOnDragCancelled(const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
    Super::NativeOnDragCancelled(InDragDropEvent, InOperation);
}

void UReusePageC::NativeOnMouseCaptureLost()
{
    Super::NativeOnMouseCaptureLost();
}

void UReusePageC::Reload(UClass* __Class, int32 __ItemCount, bool __Loop, bool __StyleUpDown, int32 __DefaultPage)
{

}

void UReusePageC::MoveNextPage()
{

}

void UReusePageC::SetPageByIdx(int32 __Idx)
{

}

bool UReusePageC::Initialize()
{
    if (!Super::Initialize())
        return false;
    CanvasPanelRoot = Cast<UCanvasPanel>(GetWidgetFromName(FName(TEXT("CanvasPanelRoot"))));
    ensure(CanvasPanelRoot.IsValid());
    return true;
}