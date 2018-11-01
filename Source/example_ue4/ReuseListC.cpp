// Fill out your copyright notice in the Description page of Project Settings.

#include "ReuseListC.h"
#include "Runtime/Engine/Classes/Kismet/KismetMathLibrary.h"

UReuseListC::UReuseListC(const FObjectInitializer& ObjectInitializer)
    :Super(ObjectInitializer)
{

    ScrollBoxList = nullptr;
    CanvasPanelBg = nullptr;
    SizeBoxBg = nullptr;
    CanvasPanelList = nullptr;

    ViewSize = FVector2D::ZeroVector;
    ContentSize = FVector2D::ZeroVector;
    ItemClass = nullptr;

    ItemCacheNum = 2;
    PaddingX = 0;
    PaddingY = 0;
    ItemCount = 0;
    ItemHeight = 100;
    MaxPos = 0;
    Offset = 0;
    OffsetEnd = 0;
    Style = 0;
    ItemWidth = 100;
    BIdx = 0;
    EIdx = 0;
    ColNum = 0;
    RowNum = 0;
    CurLine = -999;
    PaddingX = 0;
    PaddingY = 0;
    JumpIdx = 0;
    JumpIdxStyle = 0;
    ReloadJumpBegin = true;
    NeedJump = false;
}

bool UReuseListC::Initialize()
{
    if (!Super::Initialize())
        return false;
    ScrollBoxList = Cast<UScrollBox>(GetWidgetFromName(FName(TEXT("ScrollBoxList"))));
    ensure(ScrollBoxList.IsValid());

    CanvasPanelBg = Cast<UCanvasPanel>(GetWidgetFromName(FName(TEXT("CanvasPanelBg"))));
    ensure(CanvasPanelBg.IsValid());

    SizeBoxBg = Cast<USizeBox>(GetWidgetFromName(FName(TEXT("SizeBoxBg"))));
    ensure(SizeBoxBg.IsValid());

    CanvasPanelList = Cast<UCanvasPanel>(GetWidgetFromName(FName(TEXT("CanvasPanelList"))));
    ensure(CanvasPanelList.IsValid());
    return true;
}

void UReuseListC::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
    FVector2D sz = GetCachedGeometry().GetLocalSize();
    if (!sz.Equals(ViewSize, 0.0001f))
        DoReload();
    Update();
    DoJump();
}

void UReuseListC::Reload(int32 __ItemCount, int32 __ItemHeight, int32 __ItemWidth, int32 __Style, UClass* __Class, int32 __PaddingX, int32 __PaddingY, bool __ReloadJumpBegin)
{
    ItemCount = __ItemCount;
    ItemHeight = __ItemHeight;
    ItemWidth = __ItemWidth;
    PaddingX = __PaddingX;
    PaddingY = __PaddingY;
    ReloadJumpBegin = __ReloadJumpBegin;
    bool bNeedClearCache = false;
    if (ItemClass != __Class) {
        ItemClass = __Class;
        bNeedClearCache = true;
    }
    if (Style != __Style) {
        Style = __Style;
        bNeedClearCache = true;
    }
    if (Style == 0 || Style == 2) {
        ScrollBoxList->SetOrientation(Orient_Vertical);
    }
    else {
        ScrollBoxList->SetOrientation(Orient_Horizontal);
    }
    if (bNeedClearCache) {
        ItemPool.Empty();
        CanvasPanelList->ClearChildren();
    }
    DoReload();
}

void UReuseListC::Refresh()
{
    for (TMap<int32, TWeakObjectPtr<UUserWidget> >::TConstIterator iter(ItemMap); iter; ++iter) {
        RefreshOne(iter->Key);
    }
}

void UReuseListC::RefreshOne(int32 __Idx)
{
    auto v = ItemMap.Find(__Idx);
    if (v) {
        OnUpdateItem.Broadcast(v->Get(), __Idx);
    }
}

void UReuseListC::JumpByIdx(int32 __Idx, int32 __Style)
{
    JumpIdx = __Idx;
    JumpIdxStyle = __Style;
    NeedJump = true;
}

