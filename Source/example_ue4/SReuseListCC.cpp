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
            SNew(SGridPanel)
            .FillColumn(0, 0.5f)
            .FillColumn(1, 0.5f)
            // SBorder
            + SGridPanel::Slot(0, 0)
            [
                SNew(STextBlock)
                .Text(FText::FromString(TEXT("aaaaaa")))
            ]
            + SGridPanel::Slot(0, 1)
            [
                SNew(STextBlock)
                .Text(FText::FromString(TEXT("bbbbbb")))
            ]
        ]
    ];
}

void SReuseListCC::Tick(const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime)
{

}
