// Fill out your copyright notice in the Description page of Project Settings.

#include "UReuseListC.h"
#include "Runtime/UMG/Public/Components/SizeBox.h"
#include "Runtime/UMG/Public/Components/CanvasPanel.h"
#include "Runtime/UMG/Public/Components/CanvasPanelSlot.h"
#include "Runtime/UMG/Public/Components/ScrollBox.h"
#include "Runtime/UMG/Public/Components/NamedSlot.h"
#include "Runtime/UMG/Public/Blueprint/WidgetBlueprintGeneratedClass.h"
#include "Runtime/Engine/Classes/Kismet/KismetMathLibrary.h"

DEFINE_LOG_CATEGORY(LogUReuseListC);

UReuseListC::UReuseListC(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer)
    , ScrollBoxList(nullptr)
    , CanvasPanelBg(nullptr)
    , SizeBoxBg(nullptr)
    , CanvasPanelList(nullptr)
    , NamedSlotTitle(nullptr)
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
    , ScrollBoxVisibility(ESlateVisibility::Visible)
    , NotFullAlignStyle(EReuseListNotFullAlignStyle::Start)
    , NotFullScrollBoxHitTestInvisible(false)
    , ScrollBoxClipping(EWidgetClipping::ClipToBounds)
    , AlignSpace(0.f)
    , DelayUpdateTimeLimitMS(0)
    , LastOffset(0)
    , UpdateForceLayoutPrepass(false)
    , TitlePadding(0)
    , TitleSize(0)
    , AutoTitleSize(true)
    , CacheTitleSize(0.f)
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
    if (!ViewSize.Equals(lzSz, 0.0001f) || FMath::Abs(GetTitleSize() - CacheTitleSize) > 0.0001f)
        DoReload();
    DoDelayUpdate();
    Update();
    DoJump();
}

float UReuseListC::GetTitleSize()
{
    if (!NamedSlotTitle.IsValid())
        return 0.f;
    UWidget* w = NamedSlotTitle->GetChildAt(0);
    if (w) {
        FVector2D sz = w->GetCachedGeometry().GetLocalSize();
        return (IsVertical() ? sz.Y : sz.X) + TitlePadding;
    }
    return 0.f;
}

void UReuseListC::Reload(int32 __ItemCount)
{
    ItemCount = __ItemCount;
    if (ScrollBoxList.IsValid()) {
        ScrollBoxList->SetOrientation(IsVertical() ? Orient_Vertical : Orient_Horizontal);
        ScrollBoxList->SetScrollBarVisibility(ScrollBarVisibility);
    }
    const FVector2D& lzSz = GetCachedGeometry().GetLocalSize();
    if (lzSz.Equals(FVector2D::ZeroVector, 0.0001f)) {
        ViewSize = lzSz;
        ReleaseAllItem();
        return;
    }
    DoReload();
}

void UReuseListC::Refresh()
{
    for (TMap<int32, TWeakObjectPtr<UUserWidget> >::TConstIterator iter(ItemMap); iter; ++iter) {
        AddDelayUpdate(iter->Key);
    }
}

void UReuseListC::RefreshOne(int32 __Idx)
{
    TWeakObjectPtr<UUserWidget>* v = ItemMap.Find(__Idx);
    if (v && (*v).IsValid()) {
        AddDelayUpdate(__Idx);
    }
}

void UReuseListC::RefreshParam(FString _Param)
{
    for (TMap<int32, TWeakObjectPtr<UUserWidget> >::TConstIterator iter(ItemMap); iter; ++iter) {
        RefreshOneParam(iter->Key, _Param);
    }
}

void UReuseListC::RefreshOneParam(int32 __Idx, FString _Param)
{
    TWeakObjectPtr<UUserWidget>* v = ItemMap.Find(__Idx);
    if (v && (*v).IsValid()) {
        OnUpdateItemParam.Broadcast((*v).Get(), __Idx, _Param);
        if (UpdateForceLayoutPrepass) {
            (*v)->ForceLayoutPrepass();
        }
    }
}

void UReuseListC::ScrollToStart()
{
    if (ScrollBoxList.IsValid()) {
        ScrollBoxList->ScrollToStart();
    }
}

