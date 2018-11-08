// Fill out your copyright notice in the Description page of Project Settings.

#include "UtilPaint.h"

void UtilPaint::DrawBox(FPaintContext& InContext, const FVector2D& Position, const FVector2D& Size, const FSlateBrush& Brush)
{
    InContext.MaxLayer++;
    FSlateDrawElement::MakeBox(
        InContext.OutDrawElements,
        InContext.MaxLayer,
        InContext.AllottedGeometry.ToPaintGeometry(Position, Size),
        &Brush,
        ESlateDrawEffect::None,
        Brush.TintColor.GetSpecifiedColor());
}

void UtilPaint::DrawVerts(FPaintContext& InContext, const TArray<FSlateVertex>& InVerts, const TArray<SlateIndex>& InIndexes, const FSlateBrush& Brush)
{
    FSlateResourceHandle handle = FSlateApplication::Get().GetRenderer()->GetResourceHandle(Brush);
    if (handle.IsValid()) {
        InContext.MaxLayer++;
        FSlateDrawElement::MakeCustomVerts(
            InContext.OutDrawElements,
            InContext.MaxLayer,
            handle,
            InVerts,
            InIndexes,
            nullptr,
            0,
            0
        );
    }
}

void UtilPaint::DrawLines(FPaintContext& InContext, const TArray<FVector2D>& Points, FLinearColor Tint, bool bAntiAlias, float Thickness)
{
    InContext.MaxLayer++;
    FSlateDrawElement::MakeLines(
        InContext.OutDrawElements,
        InContext.MaxLayer,
        InContext.AllottedGeometry.ToPaintGeometry(),
        Points,
        ESlateDrawEffect::None,
        Tint,
        bAntiAlias,
        Thickness);
}
