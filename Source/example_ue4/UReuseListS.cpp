// Fill out your copyright notice in the Description page of Project Settings.

#include "UReuseListS.h"
#include "Runtime/UMG/Public/Components/SizeBox.h"
#include "Runtime/UMG/Public/Components/CanvasPanel.h"
#include "Runtime/UMG/Public/Components/CanvasPanelSlot.h"
#include "Runtime/UMG/Public/Components/ScrollBox.h"
#include "Runtime/UMG/Public/Blueprint/WidgetBlueprintGeneratedClass.h"
#include "Runtime/Engine/Classes/Kismet/KismetMathLibrary.h"

DEFINE_LOG_CATEGORY(LogUReuseListS);
static const float c_fLastOffsetInitValue = 0.f;

UReuseListS::UReuseListS(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer)
    , ScrollBoxList(nullptr)
    , CanvasPanelBg(nullptr)
    , SizeBoxBg(nullptr)
    , CanvasPanelList(nullptr)
    , ViewSize(FVector2D::ZeroVector)
    , ContentSize(FVector2D::ZeroVector)
    , ItemClass(nullptr)
    , ItemWidth(100)
    , ItemHeight(100)
    , ItemPaddingX(0)
    , ItemPaddingY(0)
    , ItemCount(0)
    , MaxPos(0)
    , Style1(EReuseListSStyle::Vertical)
    , PreviewCount(5)
    , ItemPoolMaxNum(100)
    , ScrollBarVisibility(ESlateVisibility::Collapsed)
    , LastOffset(0.f)
    , ArrOffset(FBox2D(FVector2D::ZeroVector, FVector2D(99999.f,99999.f)))
{
    for (int32 i = 0; i < EReuseListSMsgType::Num; ++i) {
        MsgMap[i] = 0;
    }
    ScrollBoxStyle.LeftShadowBrush = FSlateNoResource();
    ScrollBoxStyle.TopShadowBrush = FSlateNoResource();
    ScrollBoxStyle.RightShadowBrush = FSlateNoResource();
    ScrollBoxStyle.BottomShadowBrush = FSlateNoResource();
}

bool UReuseListS::Initialize()
{
    if (!Super::Initialize())
        return false;
    InitWidgetPtr();
    SyncProp();
    return true;
}

void UReuseListS::NativeConstruct()
{
    Super::NativeConstruct();
    InitWidgetPtr();
    SyncProp();
}

void UReuseListS::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
    Super::NativeTick(MyGeometry, InDeltaTime);

    if (IsInvalidParam())
        return;

    for (int32 i = 0; i < EReuseListSMsgType::Num; ++i) {
        if (MsgMap[i] > 0) {
            --MsgMap[i];
            switch (i) {
            case EReuseListSMsgType::AdjustItem: {
                AdjustItem();
                break;
            }
            case EReuseListSMsgType::FillArrOffset: {
                FillArrOffset();
                break;
            }
            case EReuseListSMsgType::AdjustItemWidgetSize: {
                AdjustItemWidgetSize();
                break;
            }
            case EReuseListSMsgType::DoReload: {
                DoReload();
                break;
            }
            }
        }
    }

    ScrollUpdate(ScrollBoxList->GetScrollOffset());

}

void UReuseListS::Reload(int32 __ItemCount)
{
    ItemCount = __ItemCount;
    const FVector2D& lzSz = GetCachedGeometry().GetLocalSize();
    if (lzSz.Equals(FVector2D::ZeroVector, 0.0001f)) {
        ViewSize = lzSz;
        ReleaseAllItem();
        MsgMap[EReuseListSMsgType::DoReload] = 1;
        return;
    }
    DoReload();
}

void UReuseListS::Refresh()
{
    for (TMap<int32, TWeakObjectPtr<UUserWidget> >::TConstIterator iter(ItemMap); iter; ++iter) {
        RefreshOne(iter->Key);
    }
}

void UReuseListS::RefreshOne(int32 __Idx)
{
    TWeakObjectPtr<UUserWidget>* v = ItemMap.Find(__Idx);
    if (v) {
        OnUpdateItem.Broadcast(v->Get(), __Idx);
    }
}

