// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Blueprint/UserWidget.h"
#include "ScriptHelperClient.generated.h"

/**
 * 
 */
UCLASS()
class EXAMPLE_UE4_API UScriptHelperClient : public UObject
{
	GENERATED_BODY()
	
public:

	UFUNCTION(BlueprintCallable)
	static UUserWidget* GetWidget(FString name);
	
	
};