void UReuseListC::Clear()
{
    Reload(0, ItemHeight, ItemWidth, Style, ItemClass.Get(), PaddingX, PaddingY, ReloadJumpBegin);
}

int32 UReuseListC::GetCurrentBegin()
{
    return BIdx;
}

int32 UReuseListC::GetCurrentEnd()
{
    return EIdx;
}

void UReuseListC::ScrollUpdate(float __Offset)
{
    int32 ItemWidthAndPad = ItemWidth + PaddingX;
    int32 ItemHeightAndPad = ItemHeight + PaddingY;
    int32 Offset = __Offset;
    Offset = UKismetMathLibrary::Max(Offset, 0);
    Offset = UKismetMathLibrary::Min(Offset, MaxPos);
    if (Style == 0) {
        OffsetEnd = UKismetMathLibrary::Min((Offset + (int32)ViewSize.Y), MaxPos);
        BIdx = UKismetMathLibrary::Max(Offset / ItemHeightAndPad - ItemCacheNum, 0);
        EIdx = UKismetMathLibrary::Min(OffsetEnd / ItemHeightAndPad + ItemCacheNum, ItemCount-1);
        RemoveNotUsed();
        for (int32 i = BIdx; i <= EIdx; i++) {
            if (!ItemMap.Contains(i)) {
                auto w = NewItem();
                auto cps = Cast<UCanvasPanelSlot>(w->Slot);
                cps->SetAnchors(FAnchors(0,0,0,0));
                cps->SetOffsets(FMargin(0, i * ItemHeightAndPad, ViewSize.X, ItemHeight));
                ItemMap.Add(i,w);
                OnUpdateItem.Broadcast(w, i);
            }
        }
    }
    else if (Style == 1) {
        OffsetEnd = UKismetMathLibrary::Min((Offset + (int32)ViewSize.X), MaxPos);
        BIdx = UKismetMathLibrary::Max(Offset / ItemWidthAndPad - ItemCacheNum, 0);
        EIdx = UKismetMathLibrary::Min(OffsetEnd / ItemWidthAndPad + ItemCacheNum, ItemCount-1);
        RemoveNotUsed();
        for (int32 i = BIdx; i <= EIdx; i++) {
            if (!ItemMap.Contains(i)) {
                auto w = NewItem();
                auto cps = Cast<UCanvasPanelSlot>(w->Slot);
                cps->SetAnchors(FAnchors(0, 0, 0, 0));
                if (ItemHeight <= 0) {
                    cps->SetOffsets(FMargin(i * ItemWidthAndPad, 0, ItemWidth, ViewSize.X));
                }
                else {
                    cps->SetOffsets(FMargin(i * ItemWidthAndPad, (ViewSize.X - ItemHeight) / 2.f, ItemWidth, ItemHeight));
                }
                ItemMap.Add(i, w);
                OnUpdateItem.Broadcast(w, i);
            }
        }
    }
    else if (Style == 2) {
        OffsetEnd = UKismetMathLibrary::Min((Offset + (int32)ViewSize.Y), MaxPos);
        BIdx = UKismetMathLibrary::Max( ( Offset / ItemHeightAndPad ) * ColNum, 0);
        int32 tmp = UKismetMathLibrary::FCeil((float)OffsetEnd / ItemHeightAndPad) + 1;
        EIdx = UKismetMathLibrary::Min(tmp * ColNum - 1, ItemCount-1);
        RemoveNotUsed();
        for (int32 i = BIdx; i <= EIdx; i++) {
            if (!ItemMap.Contains(i)) {
                auto w = NewItem();
                auto cps = Cast<UCanvasPanelSlot>(w->Slot);
                cps->SetAnchors(FAnchors(0, 0, 0, 0));
                cps->SetOffsets(FMargin((i%ColNum)*ItemWidthAndPad, (i/ColNum)*ItemHeightAndPad, ItemWidth, ItemHeight));
                ItemMap.Add(i, w);
                OnUpdateItem.Broadcast(w, i);
            }
        }
    }
    OnScrollItem.Broadcast(BIdx, EIdx);
}

