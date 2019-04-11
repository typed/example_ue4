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

static int inl_Test_TCHAR_TO_UTF8(const char* text)
{
    int n = 0;
    for (int i = 0; i < 5; i++) {
        n += text[i];
    }
    return n;
}

int UUtilGame::Test_TCHAR_TO_UTF8()
{
    return inl_Test_TCHAR_TO_UTF8(TCHAR_TO_UTF8("ZZZZZZZZZZZ"));
}