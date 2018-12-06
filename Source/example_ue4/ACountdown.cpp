// Fill out your copyright notice in the Description page of Project Settings.

#include "ACountdown.h"
#include "Runtime/Engine/Classes/Components/TextRenderComponent.h"


// 设置默认值
ACountdown::ACountdown()
{
    // 将此 actor 设为每帧调用 Tick()。不需要时可将此关闭，以提高性能。
    PrimaryActorTick.bCanEverTick = false;

    CountdownText = CreateDefaultSubobject<UTextRenderComponent>(TEXT("CountdownNumber"));
    CountdownText->SetHorizontalAlignment(EHTA_Center);
    CountdownText->SetWorldSize(150.0f);
    RootComponent = CountdownText;

    CountdownTime = 3;
}

// 游戏开始时或生成时调用
void ACountdown::BeginPlay()
{
    Super::BeginPlay();

    UpdateTimerDisplay();
    GetWorldTimerManager().SetTimer(CountdownTimerHandle, this, &ACountdown::AdvanceTimer, 1.0f, true);
}

// 每帧调用
void ACountdown::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

}

void ACountdown::UpdateTimerDisplay()
{
    CountdownText->SetText(FText::AsNumber(FMath::Max(CountdownTime, 0)));
}

void ACountdown::AdvanceTimer()
{
    --CountdownTime;
    UpdateTimerDisplay();
    if (CountdownTime < 1)
    {
        // 倒计时结束，停止运行定时器。
        GetWorldTimerManager().ClearTimer(CountdownTimerHandle);
        //在定时器结束时按需要执行特殊操作。
        CountdownHasFinished();
    }
}

void ACountdown::CountdownHasFinished()
{
    //改为一个特殊的读出
    CountdownText->SetText(FText::FromString(TEXT("GO!")));
}

