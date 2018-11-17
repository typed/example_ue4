// Fill out your copyright notice in the Description page of Project Settings.

/**************************************************************************
Author: levingong
Date: 2018-11-15
Description: ∂‡Œ¨ÕºUMG
**************************************************************************/

#pragma once

#include "CoreMinimal.h"
#include "Runtime/UMG/Public/Components/Widget.h"
#include "Runtime/SlateCore/Public/Styling/SlateBrush.h"
#include "Runtime/SlateCore/Public/Rendering/RenderingCommon.h"
#include "Runtime/Engine/Classes/Slate/SlateBrushAsset.h"
#include "URadarChart.generated.h"

/**
 * 
 */
UCLASS()
class EXAMPLE_UE4_API URadarChart : public UWidget
{
	GENERATED_BODY()
	
public:

    URadarChart(const FObjectInitializer& ObjectInitializer);

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Property)
    int32 SideCount;
    UFUNCTION(BlueprintCallable, Category = Appearance)
    void SetSideCount(int32 __SideCount);

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Property)
    float Antialias;
    UFUNCTION(BlueprintCallable, Category = Appearance)
    void SetAntialias(float __Antialias);

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Property)
    FSlateBrush Brush;
    UFUNCTION(BlueprintCallable, Category = Appearance)
    void SetBrush(const FSlateBrush& __Brush);

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Property)
    float MinProgress;
    UFUNCTION(BlueprintCallable, Category = Appearance)
    void SetMinProgress(float __MinProgress);

#if WITH_EDITOR
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Property)
    FText TestProgress;
#endif

    UFUNCTION(BlueprintCallable, Category = Property)
    void SetProgress(int32 i, float __Progress);

    UFUNCTION(BlueprintCallable, Category = Property)
    float GetProgress(int32 i) const;

    UFUNCTION(BlueprintCallable, Category = Property)
    void ResetProgress();

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

    TSharedPtr<class SRadarChart> MyRadarChart;
	
};
