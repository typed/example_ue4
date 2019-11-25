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

void SRadarChart::SetPosOffset(const TArray<FVector2D>& __PosOffset)
{
    PosOffset = __PosOffset;
}

FVector2D SRadarChart::GetPosOffset(int32 __idx) const
{
    if (PosOffset.IsValidIndex(__idx)) {
        return PosOffset[__idx];
    }
    return FVector2D::ZeroVector;
}

void SRadarChart::SetPosColor(const TArray<FColor>& __PosColor)
{
    PosColor = __PosColor;
}

FColor SRadarChart::GetPosColor(int32 __idx) const
{
    if (PosColor.IsValidIndex(__idx)) {
        return PosColor[__idx];
    }
    return FColor::White;
}

void SRadarChart::SetBrush(const FSlateBrush& __Brush)
{
    Brush = __Brush;
}

void SRadarChart::SetProgress(int32 i, float prg)
{
    Progress.FindOrAdd(i) = FMath::Clamp(prg, MinProgress, 1.f);
}

float SRadarChart::GetProgress(int32 i) const
{
    auto pf = Progress.Find(i);
    return pf ? *pf : MinProgress;
}

void SRadarChart::ResetProgress()
{
    Progress.Empty();
}

int32 SRadarChart::OnPaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry,
    const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements,
    int32 LayerId, const FWidgetStyle& InWidgetStyle, bool bParentEnabled) const
{
    const FVector2D& LocalSz = AllottedGeometry.GetLocalSize();

    TArray<FSlateVertex> av;
    TArray<SlateIndex> ai;

    //why add Reserve here. https://www.unrealengine.com/zh-CN/blog/optimizing-tarray-usage-for-performance
    av.Reserve(SideCount * 2 + 1);
    ai.Reserve(SideCount * 3 * 3);

    const FSlateRenderTransform& transform = AllottedGeometry.ToPaintGeometry().GetAccumulatedRenderTransform();
    float WedgeAngle = (2.f * PI) / SideCount;
    FVector2D PtCenter = LocalSz / 2.f;
    float radius = FMath::Min(PtCenter.X, PtCenter.Y);
    float diameter = radius * 2.f;
    FSlateVertex v;
    FVector2D vec;
    TArray<FVector2D> vec_s;
    vec_s.Reserve(SideCount);
    FVector2D TexCoords(0.5f, 0.5f);
    v = FSlateVertex::Make<ESlateVertexRounding::Disabled>(transform, PtCenter, TexCoords, GetPosColor(0));
    av.Add(v);
    vec_s.Add(vec);
    for (int32 i = 0; i < SideCount; i++) {
        float Theta = i * WedgeAngle;
        float tmp = radius * GetProgress(i);
        FVector2D pt_offset = GetPosOffset(i);
        vec.X = PtCenter.X + tmp * FMath::Cos(Theta) + pt_offset.X;
        vec.Y = PtCenter.Y - tmp * FMath::Sin(Theta) + pt_offset.Y;
        TexCoords.X = 0.5f + (vec.X - PtCenter.X) / diameter;
        TexCoords.Y = 0.5f + (vec.Y - PtCenter.Y) / diameter;
        v = FSlateVertex::Make<ESlateVertexRounding::Disabled>(transform, vec, TexCoords, GetPosColor(i + 1));
        av.Add(v);
        vec_s.Add(vec);
    }
    for (int32 i = 1; i <= SideCount; i++) {
        int32 next = i == SideCount ? 1 : i + 1;
        ai.Add(0);
        ai.Add(i);
        ai.Add(next);
    }

    if (Antialias > 0.f) {
        for (int32 i = 0; i < SideCount; i++) {
            FColor BdCol = GetPosColor(i + 1);
            BdCol.A = 0.f;
            int32 idxA = i;
            if (idxA <= 0) {
                idxA = SideCount;
            }
            int32 idxB = i + 2;
            if (idxB >= SideCount) {
                idxB = 1;
            }
            const FVector2D& svA = vec_s[idxA];
            const FVector2D& svO = vec_s[i + 1];
            const FVector2D& svB = vec_s[idxB];
            FVector2D vA = svA - svO;
            vA.Normalize();
            FVector2D vB = svB - svO;
            vB.Normalize();
            FVector2D vC = -(vA + vB);
            vC.Normalize();
            FVector2D pt_offset = GetPosOffset(i);
            vec = svO + vC * Antialias + pt_offset;
            TexCoords.X = 0.5f + (vec.X - PtCenter.X) / diameter;
            TexCoords.Y = 0.5f + (vec.Y - PtCenter.Y) / diameter;
            v = FSlateVertex::Make<ESlateVertexRounding::Disabled>(transform, vec, TexCoords, BdCol);
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