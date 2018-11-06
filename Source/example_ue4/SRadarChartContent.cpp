//Fill out your copyright notice in the Description page of Project Settings.
#include "SRadarChartContent.h"
#include "SlateOptMacros.h"
#include "Rendering/DrawElements.h"
#include "Slate/SlateBrushAsset.h"

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION
void SRadarChartContent::Construct(const FArguments& InArgs)
{
	/*
	ChildSlot
	[
		 Populate the widget
	];
	*/
	RadarBrush = nullptr;
	pChartData = new FSTRadarChartData();
	pChartData->RadarVerts.Empty();
	IsHide = false;
}

void SRadarChartContent::Destruct()
{
	IsHide = true;
}

void SRadarChartContent::OnHide()
{
	IsHide = true;
}

SRadarChartContent::~SRadarChartContent()
{
	if (pChartData != nullptr)
	{
		delete pChartData;
		pChartData = nullptr;
	}
	RadarBrush = nullptr;
}

void SRadarChartContent::InitChartData(USlateBrushAsset* Brush, const TArray<FSlateVertex>& InVerts, const TArray<SlateIndex>& InIndexes)
{
	RadarBrush = Brush;
	if (pChartData)
	{
		pChartData->IsAdjustData = false;
		pChartData->RadarVerts = InVerts;
		pChartData->RadarIndexes = InIndexes;
		OriginVertex = InVerts;
		IsHide = false;
	}
}

int32 SRadarChartContent::OnPaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyClippingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& InWidgetStyle, bool bParentEnabled) const
{
	if (RadarBrush == nullptr || pChartData == nullptr)
	{
		return 0;
	}

	if (pChartData->RadarVerts.Num() <= 0)
	{
		return 0;
	}

	//if (!ShouldBeEnabled(true))
	if(IsHide)
	{
		return 0;
	}
	// 使用pChartData指针存放数据，解决重载的OnPaint为常量函数不能修改数据成员
	if (!pChartData->IsAdjustData)
	{
		pChartData->IsAdjustData = true;
		for (auto & V : pChartData->RadarVerts)
		{
			V.Position = AllottedGeometry.ToPaintGeometry().GetAccumulatedRenderTransform().TransformPoint(V.Position);
		}
	}
	
	// 分辨率改变了，进行重新定位位置绘制
	FVector2D VertexOriginAdjustPos = AllottedGeometry.ToPaintGeometry().GetAccumulatedRenderTransform().TransformPoint(OriginVertex[0].Position);
	if ( FMath::Abs( pChartData->RadarVerts[0].Position.X - VertexOriginAdjustPos.X) > 0.00001f
		|| FMath::Abs(pChartData->RadarVerts[0].Position.Y - VertexOriginAdjustPos.Y) > 0.00001f)
	{
		for (int i = 0; i < OriginVertex.Num(); i++)
		{
			pChartData->RadarVerts[i].Position = AllottedGeometry.ToPaintGeometry().GetAccumulatedRenderTransform().TransformPoint(OriginVertex[i].Position);
		}
	}

	//LayerId++;
	FSlateDrawElement::MakeCustomVerts(
		OutDrawElements,
		LayerId,
		FSlateApplication::Get().GetRenderer()->GetResourceHandle(RadarBrush->Brush),
		pChartData->RadarVerts,
		pChartData->RadarIndexes,
		nullptr,
		0,
		0
	);

	// 使用画线函数绘制一遍边缘，以支持抗锯齿
	for (int i = 1; i < OriginVertex.Num(); i++)
	{
		TArray<FVector2D> EdgeLine;
		FVector2D StartPoint = OriginVertex[i].Position;
		EdgeLine.Add(StartPoint);
		FColor StartColor = OriginVertex[i].Color;

		FVector2D EndPoint = OriginVertex[1].Position;
		FColor EndColor = OriginVertex[1].Color;
		if (i + 1 < pChartData->RadarVerts.Num())
		{
			EndPoint = OriginVertex[i + 1].Position;
			EndColor = OriginVertex[i + 1].Color;
		}
		EdgeLine.Add(EndPoint);

		FColor AVGColor = FColor(uint8((StartColor.R + EndColor.R) / 2.0f), uint8((StartColor.G + EndColor.G) / 2.0f), uint8((StartColor.B + EndColor.B) / 2.0f), 185);
		FSlateDrawElement::MakeLines(
			OutDrawElements,
			LayerId,
			AllottedGeometry.ToPaintGeometry(),
			EdgeLine,
			ESlateDrawEffect::None,
			AVGColor,
			true,
			2.0f
		);
	}

	return LayerId;
}

END_SLATE_FUNCTION_BUILD_OPTIMIZATION
