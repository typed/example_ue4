// Fill out your copyright notice in the Description page of Project Settings.

#include "AsycLoadMgr.h"

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
    OnFinish.Broadcast(m_path, obj);
}

UAsycLoadMgr* UAsycLoadMgr::Make()
{
    return NewObject<UAsycLoadMgr>();
}