void UReuseListS::ScrollToStart()
{
    if (ScrollBoxList.IsValid())
        ScrollBoxList->ScrollToStart();
}

void UReuseListS::ScrollToEnd()
{
    if (ScrollBoxList.IsValid())
        ScrollBoxList->ScrollToEnd();
}

void UReuseListS::SetScrollOffset(float NewScrollOffset)
{
    if (ScrollBoxList.IsValid())
        ScrollBoxList->SetScrollOffset(NewScrollOffset);
}

float UReuseListS::GetScrollOffset() const
{
    if (ScrollBoxList.IsValid())
        return ScrollBoxList->GetScrollOffset();
    return 0.f;
}

void UReuseListS::InitWidgetPtr()
{
    ScrollBoxList = Cast<UScrollBox>(GetWidgetFromName(FName(TEXT("ScrollBoxList"))));
    CanvasPanelBg = Cast<UCanvasPanel>(GetWidgetFromName(FName(TEXT("CanvasPanelBg"))));
    SizeBoxBg = Cast<USizeBox>(GetWidgetFromName(FName(TEXT("SizeBoxBg"))));
    CanvasPanelList = Cast<UCanvasPanel>(GetWidgetFromName(FName(TEXT("CanvasPanelList"))));
}

void UReuseListS::ClearSpecialSize()
{
    SpecialSizeMap.Empty();
    MsgMap[EReuseListSMsgType::FillArrOffset] = 1;
    MsgMap[EReuseListSMsgType::AdjustItemWidgetSize] = 1;
}

void UReuseListS::AddSpecialSize(int32 __Idx, const FVector2D& __Size)
{
    SpecialSizeMap.Add(__Idx, __Size);
    MsgMap[EReuseListSMsgType::FillArrOffset] = 1;
    MsgMap[EReuseListSMsgType::AdjustItemWidgetSize] = 1;
}

void UReuseListS::FillArrOffset()
{
    ArrOffset.Empty();
    TransMap.Empty();
    int32 tmpOffset = 0;
    int32 sz_item = 0;
    //ViewSize.X
    FVector2D Offset = FVector2D::ZeroVector;
    FBox2D box;
    MaxPos = 0;
    if (IsVertical()) {
        float lineHeight = 0.f;
        for (int32 i = 0; i < ItemCount; i++) {
            FVector2D sz_item = GetItemSize(i);
            if (Offset.X + sz_item.X + ItemPaddingX > ViewSize.X) {
                Offset.X = 0;
                Offset.Y += lineHeight + ItemPaddingY;
                lineHeight = 0.f;
            }
            if (sz_item.Y > lineHeight) {
                lineHeight = sz_item.Y;
            }
            box.bIsValid = true;
            MaxPos = Offset.Y + sz_item.Y;
            box.Min.X = Offset.X;
            box.Min.Y = Offset.Y;
            box.Max.X = box.Min.X + sz_item.X;
            box.Max.Y = box.Min.Y + lineHeight;
            if (Offset.X + sz_item.X + ItemPaddingX <= ViewSize.X) {
                Offset.X += sz_item.X + ItemPaddingX;
            }
            ArrOffset.Insert(i, box);
            TransMap.Add(i, box);
        }
    }
    else {
        float lineWidth = 0.f;
        for (int32 i = 0; i < ItemCount; i++) {
            FVector2D sz_item = GetItemSize(i);
            if (Offset.Y + sz_item.Y + ItemPaddingY > ViewSize.Y) {
                Offset.Y = 0;
                Offset.X += lineWidth + ItemPaddingX;
                lineWidth = 0.f;
            }
            if (sz_item.X > lineWidth) {
                lineWidth = sz_item.X;
            }
            box.bIsValid = true;
            MaxPos = Offset.X + sz_item.X;
            box.Min.X = Offset.X;
            box.Min.Y = Offset.Y;
            box.Max.X = box.Min.X + lineWidth;
            box.Max.Y = box.Min.Y + sz_item.Y;
            if (Offset.Y + sz_item.Y + ItemPaddingY <= ViewSize.Y) {
                Offset.Y += sz_item.Y + ItemPaddingY;
            }
            ArrOffset.Insert(i, box);
            TransMap.Add(i, box);
        }
    }
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

    //NeedAdjustScrollOffset = true;
}

