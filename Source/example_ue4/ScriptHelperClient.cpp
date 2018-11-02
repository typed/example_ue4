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
	TSubclassOf<UUserWidget> uclass = ::LoadClass<UUserWidget>(NULL, *name);
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

UClass* UScriptHelperClient::LoadClass(FString name)
{
	return ::LoadClass<UUserWidget>(nullptr, *name);
}

UObject* UScriptHelperClient::LoadObjet(FString name)
{
    return ::LoadClass<UObject>(nullptr, *name);
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