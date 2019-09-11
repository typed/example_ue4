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
    TArray<FString> _arr_path;
    _arr_path.Add(_path);
    AsycLoadArray(_arr_path);
}

void UAsycLoadMgr::AsycLoadArray(TArray<FString> _arr_path)
{
    AddToRoot();
    m_arr_path = _arr_path;
    TArray<FStringAssetReference> itemsToStream;
    for (int32 i = 0; i < m_arr_path.Num(); ++i) {
        itemsToStream.AddUnique(m_arr_path[i]);
    }
    m_stmMgr.RequestAsyncLoad(itemsToStream, FStreamableDelegate::CreateUObject(this, &UAsycLoadMgr::OnAsycLoadFinish));
}

void UAsycLoadMgr::OnAsycLoadFinish()
{
    RemoveFromRoot();
    UE_LOG(LogAsycLoadMgr, Log, TEXT("OnAsycLoadFinish"));
    for (int32 i = 0; i < m_arr_path.Num(); ++i) {
        UObject* obj = FindObject<UObject>(NULL, *m_arr_path[i]);
        if (obj) {
            OnFinish.Broadcast(obj->GetPathName(), obj);
        }
    }
}

UAsycLoadMgr* UAsycLoadMgr::Make()
{
    return NewObject<UAsycLoadMgr>();
}