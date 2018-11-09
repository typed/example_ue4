// Fill out your copyright notice in the Description page of Project Settings.

#include "UReusePageC.h"
#include "Runtime/UMG/Public/Blueprint/WidgetBlueprintLibrary.h"
#include "Runtime/Engine/Classes/Kismet/KismetMathLibrary.h"
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
    ensure(CanvasPanelRoot);
    return true;
}

void UReusePageC::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
    if (!ViewSize.Equals(GetCachedGeometry().GetLocalSize(), 0.0001f)) {
        NeedReload = true;
        IsDrag = false;
        StopSlip();
        SetPageByIdx(Page);
    }
    Update();
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

void UReusePageC::Reload(UClass* __ItemClass, int32 __Count, bool __Loop, bool __StyleUpDown, int32 __DefaultPage)
{
    Count = __Count;
    Loop = __Loop;
    StyleUpDown = __StyleUpDown;
    NeedReload = true;
    IsDrag = false;
    StopSlip();
    SetPageByIdx(__DefaultPage);
    if (Count == 1 && Loop) {
        Loop = false;
    }
    if (ItemClass != __ItemClass) {
        ItemClass = __ItemClass;
        ItemPool.Empty();
        CanvasPanelRoot->ClearChildren();
    }
    if (Count <= 0) {
        NeedReload = false;
        Offset = 0.f;
        NeedUpdateOffset = false;
        NeedUpdatePage = false;
    }
}

void UReusePageC::MoveNextPage()
{
    if (Count > 0) {
        Slip = ViewLen;
        NeedUpdateSlip = true;
    }
}

void UReusePageC::SetPageByIdx(int32 __Idx)
{
    if (Count > 0) {
        SelectPage = __Idx;
        NeedUpdatePage = true;
        StopSlip();
    }
}

UUserWidget* UReusePageC::NewItem()
{
    UUserWidget* tmp = nullptr;
    if (ItemPool.IsValidIndex(0)) {
        tmp = ItemPool[0];
        ItemPool.RemoveAt(0);
    }
    else {
        tmp = CreateWidget<UUserWidget>(GetWorld(), ItemClass);
        CanvasPanelRoot->AddChild(tmp);
    }
    tmp->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
    return tmp;
}

void UReusePageC::ReleaseItem(UUserWidget* widget)
{
    widget->SetVisibility(ESlateVisibility::Collapsed);
    ItemPool.AddUnique(widget);
}

void UReusePageC::RemoveNotUsed(TMap<int32, int32>& ItemIdxMap)
{
    for (TMap<int32, UUserWidget* >::TIterator iter = ItemMap.CreateIterator(); iter; ++iter) {
        if (!ItemIdxMap.Contains(iter->Key)) {
            ReleaseItem(iter->Value);
            iter.RemoveCurrent();
        }
    }
}

void UReusePageC::SetOffset(float __Offset)
{
    if (Loop) {
        Offset = FMath::Fmod(__Offset, ViewLen * Count);
        if (Offset < 0.f) {
            Offset += (ViewLen * Count);
        }
    }
    else {
        Offset = __Offset;
    }
    NeedUpdateOffset = true;
    OffsetToPage();
}

void UReusePageC::StopSlip()
{
    Slip = 0.f;
    NeedUpdateSlip = false;
}

FVector2D UReusePageC::GetMousePoint()
{
    FVector2D ret;
    GetWorld()->GetFirstPlayerController()->GetMousePosition(ret.X, ret.Y);
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
    int32 blk = OffsetToBlock(Offset);
    int32 pg = BlockToPage(blk);
    if (Page != pg) {
        Page = pg;
        EventPageChanged.Broadcast(Page);
    }
}

int32 UReusePageC::BlockToPage(int32 __Block)
{
    int32 tmp = 0;
    if (Loop) {
        tmp = __Block % Count;
        if (tmp < 0) {
            tmp += Count;
        }
    }
    else {
        tmp = FMath::Clamp(__Block, 0, Count - 1);
    }
    return tmp;
}

int32 UReusePageC::OffsetToBlock(float __Offset)
{
    return (__Offset + ViewLen / 2.f) / ViewLen;
}

float UReusePageC::BlockToOffset(int32 __Block)
{
    int32 tmp = 0;
    if (Loop) {
        tmp = __Block;
    }
    else {
        tmp = FMath::Clamp(__Block, 0, Count - 1);
    }
    return tmp * ViewLen;
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
    if (!NeedReload)
        return;
    NeedReload = false;
    ViewSize = GetCachedGeometry().GetLocalSize();
    ViewLen = StyleUpDown ? ViewSize.Y : ViewSize.X;
    for (int32 i = 0; i < CanvasPanelRoot->GetChildrenCount(); i++) {
        UUserWidget* uw = Cast<UUserWidget>(CanvasPanelRoot->GetChildAt(i));
        if (uw) {
            ReleaseItem(uw);
        }
    }
    ItemMap.Empty();
}

void UReusePageC::UpdateDrag()
{
    if (!IsDrag)
        return;
    FVector2D tmp = GetMousePoint();
    float DCur = StyleUpDown ? tmp.Y : tmp.X;
    DDelta = DCur - DStart;
    SetOffset(DOffset - DDelta);
}

void UReusePageC::UpdateOffset()
{
    if (!NeedUpdateOffset)
        return;
    NeedUpdateOffset = false;
    int32 tmp = Offset / ViewLen;
    BeginBlock = tmp - 1;
    EndBlock = tmp + 1;
    TMap<int32, int32> ItemIdxMap;
    for (int32 i = BeginBlock; i <= EndBlock; i++) {
        int32 pg = BlockToPage(i);
        if (Loop) {
            ItemIdxMap.Add(pg, i);
        }
        else {
            ItemIdxMap.Add(pg, pg);
        }
    }
    RemoveNotUsed(ItemIdxMap);
    for (TMap<int32, int32>::TIterator it = ItemIdxMap.CreateIterator(); it; ++it) {
        int32 itmIdx = it->Key;
        int32 idx = it->Value;
        UUserWidget* curitem = nullptr;
        auto ptr = ItemMap.Find(itmIdx);
        if (ptr == nullptr) {
            curitem = NewItem();
            ItemMap.Add(itmIdx, curitem);
            EventUpdateItem.Broadcast(curitem, itmIdx);
        }
        else {
            curitem = *ptr;
        }
        float t = idx * ViewLen - Offset;
        FMargin mar;
        mar.Right = ViewSize.X;
        mar.Bottom = ViewSize.Y;
        if (StyleUpDown) {
            mar.Left = 0.f;
            mar.Top = t;
        }
        else {
            mar.Left = t;
            mar.Top = 0.f;
        }
        auto cps = Cast<UCanvasPanelSlot>(curitem->Slot);
        cps->SetAnchors(FAnchors(0, 0, 0, 0));
        cps->SetOffsets(mar);
    }
}

void UReusePageC::UpdateSlip()
{
    if (!NeedUpdateSlip)
        return;
    float tmp = Slip / SlipParam;
    if (UKismetMathLibrary::InRange_FloatFloat(tmp, -1, 1)) {
        tmp = Slip;
        NeedUpdateSlip = false;
    }
    Slip -= tmp;
    SetOffset(tmp + Offset);
}

void UReusePageC::UpdatePage()
{
    if (!NeedUpdatePage)
        return;
    NeedUpdatePage = false;
    float tmp = BlockToOffset(SelectPage);
    SetOffset(tmp);
}