void UReuseListC::ScrollToEnd()
{
    if (ScrollBoxList.IsValid()) {
        ScrollBoxList->ScrollToEnd();
    }
}

void UReuseListC::SetScrollOffset(float NewScrollOffset)
{
    if (ScrollBoxList.IsValid()) {
        ScrollBoxList->SetScrollOffset(NewScrollOffset);
    }
}

float UReuseListC::GetScrollOffset() const
{
    if (ScrollBoxList.IsValid()) {
        return ScrollBoxList->GetScrollOffset();
    }
    return 0.f;
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

void UReuseListC::SetTitleSize(int32 sz)
{
    TitleSize = sz;
    UpdateNamedSlotTitleAnchors();
}

void UReuseListC::SetTitleSlotAutoSize(bool as)
{
    if (NamedSlotTitle.IsValid()) {
        UCanvasPanelSlot* cps = Cast<UCanvasPanelSlot>(NamedSlotTitle->Slot);
        if (cps) {
            cps->SetAutoSize(as);
            UpdateNamedSlotTitleAnchors();
        }
    }
}

void UReuseListC::InitWidgetPtr()
{
    ScrollBoxList = Cast<UScrollBox>(GetWidgetFromName(FName(TEXT("ScrollBoxList"))));
    CanvasPanelBg = Cast<UCanvasPanel>(GetWidgetFromName(FName(TEXT("CanvasPanelBg"))));
    SizeBoxBg = Cast<USizeBox>(GetWidgetFromName(FName(TEXT("SizeBoxBg"))));
    CanvasPanelList = Cast<UCanvasPanel>(GetWidgetFromName(FName(TEXT("CanvasPanelList"))));
    NamedSlotTitle = Cast<UNamedSlot>(GetWidgetFromName(FName(TEXT("NamedSlotTitle"))));
}

void UReuseListC::ComputeAlignSpace()
{
    AlignSpace = 0.f;
    float content = GetContentSpan();
    float view = GetViewSpan();
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
    if (NotFullScrollBoxHitTestInvisible && ScrollBoxList.IsValid()) {
        ScrollBoxList->SetVisibility(GetViewSpan() > GetContentSpan() ? ESlateVisibility::SelfHitTestInvisible : ESlateVisibility::Visible);
    }
}

void UReuseListC::ScrollUpdate(float __Offset)
{
    int32 ItemWidthAndPad = ItemWidth + PaddingX;
    int32 ItemHeightAndPad = ItemHeight + PaddingY;
    int32 Offset = __Offset - CacheTitleSize;
    int32 OffsetEnd = 0;
    int32 BIdx = 0;
    int32 EIdx = 0;
    int32 _MaxPos = MaxPos - CacheTitleSize;
    float vs = GetViewSpan();
    int32 span = FMath::Clamp(vs - FMath::Clamp(CacheTitleSize - __Offset, 0.f, CacheTitleSize), 0.f, vs);
    Offset = FMath::Clamp(Offset, 0, _MaxPos);
    OffsetEnd = FMath::Min(Offset + span, _MaxPos);
    if (Style == EReuseListStyle::Vertical) {
        BIdx = FMath::Max(Offset / ItemHeightAndPad - ItemCacheNum, 0);
        EIdx = FMath::Min(OffsetEnd / ItemHeightAndPad + ItemCacheNum, ItemCount - 1);
    }
    else if (Style == EReuseListStyle::Horizontal) {
        BIdx = FMath::Max(Offset / ItemWidthAndPad - ItemCacheNum, 0);
        EIdx = FMath::Min(OffsetEnd / ItemWidthAndPad + ItemCacheNum, ItemCount - 1);
    }
    else if (Style == EReuseListStyle::VerticalGrid) {
        BIdx = FMath::Max((Offset / ItemHeightAndPad) * ColNum, 0);
        int32 tmp = FMath::CeilToFloat((float)OffsetEnd / ItemHeightAndPad) + 1;
        EIdx = FMath::Min(tmp * ColNum - 1, ItemCount - 1);
    }
    else if (Style == EReuseListStyle::HorizontalGrid) {
        BIdx = FMath::Max((Offset / ItemWidthAndPad) * RowNum, 0);
        int32 tmp = FMath::CeilToFloat((float)OffsetEnd / ItemWidthAndPad) + 1;
        EIdx = FMath::Min(tmp * RowNum - 1, ItemCount - 1);
    }
    RemoveNotUsed(BIdx, EIdx);
    if (LastOffset <= Offset) {
        for (int32 i = BIdx; i <= EIdx; ++i) {
            if (!ItemMap.Contains(i)) {
                AddDelayUpdate(i);
            }
        }
    }
    else {
        for (int32 i = EIdx; i >= BIdx; --i) {
            if (!ItemMap.Contains(i)) {
                AddDelayUpdate(i);
            }
        }
    }
    OnScrollItem.Broadcast(BIdx, EIdx);
    LastOffset = Offset;
}

void UReuseListC::UpdateContentSize(TWeakObjectPtr<UWidget> widget)
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

void UReuseListC::RemoveNotUsed(int32 BIdx, int32 EIdx)
{
    for (TMap<int32, TWeakObjectPtr<UUserWidget> >::TIterator iter = ItemMap.CreateIterator(); iter; ++iter) {
        if (!UKismetMathLibrary::InRange_IntInt(iter->Key, BIdx, EIdx)) {
            ReleaseItem(iter->Value.Get());
            iter.RemoveCurrent();
        }
    }
    for (int32 i = DelayUpdateList.Num() - 1; i >= 0; --i) {
        int32 idx = DelayUpdateList[i];
        if (!UKismetMathLibrary::InRange_IntInt(idx, BIdx, EIdx)) {
            DelayUpdateList.RemoveAt(i);
        }
    }
}

void UReuseListC::DoReload()
{
    if (IsInvalidParam())
        return;
    ViewSize = GetCachedGeometry().GetLocalSize();
    CacheTitleSize = GetTitleSize();
    switch (Style)
    {
    case EReuseListStyle::Vertical:
        MaxPos = (ItemHeight + PaddingY) * ItemCount - PaddingY + CacheTitleSize;
        MaxPos = FMath::Max(MaxPos, 0);
        ContentSize.X = ViewSize.X;
        ContentSize.Y = MaxPos;
        break;
    case EReuseListStyle::Horizontal:
        MaxPos = (ItemWidth + PaddingX) * ItemCount - PaddingX + CacheTitleSize;
        MaxPos = FMath::Max(MaxPos, 0);
        ContentSize.X = MaxPos;
        ContentSize.Y = ViewSize.Y;
        break;
    case EReuseListStyle::VerticalGrid:
        ColNum = FMath::Max((PaddingX + (int32)ViewSize.X) / (ItemWidth + PaddingX), 1);
        RowNum = FMath::CeilToFloat((float)ItemCount / ColNum);
        MaxPos = (ItemHeight + PaddingY) * RowNum - PaddingY + CacheTitleSize;
        MaxPos = FMath::Max(MaxPos, 0);
        ContentSize.X = ViewSize.X;
        ContentSize.Y = MaxPos;
        break;
    case EReuseListStyle::HorizontalGrid:
        RowNum = FMath::Max((PaddingY + (int32)ViewSize.Y) / (ItemHeight + PaddingY), 1);
        ColNum = FMath::CeilToFloat((float)ItemCount / RowNum);
        MaxPos = (ItemWidth + PaddingX) * ColNum - PaddingX + CacheTitleSize;
        MaxPos = FMath::Max(MaxPos, 0);
        ContentSize.X = MaxPos;
        ContentSize.Y = ViewSize.Y;
        break;
    }
    if (SizeBoxBg.IsValid()) {
        UpdateContentSize(SizeBoxBg);
    }
    if (CanvasPanelList.IsValid()) {
        UpdateContentSize(CanvasPanelList);
    }
    UpdateNamedSlotTitleAnchors();
    ComputeAlignSpace();
    ComputeScrollBoxHitTest();
    ReleaseAllItem();
    if (ScrollBoxList.IsValid()) {
        float TmpMaxOffset = FMath::Max((float)MaxPos - GetViewSpan(), 0.f);
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
    float content_span = GetContentSpan();
    float view_span = GetViewSpan();

    if (JumpStyle == EReuseListJumpStyle::Begin) {
        tmpItemOffset = 0;
    }
    else if (JumpStyle == EReuseListJumpStyle::End) {
        tmpItemOffset = (int32)view_span - ItemHeightAndPad;
    }
    else {
        tmpItemOffset = ((int32)view_span - ItemHeightAndPad) / 2;
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

    tmpScroll = FMath::Clamp(tmpScroll + CacheTitleSize, 0.f, content_span - view_span);
    if (ScrollBoxList.IsValid()) {
        ScrollBoxList->SetScrollOffset(tmpScroll);
    }
    ScrollUpdate(tmpScroll);
}

TWeakObjectPtr<UUserWidget> UReuseListC::NewItem()
{
    if (ItemPool.Num() > 0) {
        TWeakObjectPtr<UUserWidget> widget = ItemPool.Pop();
        if (widget.IsValid()) {
            widget->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
        }
        return widget;
    }
    else {
        TWeakObjectPtr<UUserWidget> widget = CreateWidget<UUserWidget>(GetWorld(), ItemClass);
        if (widget.IsValid() && CanvasPanelList.IsValid()) {
            CanvasPanelList->AddChild(widget.Get());
            widget->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
            OnCreateItem.Broadcast(widget.Get());
        }
        return widget;
    }
}

void UReuseListC::ReleaseItem(TWeakObjectPtr<UUserWidget> __Item)
{
    __Item->SetVisibility(ESlateVisibility::Collapsed);
    ItemPool.AddUnique(__Item);
}

void UReuseListC::ReleaseAllItem()
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
    DelayUpdateList.Empty();
}

void UReuseListC::Update()
{
    if (ScrollBoxList.IsValid()) {
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
}

void UReuseListC::ClearCache()
{
    ItemPool.Empty();
    if (CanvasPanelList.IsValid()) {
        CanvasPanelList->ClearChildren();
    }
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

int32 UReuseListC::GetDelayUpdateTimeLimitMS()
{
    UWorld* wld = GetWorld();
    if (wld && !wld->IsGameWorld()) {
        return 0;
    }
    return DelayUpdateTimeLimitMS;
}

void UReuseListC::AddDelayUpdate(int32 idx)
{
    DelayUpdateList.Remove(idx);
    DelayUpdateList.Add(idx);
    if (GetDelayUpdateTimeLimitMS() == 0) {
        DoDelayUpdate();
    }
}

void UReuseListC::DoDelayUpdate()
{
    if (DelayUpdateList.Num() == 0) {
        return;
    }
    int32 ItemWidthAndPad = ItemWidth + PaddingX;
    int32 ItemHeightAndPad = ItemHeight + PaddingY;
    FMargin mar;
    FAnchors ach(0, 0, 0, 0);
    //test
    //static int32 s_key = 0;
    //s_key++;
    int32 timeLimit = GetDelayUpdateTimeLimitMS();
    double timeLimitSec = timeLimit / 1000.f;
    double sec = FPlatformTime::Seconds();
    do {
        int32 idx = DelayUpdateList[0];
        DelayUpdateList.RemoveAt(0);
        TWeakObjectPtr<UUserWidget>* v = ItemMap.Find(idx);
        if (v && (*v).IsValid()) {
            OnUpdateItem.Broadcast((*v).Get(), idx);
        }
        else {
            if (Style == EReuseListStyle::Vertical) {
                mar.Left = 0;
                mar.Top = idx * ItemHeightAndPad + AlignSpace + CacheTitleSize;
                mar.Right = ViewSize.X;
                mar.Bottom = ItemHeight;
            }
            else if (Style == EReuseListStyle::Horizontal) {
                if (ItemHeight <= 0) {
                    mar.Left = idx * ItemWidthAndPad + AlignSpace + CacheTitleSize;
                    mar.Top = 0;
                    mar.Right = ItemWidth;
                    mar.Bottom = ViewSize.Y;
                }
                else {
                    mar.Left = idx * ItemWidthAndPad + AlignSpace + CacheTitleSize;
                    mar.Top = (ViewSize.Y - ItemHeight) / 2.f;
                    mar.Right = ItemWidth;
                    mar.Bottom = ItemHeight;
                }
            }
            else if (Style == EReuseListStyle::VerticalGrid) {
                mar.Left = (idx % ColNum) * ItemWidthAndPad;
                mar.Top = (idx / ColNum) * ItemHeightAndPad + AlignSpace + CacheTitleSize;
                mar.Right = ItemWidth;
                mar.Bottom = ItemHeight;
            }
            else if (Style == EReuseListStyle::HorizontalGrid) {
                mar.Left = (idx / RowNum) * ItemWidthAndPad + AlignSpace + CacheTitleSize;
                mar.Top = (idx % RowNum) * ItemHeightAndPad;
                mar.Right = ItemWidth;
                mar.Bottom = ItemHeight;
            }
            TWeakObjectPtr<UUserWidget> w = NewItem();
            if (w.IsValid()) {
                UCanvasPanelSlot* cps = Cast<UCanvasPanelSlot>(w->Slot);
                if (cps) {
                    cps->SetAnchors(ach);
                    cps->SetOffsets(mar);
                }
                ItemMap.Add(idx, w);
                OnUpdateItem.Broadcast(w.Get(), idx);
                if (UpdateForceLayoutPrepass) {
                    w->ForceLayoutPrepass();
                }
            }
        }
        //time limit
        if (timeLimit > 0) {
            double deltaTm = FPlatformTime::Seconds() - sec;
            if (deltaTm > timeLimitSec) {
                break;
            }
        }
        //test
        //UE_LOG(LogUReuseListC, Log, TEXT("OnUpdateItem key=%d idx=%d"), s_key, idx);
    } while (DelayUpdateList.Num() > 0);
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

void UReuseListC::OnEditReload()
{
    Reload(PreviewCount);
}

void UReuseListC::OnWidgetRebuilt()
{
    Super::OnWidgetRebuilt();
    InitWidgetPtr();
    SyncProp();
#if WITH_EDITOR
    UWorld* wld = GetWorld();
    if (wld && !wld->IsGameWorld()) {
        wld->GetTimerManager().SetTimerForNextTick(this, &UReuseListC::OnEditReload);
    }
#endif
}

void UReuseListC::SyncProp()
{
    if (ScrollBoxList.IsValid()) {
        ScrollBoxList->SetVisibility(ScrollBoxVisibility);
        ScrollBoxList->SetScrollBarVisibility(ScrollBarVisibility);
        ScrollBoxList->SetScrollbarThickness(ScrollBarThickness);
        ScrollBoxList->WidgetBarStyle = ScrollBarStyle;
        ScrollBoxList->WidgetStyle = ScrollBoxStyle;
        ScrollBoxList->SetClipping(ScrollBoxClipping);
    }
    UpdateNamedSlotTitleAnchors();
}

void UReuseListC::UpdateNamedSlotTitleAnchors()
{
    if (NamedSlotTitle.IsValid()) {
        UCanvasPanelSlot* cps = Cast<UCanvasPanelSlot>(NamedSlotTitle->Slot);
        if (cps) {
            cps->SetAutoSize(AutoTitleSize);
            if (AutoTitleSize) {
                if (IsVertical()) {
                    cps->SetAnchors(FAnchors(0, 0, 1, 0));
                    cps->SetOffsets(FMargin(0, 0, 0, TitleSize));
                }
                else {
                    cps->SetAnchors(FAnchors(0, 0, 0, 1));
                    cps->SetOffsets(FMargin(0, 0, TitleSize, 0));
                }
            }
            else {
                if (IsVertical()) {
                    cps->SetAnchors(FAnchors(0, 0, 0, 0));
                    cps->SetOffsets(FMargin(0, 0, ViewSize.X, TitleSize));
                }
                else {
                    cps->SetAnchors(FAnchors(0, 0, 0, 0));
                    cps->SetOffsets(FMargin(0, 0, TitleSize, ViewSize.Y));
                }
            }
        }
    }
    //static int32 s_num = 1;
    //UE_LOG(LogUReuseListC, Log, TEXT("UpdateNamedSlotTitleAnchors num=%d"), s_num++);
}