// Fill out your copyright notice in the Description page of Project Settings.

#include "UReusePageC.h"
#include "Runtime/UMG/Public/Components/CanvasPanel.h"
#include "Runtime/UMG/Public/Components/CanvasPanelSlot.h"
#include "Runtime/UMG/Public/Blueprint/WidgetBlueprintLibrary.h"
#include "Runtime/Engine/Classes/Kismet/KismetMathLibrary.h"

DEFINE_LOG_CATEGORY(LogUReusePageC);

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
    , DChgPageParam(4.f)
    , Slip(0)
    , SlipParam(3.f)
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

void UReusePageC::ClearCache()
{
    ItemPool.Empty();
    CanvasPanelRoot->ClearChildren();
}

void UReusePageC::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
    if (!IsValidClass())
        return;
    if (!ViewSize.Equals(GetCachedGeometry().GetLocalSize(), 0.0001f)) {
        NeedReload = true;
        IsDrag = false;
        StopSlip();
        SetPage(Page);
    }
    Update();
}

FReply UReusePageC::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
    Super::NativeOnMouseButtonDown(InGeometry, InMouseEvent);
    FEventReply reply = UWidgetBlueprintLibrary::DetectDragIfPressed(InMouseEvent, this, EKeys::LeftMouseButton);
    //UE_LOG(LogUReusePageC, Log, TEXT("UReusePageC::NativeOnMouseButtonDown"));
    return reply.NativeReply;
}

void UReusePageC::NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent, UDragDropOperation*& OutOperation)
{
    Super::NativeOnDragDetected(InGeometry, InMouseEvent, OutOperation);
    if (Count <= 0)
        return;
    OutOperation = UWidgetBlueprintLibrary::CreateDragDropOperation(nullptr);
    //UE_LOG(LogUReusePageC, Log, TEXT("UReusePageC::NativeOnDragDetected"));
    IsDrag = true;
    DOffset = Offset;
    FVector2D pt = GetMousePoint();
    DStart = StyleUpDown ? pt.Y : pt.X;
    StopSlip();
}

void UReusePageC::NativeOnDragCancelled(const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
    Super::NativeOnDragCancelled(InDragDropEvent, InOperation);
    //UE_LOG(LogUReusePageC, Log, TEXT("UReusePageC::NativeOnDragCancelled"));
    Drop();
}

void UReusePageC::NativeOnMouseCaptureLost()
{
    Super::NativeOnMouseCaptureLost();
    //UE_LOG(LogUReusePageC, Log, TEXT("UReusePageC::NativeOnMouseCaptureLost"));
    Drop();
}

bool UReusePageC::Reload(int32 __Count)
{
    if (__Count < 0)
        return false;
    Count = __Count;
    NeedReload = true;
    IsDrag = false;
    NeedUpdateOffset = true;
    StopSlip();
    if (Count == 1 && Loop) {
        Loop = false;
    }
    if (Count <= 0) {
        NeedReload = false;
        Offset = 0.f;
        NeedUpdateOffset = false;
        NeedUpdatePage = false;
    }
    return true;
}

void UReusePageC::MoveNextPage()
{
    if (Count > 0) {
        Slip = ViewLen;
        if (!Loop) {
            Slip = FMath::Min(Offset + Slip, (ViewLen * (Count - 1))) - Offset;
        }
        NeedUpdateSlip = true;
    }
}

void UReusePageC::SetPage(int32 __Idx)
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
        tmp = ItemPool[0].Get();
        ItemPool.RemoveAt(0);
        tmp->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
    }
    else {
        tmp = CreateWidget<UUserWidget>(GetWorld(), ItemClass);
        if (tmp == nullptr) {
            return nullptr;
        }
        CanvasPanelRoot->AddChild(tmp);
        tmp->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
        OnCreateItem.Broadcast(tmp);
    }
    return tmp;
}

void UReusePageC::ReleaseItem(UUserWidget* widget)
{
    widget->SetVisibility(ESlateVisibility::Collapsed);
    ItemPool.AddUnique(widget);
}

void UReusePageC::RemoveNotUsed(TMap<int32, int32>& ItemIdxMap)
{
    for (TMap<int32, TWeakObjectPtr<UUserWidget> >::TIterator iter(ItemMap); iter; ++iter) {
        if (!ItemIdxMap.Contains(iter->Key)) {
            ReleaseItem(iter->Value.Get());
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
    int32 blk = (Offset + Tmp) / ViewLen;
    Slip = BlockToOffset(blk) - Offset;
    NeedUpdateSlip = true;
}

void UReusePageC::OffsetToPage()
{
    int32 blk = OffsetToBlock(Offset);
    int32 pg = BlockToPage(blk);
    if (Page != pg) {
        Page = pg;
        OnPageChanged.Broadcast(Page);
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
    for (TMap<int32, int32>::TIterator it(ItemIdxMap); it; ++it) {
        int32 itmIdx = it->Key;
        int32 idx = it->Value;
        UUserWidget* curitem = nullptr;
        TWeakObjectPtr<UUserWidget>* ptr = ItemMap.Find(itmIdx);
        if (ptr == nullptr || !(*ptr).IsValid()) {
            curitem = NewItem();
            if (curitem) {
                ItemMap.Add(itmIdx, curitem);
                OnUpdateItem.Broadcast(curitem, itmIdx);
            }
        }
        else {
            curitem = (*ptr).Get();
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
        UCanvasPanelSlot* cps = Cast<UCanvasPanelSlot>(curitem->Slot);
        if (cps) {
            cps->SetAnchors(FAnchors(0, 0, 0, 0));
            cps->SetOffsets(mar);
        }
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
    float to = tmp + Offset;
    //UE_LOG(LogUReusePageC, Log, TEXT("UpdateSlip %f"), to);
    SetOffset(to);
}

void UReusePageC::UpdatePage()
{
    if (!NeedUpdatePage)
        return;
    NeedUpdatePage = false;
    float tmp = BlockToOffset(SelectPage);
    SetOffset(tmp);
}

void UReusePageC::OnPreviewTick()
{
    NativeTick(GetCachedGeometry(), 0.25f);
}

bool UReusePageC::IsValidClass() const
{
    return ItemClass != nullptr;
}