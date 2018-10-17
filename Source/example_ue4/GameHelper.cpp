// Fill out your copyright notice in the Description page of Project Settings.

#include "GameHelper.h"

UExampleGameInstance* UGameHelper::GetGameInstance()
{
	if (GEngine->GetWorld() != nullptr && GEngine->GetWorld()->GetGameInstance() != nullptr) {
		return Cast<UExampleGameInstance>(GEngine->GetWorld()->GetGameInstance());
	}
	return nullptr;
}