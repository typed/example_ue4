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

    virtual void Tick(const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime);

protected:

};