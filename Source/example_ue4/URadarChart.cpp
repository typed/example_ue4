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

void URadarChart::SynchronizeProperties()
{
    Super::SynchronizeProperties();

    TAttribute<int32> SideCountBinding = PROPERTY_BINDING(int32, SideCount);
    TAttribute<float> AntialiasBinding = PROPERTY_BINDING(float, Antialias);
    TAttribute<FSlateBrush> BrushBinding = PROPERTY_BINDING(FSlateBrush, Brush);
    TAttribute<float> MinProgressBinding = PROPERTY_BINDING(float, MinProgress);

#if WITH_EDITOR
    TAttribute<int32> TestProgressBinding = PROPERTY_BINDING(int32, TestProgress);
#endif

    if (MyRadarChart.IsValid()) {
        MyRadarChart->SetSideCount(SideCountBinding.Get());
        MyRadarChart->SetAntialias(AntialiasBinding.Get());
        MyRadarChart->SetBrush(BrushBinding.Get());
        MyRadarChart->SetMinProgress(MinProgressBinding.Get());

#if WITH_EDITOR
        MyRadarChart->ResetProgress();
        int32 n = TestProgressBinding.Get();
        FString str = FString::FromInt(n);
        str.ReverseString();
        n = FCString::Atoi(*str);
        int32 i = 0;
        while (n) {
            MyRadarChart->SetProgress(i++, (n % 10) / 9.f);
            n /= 10;
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