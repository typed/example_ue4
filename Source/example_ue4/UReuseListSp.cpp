// Fill out your copyright notice in the Description page of Project Settings.

#include "UReuseListSp.h"
#include "Runtime/UMG/Public/Components/SizeBox.h"
#include "Runtime/UMG/Public/Components/CanvasPanel.h"
#include "Runtime/UMG/Public/Components/CanvasPanelSlot.h"
#include "Runtime/UMG/Public/Components/ScrollBox.h"
#include "Runtime/UMG/Public/Blueprint/WidgetBlueprintGeneratedClass.h"
#include "Runtime/Engine/Classes/Kismet/KismetMathLibrary.h"

DEFINE_LOG_CATEGORY(LogUReuseListSp);

static const int32 ItemCacheNum = 2;

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
    , CurLine(-999)
    , JumpIdx(0)
    , JumpStyle(EReuseListSpJumpStyle::Middle)
    , NeedJump(false)
    , NeedFillArrOffset(true)
    , PreviewCount(5)
    , ScrollBarVisibility(ESlateVisibility::Collapsed)
    , NotFullAlignStyle(EReuseListSpNotFullAlignStyle::Start)
    , NotFullScrollBoxHitTestInvisible(false)
    , AlignSpace(0.f)
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
    const FVector2D& lzSz = GetCachedGeometry().GetLocalSize();
    if (!ViewSize.Equals(lzSz, 0.0001f))
        DoReload();
    Update();
    DoJump();
}

void UReuseListSp::Reload(int32 __ItemCount)
{
    ItemCount = __ItemCount;
    ScrollBoxList->SetOrientation(IsVertical() ? Orient_Vertical : Orient_Horizontal);
    ScrollBoxList->SetScrollBarVisibility(ScrollBarVisibility);
    const FVector2D& lzSz = GetCachedGeometry().GetLocalSize();
    if (lzSz.Equals(FVector2D::ZeroVector, 0.0001f)) {
        ViewSize = lzSz;
        return;
    }
    DoReload();
}

void UReuseListSp::Refresh()
{
    for (TMap<int32, UUserWidget*>::TConstIterator iter(ItemMap); iter; ++iter) {
        RefreshOne(iter->Key);
    }
}