void UReuseListC::UpdateContentSize(UWidget* widget)
{
    auto cps = Cast<UCanvasPanelSlot>(widget->Slot);
    if (Style == 0 || Style == 2) {
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
    for (TMap<int32, TWeakObjectPtr<UUserWidget> >::TIterator iter = ItemMap.CreateIterator(); iter; ++iter)
    {
        if (!UKismetMathLibrary::InRange_IntInt(iter->Key, BIdx, EIdx)) {
            ReleaseItem(iter->Value.Get());
            iter.RemoveCurrent();
        }
    }
}

void UReuseListC::DoReload()
{
    ViewSize = GetCachedGeometry().GetLocalSize();
    switch (Style)
    {
    case 0:
        MaxPos = (ItemHeight + PaddingY) * ItemCount - PaddingY;
        ContentSize.X = ViewSize.X;
        ContentSize.Y = MaxPos;
        break;
    case 1:
        MaxPos = (ItemWidth + PaddingX) * ItemCount - PaddingX;
        ContentSize.X = MaxPos;
        ContentSize.Y = ViewSize.Y;
        break;
    case 2:
        ColNum = UKismetMathLibrary::Max((PaddingX + (int32)ViewSize.X) / (ItemWidth + PaddingX), 1);
        RowNum = UKismetMathLibrary::FCeil((float)ItemCount / ColNum);
        MaxPos = (ItemHeight + PaddingY) * RowNum - PaddingY;
        ContentSize.X = ViewSize.X;
        ContentSize.Y = MaxPos;
        break;
    }
    UpdateContentSize(SizeBoxBg.Get());
    UpdateContentSize(CanvasPanelList.Get());
    for (int32 i = 0; i < CanvasPanelList->GetChildrenCount(); i++) {
        auto uw = Cast<UUserWidget>(CanvasPanelList->GetChildAt(i));
        ReleaseItem(uw);
    }
    ItemMap.Empty();
    if (ReloadJumpBegin) {
        ScrollBoxList->ScrollToStart();
    }
    else {
        float TmpMaxOffset = 0.f;
        if (Style == 0 || Style == 2) {
            TmpMaxOffset = UKismetMathLibrary::FMax(MaxPos - ViewSize.Y, 0.f);
        }
        else {
            TmpMaxOffset = UKismetMathLibrary::FMax(MaxPos - ViewSize.X, 0.f);
        }
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
    if (Style == 0 || Style == 2) {
        if (JumpIdxStyle == 1) {
            tmpItemOffset = 0;
        }
        else if (JumpIdxStyle == 2) {
            tmpItemOffset = (int32)ViewSize.Y - ItemHeightAndPad;
        }
        else {
            tmpItemOffset = ((int32)ViewSize.Y - ItemHeightAndPad)/2;
        }
    }
    else {
        if (JumpIdxStyle == 1) {
            tmpItemOffset = 0;
        }
        else if (JumpIdxStyle == 2) {
            tmpItemOffset = (int32)ViewSize.X - ItemWidthAndPad;
        }
        else {
            tmpItemOffset = ((int32)ViewSize.X - ItemWidthAndPad) / 2;
        }
    }
    if (Style == 0) {
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
    else if (Style == 1) {
        if (ContentSize.X < ViewSize.X) {
            return;
        }
        tmpScroll = ItemWidthAndPad * JumpIdx - tmpItemOffset;
    }
    else if (Style == 2) {
        if (ContentSize.Y < ViewSize.Y) {
            return;
        }
        tmpScroll = ItemHeightAndPad * (JumpIdx / ColNum) - tmpItemOffset;
    }
    if (Style == 0 || Style == 2) {
        tmpScroll = UKismetMathLibrary::FMax(tmpScroll, 0);
        tmpScroll = UKismetMathLibrary::FMin(tmpScroll, ContentSize.Y - ViewSize.Y);
    }
    else {
        tmpScroll = UKismetMathLibrary::FMax(tmpScroll, 0);
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
        return tmp.Get();
    }
    else {
        UUserWidget* widget = CreateWidget<UUserWidget>(GetWorld(), ItemClass.Get());
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
    if (Style == 0 || Style == 2) {
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