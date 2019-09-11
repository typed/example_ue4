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
    , ScrollBoxVisibility(ESlateVisibility::Visible)
    , NotFullAlignStyle(EReuseListNotFullAlignStyle::Start)
    , NotFullScrollBoxHitTestInvisible(false)
    , ScrollBoxClipping(EWidgetClipping::ClipToBounds)
    , AlignSpace(0.f)
    , DelayUpdateTimeLimitMS(0)
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
    DoDelayUpdate();
    Update();
    DoJump();
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
    if (NotFullScrollBoxHitTestInvisible && ScrollBoxList.IsValid()) {
        float vlen = (IsVertical() ? ViewSize.Y : ViewSize.X);
        float clen = (IsVertical() ? ContentSize.Y : ContentSize.X);
        ScrollBoxList->SetVisibility(vlen > clen ? ESlateVisibility::SelfHitTestInvisible : ESlateVisibility::Visible);
    }
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
    if (SizeBoxBg.IsValid()) {
        UpdateContentSize(SizeBoxBg);
    }
    if (CanvasPanelList.IsValid()) {
        UpdateContentSize(CanvasPanelList);
    }
    ComputeAlignSpace();
    ComputeScrollBoxHitTest();
    ReleaseAllItem();
    if (ScrollBoxList.IsValid()) {
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
            TWeakObjectPtr<UUserWidget> w = NewItem();
            if (w.IsValid()) {
                UCanvasPanelSlot* cps = Cast<UCanvasPanelSlot>(w->Slot);
                if (cps) {
                    cps->SetAnchors(ach);
                    cps->SetOffsets(mar);
                }
                ItemMap.Add(idx, w);
                OnUpdateItem.Broadcast(w.Get(), idx);
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

void UReuseListC::OnWidgetRebuilt()
{
    Super::OnWidgetRebuilt();
    InitWidgetPtr();
    SyncProp();
#if WITH_EDITOR
    UWorld* wld = GetWorld();
    if (wld && !wld->IsGameWorld()) {
        TWeakObjectPtr<UReuseListC> self = this;
        wld->GetTimerManager().SetTimerForNextTick(FTimerDelegate::CreateLambda([=]() {
            if (self.IsValid()) {
                self->Reload(PreviewCount);
            }
        }));
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
}