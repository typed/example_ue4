// Fill out your copyright notice in the Description page of Project Settings.

#include "UReuseListC.h"
#include "Runtime/UMG/Public/Components/SizeBox.h"
#include "Runtime/UMG/Public/Components/CanvasPanel.h"
#include "Runtime/UMG/Public/Components/CanvasPanelSlot.h"
#include "Runtime/UMG/Public/Components/ScrollBox.h"
#include "Runtime/UMG/Public/Blueprint/WidgetBlueprintGeneratedClass.h"
#include "Runtime/Engine/Classes/Kismet/KismetMathLibrary.h"

DEFINE_LOG_CATEGORY(LogUReuseListC);

UReuseListC::UReuseListC(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer)
    , ScrollBoxList(nullptr)
    , CanvasPanelBg(nullptr)
    , SizeBoxBg(nullptr)
    , CanvasPanelList(nullptr)
    , ViewSize(FVector2D::ZeroVector)
    , ContentSize(FVector2D::ZeroVector)
    , ItemClass(nullptr)
    , PaddingX(0)
    , PaddingY(0)
    , ItemCount(0)
    , ItemHeight(100)
    , MaxPos(0)
    , Style(EReuseListStyle::Vertical)
    , ItemCacheNum(2)
    , ItemWidth(100)
    , ColNum(0)
    , RowNum(0)
    , CurLine(-999)
    , JumpIdx(0)
    , JumpStyle(EReuseListJumpStyle::Middle)
    , NeedJump(false)
    , PreviewCount(5)
    , ScrollBarVisibility(ESlateVisibility::Collapsed)
    , NotFullAlignStyle(EReuseListNotFullAlignStyle::Start)
    , NotFullScrollBoxHitTestInvisible(false)
    , AlignSpace(0.f)
    , DelayUpdateNum(0)
    , DelayUpdateNumReal(0)
    , LastOffset(0)
{
    ScrollBoxStyle.LeftShadowBrush = FSlateNoResource();
    ScrollBoxStyle.TopShadowBrush = FSlateNoResource();
    ScrollBoxStyle.RightShadowBrush = FSlateNoResource();
    ScrollBoxStyle.BottomShadowBrush = FSlateNoResource();
}

bool UReuseListC::Initialize()
{
    if (!Super::Initialize())
        return false;
    InitWidgetPtr();
    SyncProp();
    return true;
}

void UReuseListC::NativeConstruct()
{
    Super::NativeConstruct();
    InitWidgetPtr();
    SyncProp();
}

void UReuseListC::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
    Super::NativeTick(MyGeometry, InDeltaTime);
    if (IsInvalidParam())
        return;
    const FVector2D& lzSz = GetCachedGeometry().GetLocalSize();
    if (!ViewSize.Equals(lzSz, 0.0001f))
        DoReload();
    DoUpdateItem();
    Update();
    DoJump();
}

void UReuseListC::Reload(int32 __ItemCount)
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

void UReuseListC::Refresh()
{
    if (DelayUpdateNumReal <= 0) {
        for (TMap<int32, TWeakObjectPtr<UUserWidget> >::TConstIterator iter(ItemMap); iter; ++iter) {
            OnUpdateItem.Broadcast(iter->Value.Get(), iter->Key);
        }
    }
    else {
        for (TMap<int32, TWeakObjectPtr<UUserWidget> >::TConstIterator iter(ItemMap); iter; ++iter) {
            ReleaseItem(iter->Value);
            SendDoUpdateItem(iter->Key);
        }
        ItemMap.Empty();
    }
}

void UReuseListC::RefreshOne(int32 __Idx)
{
    auto v = ItemMap.Find(__Idx);
    if (v) {
        ReleaseItem(*v);
        ItemMap.Remove(__Idx);
        SendDoUpdateItem(__Idx);
    }
}

void UReuseListC::ScrollToStart()
{
    ScrollBoxList->ScrollToStart();
}

