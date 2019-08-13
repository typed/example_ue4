// Fill out your copyright notice in the Description page of Project Settings.

/**************************************************************************
Author: levingong
Date: 2018-11-15
Description: CheckBox组件
**************************************************************************/

#pragma once

#include "CoreMinimal.h"
#include "Runtime/UMG/Public/Components/CheckBox.h"
#include "UCheckBoxEx.generated.h"

/**
 * 
 */
UCLASS()
class EXAMPLE_UE4_API UCheckBoxEx : public UWidget
{
	GENERATED_BODY()
	
public:

    UCheckBoxEx(const FObjectInitializer& ObjectInitializer);

protected:

    //~ Begin UWidget Interface
    virtual void SynchronizeProperties() override;
    //~ End UWidget Interface

    //~ Begin UVisual Interface
    virtual void ReleaseSlateResources(bool bReleaseChildren) override;
    //~ End UVisual Interface

    //~ Begin UWidget Interface
    virtual TSharedRef<SWidget> RebuildWidget() override;
    //~ End UWidget Interface

    TSharedPtr<class SCheckBoxEx> MyCheckBoxEx;
	
};
