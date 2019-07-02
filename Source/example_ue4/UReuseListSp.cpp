// Fill out your copyright notice in the Description page of Project Settings.

#include "UReuseListSp.h"
#include "Runtime/UMG/Public/Components/SizeBox.h"
#include "Runtime/UMG/Public/Components/CanvasPanel.h"
#include "Runtime/UMG/Public/Components/CanvasPanelSlot.h"
#include "Runtime/UMG/Public/Components/ScrollBox.h"
#include "Runtime/UMG/Public/Blueprint/WidgetBlueprintGeneratedClass.h"
#include "Runtime/Engine/Classes/Kismet/KismetMathLibrary.h"

DEFINE_LOG_CATEGORY(LogUReuseListSp);

UReuseListSp::UReuseListSp(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer)
    , ScrollBoxList(nullptr)
    , CanvasPanelBg(nullptr)
    , SizeBoxBg(nullptr)
    , CanvasPanelList(nullptr)
    , ViewSize(FVector2D::ZeroVector)
    , ContentSize(FVector2D::ZeroVector)
    , ItemClass(nullptr)
    , ItemPadding(0)
    , ItemCount(0)
    , MaxPos(0)
    , Style(EReuseListSpStyle::Vertical)
    , ItemSize(100)
    , JumpIdx(0)
    , JumpStyle(EReuseListSpJumpStyle::Middle)
    , NeedJump(false)
    , NeedFillArrOffset(false)
    , NeedAdjustItem(false)
    , NeedAdjustItemWidgetSize(false)
    , NeedAdjustScrollOffset(false)
    , PreviewCount(5)
    , ScrollBarVisibility(ESlateVisibility::Collapsed)
    , NotFullAlignStyle(EReuseListSpNotFullAlignStyle::Start)
    , NotFullScrollBoxHitTestInvisible(false)
    , AutoAdjustItemSize(true)
{
    ScrollBoxStyle.LeftShadowBrush = FSlateNoResource();
    ScrollBoxStyle.TopShadowBrush = FSlateNoResource();
    ScrollBoxStyle.RightShadowBrush = FSlateNoResource();
    ScrollBoxStyle.BottomShadowBrush = FSlateNoResource();
}

bool UReuseListSp::Initialize()
{
    if (!Super::Initialize())
        return false;
    InitWidgetPtr();
    SyncProp();
    return true;
}

void UReuseListSp::NativeConstruct()
{
    Super::NativeConstruct();
    InitWidgetPtr();
    SyncProp();
}

void UReuseListSp::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
    Super::NativeTick(MyGeometry, InDeltaTime);

    if (IsInvalidParam())
        return;

    if (NeedAdjustItem) {
        NeedAdjustItem = false;
        AdjustItem();
    }
    if (NeedFillArrOffset) {
        NeedFillArrOffset = false;
        FillArrOffset();
    }
    if (NeedAdjustItemWidgetSize) {
        NeedAdjustItemWidgetSize = false;
        AdjustItemWidgetSize();
    }

    const FVector2D& lzSz = GetCachedGeometry().GetLocalSize();
    if (!ViewSize.Equals(lzSz, 0.0001f)) {
        DoReload();
    }

    ScrollUpdate(ScrollBoxList->GetScrollOffset());

    if (NeedJump) {
        NeedJump = false;
        DoJump();
    }

    if (NeedAdjustScrollOffset) {
        NeedAdjustScrollOffset = false;
        AdjustScrollOffset();
    }

}

void UReuseListSp::Reload(int32 __ItemCount)
{
    ItemCount = __ItemCount;
    ScrollBoxList->SetOrientation(IsVertical() ? Orient_Vertical : Orient_Horizontal);
    ScrollBoxList->SetScrollBarVisibility(ScrollBarVisibility);
    const FVector2D& lzSz = GetCachedGeometry().GetLocalSize();
    if (lzSz.Equals(FVector2D::ZeroVector, 0.0001f)) {
        ViewSize = lzSz;
        ReleaseAllItem();
        return;
    }
    FillArrOffset();
    DoReload();
}

