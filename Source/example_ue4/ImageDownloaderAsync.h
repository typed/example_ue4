// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "UObject/ObjectMacros.h"
#include "Interfaces/IHttpRequest.h"
#include "ImageDownloaderAsync.generated.h"

class UTexture2D;
class UImageDownloaderAsync;

/**
 * 
 */
UCLASS()
class EXAMPLE_UE4_API UImageDownloaderAsync : public UObject
{
	GENERATED_BODY()
	
public:

    DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FImageDownloaderAsyncDelegate, UTexture2D*, Texture, UImageDownloaderAsync*, ImageDownloader);

	UPROPERTY(BlueprintAssignable)
	FImageDownloaderAsyncDelegate OnSuccess;

	UPROPERTY(BlueprintAssignable)
	FImageDownloaderAsyncDelegate OnFail;

    UImageDownloaderAsync();
	~UImageDownloaderAsync();

	UPROPERTY(BlueprintReadOnly)
	FString FileUrl;

	UPROPERTY(BlueprintReadOnly)
	FString FileSavePath;

	UPROPERTY(BlueprintReadOnly)
	FString UrlHash;

	UPROPERTY(BlueprintReadOnly)
	bool InvalidImageFormat;

	UPROPERTY(BlueprintReadWrite)
	bool SaveDiskFile;

	UFUNCTION(BlueprintCallable)
	static UImageDownloaderAsync* MakeDownloader();

    UFUNCTION(BlueprintCallable)
    static UImageDownloaderAsync* MakeDownloaderInGame();

	UFUNCTION(BlueprintCallable)
	static void SetSubDirTime(int32 sec);
	UFUNCTION(BlueprintCallable)
	static void ResetSubDirTime();

	UFUNCTION(BlueprintCallable)
	static void SetRootDir(FString rootDir);
	UFUNCTION(BlueprintCallable)
	static void ResetRootDir();

	UFUNCTION(BlueprintCallable)
	void Start(FString Url);

private:

	static TMap<FString, TWeakObjectPtr<UTexture2D> > s_mapTexture;

	void HandleRequest(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded);

	void CheckDiskFile();

	static FString GetSubDir();

	static FString s_strDir;
	static FString s_strRootDir;
	static FString s_strSubDir;
	static int32 s_nSubDirTime;
	static bool s_bCheckDiskFile;

    static TMap<FString, int32> s_mapInvalidFormatTryCount;
    static int32 s_nInvalidFormatTryMaxCount;

    static TWeakObjectPtr<UImageDownloaderAsync> ImageDownloaderInGame;

};

DECLARE_LOG_CATEGORY_EXTERN(LogImageDownloaderAsync, Verbose, All);