// Fill out your copyright notice in the Description page of Project Settings.

#include "UtilGame.h"

DEFINE_LOG_CATEGORY(LogUtilGame);

UWorld* UUtilGame::GetMyWorld()
{
	return GWorld->GetWorld();
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
    return inl_Test_TCHAR_TO_UTF8(TCHAR_TO_UTF8(TEXT("ZZZZZZZZZZZ")));
}

void UUtilGame::TestLowerAndUpperBound()
{
    TArray<int32> ArrTmp;
    ArrTmp.Add(12);
    ArrTmp.Add(12);
    ArrTmp.Add(12);
    ArrTmp.Add(13);

    {
        int n = Algo::LowerBound(ArrTmp, 11);
        UE_LOG(LogUtilGame, Log, TEXT("Algo::LowerBound(ArrTmp, 11) ret=%d"), n);
    }
    {
        int n = Algo::LowerBound(ArrTmp, 12);
        UE_LOG(LogUtilGame, Log, TEXT("Algo::LowerBound(ArrTmp, 12) ret=%d"), n);
    }
    {
        int n = Algo::LowerBound(ArrTmp, 13);
        UE_LOG(LogUtilGame, Log, TEXT("Algo::LowerBound(ArrTmp, 13) ret=%d"), n);
    }
    {
        int n = Algo::LowerBound(ArrTmp, 14);
        UE_LOG(LogUtilGame, Log, TEXT("Algo::LowerBound(ArrTmp, 14) ret=%d"), n);
    }

    {
        int n = Algo::UpperBound(ArrTmp, 11);
        UE_LOG(LogUtilGame, Log, TEXT("Algo::UpperBound(ArrTmp, 11) ret=%d"), n);
    }
    {
        int n = Algo::UpperBound(ArrTmp, 12);
        UE_LOG(LogUtilGame, Log, TEXT("Algo::UpperBound(ArrTmp, 12) ret=%d"), n);
    }
    {
        int n = Algo::UpperBound(ArrTmp, 13);
        UE_LOG(LogUtilGame, Log, TEXT("Algo::UpperBound(ArrTmp, 13) ret=%d"), n);
    }
    {
        int n = Algo::UpperBound(ArrTmp, 14);
        UE_LOG(LogUtilGame, Log, TEXT("Algo::UpperBound(ArrTmp, 14) ret=%d"), n);
    }

    /*
    LogUtilGame: Algo::LowerBound(ArrTmp, 11) ret=0
    LogUtilGame: Algo::LowerBound(ArrTmp, 12) ret=0
    LogUtilGame: Algo::LowerBound(ArrTmp, 13) ret=3
    LogUtilGame: Algo::LowerBound(ArrTmp, 14) ret=4
    LogUtilGame: Algo::UpperBound(ArrTmp, 11) ret=0
    LogUtilGame: Algo::UpperBound(ArrTmp, 12) ret=3
    LogUtilGame: Algo::UpperBound(ArrTmp, 13) ret=4
    LogUtilGame: Algo::UpperBound(ArrTmp, 14) ret=4
    */
}