FVector2D UReuseListS::GetItemSize(int32 idx)
{
    const FVector2D* pValue = SpecialSizeMap.Find(idx);
    if (pValue)
        return *pValue;
    else
        return FVector2D(ItemWidth, ItemHeight);
}

void UReuseListS::ScrollUpdate(float __Offset)
{
    float Offset = __Offset;
    int32 OffsetEnd = 0;
    int32 BIdx = 0;
    int32 EIdx = 0;

    FBox2D boxView;
    boxView.bIsValid = true;

    Offset = FMath::Clamp(Offset, 0.f, (float)MaxPos);
    //OffsetEnd = UKismetMathLibrary::Clamp(Offset + (int32)GetViewSpan(), 0, MaxPos);

    if (IsVertical()) {
        boxView.Min.X = 0.f;
        boxView.Min.Y = Offset;
        boxView.Max.X = GetViewSize().X;
        boxView.Max.Y = boxView.Min.Y + GetViewSize().Y;
    }
    else {
        boxView.Min.X = Offset;
        boxView.Min.Y = 0.f;
        boxView.Max.X = boxView.Min.X + GetViewSize().X;
        boxView.Max.Y = GetViewSize().Y;
    }

    TArray<int32> items;
    ArrOffset.GetElements(boxView, items);

    //UE_LOG(LogUReuseListS, Log, TEXT("UReuseListS::ScrollUpdate Offset=%d BIdx=%d EIdx=%d"), Offset, BIdx, EIdx);

    RemoveNotUsed(items);

    for (int32 k = 0; k < items.Num(); k++) {
        int32 i = items[k];
        FVector2D sz_item = GetItemSize(i);
        if (sz_item.X <= 0.f || sz_item.Y <= 0.f) {
            continue;
        }
        if (!ItemMap.Contains(i)) {
            TWeakObjectPtr<UUserWidget> w = NewItem();
            if (w.IsValid()) {
                UCanvasPanelSlot* cps = Cast<UCanvasPanelSlot>(w->Slot);
                if (cps) {
                    cps->SetAnchors(FAnchors(0, 0, 0, 0));
                    const FBox2D& box = TransMap[i];
                    cps->SetOffsets(FMargin(box.Min.X, box.Min.Y, box.Max.X - box.Min.X, box.Max.Y - box.Min.Y));
                    ItemMap.Add(i, w);
                    OnUpdateItem.Broadcast(w.Get(), i);
                    MsgMap[EReuseListSMsgType::AdjustItem] = 5;
                }
            }
        }
    }

}

void UReuseListS::UpdateContentSize(TWeakObjectPtr<UWidget> widget)
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

void UReuseListS::RemoveNotUsed(const TArray<int32>& items)
{
    for (TMap<int32, TWeakObjectPtr<UUserWidget> >::TIterator iter(ItemMap); iter; ++iter) {
        int32 idx_found = items.Find(iter->Key);
        if (idx_found == INDEX_NONE) {
            ReleaseItem(iter->Value);
            iter.RemoveCurrent();
        }
    }
    // size == 0 也是不用的
    for (TMap<int32, TWeakObjectPtr<UUserWidget> >::TIterator iter(ItemMap); iter; ++iter) {
        FVector2D sz_item = GetItemSize(iter->Key);
        if (sz_item.X <= 0.f || sz_item.Y <= 0.f) {
            ReleaseItem(iter->Value);
            iter.RemoveCurrent();
        }
    }
}

void UReuseListS::DoReload()
{
    if (IsInvalidParam())
        return;
    ViewSize = GetCachedGeometry().GetLocalSize();
    SpecialSizeMap.Empty();
    FillArrOffset();
    for (int32 i = 0; i < CanvasPanelList->GetChildrenCount(); i++) {
        UUserWidget* uw = Cast<UUserWidget>(CanvasPanelList->GetChildAt(i));
        if (uw) {
            ReleaseItem(uw);
        }
    }
    ItemMap.Empty();
}

