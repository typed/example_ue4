// Fill out your copyright notice in the Description page of Project Settings.

#include "URadarChart.h"
#include "SRadarChart.h"

URadarChart::URadarChart(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer)
{
    SideCount = 3;
    UseBorder = true;
    BorderThickness = 2.f;
    BorderColor = FLinearColor::White;
    Visibility = ESlateVisibility::HitTestInvisible;
}

void URadarChart::SetSideCount(int32 __SideCount)
{
    SideCount = __SideCount;
    if (MyRadarChart.IsValid()) {
        MyRadarChart->SetSideCount(SideCount);
    }
}

void URadarChart::SetBorderColor(const FLinearColor& __BorderColor)
{
    BorderColor = __BorderColor;
    if (MyRadarChart.IsValid()) {
        MyRadarChart->SetBorderColor(BorderColor);
    }
}

void URadarChart::SetUseBorder(bool __UseBorder)
{
    UseBorder = __UseBorder;
    if (MyRadarChart.IsValid()) {
        MyRadarChart->SetUseBorder(UseBorder);
    }
}

void URadarChart::SetBorderThickness(float __BorderThickness)
{
    BorderThickness = __BorderThickness;
    if (MyRadarChart.IsValid()) {
        MyRadarChart->SetBorderThickness(BorderThickness);
    }
}

void URadarChart::SetBrush(const FSlateBrush& __Brush)
{
    Brush = __Brush;
    if (MyRadarChart.IsValid()) {
        MyRadarChart->SetBrush(Brush);
    }
}

void URadarChart::SynchronizeProperties()
{
    Super::SynchronizeProperties();

    TAttribute<int32> SideCountBinding = PROPERTY_BINDING(int32, SideCount);
    TAttribute<bool> UseBorderBinding = PROPERTY_BINDING(bool, UseBorder);
    TAttribute<float> BorderThicknessBinding = PROPERTY_BINDING(float, BorderThickness);
    TAttribute<FSlateBrush> BrushBinding = PROPERTY_BINDING(FSlateBrush, Brush);
    TAttribute<FLinearColor> BorderColorBinding = PROPERTY_BINDING(FLinearColor, BorderColor);

    if (MyRadarChart.IsValid()) {
        MyRadarChart->SetSideCount(SideCountBinding.Get());
        MyRadarChart->SetUseBorder(UseBorderBinding.Get());
        MyRadarChart->SetBorderThickness(BorderThicknessBinding.Get());
        MyRadarChart->SetBrush(BrushBinding.Get());
        MyRadarChart->SetBorderColor(BorderColorBinding.Get());
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