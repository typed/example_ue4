// Fill out your copyright notice in the Description page of Project Settings.

#include "AsycLoadMgr.h"
#include "UtilScript.h"

DEFINE_LOG_CATEGORY(LogAsycLoadMgr);

UAsycLoadMgr::UAsycLoadMgr()
{

}

UAsycLoadMgr::~UAsycLoadMgr()
{

}

void UAsycLoadMgr::AsycLoadObject(FString _path)
{
    AddToRoot();
    m_path = _path;
    TArray<FStringAssetReference> itemsToStream;
    itemsToStream.AddUnique(m_path);
    m_stmMgr.RequestAsyncLoad(itemsToStream, FStreamableDelegate::CreateUObject(this, &UAsycLoadMgr::OnAsycLoadFinish));
}

void UAsycLoadMgr::OnAsycLoadFinish()
{
    RemoveFromRoot();
    UE_LOG(LogAsycLoadMgr, Log, TEXT("OnAsycLoadFinish"));
    UObject* obj = FindObject<UObject>(NULL, *m_path);
    if (obj) {
        OnFinish.Broadcast(obj->GetPathName(), obj);
    }
}

UAsycLoadMgr* UAsycLoadMgr::Make()
{
    return NewObject<UAsycLoadMgr>();
}