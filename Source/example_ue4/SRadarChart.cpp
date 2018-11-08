// Fill out your copyright notice in the Description page of Project Settings.

#include "SRadarChart.h"

SRadarChart::SRadarChart()
{
    SideCount = 3;
    UseBorder = true;
    BorderThickness = 2.f;
    BorderColor = FLinearColor::White;
    bCanTick = false;
    bCanSupportFocus = false;
}

void SRadarChart::Construct(const FArguments& InArgs)
{

}

void SRadarChart::SetSideCount(int32 __SideCount)
{
    SideCount = __SideCount;
}

void SRadarChart::SetBorderColor(const FLinearColor& __BorderColor)
{
    BorderColor = __BorderColor;
}

void SRadarChart::SetUseBorder(bool __UseBorder)
{
    UseBorder = __UseBorder;
}

void SRadarChart::SetBorderThickness(float __BorderThickness)
{
    BorderThickness = __BorderThickness;
}

void SRadarChart::SetBrush(const FSlateBrush& __Brush)
{
    Brush = __Brush;
}

int32 SRadarChart::OnPaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& InWidgetStyle, bool bParentEnabled) const
{
    const FVector2D& LocalSz = AllottedGeometry.GetLocalSize();

    TArray<FSlateVertex> av;
    TArray<SlateIndex> ai;
    TArray<FVector2D> aBorder;

    //why add Reserve here. https://www.unrealengine.com/zh-CN/blog/optimizing-tarray-usage-for-performance
    av.Reserve(SideCount + 2);
    aBorder.Reserve(SideCount + 1);
    ai.Reserve(SideCount * 3);

    const FSlateRenderTransform& transform = AllottedGeometry.ToPaintGeometry().GetAccumulatedRenderTransform();
    float WedgeAngle = (2.f * PI) / SideCount;
    float radius = FMath::Min(LocalSz.X / 2.f, LocalSz.Y / 2.f);
    float diameter = radius * 2.f;
    FVector2D PtCenter;
    PtCenter.X = LocalSz.X / 2.f;
    PtCenter.Y = LocalSz.Y / 2.f;
    FSlateVertex v;
    FVector2D TexCoords(0.5f, 0.5f);
    v = FSlateVertex::Make<ESlateVertexRounding::Disabled>(transform, PtCenter, TexCoords, FColor::White);
    av.Add(v);
    float Theta;
    FVector2D vec;
    for (int32 i = 0; i < SideCount; i++) {
        Theta = i * WedgeAngle;
        vec.X = PtCenter.X + radius * FMath::Cos(Theta);
        vec.Y = PtCenter.Y - radius * FMath::Sin(Theta);
        TexCoords.X = 0.5f + (vec.X - PtCenter.X) / diameter;
        TexCoords.Y = 0.5f + (vec.Y - PtCenter.Y) / diameter;
        v = FSlateVertex::Make<ESlateVertexRounding::Disabled>(transform, vec, TexCoords, FColor::White);
        av.Add(v);
        aBorder.Add(vec);
    }
    for (int32 i = 1; i <= SideCount; i++) {
        ai.Add(0);
        ai.Add(i);
        ai.Add(i == SideCount ? 1 : i + 1);
    }
    FSlateResourceHandle handle = FSlateApplication::Get().GetRenderer()->GetResourceHandle(Brush);
    if (handle.IsValid()) {
        FSlateDrawElement::MakeCustomVerts(
            OutDrawElements,
            LayerId,
            handle,
            av,
            ai,
            nullptr,
            0,
            0
        );
    }
    if (UseBorder && aBorder.Num() > 0) {
        auto v = aBorder[0];
        aBorder.Add(v);
        FSlateDrawElement::MakeLines(
            OutDrawElements,
            LayerId,
            AllottedGeometry.ToPaintGeometry(),
            aBorder,
            ESlateDrawEffect::None,
            BorderColor,
            true,
            BorderThickness);
    }
    return LayerId;
}

FVector2D SRadarChart::ComputeDesiredSize(float) const
{
    return Brush.ImageSize;
}