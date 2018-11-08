// Fill out your copyright notice in the Description page of Project Settings.

#include "UtilGame.h"

UWorld* UUtilGame::GetMyWorld()
{
	return GEngine->GetWorld();
}

UExampleGameInstance* UUtilGame::GetGameInstance()
{
	UWorld* pWorld = GetMyWorld();
	if (pWorld != nullptr) {
		UGameInstance* pGameInstance = pWorld->GetGameInstance();
		if (pGameInstance != nullptr) {
			return Cast<UExampleGameInstance>(pGameInstance);
		}
	}
	return nullptr;
}