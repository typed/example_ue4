// Fill out your copyright notice in the Description page of Project Settings.

#include "SCheckBoxEx.h"
#include "Runtime/Slate/Public/Widgets/Input/SCheckBox.h"
#include "Runtime/Slate/Public/Widgets/Text/STextBlock.h"
#include "Runtime/Slate/Public/Widgets/Layout/SConstraintCanvas.h"

void SCheckBoxEx::Construct(const FArguments& InArgs)
{
    FAnchors Anchors(0.5, 0.5, 0.5, 0.5);
    FVector2D Align(0.5, 0.5);
    FMargin Mar(0, 0, 100, 20);
    FString StrText = TEXT("这是CheckBoxEx");
    ChildSlot
    [
        SNew(SConstraintCanvas)
        + SConstraintCanvas::Slot()
        .Anchors(Anchors)
        .Offset(Mar)
        .Alignment(Align)
        [
            SNew(STextBlock)
            .Text(FText::FromString(StrText))
        ]
        + SConstraintCanvas::Slot()
        .Anchors(Anchors)
        .Offset(Mar)
        .Alignment(Align)
        [
            SNew(SCheckBox)
        ]
    ];
}