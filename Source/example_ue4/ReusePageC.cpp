// Fill out your copyright notice in the Description page of Project Settings.

#include "ReusePageC.h"
#include "Runtime/UMG/Public/Blueprint/WidgetBlueprintLibrary.h"

UReusePageC::UReusePageC(const FObjectInitializer& ObjectInitializer)
    :Super(ObjectInitializer)
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