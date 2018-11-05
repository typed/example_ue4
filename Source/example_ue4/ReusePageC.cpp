// Fill out your copyright notice in the Description page of Project Settings.

#include "ReusePageC.h"
#include "Runtime/UMG/Public/Blueprint/WidgetBlueprintLibrary.h"
#include "LogDefine.h"

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

bool UReusePageC::Initialize()
{
    if (!Super::Initialize())
        return false;
    CanvasPanelRoot = Cast<UCanvasPanel>(GetWidgetFromName(FName(TEXT("CanvasPanelRoot"))));
    ensure(CanvasPanelRoot.IsValid());
    return true;
}

void UReusePageC::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{

}

FReply UReusePageC::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
    Super::NativeOnMouseButtonDown(InGeometry, InMouseEvent);
    FEventReply reply = UWidgetBlueprintLibrary::DetectDragIfPressed(InMouseEvent, this, EKeys::LeftMouseButton);
    //UE_LOG(LogMyUMG, Log, TEXT("UReusePageC::NativeOnMouseButtonDown"));
    return reply.NativeReply;
}

void UReusePageC::NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent, UDragDropOperation*& OutOperation)
{
    Super::NativeOnDragDetected(InGeometry, InMouseEvent, OutOperation);
    if (Count <= 0)
        return;
    OutOperation = UWidgetBlueprintLibrary::CreateDragDropOperation(nullptr);
    //UE_LOG(LogMyUMG, Log, TEXT("UReusePageC::NativeOnDragDetected"));
}

void UReusePageC::NativeOnDragCancelled(const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
    Super::NativeOnDragCancelled(InDragDropEvent, InOperation);
    //UE_LOG(LogMyUMG, Log, TEXT("UReusePageC::NativeOnDragCancelled"));
    Drop();
}

void UReusePageC::NativeOnMouseCaptureLost()
{
    Super::NativeOnMouseCaptureLost();
    //UE_LOG(LogMyUMG, Log, TEXT("UReusePageC::NativeOnMouseCaptureLost"));
    Drop();
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

UUserWidget* UReusePageC::NewItem()
{
    return nullptr;
}

void UReusePageC::ReleaseItem(UUserWidget* widget)
{

}

void UReusePageC::RemoveNotUsed(TMap<int32, int32>& ItemIdxMap)
{

}

void UReusePageC::SetOffset(float __Offset)
{

}

void UReusePageC::StopSlip()
{

}

FVector2D UReusePageC::GetMousePoint()
{
    FVector2D ret;
    return ret;
}

void UReusePageC::Drop()
{
    IsDrag = false;
    float Tmp = 0.f;
    if (DDelta > 0.f) {
        Tmp = DChgPageParam / ViewLen;
    }
    else {
        Tmp = ViewLen - DChgPageParam / ViewLen;
    }
    DStart = 0.f;
    DOffset = 0.f;
}

void UReusePageC::OffsetToPage()
{

}

int32 UReusePageC::BlockToPage(int32 __Block)
{
    return 0;
}

int32 UReusePageC::OffsetToBlock(float __Offset)
{
    return 0;
}

float UReusePageC::BlockToOffset(int32 __Block)
{
    return 0;
}

void UReusePageC::Update()
{
    UpdateReload();
    UpdateDrag();
    UpdateSlip();
    UpdatePage();
    UpdateOffset();
}

void UReusePageC::UpdateReload()
{

}

void UReusePageC::UpdateDrag()
{

}

void UReusePageC::UpdateOffset()
{

}

void UReusePageC::UpdateSlip()
{

}

void UReusePageC::UpdatePage()
{

}