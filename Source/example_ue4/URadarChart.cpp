// Fill out your copyright notice in the Description page of Project Settings.

#include "URadarChart.h"
#include "SRadarChart.h"

URadarChart::URadarChart(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer)
{
    SideCount = 3;
    Antialias = 2;
    MinProgress = 0.1f;
    Visibility = ESlateVisibility::HitTestInvisible;
#if WITH_EDITOR
    TestProgress = FText::FromString(TEXT("9999999999999999999999"));
#endif
}

void URadarChart::SetSideCount(int32 __SideCount)
{
    SideCount = __SideCount;
    if (MyRadarChart.IsValid()) {
        MyRadarChart->SetSideCount(SideCount);
    }
}

void URadarChart::SetAntialias(float __Antialias)
{
    Antialias = __Antialias;
    if (MyRadarChart.IsValid()) {
        MyRadarChart->SetAntialias(Antialias);
    }
}

void URadarChart::SetBrush(const FSlateBrush& __Brush)
{
    Brush = __Brush;
    if (MyRadarChart.IsValid()) {
        MyRadarChart->SetBrush(Brush);
    }
}

void URadarChart::SetMinProgress(float __MinProgress)
{
    MinProgress = __MinProgress;
    if (MyRadarChart.IsValid()) {
        MyRadarChart->SetMinProgress(MinProgress);
    }
}

void URadarChart::SetProgress(int32 i, float __Progress)
{
    if (MyRadarChart.IsValid()) {
        MyRadarChart->SetProgress(i, __Progress);
    }
}

float URadarChart::GetProgress(int32 i) const
{
    if (MyRadarChart.IsValid()) {
        return MyRadarChart->GetProgress(i);
    }
    else {
        return MinProgress;
    }
}

void URadarChart::ResetProgress()
{
    if (MyRadarChart.IsValid()) {
        MyRadarChart->ResetProgress();
    }
}

void URadarChart::SynchronizeProperties()
{
    Super::SynchronizeProperties();

    TAttribute<int32> SideCountBinding = PROPERTY_BINDING(int32, SideCount);
    TAttribute<float> AntialiasBinding = PROPERTY_BINDING(float, Antialias);
    TAttribute<FSlateBrush> BrushBinding = PROPERTY_BINDING(FSlateBrush, Brush);
    TAttribute<float> MinProgressBinding = PROPERTY_BINDING(float, MinProgress);

#if WITH_EDITOR
    TAttribute<FText> TestProgressBinding = PROPERTY_BINDING(FText, TestProgress);
#endif

    if (MyRadarChart.IsValid()) {
        MyRadarChart->SetSideCount(SideCountBinding.Get());
        MyRadarChart->SetAntialias(AntialiasBinding.Get());
        MyRadarChart->SetBrush(BrushBinding.Get());
        MyRadarChart->SetMinProgress(MinProgressBinding.Get());

#if WITH_EDITOR
        MyRadarChart->ResetProgress();
        FString str = TestProgressBinding.Get().ToString();
        str.ReverseString();
        for (int32 i = 0; i < str.Len(); i++) {
            MyRadarChart->SetProgress(i, (str[i] - 48) / 9.f);
        }
#endif
        
    }
}

void URadarChart::ReleaseSlateResources(bool bReleaseChildren)
{
    Super::ReleaseSlateResources(bReleaseChildren);

    MyRadarChart.Reset();
}

TSharedRef<SWidget> URadarChart::RebuildWidget()
{
    MyRadarChart = SNew(SRadarChart);
    return MyRadarChart.ToSharedRef();
}