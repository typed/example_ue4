// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Runtime/SlateCore/Public/Styling/SlateBrush.h"
#include "ShapeWidget.generated.h"

/**
 * 
 */
UCLASS()
class EXAMPLE_UE4_API UShapeWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:

    UShapeWidget(const FObjectInitializer& ObjectInitializer);

    UFUNCTION(BlueprintCallable)
    void SetDrawTest(bool bDrawTest);

protected:

    virtual void NativeConstruct();

    virtual void NativePaint(FPaintContext& InContext) const;

private:

    static void DrawBox(FPaintContext& InContext, const FVector2D& Position, const FVector2D& Size, FSlateBrush* Brush, const FLinearColor& Tint = FLinearColor::White);

    bool m_bDrawTest;
    FSlateBrush m_sbTestBox;
	
};
