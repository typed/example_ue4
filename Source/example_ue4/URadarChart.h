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

    UFUNCTION(BlueprintCallable, Category = Property)
    void SetSideCount(int32 __SideCount);
    
    UFUNCTION(BlueprintCallable, Category = Property)
    void SetAntialias(float __Antialias);
    
    UFUNCTION(BlueprintCallable, Category = Property)
    void SetBrush(const FSlateBrush& __Brush);

    UFUNCTION(BlueprintCallable, Category = Property)
    void SetMinProgress(float __MinProgress);

    UFUNCTION(BlueprintCallable, Category = Property)
    void SetProgress(int32 i, float __Progress);

    UFUNCTION(BlueprintCallable, Category = Property)
    float GetProgress(int32 i) const;

    UFUNCTION(BlueprintCallable, Category = Property)
    void ResetProgress();

protected:

    UPROPERTY(EditAnywhere, Category = Property, meta = (ClampMin = "3"))
    int32 SideCount;

    UPROPERTY(EditAnywhere, Category = Property, meta = (ClampMin = "0"))
    float Antialias;

    UPROPERTY(EditAnywhere, Category = Property)
    FSlateBrush Brush;

    UPROPERTY(EditAnywhere, Category = Property, meta = (ClampMin = "0"))
    float MinProgress;

    UPROPERTY(EditAnywhere, Category = Property)
    TArray<FVector2D> PosOffset;

    UPROPERTY(EditAnywhere, Category = Property)
    TArray<float> TestProgress;

    //~ Begin UWidget Interface
    virtual void SynchronizeProperties() override;
    //~ End UWidget Interface

    //~ Begin UVisual Interface
    virtual void ReleaseSlateResources(bool bReleaseChildren) override;
    //~ End UVisual Interface

    //~ Begin UWidget Interface
    virtual TSharedRef<SWidget> RebuildWidget() override;
    //~ End UWidget Interface

    TSharedPtr<class SRadarChart> MyRadarChart;
	
};
