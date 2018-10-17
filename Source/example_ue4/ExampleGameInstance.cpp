// Fill out your copyright notice in the Description page of Project Settings.

#include "ExampleGameInstance.h"

DECLARE_LOG_CATEGORY_EXTERN(LogExampleGameInstance, Verbose, All);
DEFINE_LOG_CATEGORY(LogExampleGameInstance);


void UExampleGameInstance::Init()
{
	UGameInstance::Init();
	UE_LOG(LogExampleGameInstance, Log, TEXT("UExampleGameInstance::Init"));
}