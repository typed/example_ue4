// Fill out your copyright notice in the Description page of Project Settings.

#include "MyTestUIActor.h"
#include "UtilScript.h"
#include "UMG.h"


// Sets default values
AMyTestUIActor::AMyTestUIActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AMyTestUIActor::BeginPlay()
{
	Super::BeginPlay();

	UUserWidget* w = UUtilScript::CreateUserWidget("/Game/ExampleMain/ExampleMainUI.ExampleMainUI_C");
	w->AddToViewport(0);
	auto btn = Cast<UButton>(w->GetWidgetFromName("Button_ReuseList"));
	FScriptDelegate Delegate;
	Delegate.BindUFunction(this, TEXT("OnClickOpenWidget"));
	btn->OnClicked.Add(Delegate);

	btn = Cast<UButton>(w->GetWidgetFromName("Button_GC"));
	FScriptDelegate Delegate1;
	Delegate1.BindUFunction(this, TEXT("OnClickGC"));
	btn->OnClicked.Add(Delegate1);
	
}

void AMyTestUIActor::OnClickGC()
{
    UUtilScript::GC();
    UUtilScript::TraceAllObject();
}

void AMyTestUIActor::OnClickOpenWidget()
{
	m_pUserWidget = UUtilScript::CreateUserWidget("/Game/ExampleReuseList/ExampleReuseListUI.ExampleReuseListUI_C");
	m_pUserWidget->AddToViewport(1);
	auto btn = Cast<UButton>(m_pUserWidget->GetWidgetFromName("Button_Close"));
	FScriptDelegate Delegate;
	Delegate.BindUFunction(this, TEXT("OnClickClose"));
	btn->OnClicked.Add(Delegate);
}

void AMyTestUIActor::OnClickClose()
{
	if (m_pUserWidget.IsValid()) {
		m_pUserWidget->RemoveFromViewport();
	}
}

// Called every frame
void AMyTestUIActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

