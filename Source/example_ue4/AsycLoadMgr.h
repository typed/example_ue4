// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/StreamableManager.h"
#include "AsycLoadMgr.generated.h"

/**
 * 
 */
UCLASS()
class EXAMPLE_UE4_API UAsycLoadMgr : public UObject
{
	GENERATED_BODY()
	
public:

    DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FAsycLoadMgrFinishDelegate, FString, _path, UObject*, obj);

	UPROPERTY(BlueprintAssignable)
    FAsycLoadMgrFinishDelegate OnFinish;

    UAsycLoadMgr();
	~UAsycLoadMgr();

	UFUNCTION(BlueprintCallable)
	void AsycLoadObject(FString _path);

    UFUNCTION(BlueprintCallable)
    static UAsycLoadMgr* Make();

private:

    void OnAsycLoadFinish();

    FStreamableManager m_stmMgr;
    FString m_path;

};

DECLARE_LOG_CATEGORY_EXTERN(LogAsycLoadMgr, Verbose, All);