// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ACountdown.generated.h"

class UTextRenderComponent;

UCLASS()
class EXAMPLE_UE4_API ACountdown : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ACountdown();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

    //倒计时运行时长，按秒计
    int32 CountdownTime;

    UTextRenderComponent* CountdownText;

    void UpdateTimerDisplay();

    void AdvanceTimer();

    void CountdownHasFinished();

    FTimerHandle CountdownTimerHandle;
	
};
