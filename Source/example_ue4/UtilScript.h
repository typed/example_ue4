// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "UtilScript.generated.h"

class UClass;
class UUserWidget;
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
    static UObject* FindRes(FString name);

	UFUNCTION(BlueprintCallable)
	static void TraceAllObject();

    UFUNCTION(BlueprintCallable)
    static void TraceAllUserWidget();

    UFUNCTION(BlueprintCallable)
    static void TraceAllTexture2D();

	UFUNCTION(BlueprintCallable)
	static void TestShowUserWidget(FString name, int idx);

    UFUNCTION(BlueprintCallable)
    static void TraceClass(FString name);

    UFUNCTION(BlueprintCallable)
    static void DoFile(FString name);

    //SaveDir
    UFUNCTION(BlueprintCallable)
    static FString GetSavedDir();

    //Search File
    UFUNCTION(BlueprintCallable)
    static TArray<FString> SearchFileAbsPath(FString base_dir, FString dir, FString ext, bool isRecursive);

    //Delete File
    UFUNCTION(BlueprintCallable)
    static bool DeleteFileAbsPath(FString path);

    UFUNCTION(BlueprintCallable)
    static void ScreenMessage(FString text);

    static TMap<FString, double> s_mapCostTime;
    UFUNCTION(BlueprintCallable)
    static void BeginCost(FString key);
    UFUNCTION(BlueprintCallable)
    static void EndCost(FString key);

    UFUNCTION(BlueprintCallable)
    static void UpdateAniKeyValue(UWidgetAnimation* InAnimation, float InTime, const TArray<float>& v);

    UFUNCTION(BlueprintCallable)
    static void UpdateAniKeyValue_2DTransform_Rotation(UWidgetAnimation* InAnimation, const FString& InName, float InTime, float v);
	
};