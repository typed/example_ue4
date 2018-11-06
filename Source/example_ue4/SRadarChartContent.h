// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Widgets/SCompoundWidget.h"
#include "Rendering/RenderingCommon.h"
#include "UserWidget.h"
/**
* 雷达图的实体
*/
class USlateBrushAsset;
struct EXAMPLE_UE4_API FSTRadarChartData
{
	bool IsAdjustData;
	TArray<FSlateVertex> RadarVerts;
	TArray<SlateIndex> RadarIndexes;
	void Reset()
	{
		IsAdjustData = false;
		RadarVerts.Empty();
		RadarIndexes.Empty();
	}
	FSTRadarChartData()
	{
		Reset();
	}

	~FSTRadarChartData()
	{
		Reset();
	}
};

class EXAMPLE_UE4_API SRadarChartContent : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SRadarChartContent)
	{}
	SLATE_END_ARGS()

	/** Constructs this widget with InArgs */
	void Construct(const FArguments& InArgs);
	void Destruct();
	~SRadarChartContent();
	void InitChartData(USlateBrushAsset* Brush, const TArray<FSlateVertex>& InVerts, const TArray<SlateIndex>& InIndexes);
	virtual int32 OnPaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyClippingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& InWidgetStyle, bool bParentEnabled) const override;
	void OnHide();
private:
	USlateBrushAsset* RadarBrush;
	FSTRadarChartData* pChartData;
	TArray<FSlateVertex> OriginVertex;
	bool IsHide;
};