void UReuseListC::ScrollToEnd()
{
    ScrollBoxList->ScrollToEnd();
}

void UReuseListC::SetScrollOffset(float NewScrollOffset)
{
    ScrollBoxList->SetScrollOffset(NewScrollOffset);
}

float UReuseListC::GetScrollOffset() const
{
    return ScrollBoxList->GetScrollOffset();
}

const FVector2D& UReuseListC::GetViewSize() const
{
    return ViewSize;
}

const FVector2D& UReuseListC::GetContentSize() const
{
    return ContentSize;
}

void UReuseListC::JumpByIdxStyle(int32 __Idx, EReuseListJumpStyle __Style)
{
    JumpIdx = __Idx;
    JumpStyle = __Style;
    NeedJump = true;
}

void UReuseListC::Clear()
{
    Reload(0);
}

void UReuseListC::InitWidgetPtr()
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

void UReuseListC::ComputeAlignSpace()
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
        case EReuseListNotFullAlignStyle::Start:
            AlignSpace = 0.f;
            break;
        case EReuseListNotFullAlignStyle::Middle:
            AlignSpace = (view - content) / 2.f;
            break;
        case EReuseListNotFullAlignStyle::End:
            AlignSpace = view - content;
            break;
        }
    }
}

void UReuseListC::ComputeScrollBoxHitTest()
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

void UReuseListC::ScrollUpdate(float __Offset)
{
    int32 ItemWidthAndPad = ItemWidth + PaddingX;
    int32 ItemHeightAndPad = ItemHeight + PaddingY;
    int32 Offset = __Offset;
    int32 OffsetEnd = 0;
    int32 BIdx = 0;
    int32 EIdx = 0;
    Offset = UKismetMathLibrary::Max(Offset, 0);
    Offset = UKismetMathLibrary::Min(Offset, MaxPos);
    if (Style == EReuseListStyle::Vertical) {
        OffsetEnd = UKismetMathLibrary::Min((Offset + (int32)ViewSize.Y), MaxPos);
        BIdx = UKismetMathLibrary::Max(Offset / ItemHeightAndPad - ItemCacheNum, 0);
        EIdx = UKismetMathLibrary::Min(OffsetEnd / ItemHeightAndPad + ItemCacheNum, ItemCount-1);
    }
    else if (Style == EReuseListStyle::Horizontal) {
        OffsetEnd = UKismetMathLibrary::Min((Offset + (int32)ViewSize.X), MaxPos);
        BIdx = UKismetMathLibrary::Max(Offset / ItemWidthAndPad - ItemCacheNum, 0);
        EIdx = UKismetMathLibrary::Min(OffsetEnd / ItemWidthAndPad + ItemCacheNum, ItemCount-1);
    }
    else if (Style == EReuseListStyle::VerticalGrid) {
        OffsetEnd = UKismetMathLibrary::Min((Offset + (int32)ViewSize.Y), MaxPos);
        BIdx = UKismetMathLibrary::Max( (Offset / ItemHeightAndPad ) * ColNum, 0);
        int32 tmp = UKismetMathLibrary::FCeil((float)OffsetEnd / ItemHeightAndPad) + 1;
        EIdx = UKismetMathLibrary::Min(tmp * ColNum - 1, ItemCount - 1);
    }
    else if (Style == EReuseListStyle::HorizontalGrid) {
        OffsetEnd = UKismetMathLibrary::Min((Offset + (int32)ViewSize.X), MaxPos);
        BIdx = UKismetMathLibrary::Max((Offset / ItemWidthAndPad) * RowNum, 0);
        int32 tmp = UKismetMathLibrary::FCeil((float)OffsetEnd / ItemWidthAndPad) + 1;
        EIdx = UKismetMathLibrary::Min(tmp * RowNum - 1, ItemCount - 1);
    }
    RemoveNotUsed(BIdx, EIdx);
    if (LastOffset <= Offset) {
        for (int32 i = BIdx; i <= EIdx; ++i) {
            if (!ItemMap.Contains(i)) {
                SendDoUpdateItem(i);
            }
        }
    }
    else {
        for (int32 i = EIdx; i >= BIdx; --i) {
            if (!ItemMap.Contains(i)) {
                SendDoUpdateItem(i);
            }
        }
    }
    OnScrollItem.Broadcast(BIdx, EIdx);
    LastOffset = Offset;
}

