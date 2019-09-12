// Fill out your copyright notice in the Description page of Project Settings.

#include "AsyncLoadMgr.h"
#include "UtilScript.h"

DEFINE_LOG_CATEGORY(LogAsyncLoadMgr);

UAsyncLoadMgr::UAsyncLoadMgr()
{

}

UAsyncLoadMgr::~UAsyncLoadMgr()
{

}

void UAsyncLoadMgr::LoadObject(FString _path)
{
    TArray<FString> _arr_path;
    _arr_path.Add(_path);
    LoadArray(_arr_path);
}

void UAsyncLoadMgr::LoadArray(TArray<FString> _arr_path)
{
    AddToRoot();
    m_arr_path = _arr_path;
    TArray<FStringAssetReference> itemsToStream;
    for (int32 i = 0; i < m_arr_path.Num(); ++i) {
        itemsToStream.AddUnique(m_arr_path[i]);
    }
    m_stmMgr.RequestAsyncLoad(itemsToStream, FStreamableDelegate::CreateUObject(this, &UAsyncLoadMgr::OnLoadFinish));
}

void UAsyncLoadMgr::OnLoadFinish()
{
    RemoveFromRoot();
    UE_LOG(LogAsyncLoadMgr, Log, TEXT("OnAsyncLoadFinish"));
    for (int32 i = 0; i < m_arr_path.Num(); ++i) {
        UObject* obj = FindObject<UObject>(NULL, *m_arr_path[i]);
        if (obj) {
            OnFinish.Broadcast(obj->GetPathName(), obj);
        }
    }
}

UAsyncLoadMgr* UAsyncLoadMgr::Make()
{
    return NewObject<UAsyncLoadMgr>();
}