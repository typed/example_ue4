// Fill out your copyright notice in the Description page of Project Settings.

#include "UtilScript.h"
#include "SluaActor.h"
#include "Engine/Engine.h"
#include "UObject/Object.h"
//#include "UObject/ObjectIterator.h"
#include "UObject/Class.h"
#include "Blueprint/UserWidget.h"
#include "UtilGame.h"
#include "UMG.h"
#include "LogDefine.h"
#include "Runtime/MovieScene/Public/MovieScene.h"


UUserWidget* UUtilScript::CreateUserWidget(FString name)
{
    BeginCost(name);
	TSubclassOf<UUserWidget> uclass = ::LoadClass<UUserWidget>(nullptr, *name);
	if (uclass == nullptr)
		return nullptr;
    UWorld* wld = UUtilGame::GetMyWorld();
    if (wld == nullptr)
        return nullptr;
	UUserWidget* widget = CreateWidget<UUserWidget>(UUtilGame::GetMyWorld(), uclass);
    EndCost(name);
	UE_LOG(LogUtil, Log, TEXT("UUtilScript::CreateUserWidget Widget:%x Name: %s"), widget, *(widget->GetName()));
	return widget;
}

void UUtilScript::GC()
{
	GEngine->ForceGarbageCollection(true);
}

UObject* UUtilScript::LoadRes(FString name)
{
    return ::LoadObject<UObject>(nullptr, *name);
}

UObject* UUtilScript::FindRes(FString name)
{
    return ::FindObject<UObject>(nullptr, *name);
}

UClass* UUtilScript::LoadBpClass(FString name)
{
    return ::LoadObject<UClass>(nullptr, *name);
}

void UUtilScript::TraceAllObject()
{
    UE_LOG(LogUtil, Log, TEXT("TraceAllObject"));
	for (TObjectIterator<UObject> It; It; ++It) {
        UObject* w = *It;
		UE_LOG(LogUtil, Log, TEXT("Found UObject:%x PathName: %s"), w, *(w->GetPathName()));
	}
}

void UUtilScript::TraceAllUserWidget()
{
    UE_LOG(LogUtil, Log, TEXT("TraceAllUserWidget"));
    for (TObjectIterator<UUserWidget> It; It; ++It) {
        UUserWidget* w = *It;
        UE_LOG(LogUtil, Log, TEXT("Found UUserWidget:%x PathName: %s"), w, *(w->GetPathName()));
    }
}

void UUtilScript::TraceAllTexture2D()
{
    UE_LOG(LogUtil, Log, TEXT("TraceAllTexture2D"));
    for (TObjectIterator<UTexture2D> It; It; ++It) {
        UTexture2D* w = *It;
        UE_LOG(LogUtil, Log, TEXT("Found UTexture2D:%x PathName: %s"), w, *(w->GetPathName()));
    }
}

void UUtilScript::TestShowUserWidget(FString name, int idx)
{
	TWeakObjectPtr<UUserWidget> w = UUtilScript::CreateUserWidget(name);
	w->AddToViewport(idx);
}

void UUtilScript::TraceClass(FString name)
{
    UClass* pClass = nullptr;
    pClass = ::LoadClass<UClass>(nullptr, *name);
    if (pClass == nullptr) {
        pClass = ::LoadObject<UClass>(nullptr, *name);
    }
    if (pClass == nullptr) {
        UE_LOG(LogUtil, Log, TEXT("UClass not found."), *name);
        return;
    }
    UE_LOG(LogUtil, Log, TEXT("UClass %s"), *pClass->GetName());
    for (TFieldIterator<UObjectProperty> it(pClass); it; ++it) {
        UObjectProperty* prop = *it;
        if (prop->GetOwnerClass() != pClass) {
            continue;
        }
        FString prop_name = prop->GetName();
        FString class_name = prop->PropertyClass->GetName();
        uint64 propflag = prop->GetPropertyFlags();
        UE_LOG(LogUtil, Log, TEXT("Name %s ClassName %s propflag %x"), *prop_name, *class_name, propflag);
    }
    for (TFieldIterator<UFunction> FuncIt(pClass); FuncIt; ++FuncIt) {
        UFunction* func = *FuncIt;
        if (func->GetOwnerClass() != pClass) {
            continue;
        }
        int32 i = 1;
        UE_LOG(LogUtil, Log, TEXT("Function %s"), *func->GetName());
        for (TFieldIterator<UProperty> it(func); it && (it->PropertyFlags&CPF_Parm); ++it) {
            UProperty* prop = *it;
            uint64 propflag = prop->GetPropertyFlags();
            UE_LOG(LogUtil, Log, TEXT("Param%d %s propflag %x"), i++, *prop->GetName(), propflag);
        }
    }
}

void UUtilScript::DoFile(FString name)
{
    auto state = UUtilGame::GetGameInstance()->State();
    if (state) {
        state->doFile(TCHAR_TO_UTF8(*name));
    }
}

//SaveDir
FString UUtilScript::GetSavedDir()
{
    return FPaths::ProjectSavedDir();
}

//Search File
TArray<FString> UUtilScript::SearchFileAbsPath(FString base_dir, FString dir, FString ext, bool isRecursive)
{
    TArray<FString> aRet;
    FString sPath = base_dir + dir;
    IPlatformFile& PlatformFile = FPlatformFileManager::Get().GetPlatformFile();
    if (isRecursive) {
        PlatformFile.FindFilesRecursively(aRet, *sPath, *ext);
    }
    else {
        PlatformFile.FindFiles(aRet, *sPath, *ext);
    }
    return aRet;
}

//Delete File
bool UUtilScript::DeleteFileAbsPath(FString path)
{
    IPlatformFile& PlatformFile = FPlatformFileManager::Get().GetPlatformFile();
    return PlatformFile.DeleteFile(*path);
}

void UUtilScript::ScreenMessage(FString text)
{
    GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, text);
}

//Output Cost Time
TMap<FString, double> UUtilScript::s_mapCostTime;
void UUtilScript::BeginCost(FString key)
{
    s_mapCostTime.Add(key, FPlatformTime::Seconds());
}
void UUtilScript::EndCost(FString key)
{
    double* p_v = s_mapCostTime.Find(key);
    if (p_v) {
        double delta = FPlatformTime::Seconds() - (*p_v);
        s_mapCostTime.Remove(key);
        TArray<FStringFormatArg> Args;
        Args.Add(key);
        Args.Add(delta);
        FString str = FString::Format(TEXT("{0} cost {1} s."), Args);
        UE_LOG(LogUtil, Log, TEXT("%s"), (*str));
        ScreenMessage(str);
    }
}