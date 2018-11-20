// Fill out your copyright notice in the Description page of Project Settings.

#include "ExampleGameInstance.h"
#include "HAL/PlatformFileManager.h"
#include "Misc/Paths.h"
#include "GenericPlatformFile.h"
#include "SluaFix.h"
#include "MyUserWidget.h"


DECLARE_LOG_CATEGORY_EXTERN(LogExampleGameInstance, Verbose, All);
DEFINE_LOG_CATEGORY(LogExampleGameInstance);

static uint8* ReadFile(IPlatformFile& PlatformFile, FString path, uint32& len) {
    IFileHandle* FileHandle = PlatformFile.OpenRead(*path);
    if (FileHandle) {
        len = (uint32)FileHandle->Size();
        uint8* buf = new uint8[len];

        FileHandle->Read(buf, len);

        // Close the file again
        delete FileHandle;

        return buf;
    }

    return nullptr;
}

void UExampleGameInstance::Init()
{
	UGameInstance::Init();
	UE_LOG(LogExampleGameInstance, Log, TEXT("UExampleGameInstance::Init"));
    slua::SluaFix::initSluaState(this);
    slua::SluaFix::getSluaState()->setLoadFileDelegate([](const char* fn, uint32& len, FString& filepath)->uint8* {
        IPlatformFile& PlatformFile = FPlatformFileManager::Get().GetPlatformFile();
        FString path = FPaths::ProjectContentDir();
        path += "/Lua/";
        path += UTF8_TO_TCHAR(fn);
        TArray<FString> luaExts = { UTF8_TO_TCHAR(".lua"), UTF8_TO_TCHAR(".luac") };
        for (auto ptr = luaExts.CreateConstIterator(); ptr; ++ptr) {
            auto fullPath = path + *ptr;
            auto buf = ReadFile(PlatformFile, fullPath, len);
            if (buf) {
                filepath = fullPath;
                return buf;
            }
        }
        return nullptr;
    });

    UMyUserWidget::bindLuaFunc();
}

void UExampleGameInstance::Shutdown()
{
    slua::SluaFix::releaseSluaState();
    //state.close();
    UGameInstance::Shutdown();
    UE_LOG(LogExampleGameInstance, Log, TEXT("UExampleGameInstance::Shutdown"));
}

slua::LuaState* UExampleGameInstance::State()
{
    return slua::SluaFix::getSluaState();
}