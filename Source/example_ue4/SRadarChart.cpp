// Fill out your copyright notice in the Description page of Project Settings.

#include "SRadarChart.h"

SRadarChart::SRadarChart()
{
    SideCount = 3;
    Antialias = 2.f;
    MinProgress = 0.1f;
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

void SRadarChart::SetAntialias(float __Antialias)
{
    Antialias = __Antialias;
}

void SRadarChart::SetMinProgress(float __MinProgress)
{
    MinProgress = __MinProgress;
}

void SRadarChart::SetBrush(const FSlateBrush& __Brush)
{
    Brush = __Brush;
}

void SRadarChart::SetProgress(int32 i, float prg)
{
    Progress.FindOrAdd(i) = FMath::Clamp(prg, 0.f, 1.f);
}

void SRadarChart::ResetProgress()
{
    Progress.Empty();
}

#define ComputeSlateVertex(ant, col) \
    float tmp; \
    float Theta = i * WedgeAngle; \
    auto pf = Progress.Find(i); \
    if (pf) \
        tmp = radius * (*pf); \
    else \
        tmp = radius; \
    tmp += ant; \
    vec.X = PtCenter.X + tmp * FMath::Cos(Theta); \
    vec.Y = PtCenter.Y - tmp * FMath::Sin(Theta); \
    TexCoords.X = 0.5f + (vec.X - PtCenter.X) / diameter; \
    TexCoords.Y = 0.5f + (vec.Y - PtCenter.Y) / diameter; \
    v = FSlateVertex::Make<ESlateVertexRounding::Disabled>(transform, vec, TexCoords, col);

int32 SRadarChart::OnPaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& InWidgetStyle, bool bParentEnabled) const
{
    const FVector2D& LocalSz = AllottedGeometry.GetLocalSize();

    TArray<FSlateVertex> av;
    TArray<SlateIndex> ai;

    //why add Reserve here. https://www.unrealengine.com/zh-CN/blog/optimizing-tarray-usage-for-performance
    av.Reserve(SideCount * 2 + 1);
    ai.Reserve(SideCount * 3 * 3);

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
    FVector2D vec;
    for (int32 i = 0; i < SideCount; i++) {
        ComputeSlateVertex(0.f, FColor::White)
        av.Add(v);
    }
    for (int32 i = 1; i <= SideCount; i++) {
        int32 next = i == SideCount ? 1 : i + 1;
        ai.Add(0);
        ai.Add(i);
        ai.Add(next);
    }

    if (Antialias > 0.f) {
        FColor BdCol = FColor::White;
        BdCol.A = 0.f;
        for (int32 i = 0; i < SideCount; i++) {
            ComputeSlateVertex(Antialias, BdCol)
            av.Add(v);
        }
        int32 next, next_border;
        for (int32 i = 1; i <= SideCount; i++) {
            if (i == SideCount) {
                next = 1;
                next_border = SideCount + 1;
            }
            else {
                next = i + 1;
                next_border = SideCount + i + 1;
            }
            ai.Add(i);
            ai.Add(SideCount + i);
            ai.Add(next);
            ai.Add(next);
            ai.Add(SideCount + i);
            ai.Add(next_border);
        }
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
    return LayerId;
}

FVector2D SRadarChart::ComputeDesiredSize(float) const
{
    return Brush.ImageSize;
}