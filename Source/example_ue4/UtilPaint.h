// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
//#include "UObject/NoExportTypes.h"
#include "Runtime/SlateCore/Public/Styling/SlateBrush.h"
#include "Runtime/SlateCore/Public/Rendering/RenderingCommon.h"
#include "Runtime/Engine/Classes/Slate/SlateBrushAsset.h"
#include "Runtime/UMG/Public/Blueprint/UserWidget.h"


/**
 * 
 */
class EXAMPLE_UE4_API UtilPaint
{
public:

    static void DrawBox(FPaintContext& InContext, const FVector2D& Position, const FVector2D& Size, const FSlateBrush& Brush);

    static void DrawVerts(FPaintContext& InContext, const TArray<FSlateVertex>& InVerts, const TArray<SlateIndex>& InIndexes, const FSlateBrush& Brush);

    static void DrawLines(FPaintContext& InContext, const TArray<FVector2D>& Points, FLinearColor Tint, bool bAntiAlias, float Thickness);

};
