// Fill out your copyright notice in the Description page of Project Settings.

#include "SRadarChartUserWidget.h"
#include "SlateOptMacros.h"
#include "SRadarChartContent.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
//#include "TimerManager.h"

USRadarChartUserWidget::USRadarChartUserWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer), ChartContent(nullptr), CenterPointImg(nullptr), RadarBrush(nullptr)
{
}

TSharedRef<SWidget> USRadarChartUserWidget::RebuildWidget()
{
	if (!ChartContent.IsValid())
	{
		ChartContent = SNew(SRadarChartContent);
	}
	//FreshChartDataToContent(); // 这里还获取不到反射数据，需要延后手动刷新
	return ChartContent.ToSharedRef();
}

void USRadarChartUserWidget::ReleaseSlateResources(bool bReleaseChildren)
{
	Super::ReleaseSlateResources(bReleaseChildren);

	ChartContent.Reset();
}

void USRadarChartUserWidget::FreshChartDataToContent()
{
	if (!ChartContent.IsValid())
	{
		RebuildWidget();
		if (!ChartContent.IsValid())
		{
			return;
		}
	}

	//if (DelayFreshToContent)
	//{
	//	// 下一帧刷新
	//	GetWorld()->GetTimerManager().SetTimer(NextFrameTimerHandle, this, &USRadarChartUserWidget::FreshChartDataToContentDetail, 0.01f, false);
	//}
	//else
	//{
		FreshChartDataToContentDetail();
	//}
}

void USRadarChartUserWidget::FreshChartDataToContentDetail()
{
	if (VertexColor.Num() == 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("USRadarChartUserWidget::FreshChartDataToContent VertexColor Num:%d"), VertexColor.Num());
		return;
	}
	if (VertexColor.Num() != VertexFarPointImg.Num() || VertexColor.Num() != VertexScale.Num() )//|| VertexColor.Num() != VertexUV.Num())
	{
		UE_LOG(LogTemp, Warning, TEXT("USRadarChartUserWidget::FreshChartDataToContent VertexColor.Num() != VertexFarPointImg.Num() || VertexColor.Num() != VertexScale.Num()"));
		return;
	}

	TArray<FSlateVertex> RadarVerts;
	TArray<SlateIndex> RadarIndexes;

	RadarVerts.Empty();
	RadarIndexes.Empty();

	int32 TriangeNum = (VertexColor.Num());
	if (TriangeNum <= 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("USRadarChartUserWidget::FreshChartDataToContent TriangeNum:%d"), TriangeNum);
		return;
	}

	// 根据配置的顶点挂点图片 获取顶点位置
	RadarVerts.Reserve(VertexColor.Num() + 1);
	
	// 中心点的颜色
	FColor CenterColor = FColor(0, 0, 0, 0);
	float ColorScaleAll = 0.0f;
	for (int si = 0; si < VertexScale.Num(); si++)
	{
		float ValidateScale = FMath::Clamp(VertexScale[si], 0.0f, 1.0f);
		ColorScaleAll += (1.0f - ValidateScale);
	}
	
	float AVGScale = 1.0f / VertexColor.Num();;
	for (int ci = 0; ci < VertexColor.Num(); ci++)
	{
		float CurColorScale = 1.0f;
		if (ci < VertexScale.Num())
		{
			float ValidateScale = FMath::Clamp(VertexScale[ci], 0.0f, 1.0f);
			CurColorScale = 1.0f - ValidateScale;
		}
		if (ColorScaleAll > 0.000001f)
		{
			CurColorScale = CurColorScale / ColorScaleAll;
		}
		else
		{
			CurColorScale = AVGScale;
		}
		
		CenterColor = FColor(uint8(CenterColor.R + VertexColor[ci].R *CurColorScale), uint8(CenterColor.G + VertexColor[ci].G *CurColorScale),
			uint8(CenterColor.B + VertexColor[ci].B *CurColorScale), uint8(CenterColor.A + VertexColor[ci].A *CurColorScale));
	}
	RadarVerts.Add(GetVertexData(CenterColor, VertexFarPointImg[0], FVector2D::ZeroVector, 0));

	for (int i = 0; i < VertexColor.Num(); i++)
	{
		FVector2D UVValue = FVector2D::ZeroVector;
		if (VertexUV.Num() > i)
		{
			UVValue = VertexUV[i];
		}

		float scale = VertexScale[i];
		// 先放在这里，不好判定哪里初始化scale
		if (scale < 0.01f)
			scale = 0.01f;

		FSlateVertex VertexData = GetVertexData(VertexColor[i], VertexFarPointImg[i], UVValue, scale);
		RadarVerts.Add(VertexData);
	}

	// 绘制多边形的顶点索引位置，按照左手定则，从上方开始顺时针布点
	int32 IndexNum = TriangeNum * 3;
	RadarIndexes.Reserve(IndexNum);
	int32 StartIndex = 0;
	int32 IterateIndex = 0;
	for (int t = 0; t < TriangeNum; t++)
	{
		IterateIndex = t + 1;
		RadarIndexes.Add((SlateIndex)StartIndex);
		RadarIndexes.Add((SlateIndex)IterateIndex);

		SlateIndex siblingIndex = (IterateIndex + 1) % TriangeNum;
		if (0 == siblingIndex)
			siblingIndex = TriangeNum;

		RadarIndexes.Add(siblingIndex);
	}

	ChartContent->InitChartData(RadarBrush, RadarVerts, RadarIndexes);

	/*if (DelayFreshToContent)
	{
		GetWorld()->GetTimerManager().ClearTimer(NextFrameTimerHandle);
	}*/
}

FSlateVertex USRadarChartUserWidget::GetVertexData(FColor VColor, UImage* VImage, FVector2D Vuv, float VScale)
{
	FSlateVertex result;
	result.TexCoords[0] = Vuv.X;
	result.TexCoords[1] = Vuv.Y;
	result.TexCoords[2] = 1.0f;
	result.TexCoords[3] = 1.0f;
	result.MaterialTexCoords = FVector2D::ZeroVector;
	result.Position = GetVertexPosition(VImage, VScale);
	result.Color = VColor;
	result.PixelSize[0] = 0.0f;
	result.PixelSize[1] = 0.0f;

	return result;
}

FVector2D USRadarChartUserWidget::GetVertexPosition(UImage* VImage, float VScale)
{
	if (VImage == nullptr || CenterPointImg == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("USRadarChartUserWidget::GetVertexPosition ertexBP.FarPointImg == nullptr || CenterPointImg == nullptr"));
		return FVector2D::ZeroVector;
	}

	UCanvasPanelSlot* FarPanelSlot = Cast<UCanvasPanelSlot>(VImage->Slot);
	UCanvasPanelSlot* CenterPanelSlot = Cast<UCanvasPanelSlot>(CenterPointImg->Slot);
	if (FarPanelSlot == nullptr || CenterPanelSlot == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("USRadarChartUserWidget::GetVertexPosition FarPanelSlot == nullptr || CenterPanelSlot == nullptr"));
		return FVector2D::ZeroVector;
	}

	FVector2D FarPos = FarPanelSlot->GetPosition();
	FVector2D CenterPos = CenterPanelSlot->GetPosition();

	FVector2D Distance = FarPos - CenterPos;
	VScale = FMath::Clamp(VScale, 0.0f, 1.0f);
	FVector2D CurLen = VScale * Distance;
	FVector2D FindPosition = CenterPos + CurLen;
	return FindPosition;
}

void USRadarChartUserWidget::OnHide()
{
	if (ChartContent.IsValid())
	{
		ChartContent->OnHide();
	}
}

