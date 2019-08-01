// Fill out your copyright notice in the Description page of Project Settings.

#include "UTextBlockEx.h"
#include "Runtime/UMG/Public/Components/TextBlock.h"
#include "Runtime/UMG/Public/Blueprint/WidgetBlueprintLibrary.h"
#include "Runtime/Engine/Classes/Kismet/KismetMathLibrary.h"

DEFINE_LOG_CATEGORY(LogUTextBlockEx);

UTextBlockEx::UTextBlockEx(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer)
    , NeedBuildString(false)
    , Ticked(false)
{

}

bool UTextBlockEx::Initialize()
{
    //UE_LOG(LogUTextBlockEx, Log, TEXT("UTextBlockEx::Initialize"));
    if (!Super::Initialize())
        return false;
    InitWidgetPtr();
    SyncProp();
#if WITH_EDITOR
    UWorld* wld = GetWorld();
    if (wld && !wld->IsGameWorld()) {
        TWeakObjectPtr<UTextBlockEx> self = this;
        wld->GetTimerManager().SetTimer(OnTickTimerHandle, FTimerDelegate::CreateLambda([=]() {
            if (self.IsValid()) {
                self->OnMyTick();
            }
        }), 0.001f, true);
    }
#endif
    return true;
}

void UTextBlockEx::BeginDestroy()
{
    //UE_LOG(LogUTextBlockEx, Log, TEXT("UTextBlockEx::BeginDestroy"));
    Super::BeginDestroy();
}

void UTextBlockEx::NativeConstruct()
{
    //UE_LOG(LogUTextBlockEx, Log, TEXT("UTextBlockEx::NativeConstruct"));
    Super::NativeConstruct();
    InitWidgetPtr();
    SyncProp();
}

void UTextBlockEx::NativeDestruct()
{
    //UE_LOG(LogUTextBlockEx, Log, TEXT("UTextBlockEx::NativeDestruct"));
    Super::NativeDestruct();
}

void UTextBlockEx::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
    Super::NativeTick(MyGeometry, InDeltaTime);
    Ticked = true;
    OnMyTick();
}

void UTextBlockEx::OnMyTick()
{
    const FVector2D& lzSz = GetCachedGeometry().GetLocalSize();
    if (!ViewSize.Equals(lzSz, 0.0001f)) {
        ViewSize = lzSz;
        NeedBuildString = true;
    }
    if (NeedBuildString) {
        NeedBuildString = false;
        BuildString();
    }
}

void UTextBlockEx::ReleaseSlateResources(bool bReleaseChildren)
{
    //UE_LOG(LogUTextBlockEx, Log, TEXT("UTextBlockEx::ReleaseSlateResources"));
    Super::ReleaseSlateResources(bReleaseChildren);
#if WITH_EDITOR
    UWorld* wld = GetWorld();
    if (wld && !wld->IsGameWorld()) {
        if (OnTickTimerHandle.IsValid()) {
            wld->GetTimerManager().ClearTimer(OnTickTimerHandle);
        }
    }
#endif
}

void UTextBlockEx::SynchronizeProperties()
{
    //UE_LOG(LogUTextBlockEx, Log, TEXT("UTextBlockEx::SynchronizeProperties"));
    Super::SynchronizeProperties();
    NeedBuildString = true;
}

#if WITH_EDITOR
void UTextBlockEx::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
    //UE_LOG(LogUTextBlockEx, Log, TEXT("UTextBlockEx::PostEditChangeProperty"));
    Super::PostEditChangeProperty(PropertyChangedEvent);
    SyncProp();
}
#endif

void UTextBlockEx::OnWidgetRebuilt()
{
    //UE_LOG(LogUTextBlockEx, Log, TEXT("UTextBlockEx::OnWidgetRebuilt"));
    Super::OnWidgetRebuilt();
    InitWidgetPtr();
    SyncProp();
}

void UTextBlockEx::InitWidgetPtr()
{
    TextBlockMain = Cast<UTextBlock>(GetWidgetFromName(FName(TEXT("TextBlockMain"))));
    ensure(TextBlockMain.IsValid());
}

void UTextBlockEx::SyncProp()
{
    NeedBuildString = true;
}

void UTextBlockEx::SetString(FString InText)
{
    Content = InText;
    if (!Ticked) {
        NeedBuildString = true;
        return;
    }
    BuildString();
}

FString UTextBlockEx::GetString() const
{
    return Content;
}

void UTextBlockEx::BuildString()
{
    if (!TextBlockMain.IsValid())
        return;

    FVector2D lsz = GetCachedGeometry().GetLocalSize();
    if (lsz.Equals(FVector2D::ZeroVector, 0.0001f)) {
        ViewSize = lsz;
        return;
    }
    const float lsz_Add_Y = 1.f;

    const FString Ellipsis = TEXT("...");

    //2分遍历
    int32 dir = 1;
    int32 last_dir = 0;
    const int32 len = Content.Len();
    int32 step = len / 2;
    step = step == 0 ? 1 : step;
    int32 num = step;
    int32 last_num = -1;
    bool compute_ellipsis = false;
    for (int32 i = 1;;++i) {
        if (last_num == num) {
            if (num >= Content.Len()) {
                //到尾了
                TextBlockMain->SetText(FText::FromString(Content));
                break;
            }
            else if (num <= 1) {
                //到头了
                TextBlockMain->SetText(FText::FromString(TEXT("")));
                break;
            }
            else {
                num += dir;
            }
        }
        FString tmp2 = Content.Mid(0, num);
        TextBlockMain->SetText(FText::FromString(tmp2));
        ForceLayoutPrepass();
        FVector2D sz = GetDesiredSize();
        dir = (sz.X > lsz.X || sz.Y > lsz.Y + lsz_Add_Y) ? -1 : 1;
        if (last_dir != 0 && last_dir != dir && step == 1) {
            //变方向了，结束循环
            compute_ellipsis = true;
            break;
        }
        last_num = num;
        last_dir = dir;
        step = (step / 2 <= 0 ? 1 : step / 2);
        num += dir * step;
        num = FMath::Clamp(num, 1, len);
        //UE_LOG(LogUTextBlockEx, Log, TEXT("UTextBlockEx::SetText %d x=%f y=%f lx=%f ly=%f"), i, sz.X, sz.Y, lsz.X, lsz.Y);
    }
    if (compute_ellipsis) {
        bool done = false;
        for (int32 i = num; i >= 0; --i) {
            FString tmp2 = Content.Mid(0, i);
            tmp2.Append(Ellipsis);
            TextBlockMain->SetText(FText::FromString(tmp2));
            ForceLayoutPrepass();
            FVector2D sz = GetDesiredSize();
            if (sz.X <= lsz.X && sz.Y <= lsz.Y + lsz_Add_Y) {
                done = true;
                break;
            }
        }
        if (!done) {
            TextBlockMain->SetText(FText::FromString(TEXT("")));
        }
    }
}

