// Fill out your copyright notice in the Description page of Project Settings.

#include "ShapeWidget.h"

UShapeWidget::UShapeWidget(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer)
    , m_bDrawTest(true)
{

}

void UShapeWidget::SetDrawTest(bool bDrawTest)
{
    m_bDrawTest = bDrawTest;
}

void UShapeWidget::NativeConstruct()
{
    m_sbTestBox.TintColor = FSlateColor(FLinearColor::Red);
}

void UShapeWidget::NativePaint(FPaintContext& InContext) const
{
    Super::NativePaint(InContext);
    if (m_bDrawTest) {
        FVector2D LocalSz = GetCachedGeometry().GetLocalSize();
        FVector2D pos(0, 0);
        FVector2D sz(200, 200);
        DrawBox(InContext, pos, sz, (FSlateBrush*)&m_sbTestBox, m_sbTestBox.TintColor.GetSpecifiedColor());
    }

}

void UShapeWidget::DrawBox(FPaintContext& InContext, const FVector2D& Position, const FVector2D& Size, FSlateBrush* Brush, const FLinearColor& Tint)
{
    InContext.MaxLayer++;
    FSlateDrawElement::MakeBox(
        InContext.OutDrawElements,
        InContext.MaxLayer,
        InContext.AllottedGeometry.ToPaintGeometry(Position, Size),
        Brush,
        ESlateDrawEffect::None,
        Tint);
}