// Fill out your copyright notice in the Description page of Project Settings.


#include "SluaActor.h"
#include "SluaComponent.h"
#include "HAL/PlatformFileManager.h"
#include "Misc/Paths.h"
#include "GenericPlatformFile.h"
#include "UtilGame.h"
#include "SluaFix.h"


ASluaActor* ASluaActor::instance=nullptr;

// Sets default values
ASluaActor::ASluaActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	instance = this;
}

// Called when the game starts or when spawned
void ASluaActor::BeginPlay()
{
	Super::BeginPlay();
	
	auto state = UUtilGame::GetGameInstance()->State();
    state->doFile("SLuaTest");
}

// Called every frame
void ASluaActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	//auto state = UUtilGame::GetGameInstance()->State();
	//if (state == nullptr)
    //    return;
	//state->call("Tick", DeltaTime, this);
	//GEngine->ForceGarbageCollection(true);
}

void ASluaActor::SetFName(FName name) {
	slua::Log::Log("set fname %s", TCHAR_TO_UTF8(*(name.ToString())));	
}

