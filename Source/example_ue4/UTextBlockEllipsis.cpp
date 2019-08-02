// Fill out your copyright notice in the Description page of Project Settings.

#include "UTextBlockEllipsis.h"
#include "Runtime/UMG/Public/Components/TextBlock.h"
#include "Runtime/UMG/Public/Blueprint/WidgetBlueprintLibrary.h"
#include "Runtime/Engine/Classes/Kismet/KismetMathLibrary.h"

DEFINE_LOG_CATEGORY(LogUTextBlockEllipsis);

UTextBlockEllipsis::UTextBlockEllipsis(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer)
    , NeedBuildString(false)
    , Ticked(false)
{

}

bool UTextBlockEllipsis::Initialize()
{
    //UE_LOG(LogUTextBlockEllipsis, Log, TEXT("UTextBlockEllipsis::Initialize"));
    if (!Super::Initialize())
        return false;
    InitWidgetPtr();
    SyncProp();
#if WITH_EDITOR
    UWorld* wld = GetWorld();
    if (wld && !wld->IsGameWorld()) {
        TWeakObjectPtr<UTextBlockEllipsis> self = this;
        wld->GetTimerManager().SetTimer(OnTickTimerHandle, FTimerDelegate::CreateLambda([=]() {
            if (self.IsValid()) {
                self->OnMyTick();
            }
        }), 0.001f, true);
    }
#endif
    return true;
}

void UTextBlockEllipsis::NativeConstruct()
{
    //UE_LOG(LogUTextBlockEllipsis, Log, TEXT("UTextBlockEllipsis::NativeConstruct"));
    Super::NativeConstruct();
    InitWidgetPtr();
    SyncProp();
}

void UTextBlockEllipsis::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
    Super::NativeTick(MyGeometry, InDeltaTime);
    Ticked = true;
    OnMyTick();
}

void UTextBlockEllipsis::OnMyTick()
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

void UTextBlockEllipsis::ReleaseSlateResources(bool bReleaseChildren)
{
    //UE_LOG(LogUTextBlockEllipsis, Log, TEXT("UTextBlockEllipsis::ReleaseSlateResources"));
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

void UTextBlockEllipsis::SynchronizeProperties()
{
    //UE_LOG(LogUTextBlockEllipsis, Log, TEXT("UTextBlockEllipsis::SynchronizeProperties"));
    Super::SynchronizeProperties();
    NeedBuildString = true;
}

#if WITH_EDITOR
void UTextBlockEllipsis::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
    //UE_LOG(LogUTextBlockEllipsis, Log, TEXT("UTextBlockEllipsis::PostEditChangeProperty"));
    Super::PostEditChangeProperty(PropertyChangedEvent);
    SyncProp();
}
#endif

void UTextBlockEllipsis::OnWidgetRebuilt()
{
    //UE_LOG(LogUTextBlockEllipsis, Log, TEXT("UTextBlockEllipsis::OnWidgetRebuilt"));
    Super::OnWidgetRebuilt();
    InitWidgetPtr();
    SyncProp();
}

void UTextBlockEllipsis::InitWidgetPtr()
{
    TextBlockMain = Cast<UTextBlock>(GetWidgetFromName(FName(TEXT("TextBlockMain"))));
    ensure(TextBlockMain.IsValid());
}

void UTextBlockEllipsis::SyncProp()
{
    NeedBuildString = true;
}

void UTextBlockEllipsis::SetString(FString InText)
{
    Content = InText;
    if (!Ticked) {
        NeedBuildString = true;
        return;
    }
    BuildString();
}

FString UTextBlockEllipsis::GetString() const
{
    return Content;
}

void UTextBlockEllipsis::BuildString()
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
        //UE_LOG(LogUTextBlockEllipsis, Log, TEXT("UTextBlockEllipsis::SetText %d x=%f y=%f lx=%f ly=%f"), i, sz.X, sz.Y, lsz.X, lsz.Y);
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

