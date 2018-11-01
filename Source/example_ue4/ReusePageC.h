// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ReusePageC.generated.h"

/**
 * 
 */
UCLASS()
class EXAMPLE_UE4_API UReusePageC : public UUserWidget
{
	GENERATED_BODY()
	
public:

	UFUNCTION(BlueprintCallable)
	void Init();

protected:

	
};
