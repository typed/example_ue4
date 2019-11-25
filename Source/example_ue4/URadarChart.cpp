// Fill out your copyright notice in the Description page of Project Settings.

#include "URadarChart.h"
#include "SRadarChart.h"

URadarChart::URadarChart(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer)
{
    SideCount = 3;
    Antialias = 2.f;
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

    if (MyRadarChart.IsValid()) {
        MyRadarChart->SetSideCount(SideCount);
        MyRadarChart->SetAntialias(Antialias);
        MyRadarChart->SetBrush(Brush);
        MyRadarChart->SetMinProgress(MinProgress);
        MyRadarChart->SetPosOffset(PosOffset);
        MyRadarChart->SetPosColor(PosColor);

#if WITH_EDITOR
        auto wld = GetWorld();
        if (wld && !wld->IsGameWorld()) {
            MyRadarChart->ResetProgress();
            for (int32 i = 0; i < SideCount; i++) {
                if (TestProgress.IsValidIndex(i)) {
                    MyRadarChart->SetProgress(i, TestProgress[i]);
                }
                else {
                    MyRadarChart->SetProgress(i, 1.f);
                }
            }
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