void UReuseListC::UpdateContentSize(TWeakObjectPtr<UWidget> widget)
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

void UReuseListC::RemoveNotUsed(int32 BIdx, int32 EIdx)
{
    for (TMap<int32, TWeakObjectPtr<UUserWidget> >::TIterator iter = ItemMap.CreateIterator(); iter; ++iter) {
        if (!UKismetMathLibrary::InRange_IntInt(iter->Key, BIdx, EIdx)) {
            ReleaseItem(iter->Value.Get());
            iter.RemoveCurrent();
        }
    }
    for (int32 i = QueueDoUpdateItem.Num() - 1; i >= 0; --i) {
        int32 idx = QueueDoUpdateItem[i];
        if (!UKismetMathLibrary::InRange_IntInt(idx, BIdx, EIdx)) {
            QueueDoUpdateItem.RemoveAt(i);
        }
    }
}

void UReuseListC::DoReload()
{
    if (IsInvalidParam())
        return;
    ViewSize = GetCachedGeometry().GetLocalSize();
    switch (Style)
    {
    case EReuseListStyle::Vertical:
        MaxPos = (ItemHeight + PaddingY) * ItemCount - PaddingY;
        ContentSize.X = ViewSize.X;
        ContentSize.Y = MaxPos;
        break;
    case EReuseListStyle::Horizontal:
        MaxPos = (ItemWidth + PaddingX) * ItemCount - PaddingX;
        ContentSize.X = MaxPos;
        ContentSize.Y = ViewSize.Y;
        break;
    case EReuseListStyle::VerticalGrid:
        ColNum = UKismetMathLibrary::Max((PaddingX + (int32)ViewSize.X) / (ItemWidth + PaddingX), 1);
        RowNum = UKismetMathLibrary::FCeil((float)ItemCount / ColNum);
        MaxPos = (ItemHeight + PaddingY) * RowNum - PaddingY;
        ContentSize.X = ViewSize.X;
        ContentSize.Y = MaxPos;
        break;
    case EReuseListStyle::HorizontalGrid:
        RowNum = UKismetMathLibrary::Max((PaddingY + (int32)ViewSize.Y) / (ItemHeight + PaddingY), 1);
        ColNum = UKismetMathLibrary::FCeil((float)ItemCount / RowNum);
        MaxPos = (ItemWidth + PaddingX) * ColNum - PaddingX;
        ContentSize.X = MaxPos;
        ContentSize.Y = ViewSize.Y;
        break;
    }
    UpdateContentSize(SizeBoxBg.Get());
    UpdateContentSize(CanvasPanelList.Get());
    ComputeAlignSpace();
    ComputeScrollBoxHitTest();
    for (int32 i = 0; i < CanvasPanelList->GetChildrenCount(); i++) {
        auto uw = Cast<UUserWidget>(CanvasPanelList->GetChildAt(i));
        if (uw) {
            ReleaseItem(uw);
        }
    }
    ItemMap.Empty();
    QueueDoUpdateItem.Empty();
    float TmpMaxOffset = 0.f;
    ScrollBoxList->GetScrollOffset();
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

void UReuseListC::DoJump()
{
    if (!NeedJump)
        return;
    NeedJump = false;
    float tmpScroll = 0;
    int32 ItemWidthAndPad = ItemWidth + PaddingX;
    int32 ItemHeightAndPad = ItemHeight + PaddingY;
    int32 tmpItemOffset = 0;

    if (IsVertical()) {
        if (JumpStyle == EReuseListJumpStyle::Begin) {
            tmpItemOffset = 0;
        }
        else if (JumpStyle == EReuseListJumpStyle::End) {
            tmpItemOffset = (int32)ViewSize.Y - ItemHeightAndPad;
        }
        else {
            tmpItemOffset = ((int32)ViewSize.Y - ItemHeightAndPad)/2;
        }
    }
    else {
        if (JumpStyle == EReuseListJumpStyle::Begin) {
            tmpItemOffset = 0;
        }
        else if (JumpStyle == EReuseListJumpStyle::End) {
            tmpItemOffset = (int32)ViewSize.X - ItemWidthAndPad;
        }
        else {
            tmpItemOffset = ((int32)ViewSize.X - ItemWidthAndPad) / 2;
        }
    }

    if (Style == EReuseListStyle::Vertical) {
        if (ContentSize.Y < ViewSize.Y) {
            return;
        }
        if ((int32)ViewSize.Y == 0 && JumpIdx == 0) {
            tmpScroll = 0.f;
        }
        else {
            tmpScroll = ItemHeightAndPad * JumpIdx - tmpItemOffset;
        }
    }
    else if (Style == EReuseListStyle::Horizontal) {
        if (ContentSize.X < ViewSize.X) {
            return;
        }
        tmpScroll = ItemWidthAndPad * JumpIdx - tmpItemOffset;
    }
    else if (Style == EReuseListStyle::VerticalGrid) {
        if (ContentSize.Y < ViewSize.Y) {
            return;
        }
        tmpScroll = ItemHeightAndPad * (JumpIdx / ColNum) - tmpItemOffset;
    }
    else if (Style == EReuseListStyle::HorizontalGrid) {
        if (ContentSize.X < ViewSize.X) {
            return;
        }
        tmpScroll = ItemWidthAndPad * (JumpIdx / RowNum) - tmpItemOffset;
    }

    tmpScroll = UKismetMathLibrary::FMax(tmpScroll, 0);
    if (IsVertical()) {
        tmpScroll = UKismetMathLibrary::FMin(tmpScroll, ContentSize.Y - ViewSize.Y);
    }
    else {
        tmpScroll = UKismetMathLibrary::FMin(tmpScroll, ContentSize.X - ViewSize.X);
    }
    ScrollBoxList->SetScrollOffset(tmpScroll);
    ScrollUpdate(tmpScroll);
}

TWeakObjectPtr<UUserWidget> UReuseListC::NewItem()
{
    if (ItemPool.IsValidIndex(0)) {
        auto tmp = ItemPool[0];
        ItemPool.RemoveAt(0);
        tmp->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
        return tmp;
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

void UReuseListC::ReleaseItem(TWeakObjectPtr<UUserWidget> __Item)
{
    __Item->SetVisibility(ESlateVisibility::Collapsed);
    ItemPool.AddUnique(__Item);
}

void UReuseListC::Update()
{
    int32 tmpLine = 0;
    float tmpOffset = ScrollBoxList->GetScrollOffset();
    if (IsVertical()) {
        tmpLine = (int32)tmpOffset / (ItemHeight + PaddingY);
    }
    else {
        tmpLine = (int32)tmpOffset / (ItemWidth + PaddingX);
    }
    if (tmpLine != CurLine) {
        CurLine = tmpLine;
        ScrollUpdate(tmpOffset);
    }
}

void UReuseListC::ClearCache()
{
    ItemPool.Empty();
    CanvasPanelList->ClearChildren();
}

bool UReuseListC::IsVertical() const
{
    return Style == EReuseListStyle::Vertical || Style == EReuseListStyle::VerticalGrid;
}

bool UReuseListC::IsInvalidParam() const
{
    if (ItemClass == nullptr)
        return true;
    if (IsVertical())
        return ItemHeight <= 0;
    else
        return ItemWidth <= 0;
}

void UReuseListC::SendDoUpdateItem(int32 idx)
{
    QueueDoUpdateItem.Remove(idx);
    QueueDoUpdateItem.Add(idx);
    if (DelayUpdateNumReal <= 0) {
        DoUpdateItem();
    }
}

void UReuseListC::DoUpdateItem()
{
    if (QueueDoUpdateItem.Num() == 0) {
        return;
    }
    int32 ItemWidthAndPad = ItemWidth + PaddingX;
    int32 ItemHeightAndPad = ItemHeight + PaddingY;
    int32 tmp = DelayUpdateNumReal;
    FMargin mar;
    do {
        int32 idx = QueueDoUpdateItem[0];
        QueueDoUpdateItem.RemoveAt(0);
        --tmp;
        if (Style == EReuseListStyle::Vertical) {
            mar.Left = 0;
            mar.Top = idx * ItemHeightAndPad + AlignSpace;
            mar.Right = ViewSize.X;
            mar.Bottom = ItemHeight;
        }
        else if (Style == EReuseListStyle::Horizontal) {
            if (ItemHeight <= 0) {
                mar.Left = idx * ItemWidthAndPad + AlignSpace;
                mar.Top = 0;
                mar.Right = ItemWidth;
                mar.Bottom = ViewSize.Y;
            }
            else {
                mar.Left = idx * ItemWidthAndPad + AlignSpace;
                mar.Top = (ViewSize.Y - ItemHeight) / 2.f;
                mar.Right = ItemWidth;
                mar.Bottom = ItemHeight;
            }
        }
        else if (Style == EReuseListStyle::VerticalGrid) {
            mar.Left = (idx % ColNum) * ItemWidthAndPad;
            mar.Top = (idx / ColNum) * ItemHeightAndPad + AlignSpace;
            mar.Right = ItemWidth;
            mar.Bottom = ItemHeight;
        }
        else if (Style == EReuseListStyle::HorizontalGrid) {
            mar.Left = (idx / RowNum) * ItemWidthAndPad + AlignSpace;
            mar.Top = (idx % RowNum) * ItemHeightAndPad;
            mar.Right = ItemWidth;
            mar.Bottom = ItemHeight;
        }
        auto w = NewItem();
        if (w.IsValid()) {
            auto cps = Cast<UCanvasPanelSlot>(w->Slot);
            cps->SetAnchors(FAnchors(0, 0, 0, 0));
            cps->SetOffsets(mar);
            ItemMap.Add(idx, w);
            OnUpdateItem.Broadcast(w.Get(), idx);
        }
    } while (QueueDoUpdateItem.Num() > 0 && tmp > 0);
}

void UReuseListC::Reset(TSubclassOf<UUserWidget> __ItemClass, EReuseListStyle __Style, int32 __ItemWidth, int32 __ItemHeight, int32 __PaddingX, int32 __PaddingY)
{
    Clear();
    ClearCache();
    ItemClass = __ItemClass;
    Style = __Style;
    ItemWidth = __ItemWidth;
    ItemHeight = __ItemHeight;
    PaddingX = __PaddingX;
    PaddingY = __PaddingY;
}

#if WITH_EDITOR
void UReuseListC::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
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

void UReuseListC::OnWidgetRebuilt()
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

void UReuseListC::SyncProp()
{
    if (ScrollBoxList.IsValid()) {
        ScrollBoxList->SetScrollBarVisibility(ScrollBarVisibility);
        ScrollBoxList->SetScrollbarThickness(ScrollBarThickness);
        ScrollBoxList->WidgetBarStyle = ScrollBarStyle;
        ScrollBoxList->WidgetStyle = ScrollBoxStyle;
    }
    auto wld = GetWorld();
    if (wld && !wld->IsGameWorld())
        DelayUpdateNumReal = 0;
    else
        DelayUpdateNumReal = DelayUpdateNum;
}