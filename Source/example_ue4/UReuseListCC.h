// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Runtime/UMG/Public/UMG.h"
#include "UReuseListCC.generated.h"

/**
 * 
 */

class SReuseListCC;

UCLASS()
class EXAMPLE_UE4_API UReuseListCC
    : public UWidget
{
	GENERATED_BODY()
	
public:

    UReuseListCC(const FObjectInitializer& ObjectInitializer);

    //~ Begin UWidget Interface
    virtual void SynchronizeProperties() override;
    //~ End UWidget Interface

    //~ Begin UVisual Interface
    virtual void ReleaseSlateResources(bool bReleaseChildren) override;
    //~ End UVisual Interface

protected:

    //~ Begin UWidget Interface
    virtual TSharedRef<SWidget> RebuildWidget() override;
    //~ End UWidget Interface

private:

    TSharedPtr<SReuseListCC> MyReuseListCC;

};