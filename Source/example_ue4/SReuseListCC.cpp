// Fill out your copyright notice in the Description page of Project Settings.

#include "SReuseListCC.h"
#include "LogDefine.h"

SReuseListCC::SReuseListCC()
{
}

void SReuseListCC::Construct(const FArguments& InArgs)
{
    ChildSlot
    [
        SNew(SScrollBox)
        + SScrollBox::Slot()
        [
            SNew(STextBlock)
            .Text(FText::FromString(TEXT("aaaaaa")))
        ]
    ];
}

int32 SReuseListCC::OnPaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& InWidgetStyle, bool bParentEnabled) const
{
    return LayerId;
}

FVector2D SReuseListCC::ComputeDesiredSize(float) const
{
    FVector2D sz;
    return sz;
}
