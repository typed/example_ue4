// Fill out your copyright notice in the Description page of Project Settings.

#include "GameHelper.h"

UWorld* UGameHelper::GetWorld()
{
	return GEngine->GetWorld();
}

UExampleGameInstance* UGameHelper::GetGameInstance()
{
	UWorld* pWorld = GetWorld();
	if (pWorld != nullptr) {
		UGameInstance* pGameInstance = pWorld->GetGameInstance();
		if (pGameInstance != nullptr) {
			return Cast<UExampleGameInstance>(pGameInstance);
		}
	}
	return nullptr;
}