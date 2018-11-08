// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Runtime/SlateCore/Public/Styling/SlateBrush.h"
#include "Runtime/SlateCore/Public/Rendering/RenderingCommon.h"
#include "Runtime/Engine/Classes/Slate/SlateBrushAsset.h"

class EXAMPLE_UE4_API SRadarChart
    : public SLeafWidget
{
public:
    SLATE_BEGIN_ARGS(SRadarChart)
        {}
    SLATE_END_ARGS()

    SRadarChart();

    void Construct(const FArguments& InArgs);

    void SetSideCount(int32 __SideCount);

    void SetBorderColor(const FLinearColor& __BorderColor);

    void SetUseBorder(bool __UseBorder);

    void SetBorderThickness(float __BorderThickness);

    void SetBrush(const FSlateBrush& __Brush);

    virtual int32 OnPaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& InWidgetStyle, bool bParentEnabled) const;

protected:

    // Begin SWidget overrides.
    virtual FVector2D ComputeDesiredSize(float) const override;
    // End SWidget overrides.

    int32 SideCount;
    
    FLinearColor BorderColor;
    
    bool UseBorder;
    
    float BorderThickness;
    
    FSlateBrush Brush;

};