void UReuseListSp::Refresh()
{
    for (TMap<int32, TWeakObjectPtr<UUserWidget> >::TConstIterator iter(ItemMap); iter; ++iter) {
        RefreshOne(iter->Key);
    }
}

void UReuseListSp::RefreshOne(int32 __Idx)
{
    TWeakObjectPtr<UUserWidget>* v = ItemMap.Find(__Idx);
    if (v) {
        OnUpdateItem.Broadcast(v->Get(), __Idx);
        NeedAdjustItem = true;
    }
}

void UReuseListSp::ScrollToStart()
{
    ScrollBoxList->ScrollToStart();
}

void UReuseListSp::ScrollToEnd()
{
    ScrollBoxList->ScrollToEnd();
}

void UReuseListSp::SetScrollOffset(float NewScrollOffset)
{
    ScrollBoxList->SetScrollOffset(NewScrollOffset);
}

float UReuseListSp::GetScrollOffset() const
{
    return ScrollBoxList->GetScrollOffset();
}

const FVector2D& UReuseListSp::GetViewSize() const
{
    return ViewSize;
}

const FVector2D& UReuseListSp::GetContentSize() const
{
    return ContentSize;
}

// this jump function is not good!
void UReuseListSp::JumpByIdxStyle(int32 __Idx, EReuseListSpJumpStyle __Style)
{
    JumpIdx = __Idx;
    JumpStyle = __Style;
    NeedJump = true;
}

void UReuseListSp::Clear()
{
    Reload(0);
}

void UReuseListSp::InitWidgetPtr()
{
    ScrollBoxList = Cast<UScrollBox>(GetWidgetFromName(FName(TEXT("ScrollBoxList"))));
    ensure(ScrollBoxList.IsValid());

    CanvasPanelBg = Cast<UCanvasPanel>(GetWidgetFromName(FName(TEXT("CanvasPanelBg"))));
    ensure(CanvasPanelBg.IsValid());

    SizeBoxBg = Cast<USizeBox>(GetWidgetFromName(FName(TEXT("SizeBoxBg"))));
    ensure(SizeBoxBg.IsValid());

    CanvasPanelList = Cast<UCanvasPanel>(GetWidgetFromName(FName(TEXT("CanvasPanelList"))));
    ensure(CanvasPanelList.IsValid());
}

float UReuseListSp::GetAlignSpace()
{
    float AlignSpace = 0.f;
    float content = 0.f;
    float view = 0.f;
    if (IsVertical()) {
        content = ContentSize.Y;
        view = ViewSize.Y;
    }
    else {
        content = ContentSize.X;
        view = ViewSize.X;
    }
    if (content < view) {
        switch (NotFullAlignStyle)
        {
        case EReuseListSpNotFullAlignStyle::Start:
            AlignSpace = 0.f;
            break;
        case EReuseListSpNotFullAlignStyle::Middle:
            AlignSpace = (view - content) / 2.f;
            break;
        case EReuseListSpNotFullAlignStyle::End:
            AlignSpace = view - content;
            break;
        }
    }
    return AlignSpace;
}

void UReuseListSp::ComputeScrollBoxHitTest()
{
    if (NotFullScrollBoxHitTestInvisible) {
        float vlen = (IsVertical() ? ViewSize.Y : ViewSize.X);
        float clen = (IsVertical() ? ContentSize.Y : ContentSize.X);
        if (vlen > clen) {
            ScrollBoxList->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
            return;
        }
    }
    ScrollBoxList->SetVisibility(ESlateVisibility::Visible);
}

void UReuseListSp::ClearSpecialSize()
{
    SpecialSizeMap.Empty();
    NeedFillArrOffset = true;
    NeedAdjustItemWidgetSize = true;
}

void UReuseListSp::AddSpecialSize(int32 __Idx, int32 __Size)
{
    SpecialSizeMap.Add(__Idx, __Size);
    NeedFillArrOffset = true;
    NeedAdjustItemWidgetSize = true;
}

