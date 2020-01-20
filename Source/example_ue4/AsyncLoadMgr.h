// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/StreamableManager.h"
#include "AsyncLoadMgr.generated.h"

/**
 * 
 */
UCLASS()
class EXAMPLE_UE4_API UAsyncLoadMgr : public UObject
{
	GENERATED_BODY()
	
public:

    DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FAsycLoadMgrFinishDelegate, FString, _path, UObject*, obj);

	UPROPERTY(BlueprintAssignable)
    FAsycLoadMgrFinishDelegate OnFinish;

    UAsyncLoadMgr();
	~UAsyncLoadMgr();

	UFUNCTION(BlueprintCallable)
	void LoadObject(FString _path);

    UFUNCTION(BlueprintCallable)
    void LoadArray(TArray<FString> arr_path);

    UFUNCTION(BlueprintCallable)
    static UAsyncLoadMgr* Make();

private:

    void OnLoadFinish();

    FStreamableManager m_stmMgr;
    TArray<FString> m_arr_path;

};

DECLARE_LOG_CATEGORY_EXTERN(LogAsyncLoadMgr, Verbose, All);