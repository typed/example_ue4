// Fill out your copyright notice in the Description page of Project Settings.

#include "ScriptHelperClient.h"
#include "SluaActor.h"
#include "Engine/Engine.h"
#include "UObject/UObject.h"
#include "UObject/UObjectIterator.h"
#include "GameHelper.h"
#include "UMG.h"

DEFINE_LOG_CATEGORY(LogScriptHelper);

UUserWidget* UScriptHelperClient::CreateUserWidget(FString name)
{
	TSubclassOf<UUserWidget> uclass = ::LoadClass<UUserWidget>(nullptr, *name);
	if (uclass == nullptr)
		return nullptr;
	if (!ASluaActor::instance)
		return nullptr;
	UWorld* wld = ASluaActor::instance->GetWorld();
	if (!wld)
		return nullptr;
	UUserWidget* widget = CreateWidget<UUserWidget>(wld, uclass);
	//UE_LOG(LogScriptHelper, Log, TEXT("UScriptHelperClient::CreateUserWidget World:%x Name: %s"), wld, *(wld->GetName()));
	return widget;
}

void UScriptHelperClient::GC()
{
	GEngine->ForceGarbageCollection(true);
}

UClass* UScriptHelperClient::LoadUserWidgetClass(FString name)
{
	return ::LoadClass<UUserWidget>(nullptr, *name);
}

UObject* UScriptHelperClient::LoadObjet(FString name)
{
    return ::LoadObject<UObject>(nullptr, *name);
}

UClass* UScriptHelperClient::LoadBpClass(FString name)
{
    return ::LoadObject<UClass>(nullptr, *name);
}

void UScriptHelperClient::TraceAllObject()
{
	//for (TObjectIterator<UObject> It; It; ++It) {
	//	UObject* CurrentObject = *It;
	//	UE_LOG(LogScriptHelper, Log, TEXT("Found UObject named: %s"), *(CurrentObject->GetName()));
	//}
	for (TObjectIterator<UUserWidget> It; It; ++It) {
		UUserWidget* w = *It;
		UE_LOG(LogScriptHelper, Log, TEXT("Found UUserWidget:%x Name: %s"), w, *(w->GetName()));
	}
}

void UScriptHelperClient::TestShowUserWidget(FString name, int idx)
{
	TWeakObjectPtr<UUserWidget> w = UScriptHelperClient::CreateUserWidget(name);
	w->AddToViewport(idx);
}

void UScriptHelperClient::TraceClass(FString name)
{
    UClass* pClass = nullptr;
    pClass = ::LoadClass<UClass>(nullptr, *name);
    if (pClass == nullptr) {
        pClass = ::LoadObject<UClass>(nullptr, *name);
    }
    if (pClass == nullptr) {
        UE_LOG(LogScriptHelper, Log, TEXT("UClass not found."), *name);
        return;
    }
    UE_LOG(LogScriptHelper, Log, TEXT("UClass %s"), *pClass->GetName());
    for (TFieldIterator<UProperty> it(pClass); it; ++it) {
        UProperty* prop = *it;
        if (prop->GetOwnerClass() != pClass) {
            continue;
        }
        uint64 propflag = prop->GetPropertyFlags();
        UE_LOG(LogScriptHelper, Log, TEXT("Class Member %s propflag %x"), *prop->GetName(), propflag);
    }
    for (TFieldIterator<UFunction> FuncIt(pClass); FuncIt; ++FuncIt) {
        UFunction* func = *FuncIt;
        if (func->GetOwnerClass() != pClass) {
            continue;
        }
        int32 i = 1;
        UE_LOG(LogScriptHelper, Log, TEXT("Function %s"), *func->GetName());
        for (TFieldIterator<UProperty> it(func); it && (it->PropertyFlags&CPF_Parm); ++it) {
            UProperty* prop = *it;
            uint64 propflag = prop->GetPropertyFlags();
            UE_LOG(LogScriptHelper, Log, TEXT("Param%d %s propflag %x"), i++, *prop->GetName(), propflag);
        }
    }
}