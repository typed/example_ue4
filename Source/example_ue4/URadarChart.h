// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Runtime/UMG/Public/Components/Widget.h"
#include "Runtime/SlateCore/Public/Styling/SlateBrush.h"
#include "Runtime/SlateCore/Public/Rendering/RenderingCommon.h"
#include "Runtime/Engine/Classes/Slate/SlateBrushAsset.h"
#include "URadarChart.generated.h"

class SRadarChart;
/**
 * 
 */
UCLASS()
class EXAMPLE_UE4_API URadarChart : public UWidget
{
	GENERATED_BODY()
	
public:

    URadarChart(const FObjectInitializer& ObjectInitializer);

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Appearance)
    int32 SideCount;
    UPROPERTY()
    FGetInt32 SideCountDelegate;
    PROPERTY_BINDING_IMPLEMENTATION(int32, SideCount);
    UFUNCTION(BlueprintCallable, Category = Appearance)
    void SetSideCount(int32 __SideCount);

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Appearance)
    float Antialias;
    UPROPERTY()
    FGetFloat AntialiasDelegate;
    PROPERTY_BINDING_IMPLEMENTATION(float, Antialias);
    UFUNCTION(BlueprintCallable, Category = Appearance)
    void SetAntialias(float __Antialias);

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Appearance)
    FSlateBrush Brush;
    UPROPERTY()
    FGetSlateBrush BrushDelegate;
    PROPERTY_BINDING_IMPLEMENTATION(FSlateBrush, Brush);
    UFUNCTION(BlueprintCallable, Category = Appearance)
    void SetBrush(const FSlateBrush& __Brush);

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Appearance)
    float MinProgress;
    UPROPERTY()
    FGetFloat MinProgressDelegate;
    PROPERTY_BINDING_IMPLEMENTATION(float, MinProgress);
    UFUNCTION(BlueprintCallable, Category = Appearance)
    void SetMinProgress(float __MinProgress);

#if WITH_EDITOR
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Appearance)
    int32 TestProgress;
    UPROPERTY()
    FGetInt32 TestProgressDelegate;
    PROPERTY_BINDING_IMPLEMENTATION(int32, TestProgress);
#endif
    

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

    TSharedPtr<SRadarChart> MyRadarChart;
	
};
