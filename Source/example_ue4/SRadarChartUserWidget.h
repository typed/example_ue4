// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Widget.h"
#include "Components/Image.h"
#include "SRadarChartUserWidget.generated.h"
/**
 * 雷达图是自绘制的多边形，写在cpp中作为一个通用组件，方便美术编辑. by samcen
 */
class SRadarChartContent;

UCLASS(meta = (DisplayName = "USRadarChartUserWidget"), ClassGroup = UserInterface)
class EXAMPLE_UE4_API USRadarChartUserWidget : public UWidget
{
	GENERATED_BODY()
public:
	USRadarChartUserWidget(const FObjectInitializer& ObjectInitializer);

	TSharedPtr<SRadarChartContent> ChartContent;
	virtual TSharedRef<SWidget> RebuildWidget() override;
	virtual void ReleaseSlateResources(bool bReleaseChildren) override;

	// 需要手动刷新数据到Content中，一般在BeginPlay中调用；因UWidget中暂时没有找到反射完成的事件函数by samcen
	UFUNCTION(BlueprintCallable)
	void FreshChartDataToContent();

	void FreshChartDataToContentDetail();

	UFUNCTION(BlueprintCallable)
	void OnHide();
public:
	/*UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = RadarChart)
		bool DelayFreshToContent = false;*/

	// 图元画刷
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = RadarChart)
		USlateBrushAsset* RadarBrush;

	// 底图中心的定位小图片
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = RadarChart)
		UImage* CenterPointImg;

	// VertexColor是顶点的颜色
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = RadarChart)
		TArray<FColor> VertexColor;

	// VertexFarPointImg 是顶点最远点的挂载图片
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = RadarChart)
		TArray<UImage*> VertexFarPointImg;

	// VertexScale 是0到1的值
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = RadarChart)
		TArray<float> VertexScale;
	// 顶点UV
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = RadarChart)
		TArray<FVector2D> VertexUV;

	
private:
	FVector2D GetVertexPosition(UImage* VImage, float VScale);
	FSlateVertex GetVertexData(FColor VColor, UImage* VImage, FVector2D Vuv, float VScale);
	//FTimerHandle NextFrameTimerHandle;
};
