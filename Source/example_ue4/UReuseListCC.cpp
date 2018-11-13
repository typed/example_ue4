// Fill out your copyright notice in the Description page of Project Settings.

#include "UReuseListCC.h"
#include "SReuseListCC.h"
#include "Runtime/Engine/Classes/Kismet/KismetMathLibrary.h"
#include "LogDefine.h"

UReuseListCC::UReuseListCC(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer)
{
}

void UReuseListCC::SynchronizeProperties()
{
    Super::SynchronizeProperties();
}

void UReuseListCC::ReleaseSlateResources(bool bReleaseChildren)
{
    Super::ReleaseSlateResources(bReleaseChildren);

    MyReuseListCC.Reset();
}

TSharedRef<SWidget> UReuseListCC::RebuildWidget()
{
    MyReuseListCC = SNew(SReuseListCC);
    return MyReuseListCC.ToSharedRef();
}