// Fill out your copyright notice in the Description page of Project Settings.

/**************************************************************************
Author: levingong
Date: 2018-11-15
Description: 多维图Slate
**************************************************************************/

#pragma once

#include "CoreMinimal.h"
#include "Runtime/SlateCore/Public/Widgets/SCompoundWidget.h"

class EXAMPLE_UE4_API SCheckBoxEx : public SCompoundWidget
{
public:
    SLATE_BEGIN_ARGS(SCheckBoxEx)
        {}
    SLATE_END_ARGS()

    void Construct(const FArguments& InArgs);

protected:

};
