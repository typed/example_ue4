// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "UObject/Class.h"
#include "Blueprint/UserWidget.h"
#include "UtilScript.generated.h"

/**
 * 
 */
UCLASS()
class EXAMPLE_UE4_API UUtilScript : public UObject
{
	GENERATED_BODY()
	
public:

	UFUNCTION(BlueprintCallable)
	static UUserWidget* CreateUserWidget(FString name);
	
	UFUNCTION(BlueprintCallable)
	static void GC();

    UFUNCTION(BlueprintCallable)
    static UClass* LoadBpClass(FString name);

    UFUNCTION(BlueprintCallable)
    static UObject* LoadRes(FString name);

	UFUNCTION(BlueprintCallable)
	static void TraceAllObject();

	UFUNCTION(BlueprintCallable)
	static void TestShowUserWidget(FString name, int idx);

    UFUNCTION(BlueprintCallable)
    static void TraceClass(FString name);
	
};