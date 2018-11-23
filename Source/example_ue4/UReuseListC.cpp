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
    , ItemCacheNum(2)
    , PaddingX(0)
    , PaddingY(0)
    , ItemCount(0)
    , ItemHeight(100)
    , MaxPos(0)
    , Offset(0)
    , OffsetEnd(0)
    , Style(EReuseListStyle::Vertical)
    , ItemWidth(100)
    , BIdx(0)
    , EIdx(0)
    , ColNum(0)
    , RowNum(0)
    , CurLine(-999)
    , JumpIdx(0)
    , JumpStyle(EReuseListJumpStyle::Middle)
    , NeedJump(false)
    , PreviewCount(5)
    , ScrollBarVisibility(ESlateVisibility::Collapsed)
{
    ScrollBoxStyle.LeftShadowBrush = FSlateNoResource();
    ScrollBoxStyle.TopShadowBrush = FSlateNoResource();
    ScrollBoxStyle.RightShadowBrush = FSlateNoResource();
    ScrollBoxStyle.BottomShadowBrush = FSlateNoResource();
    tmhOnPreviewTick.Invalidate();
}

bool UReuseListC::Initialize()
{
    if (!Super::Initialize())
        return false;
    InitWidgetPtr();
    return true;
}

void UReuseListC::ReleaseSlateResources(bool bReleaseChildren)
{
    Super::ReleaseSlateResources(bReleaseChildren);
    auto wld = GetWorld();
    if (wld && !wld->IsGameWorld()) {
        if (tmhOnPreviewTick.IsValid()) {
            GetWorld()->GetTimerManager().ClearTimer(tmhOnPreviewTick);
            tmhOnPreviewTick.Invalidate();
        }
    }
}

void UReuseListC::SynchronizeProperties()
{
    Super::SynchronizeProperties();

    ScrollBoxList->SetScrollBarVisibility(ScrollBarVisibility);
    ScrollBoxList->SetScrollbarThickness(ScrollBarThickness);
    ScrollBoxList->WidgetBarStyle = ScrollBarStyle;
    ScrollBoxList->WidgetStyle = ScrollBoxStyle;

    auto wld = GetWorld();
    if (wld && !wld->IsGameWorld()) {
        bool bClearCache = false;
        if (CanvasPanelList->GetChildrenCount() == 0) {
            if (ItemPool.IsValidIndex(0)) {
                bClearCache = ItemPool[0]->GetClass() != ItemClass;
            }
        }
        else {
            bClearCache = CanvasPanelList->GetChildAt(0)->GetClass() != ItemClass;
        }
        if (bClearCache) {
            ClearCache();
        }
        Reload(PreviewCount);

        if (!tmhOnPreviewTick.IsValid()) {
            GetWorld()->GetTimerManager().SetTimer(tmhOnPreviewTick, this, &UReuseListC::OnPreviewTick, 0.5f, true);
        }

    }
}

void UReuseListC::NativeConstruct()
{
    Super::NativeConstruct();
    InitWidgetPtr();
}

void UReuseListC::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
    Super::NativeTick(MyGeometry, InDeltaTime);
    if (IsInvalidParam())
        return;
    if (!ViewSize.Equals(GetCachedGeometry().GetLocalSize(), 0.0001f))
        DoReload();
    Update();
    DoJump();
}

void UReuseListC::Reload(int32 __ItemCount)
{
    ItemCount = __ItemCount;
    ScrollBoxList->SetOrientation(IsVertical() ? Orient_Vertical : Orient_Horizontal);
    ScrollBoxList->SetScrollBarVisibility(ScrollBarVisibility);
    DoReload();
}

void UReuseListC::Refresh()
{
    for (TMap<int32, UUserWidget*>::TConstIterator iter(ItemMap); iter; ++iter) {
        RefreshOne(iter->Key);
    }
}

void UReuseListC::RefreshOne(int32 __Idx)
{
    auto v = ItemMap.Find(__Idx);
    if (v) {
        OnUpdateItem.Broadcast(*v, __Idx);
    }
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
    ensure(ScrollBoxList);

    CanvasPanelBg = Cast<UCanvasPanel>(GetWidgetFromName(FName(TEXT("CanvasPanelBg"))));
    ensure(CanvasPanelBg);

    SizeBoxBg = Cast<USizeBox>(GetWidgetFromName(FName(TEXT("SizeBoxBg"))));
    ensure(SizeBoxBg);

    CanvasPanelList = Cast<UCanvasPanel>(GetWidgetFromName(FName(TEXT("CanvasPanelList"))));
    ensure(CanvasPanelList);
}

