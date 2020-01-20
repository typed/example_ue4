// Fill out your copyright notice in the Description page of Project Settings.

#include "UCheckBoxEx.h"
#include "SCheckBoxEx.h"

UCheckBoxEx::UCheckBoxEx(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer)
{

}

void UCheckBoxEx::SynchronizeProperties()
{
    Super::SynchronizeProperties();
}

void UCheckBoxEx::ReleaseSlateResources(bool bReleaseChildren)
{
    Super::ReleaseSlateResources(bReleaseChildren);
}

TSharedRef<SWidget> UCheckBoxEx::RebuildWidget()
{
    MyCheckBoxEx = SNew(SCheckBoxEx);

    return MyCheckBoxEx.ToSharedRef();
}