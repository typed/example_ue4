// Fill out your copyright notice in the Description page of Project Settings.

#include "UTextBlockEx.h"
#include "Runtime/UMG/Public/Components/TextBlock.h"
#include "Runtime/UMG/Public/Blueprint/WidgetBlueprintLibrary.h"
#include "Runtime/Engine/Classes/Kismet/KismetMathLibrary.h"

DEFINE_LOG_CATEGORY(LogUTextBlockEx);

UTextBlockEx::UTextBlockEx(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer)
    , NeedBuildString(false)
{

}

bool UTextBlockEx::Initialize()
{
    if (!Super::Initialize())
        return false;
    InitWidgetPtr();
    SyncProp();
    return true;
}

void UTextBlockEx::NativeConstruct()
{
    Super::NativeConstruct();
    InitWidgetPtr();
    SyncProp();
}

void UTextBlockEx::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
    Super::NativeTick(MyGeometry, InDeltaTime);
    if (NeedBuildString) {
        NeedBuildString = false;
        BuildString();
    }
}

#if WITH_EDITOR
void UTextBlockEx::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
    Super::PostEditChangeProperty(PropertyChangedEvent);
    SyncProp();
}
#endif

void UTextBlockEx::OnWidgetRebuilt()
{
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
    m_text = InText;
    NeedBuildString = true;
}

FString UTextBlockEx::GetString() const
{
    return m_text;
}

void UTextBlockEx::BuildString()
{
    if (!TextBlockMain.IsValid())
        return;
    TextBlockMain->SetText(FText::FromString(TEXT("...")));
    ForceLayoutPrepass();
    FVector2D sz_dot = GetDesiredSize();
    //...的大小
    //这里最好用2分遍历，减少遍历次数
    for (int32 i = 1; i <= m_text.Len(); i++) {
        FString tmp2 = m_text.Mid(0, i);
        TextBlockMain->SetText(FText::FromString(tmp2));
        ForceLayoutPrepass();
        FVector2D sz = GetDesiredSize();
        FVector2D lsz = GetCachedGeometry().GetLocalSize();
        if (sz.X + sz_dot.X > lsz.X) {
            FString tmp2 = m_text.Mid(0, i - 1);
            tmp2.Append(TEXT("..."));
            TextBlockMain->SetText(FText::FromString(tmp2));
            return;
        }
        if (sz.Y > lsz.Y) {
            FString tmp2 = m_text.Mid(0, i - 1);
            tmp2.Append(TEXT("..."));
            TextBlockMain->SetText(FText::FromString(tmp2));
            ForceLayoutPrepass();
            FVector2D sz = GetDesiredSize();
            FVector2D lsz = GetCachedGeometry().GetLocalSize();
            if (sz.X + sz_dot.X > lsz.X) {
                FString tmp2 = m_text.Mid(0, i - 2);
                tmp2.Append(TEXT("..."));
                TextBlockMain->SetText(FText::FromString(tmp2));
            }
            return;
        }
        UE_LOG(LogUTextBlockEx, Log, TEXT("UTextBlockEx::SetText %d x=%f y=%f lx=%f ly=%f"), i, sz.X, sz.Y, lsz.X, lsz.Y);
    }
    TextBlockMain->SetText(FText::FromString(m_text));
}

