// Fill out your copyright notice in the Description page of Project Settings.

#include "ScriptHelperClient.h"
#include "SluaActor.h"



UUserWidget* UScriptHelperClient::GetWidget(FString name)
{
	TArray<FStringFormatArg> Args;
	Args.Add(name);

	// load blueprint widget from cpp, need add '_C' tail
	auto cui = FString::Format(TEXT("Blueprint'{0}_C'"), Args);
	TSubclassOf<UUserWidget> uclass = LoadClass<UUserWidget>(NULL, *cui);
	if (uclass == nullptr)
		return nullptr;
	if (!ASluaActor::instance)
		return nullptr;
	UWorld* wld = ASluaActor::instance->GetWorld();
	if (!wld)
		return nullptr;
	UUserWidget* widget = CreateWidget<UUserWidget>(wld, uclass);
	return widget;
}