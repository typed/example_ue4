// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Runtime/Slate/Public/Slate.h"

class EXAMPLE_UE4_API SReuseListCC
    : public SCompoundWidget
{
public:

    SLATE_BEGIN_ARGS(SReuseListCC)
    {}
    SLATE_END_ARGS()

    SReuseListCC();

    void Construct(const FArguments& InArgs);

    virtual int32 OnPaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& InWidgetStyle, bool bParentEnabled) const;

protected:

    // Begin SWidget overrides.
    virtual FVector2D ComputeDesiredSize(float) const override;
    // End SWidget overrides.

};