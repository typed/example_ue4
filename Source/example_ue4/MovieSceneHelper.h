// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "MovieSceneHelper.generated.h"

/**
 * 
 */

UENUM(BlueprintType)
enum class EMovieSceneHelperProperty : uint8
{
    TranslationX,
    TranslationY,
    Rotation,
    ScaleX,
    ScaleY,
    PivotX,
    PivotY,
};

UCLASS()
class EXAMPLE_UE4_API UMovieSceneHelper : public UObject
{
	GENERATED_BODY()
	
public:

    UFUNCTION(BlueprintCallable)
    static void UpdateAniKeyValue_Float(UWidgetAnimation* InAnimation, const FString& InBindingName,
        const FString& InTrackName, EMovieSceneHelperProperty InProperty, float InTime, float v, int32 section = 0);

    UFUNCTION(BlueprintCallable)
    static void RemoveAniKey(UWidgetAnimation* InAnimation, const FString& InBindingName,
        const FString& InTrackName, EMovieSceneHelperProperty InProperty, float InTime, int32 section = 0);

    UFUNCTION(BlueprintCallable)
    static void UpdateOrAddAniKey_Float(UWidgetAnimation* InAnimation, const FString& InBindingName,
        const FString& InTrackName, EMovieSceneHelperProperty InProperty, float InTime, float v, int32 section = 0);
	
};