void UReuseListSp::FillArrOffset()
{
    ArrOffset.Empty();
    int32 tmpOffset = 0;
    for (int32 i = 0; i < ItemCount; i++) {
        ArrOffset.Add(tmpOffset);
        tmpOffset += (GetItemSize(i) + ItemPadding);
    }
    MaxPos = tmpOffset;
    MaxPos -= ItemPadding;

    if (IsVertical()) {
        ContentSize.X = ViewSize.X;
        ContentSize.Y = MaxPos;
    }
    else {
        ContentSize.X = MaxPos;
        ContentSize.Y = ViewSize.Y;
    }
    UpdateContentSize(SizeBoxBg);
    UpdateContentSize(CanvasPanelList);

    NeedAdjustScrollOffset = true;

}

int32 UReuseListSp::GetItemSize(int32 idx)
{
    const int32* pValue = SpecialSizeMap.Find(idx);
    if (pValue)
        return (*pValue > 0 ? *pValue : 1);
    else
        return ItemSize;
}

void UReuseListSp::ScrollUpdate(float __Offset)
{
    int32 Offset = __Offset;
    int32 OffsetEnd = 0;
    int32 BIdx = 0;
    int32 EIdx = 0;
    Offset = UKismetMathLibrary::Max(Offset, 0);
    Offset = UKismetMathLibrary::Min(Offset, MaxPos);
    int32 vlen = (IsVertical() ? ViewSize.Y : ViewSize.X);
    OffsetEnd = Offset + vlen;

    BIdx = Algo::LowerBound(ArrOffset, Offset) - 1;
    BIdx = UKismetMathLibrary::Max(BIdx, 0);
    EIdx = Algo::LowerBound(ArrOffset, OffsetEnd);
    EIdx = UKismetMathLibrary::Min(EIdx, ItemCount - 1);

    //UE_LOG(LogUReuseListSp, Log, TEXT("UReuseListSp::ScrollUpdate Offset=%d BIdx=%d EIdx=%d"), Offset, BIdx, EIdx);

    RemoveNotUsed(BIdx, EIdx);

    float AlignSpace = GetAlignSpace();
    for (int32 i = BIdx; i <= EIdx; i++) {
        if (!ItemMap.Contains(i)) {
            TWeakObjectPtr<UUserWidget> w = NewItem();
            if (w.IsValid()) {
                UCanvasPanelSlot* cps = Cast<UCanvasPanelSlot>(w->Slot);
                if (cps) {
                    cps->SetAnchors(FAnchors(0, 0, 0, 0));
                    int32 offset = (ArrOffset.IsValidIndex(i) ? ArrOffset[i] : 0);
                    int32 sz_item = GetItemSize(i);
                    if (IsVertical())
                        cps->SetOffsets(FMargin(0, offset + AlignSpace, ViewSize.X, sz_item));
                    else
                        cps->SetOffsets(FMargin(offset + AlignSpace, 0, sz_item, ViewSize.Y));
                    ItemMap.Add(i, w);
                    OnUpdateItem.Broadcast(w.Get(), i);
                    NeedAdjustItem = true;
                }
            }
        }
    }
}

void UReuseListSp::UpdateContentSize(TWeakObjectPtr<UWidget> widget)
{
    UCanvasPanelSlot* cps = Cast<UCanvasPanelSlot>(widget->Slot);
    if (cps) {
        if (IsVertical()) {
            cps->SetAnchors(FAnchors(0, 0, 1, 0));
            cps->SetOffsets(FMargin(0, 0, 0, ContentSize.Y));
        }
        else {
            cps->SetAnchors(FAnchors(0, 0, 0, 1));
            cps->SetOffsets(FMargin(0, 0, ContentSize.X, 0));
        }
    }
}

void UReuseListSp::RemoveNotUsed(int32 BIdx, int32 EIdx)
{
    for (TMap<int32, TWeakObjectPtr<UUserWidget> >::TIterator iter = ItemMap.CreateIterator(); iter; ++iter) {
        if (!UKismetMathLibrary::InRange_IntInt(iter->Key, BIdx, EIdx)) {
            ReleaseItem(iter->Value);
            iter.RemoveCurrent();
        }
    }
}