void UReuseListC::ScrollUpdate(float __Offset)
{
    int32 ItemWidthAndPad = ItemWidth + PaddingX;
    int32 ItemHeightAndPad = ItemHeight + PaddingY;
    int32 Offset = __Offset;
    Offset = UKismetMathLibrary::Max(Offset, 0);
    Offset = UKismetMathLibrary::Min(Offset, MaxPos);
    if (Style == EReuseListStyle::Vertical) {
        OffsetEnd = UKismetMathLibrary::Min((Offset + (int32)ViewSize.Y), MaxPos);
        BIdx = UKismetMathLibrary::Max(Offset / ItemHeightAndPad - ItemCacheNum, 0);
        EIdx = UKismetMathLibrary::Min(OffsetEnd / ItemHeightAndPad + ItemCacheNum, ItemCount-1);
        RemoveNotUsed();
        for (int32 i = BIdx; i <= EIdx; i++) {
            if (!ItemMap.Contains(i)) {
                auto w = NewItem();
                if (w) {
                    auto cps = Cast<UCanvasPanelSlot>(w->Slot);
                    cps->SetAnchors(FAnchors(0, 0, 0, 0));
                    cps->SetOffsets(FMargin(0, i * ItemHeightAndPad, ViewSize.X, ItemHeight));
                    ItemMap.Add(i, w);
                    OnUpdateItem.Broadcast(w, i);
                }
            }
        }
    }
    else if (Style == EReuseListStyle::Horizontal) {
        OffsetEnd = UKismetMathLibrary::Min((Offset + (int32)ViewSize.X), MaxPos);
        BIdx = UKismetMathLibrary::Max(Offset / ItemWidthAndPad - ItemCacheNum, 0);
        EIdx = UKismetMathLibrary::Min(OffsetEnd / ItemWidthAndPad + ItemCacheNum, ItemCount-1);
        RemoveNotUsed();
        for (int32 i = BIdx; i <= EIdx; i++) {
            if (!ItemMap.Contains(i)) {
                auto w = NewItem();
                if (w) {
                    auto cps = Cast<UCanvasPanelSlot>(w->Slot);
                    cps->SetAnchors(FAnchors(0, 0, 0, 0));
                    if (ItemHeight <= 0) {
                        cps->SetOffsets(FMargin(i * ItemWidthAndPad, 0, ItemWidth, ViewSize.Y));
                    }
                    else {
                        cps->SetOffsets(FMargin(i * ItemWidthAndPad, (ViewSize.Y - ItemHeight) / 2.f, ItemWidth, ItemHeight));
                    }
                    ItemMap.Add(i, w);
                    OnUpdateItem.Broadcast(w, i);
                }
            }
        }
    }
    else if (Style == EReuseListStyle::VerticalGrid) {
        OffsetEnd = UKismetMathLibrary::Min((Offset + (int32)ViewSize.Y), MaxPos);
        BIdx = UKismetMathLibrary::Max( ( Offset / ItemHeightAndPad ) * ColNum, 0);
        int32 tmp = UKismetMathLibrary::FCeil((float)OffsetEnd / ItemHeightAndPad) + 1;
        EIdx = UKismetMathLibrary::Min(tmp * ColNum - 1, ItemCount-1);
        RemoveNotUsed();
        for (int32 i = BIdx; i <= EIdx; i++) {
            if (!ItemMap.Contains(i)) {
                auto w = NewItem();
                if (w) {
                    auto cps = Cast<UCanvasPanelSlot>(w->Slot);
                    cps->SetAnchors(FAnchors(0, 0, 0, 0));
                    cps->SetOffsets(FMargin((i%ColNum)*ItemWidthAndPad, (i / ColNum)*ItemHeightAndPad, ItemWidth, ItemHeight));
                    ItemMap.Add(i, w);
                    OnUpdateItem.Broadcast(w, i);
                }
            }
        }
    }
    OnScrollItem.Broadcast(BIdx, EIdx);
}

void UReuseListC::UpdateContentSize(UWidget* widget)
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

void UReuseListC::RemoveNotUsed()
{
    for (TMap<int32, UUserWidget*>::TIterator iter = ItemMap.CreateIterator(); iter; ++iter) {
        if (!UKismetMathLibrary::InRange_IntInt(iter->Key, BIdx, EIdx)) {
            ReleaseItem(iter->Value);
            iter.RemoveCurrent();
        }
    }
}

void UReuseListC::OnPreviewTick()
{
    Reload(PreviewCount);
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
    }
    UpdateContentSize(SizeBoxBg);
    UpdateContentSize(CanvasPanelList);
    for (int32 i = 0; i < CanvasPanelList->GetChildrenCount(); i++) {
        auto uw = Cast<UUserWidget>(CanvasPanelList->GetChildAt(i));
        ReleaseItem(uw);
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

UUserWidget* UReuseListC::NewItem()
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

void UReuseListC::ReleaseItem(UUserWidget* __Item)
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

bool UReuseListC::ChangeItemClass(const FString& StrItemClass)
{
    UWidgetBlueprintGeneratedClass* _WidgetClass = ::LoadObject<UWidgetBlueprintGeneratedClass>(nullptr, *StrItemClass);
    if (_WidgetClass == nullptr)
        return false;
    Clear();
    ClearCache();
    ItemClass = _WidgetClass;
    return true;
}