TWeakObjectPtr<UUserWidget> UReuseListS::NewItem()
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
        if (!CanvasPanelList.IsValid()) {
            return nullptr;
        }
        if (CanvasPanelList->GetChildrenCount() >= ItemPoolMaxNum) {
            return nullptr;
        }
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

void UReuseListS::ReleaseItem(TWeakObjectPtr<UUserWidget> __Item)
{
    __Item->SetVisibility(ESlateVisibility::Collapsed);
    ItemPool.AddUnique(__Item);
}

void UReuseListS::ReleaseAllItem()
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

void UReuseListS::ClearCache()
{
    ItemMap.Empty();
    ItemPool.Empty();
    CanvasPanelList->ClearChildren();
}

bool UReuseListS::IsVertical() const
{
    return Style1 == EReuseListSStyle::Vertical;
}

bool UReuseListS::IsInvalidParam() const
{
    if (ItemClass == nullptr)
        return true;
    return IsVertical() ? ItemHeight <= 0 : ItemWidth <= 0;
}

#if WITH_EDITOR
void UReuseListS::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
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
        ScrollUpdate(ScrollBoxList->GetScrollOffset());
    }
}
#endif

void UReuseListS::OnEditReload()
{
    Reload(PreviewCount);
    if (ScrollBoxList.IsValid()) {
        ScrollUpdate(ScrollBoxList->GetScrollOffset());
    }
}

void UReuseListS::OnWidgetRebuilt()
{
    Super::OnWidgetRebuilt();
    InitWidgetPtr();
    SyncProp();
#if WITH_EDITOR
    UWorld* wld = GetWorld();
    if (wld && !wld->IsGameWorld()) {
        wld->GetTimerManager().SetTimerForNextTick(this, &UReuseListS::OnEditReload);
    }
#endif
}

void UReuseListS::SyncProp()
{
    if (ScrollBoxList.IsValid()) {
        ScrollBoxList->SetOrientation(IsVertical() ? Orient_Vertical : Orient_Horizontal);
        ScrollBoxList->SetScrollBarVisibility(ScrollBarVisibility);
        ScrollBoxList->SetScrollbarThickness(ScrollBarThickness);
        ScrollBoxList->WidgetBarStyle = ScrollBarStyle;
        ScrollBoxList->WidgetStyle = ScrollBoxStyle;
    }
}

void UReuseListS::AdjustItem()
{
    for (TMap<int32, TWeakObjectPtr<UUserWidget> >::TIterator iter(ItemMap); iter; ++iter) {
        TWeakObjectPtr<UUserWidget> wdg = iter->Value;
        if (wdg.IsValid()) {
            int32 idx = iter->Key;
            FVector2D size_now = wdg->GetDesiredSize();
            FVector2D size = GetItemSize(idx);
            if (!size.Equals(size_now)) {
                AddSpecialSize(idx, size_now);
                //int32 delta_sz = size_now - size;
                FVector2D delta_sz = size_now - size;
                float fOffset = ScrollBoxList->GetScrollOffset();
                if (LastOffset > fOffset) {
                    //向上或向左滑动，需要补滑动差值
                    SetScrollOffset(IsVertical() ? fOffset + delta_sz.Y : fOffset + delta_sz.X);
                }
                LastOffset = fOffset;
            }
        }
    }
}

void UReuseListS::AdjustItemWidgetSize()
{
    for (TMap<int32, TWeakObjectPtr<UUserWidget> >::TIterator iter(ItemMap); iter; ++iter) {
        int32 idx = iter->Key;
        TWeakObjectPtr<UUserWidget> wdg = iter->Value;
        if (wdg.IsValid()) {
            UCanvasPanelSlot* cps = Cast<UCanvasPanelSlot>(wdg->Slot);
            if (cps) {
                const FBox2D& box = TransMap[idx];
                cps->SetOffsets(FMargin(box.Min.X, box.Min.Y, box.Max.X - box.Min.X, box.Max.Y - box.Min.Y));
            }
        }
    }
}