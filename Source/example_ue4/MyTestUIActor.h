// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MyTestUIActor.generated.h"

UCLASS()
class EXAMPLE_UE4_API AMyTestUIActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AMyTestUIActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION()
	void OnClickOpenWidget();

	UFUNCTION()
	void OnClickGC();

	UFUNCTION()
	void OnClickClose();

	TWeakObjectPtr<UUserWidget> m_pUserWidget;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	
	
};
