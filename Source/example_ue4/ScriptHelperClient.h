// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "UObject/Class.h"
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
	static UUserWidget* CreateUserWidget(FString name);
	
	UFUNCTION(BlueprintCallable)
	static void GC();

	UFUNCTION(BlueprintCallable)
	static UClass* LoadClass(FString name);

	UFUNCTION(BlueprintCallable)
	static void TraceAllObject();

	UFUNCTION(BlueprintCallable)
	static void TestShowUserWidget(FString name, int idx);
	
};

DECLARE_LOG_CATEGORY_EXTERN(LogScriptHelper, Verbose, All);