void UReuseListSp::DoReload()
{
    if (IsInvalidParam())
        return;
    ViewSize = GetCachedGeometry().GetLocalSize();
    FillArrOffset();
    ComputeScrollBoxHitTest();
    for (int32 i = 0; i < CanvasPanelList->GetChildrenCount(); i++) {
        UUserWidget* uw = Cast<UUserWidget>(CanvasPanelList->GetChildAt(i));
        if (uw) {
            ReleaseItem(uw);
        }
    }
    ItemMap.Empty();
    NeedAdjustScrollOffset = true;
}

void UReuseListSp::DoJump()
{

    if (JumpIdx < 0 || JumpIdx >= ItemCount) {
        return;
    }

    float sz_view = IsVertical() ? ViewSize.Y : ViewSize.X;
    float sz_content = IsVertical() ? ContentSize.Y : ContentSize.X;

    if (sz_content < sz_view) {
        return;
    }

    float tmpScroll = 0;
    int32 ItemSizeAndPad = GetItemSize(JumpIdx) + ItemPadding;
    int32 tmpItemOffset = 0;

    if (JumpStyle == EReuseListSpJumpStyle::Begin) {
        tmpItemOffset = 0;
    }
    else if (JumpStyle == EReuseListSpJumpStyle::End) {
        tmpItemOffset = (int32)sz_view - ItemSizeAndPad;
    }
    else {
        tmpItemOffset = ((int32)sz_view - ItemSizeAndPad) / 2;
    }
    for (int32 i = 0; i < JumpIdx; i++) {
        tmpScroll += (GetItemSize(i) + ItemPadding);
    }
    tmpScroll -= tmpItemOffset;

    tmpScroll = UKismetMathLibrary::FMax(tmpScroll, 0);
    tmpScroll = UKismetMathLibrary::FMin(tmpScroll, sz_content - sz_view);

    ScrollBoxList->SetScrollOffset(tmpScroll);
    ScrollUpdate(tmpScroll);
}

TWeakObjectPtr<UUserWidget> UReuseListSp::NewItem()
{
    if (ItemPool.IsValidIndex(0)) {
        TWeakObjectPtr<UUserWidget> widget = ItemPool[0];
        ItemPool.RemoveAt(0);
        if (widget.IsValid()) {
            widget->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
        }
        return widget;
    }
    else {
        TWeakObjectPtr<UUserWidget> widget = CreateWidget<UUserWidget>(GetWorld(), ItemClass);
        if (!widget.IsValid()) {
            return nullptr;
        }
        CanvasPanelList->AddChild(widget.Get());
        widget->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
        OnCreateItem.Broadcast(widget.Get());
        return widget;
    }
}

void UReuseListSp::ReleaseItem(TWeakObjectPtr<UUserWidget> __Item)
{
    __Item->SetVisibility(ESlateVisibility::Collapsed);
    ItemPool.AddUnique(__Item);
}

void UReuseListSp::ReleaseAllItem()
{
    if (CanvasPanelList.IsValid()) {
        for (int32 i = 0; i < CanvasPanelList->GetChildrenCount(); i++) {
            UUserWidget* uw = Cast<UUserWidget>(CanvasPanelList->GetChildAt(i));
            if (uw) {
                ReleaseItem(uw);
            }
        }
    }
    ItemMap.Empty();
}

void UReuseListSp::ClearCache()
{
    ItemPool.Empty();
    CanvasPanelList->ClearChildren();
}

bool UReuseListSp::IsVertical() const
{
    return Style == EReuseListSpStyle::Vertical;
}

bool UReuseListSp::IsInvalidParam() const
{
    if (ItemClass == nullptr)
        return true;
    return ItemSize <= 0;
}

void UReuseListSp::Reset(TSubclassOf<UUserWidget> __ItemClass, EReuseListSpStyle __Style, int32 __ItemSize, int32 __ItemPadding)
{
    Clear();
    ClearCache();
    ItemClass = __ItemClass;
    Style = __Style;
    ItemSize = __ItemSize;
    ItemPadding = __ItemPadding;
}

