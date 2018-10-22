// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Engine/Engine.h"
#include "Engine/GameInstance.h"
#include "ExampleGameInstance.h"
#include "GameHelper.generated.h"

/**
 * 
 */
UCLASS()
class EXAMPLE_UE4_API UGameHelper : public UObject
{
	GENERATED_BODY()
	
public:

	UFUNCTION(BlueprintCallable)
	static UWorld* GetMyWorld();

	UFUNCTION(BlueprintCallable)
	static UExampleGameInstance* GetGameInstance();
	
	
};