void UReuseListSp::RefreshOne(int32 __Idx)
{
    auto v = ItemMap.Find(__Idx);
    if (v) {
        OnUpdateItem.Broadcast(*v, __Idx);
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
    ensure(ScrollBoxList);

    CanvasPanelBg = Cast<UCanvasPanel>(GetWidgetFromName(FName(TEXT("CanvasPanelBg"))));
    ensure(CanvasPanelBg);

    SizeBoxBg = Cast<USizeBox>(GetWidgetFromName(FName(TEXT("SizeBoxBg"))));
    ensure(SizeBoxBg);

    CanvasPanelList = Cast<UCanvasPanel>(GetWidgetFromName(FName(TEXT("CanvasPanelList"))));
    ensure(CanvasPanelList);
}

void UReuseListSp::ComputeAlignSpace()
{
    AlignSpace = 0.f;
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
}

void UReuseListSp::AddSpecialSize(int32 __Idx, int32 __Size)
{
    SpecialSizeMap.Add(__Idx, __Size);
    NeedFillArrOffset = true;
}

void UReuseListSp::FillArrOffset()
{
    if (!NeedFillArrOffset)
        return;
    NeedFillArrOffset = false;
    ArrOffset.Empty();
    int32 tmpOffset = 0;
    for (int32 i = 0; i < ItemCount; i++) {
        ArrOffset.Add(tmpOffset);
        tmpOffset += (GetItemSize(i) + ItemPadding);
    }
    MaxPos = tmpOffset;
    MaxPos -= ItemPadding;
}

int32 UReuseListSp::GetItemSize(int32 idx)
{
    const int32 *pValue = SpecialSizeMap.Find(idx);
    if (pValue)
        return *pValue;
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
    
    BIdx = Algo::LowerBound(ArrOffset, Offset);
    BIdx = UKismetMathLibrary::Max(BIdx - ItemCacheNum, 0);
    EIdx = Algo::LowerBound(ArrOffset, OffsetEnd);
    EIdx = UKismetMathLibrary::Min(EIdx + ItemCacheNum, ItemCount - 1);

    RemoveNotUsed(BIdx, EIdx);
    for (int32 i = BIdx; i <= EIdx; i++) {
        if (!ItemMap.Contains(i)) {
            auto w = NewItem();
            if (w) {
                int32 sz_item = GetItemSize(i);
                auto cps = Cast<UCanvasPanelSlot>(w->Slot);
                cps->SetAnchors(FAnchors(0, 0, 0, 0));
                if (IsVertical())
                    cps->SetOffsets(FMargin(0, ArrOffset[i] + AlignSpace, ViewSize.X, sz_item));
                else
                    cps->SetOffsets(FMargin(ArrOffset[i] + AlignSpace, 0, sz_item, ViewSize.Y));
                ItemMap.Add(i, w);
                OnUpdateItem.Broadcast(w, i);
            }
        }
    }
    OnScrollItem.Broadcast(BIdx, EIdx);
}

void UReuseListSp::UpdateContentSize(UWidget* widget)
{
    auto cps = Cast<UCanvasPanelSlot>(widget->Slot);
    if (IsVertical()) {
        cps->SetAnchors(FAnchors(0, 0, 1, 0));
        cps->SetOffsets(FMargin(0, 0, 0, ContentSize.Y));
    }
    else {
        cps->SetAnchors(FAnchors(0, 0, 0, 1));
        cps->SetOffsets(FMargin(0, 0, ContentSize.X, 0));
    }
}

void UReuseListSp::RemoveNotUsed(int32 BIdx, int32 EIdx)
{
    for (TMap<int32, UUserWidget*>::TIterator iter = ItemMap.CreateIterator(); iter; ++iter) {
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
    FillArrOffset();
    ViewSize = GetCachedGeometry().GetLocalSize();
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
    ComputeAlignSpace();
    ComputeScrollBoxHitTest();
    for (int32 i = 0; i < CanvasPanelList->GetChildrenCount(); i++) {
        auto uw = Cast<UUserWidget>(CanvasPanelList->GetChildAt(i));
        if (uw) {
            ReleaseItem(uw);
        }
    }
    ItemMap.Empty();
    float TmpMaxOffset = 0.f;
    TmpMaxOffset = UKismetMathLibrary::FMax(MaxPos - (IsVertical() ? ViewSize.Y : ViewSize.X), 0.f);
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
    ScrollUpdate(ScrollBoxList->GetScrollOffset());
}

void UReuseListSp::DoJump()
{
    if (!NeedJump)
        return;
    NeedJump = false;

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

UUserWidget* UReuseListSp::NewItem()
{
    if (ItemPool.IsValidIndex(0)) {
        auto tmp = ItemPool[0];
        ItemPool.RemoveAt(0);
        tmp->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
        return tmp;
    }
    else {
        UUserWidget* widget = CreateWidget<UUserWidget>(GetWorld(), ItemClass);
        if (widget == nullptr) {
            return nullptr;
        }
        CanvasPanelList->AddChild(widget);
        widget->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
        OnCreateItem.Broadcast(widget);
        return widget;
    }
}

void UReuseListSp::ReleaseItem(UUserWidget* __Item)
{
    __Item->SetVisibility(ESlateVisibility::Collapsed);
    ItemPool.AddUnique(__Item);
}

void UReuseListSp::Update()
{
    int32 tmpLine = 0;
    float tmpOffset = ScrollBoxList->GetScrollOffset();
    tmpLine = (int32)tmpOffset / (ItemSize + ItemPadding);
    if (tmpLine != CurLine) {
        CurLine = tmpLine;
        ScrollUpdate(tmpOffset);
    }
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

void UReuseListSp::Reset(UClass* __ItemClass, EReuseListSpStyle __Style, int32 __ItemSize, int32 __ItemPadding)
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
    auto wld = GetWorld();
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
    auto wld = GetWorld();
    if (wld && !wld->IsGameWorld()) {
        GetWorld()->GetTimerManager().SetTimerForNextTick(FTimerDelegate::CreateLambda([this]() {
            if (ScrollBoxList->IsValidLowLevelFast() &&
                CanvasPanelBg->IsValidLowLevelFast() &&
                SizeBoxBg->IsValidLowLevelFast() &&
                CanvasPanelList->IsValidLowLevelFast())
            {
                Reload(PreviewCount);
            }
        }));
    }
}

void UReuseListSp::SyncProp()
{
    if (ScrollBoxList && ScrollBoxList->IsValidLowLevelFast()) {
        ScrollBoxList->SetScrollBarVisibility(ScrollBarVisibility);
        ScrollBoxList->SetScrollbarThickness(ScrollBarThickness);
        ScrollBoxList->WidgetBarStyle = ScrollBarStyle;
        ScrollBoxList->WidgetStyle = ScrollBoxStyle;
    }
}