#if WITH_EDITOR
void UReuseListSp::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
    Super::PostEditChangeProperty(PropertyChangedEvent);
    SyncProp();
    UWorld* wld = GetWorld();
    if (wld && !wld->IsGameWorld()) {
        static const FName TmpName = TEXT("ItemClass");
        if (PropertyChangedEvent.GetPropertyName().IsEqual(TmpName)) {
            ClearCache();
        }
        Reload(PreviewCount);
    }
}
#endif

void UReuseListSp::OnWidgetRebuilt()
{
    Super::OnWidgetRebuilt();
    InitWidgetPtr();
    SyncProp();
#if WITH_EDITOR
    UWorld* wld = GetWorld();
    if (wld && !wld->IsGameWorld()) {
        TWeakObjectPtr<UReuseListSp> self = this;
        wld->GetTimerManager().SetTimerForNextTick(FTimerDelegate::CreateLambda([=]() {
            if (self.IsValid()) {
                self->Reload(PreviewCount);
            }
        }));
    }
#endif
}

void UReuseListSp::SyncProp()
{
    if (ScrollBoxList.IsValid()) {
        ScrollBoxList->SetScrollBarVisibility(ScrollBarVisibility);
        ScrollBoxList->SetScrollbarThickness(ScrollBarThickness);
        ScrollBoxList->WidgetBarStyle = ScrollBarStyle;
        ScrollBoxList->WidgetStyle = ScrollBoxStyle;
    }
}

void UReuseListSp::AdjustItem()
{
    if (!AutoAdjustItemSize)
        return;
    //bool print_log = false;
    for (TMap<int32, TWeakObjectPtr<UUserWidget> >::TIterator iter = ItemMap.CreateIterator(); iter; ++iter) {
        TWeakObjectPtr<UUserWidget> wdg = iter->Value;
        if (wdg.IsValid()) {
            int32 idx = iter->Key;
            FVector2D sz = wdg->GetDesiredSize();
            int32 size = GetItemSize(idx);
            int32 size_now = (IsVertical() ? sz.Y : sz.X);
            if (size_now != size) {
                AddSpecialSize(idx, size_now);
                //print_log = true;
            }
        }
    }
//     if (print_log) {
//         for (TMap<int32, TWeakObjectPtr<UUserWidget> >::TIterator iter = ItemMap.CreateIterator(); iter; ++iter) {
//             FVector2D sz = iter->Value->GetDesiredSize();
//             UE_LOG(LogUReuseListSp, Log, TEXT("AdjustItem idx=%d x=%f y=%f"), iter->Key, sz.X, sz.Y);
//         }
//     }
}

void UReuseListSp::AdjustItemWidgetSize()
{
    float AlignSpace = GetAlignSpace();
    for (TMap<int32, TWeakObjectPtr<UUserWidget> >::TIterator iter = ItemMap.CreateIterator(); iter; ++iter) {
        int32 idx = iter->Key;
        TWeakObjectPtr<UUserWidget> wdg = iter->Value;
        if (wdg.IsValid()) {
            UCanvasPanelSlot* cps = Cast<UCanvasPanelSlot>(wdg->Slot);
            if (cps) {
                int32 size = GetItemSize(idx);
                int32 offset = (ArrOffset.IsValidIndex(idx) ? ArrOffset[idx] : 0);
                if (IsVertical())
                    cps->SetOffsets(FMargin(0, offset + AlignSpace, ViewSize.X, size));
                else
                    cps->SetOffsets(FMargin(offset + AlignSpace, 0, size, ViewSize.Y));
            }
        }
    }
}

void UReuseListSp::AdjustScrollOffset()
{
    float TmpMaxOffset = UKismetMathLibrary::FMax(MaxPos - (IsVertical() ? ViewSize.Y : ViewSize.X), 0.f);
    if (TmpMaxOffset <= 0.f) {
        ScrollBoxList->ScrollToStart();
    }
    else {
        float cur = ScrollBoxList->GetScrollOffset();
        if (cur <= 0.f) {
            ScrollBoxList->ScrollToStart();
        }
        if (cur >= TmpMaxOffset) {
            ScrollBoxList->ScrollToEnd();
        }
    }
}