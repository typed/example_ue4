// Fill out your copyright notice in the Description page of Project Settings.

#include "ImageDownloader.h"
#include "HAL/PlatformFilemanager.h"
#include "IImageWrapper.h"
#include "IImageWrapperModule.h"
#include "ModuleManager.h"
#include "FileHelper.h"
#include "Interfaces/IHttpResponse.h"
#include "HttpModule.h"
#include "Engine/Texture2D.h"

DEFINE_LOG_CATEGORY(LogImageDownloader);

std::map<FString, FString> UImageDownloader::s_mapHashPath;
FString UImageDownloader::s_strDir;
FString UImageDownloader::s_strRootDir = "ImageDownload";
FString UImageDownloader::s_strSubDir;
int32 UImageDownloader::s_nSubDirTime = 24*3600;
bool UImageDownloader::s_bCheckDiskFile = true;

static UTexture2D* GetTexture2DFromArray(const TArray<uint8>& OutArray, bool& InvalidImageFormat)
{
	InvalidImageFormat = false;
	IImageWrapperModule& imageWrapperModule = FModuleManager::LoadModuleChecked<IImageWrapperModule>(FName("ImageWrapper"));
	EImageFormat format = imageWrapperModule.DetectImageFormat(OutArray.GetData(), OutArray.Num());
	if (format == EImageFormat::Invalid) {
		InvalidImageFormat = true;
		return nullptr;
	}
	UTexture2D* OutTex = nullptr;
	TSharedPtr<IImageWrapper> imageWrapper = imageWrapperModule.CreateImageWrapper(format);
	if (imageWrapper.IsValid() && imageWrapper->SetCompressed(OutArray.GetData(), OutArray.Num())) {
		const TArray<uint8>* uncompressedData = nullptr;
		if (imageWrapper->GetRaw(ERGBFormat::BGRA, 8, uncompressedData)) {
			OutTex = UTexture2D::CreateTransient(imageWrapper->GetWidth(), imageWrapper->GetHeight(), PF_B8G8R8A8);
			if (OutTex) {
				void* TextureData = OutTex->PlatformData->Mips[0].BulkData.Lock(LOCK_READ_WRITE);
				FMemory::Memcpy(TextureData, uncompressedData->GetData(), uncompressedData->Num());
				OutTex->PlatformData->Mips[0].BulkData.Unlock();
				OutTex->UpdateResource();
			}
		}
	}
	return OutTex;
}

static UTexture2D* GetTexture2DFromDisk(FString PathName, bool& InvalidImageFormat)
{
	InvalidImageFormat = false;
	if (!FPaths::FileExists(PathName)) {
		return nullptr;
	}
	TArray<uint8> OutArray;
	if (FFileHelper::LoadFileToArray(OutArray, *PathName)) {
		return GetTexture2DFromArray(OutArray, InvalidImageFormat);
	}
	return nullptr;
}

UImageDownloader::UImageDownloader()
	: FileUrl(TEXT(""))
	, FileSavePath(TEXT(""))
	, UrlHash(TEXT(""))
	, InvalidImageFormat(false)
	, SaveDiskFile(true)
{
	if (s_strDir.IsEmpty()) {
		s_strDir = FPaths::ProjectSavedDir() + s_strRootDir;
	}
	s_strSubDir = GetSubDir();
}

UImageDownloader::~UImageDownloader()
{

}

TWeakObjectPtr<UImageDownloader> UImageDownloader::ImageDownloaderInGame = nullptr;

UImageDownloader* UImageDownloader::MakeDownloader()
{
	UImageDownloader* DownloadTask = NewObject<UImageDownloader>();
	return DownloadTask;
}

UImageDownloader* UImageDownloader::MakeDownloaderInGame()
{
    if (!ImageDownloaderInGame.IsValid())
    {
        ImageDownloaderInGame = NewObject<UImageDownloader>();
    }
    return ImageDownloaderInGame.Get();
}

void UImageDownloader::SetSubDirTime(int32 sec)
{
	s_nSubDirTime = sec;
	s_strSubDir = GetSubDir();
}

void UImageDownloader::ResetSubDirTime()
{
	s_nSubDirTime = 24 * 3600;
}

void UImageDownloader::SetRootDir(FString rootDir)
{
	s_strDir = FPaths::ProjectSavedDir() + rootDir;
}

void UImageDownloader::ResetRootDir()
{
	s_strDir = FPaths::ProjectSavedDir() + s_strRootDir;
}

FString UImageDownloader::GetSubDir()
{
	static int64 s_i64SubDirNum = 0;
	int64 n = FDateTime::Now().ToUnixTimestamp() / s_nSubDirTime;
	if (s_i64SubDirNum != n) {
		s_i64SubDirNum = n;
		s_bCheckDiskFile = true;
	}
	return TEXT("imgd_") + Lex::ToString(n);
}

void UImageDownloader::CheckDiskFile()
{
	if (!s_bCheckDiskFile) {
		return;
	}
	class cCheckDiskFile : public IPlatformFile::FDirectoryVisitor
	{
	public:
		bool bDel;
		IPlatformFile& PlatformFile;
		cCheckDiskFile(IPlatformFile& pf)
			:bDel(false)
			,PlatformFile(pf)
		{
			bDel = false;
		}
		virtual bool Visit(const TCHAR* FilenameOrDirectory, bool bIsDirectory) override
		{
			if (bIsDirectory) {
				FString sName(FilenameOrDirectory);
				int32 ret = sName.Find(s_strSubDir);
				if (ret != -1) {
					return true;
				}
				PlatformFile.DeleteDirectoryRecursively(FilenameOrDirectory);
				//UE_LOG(LogImageDownloader, Log, TEXT("DeleteDir:%s"), FilenameOrDirectory);
				bDel = true;
			}
			else {
				PlatformFile.DeleteFile(FilenameOrDirectory);
				//UE_LOG(LogImageDownloader, Log, TEXT("DeleteFile:%s"), FilenameOrDirectory);
				bDel = true;
			}
			return false;
		}
	};
	IPlatformFile& PlatformFile = FPlatformFileManager::Get().GetPlatformFile();
	cCheckDiskFile cdf(PlatformFile);
	PlatformFile.IterateDirectory(*s_strDir, cdf);
	if (!cdf.bDel) {
		s_bCheckDiskFile = false;
	}
}

void UImageDownloader::Start(FString Url)
{
	CheckDiskFile();

	if (Url.IsEmpty()) {
		OnFail.Broadcast(nullptr, this);
		return;
	}
	
	FileUrl = Url;
	UrlHash = FMD5::HashAnsiString(*FileUrl);
	FileSavePath = s_strDir / s_strSubDir / UrlHash;

	//from memory
	std::map<FString, FString>::iterator it = s_mapHashPath.find(UrlHash);
	if (it != s_mapHashPath.end()) {
		FString PathName = it->second;
		UTexture2D* pTexture = LoadObject<UTexture2D>(nullptr, *PathName);
		if (pTexture) {
			//UE_LOG(LogImageDownloader, Log, TEXT("from memory %s %x"), *PathName, (uint64)pTexture);
			OnSuccess.Broadcast(pTexture, this);
			return;
		}
		s_mapHashPath.erase(it);
	}

	//from disk
	UTexture2D* pTexture = GetTexture2DFromDisk(FileSavePath, InvalidImageFormat);
	if (pTexture) {
		FString PathName = pTexture->GetPathName();
		s_mapHashPath[UrlHash] = PathName;
		//UE_LOG(LogImageDownloader, Log, TEXT("from disk %s"), *PathName);
		OnSuccess.Broadcast(pTexture, this);
		return;
	}
	if (InvalidImageFormat) {
		OnFail.Broadcast(nullptr, this);
		return;
	}

	//from http
	TSharedRef<IHttpRequest> HttpRequest = FHttpModule::Get().CreateRequest();
	HttpRequest->OnProcessRequestComplete().BindUObject(this, &UImageDownloader::HandleRequest);
	HttpRequest->SetURL(Url);
	HttpRequest->SetVerb(TEXT("GET"));
	if (HttpRequest->ProcessRequest()) {
		AddToRoot();
		return;
	}
	OnFail.Broadcast(nullptr, this);
	//UE_LOG(LogImageDownloader, Log, TEXT("HttpRequest->ProcessRequest error"));
}

void UImageDownloader::HandleRequest(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded)
{
	RemoveFromRoot();
	HttpRequest->OnProcessRequestComplete().Unbind();
	if (HttpResponse.IsValid() && EHttpResponseCodes::IsOk(HttpResponse->GetResponseCode())) {

		const TArray<uint8>& OutArray = HttpResponse->GetContent();

        if (HttpRequest.IsValid() &&
            !FileUrl.Equals(HttpRequest->GetURL()))
        {
            FileUrl = HttpRequest->GetURL();
            UrlHash = FMD5::HashAnsiString(*FileUrl);
            FileSavePath = s_strDir / s_strSubDir / UrlHash;
        }

		std::map<FString, FString>::iterator it = s_mapHashPath.find(UrlHash);
		if (it != s_mapHashPath.end()) {
			FString PathName = it->second;
			UTexture2D* pTexture = LoadObject<UTexture2D>(nullptr, *PathName);
			if (pTexture) {
				//UE_LOG(LogImageDownloader, Log, TEXT("from memory %s %x HandleRequest"), *PathName, (uint64)pTexture);
				OnSuccess.Broadcast(pTexture, this);
				return;
			}
		}

		if (SaveDiskFile) {
			IPlatformFile& PlatformFile = FPlatformFileManager::Get().GetPlatformFile();
			FString Path, Filename, Extension;
			FPaths::Split(FileSavePath, Path, Filename, Extension);
			if (!PlatformFile.DirectoryExists(*Path)) {
				PlatformFile.CreateDirectoryTree(*Path);
			}
			IFileHandle* FileHandle = PlatformFile.OpenWrite(*FileSavePath);
			if (FileHandle) {
				FileHandle->Write(OutArray.GetData(), OutArray.Num());
				delete FileHandle;
			}
		}

		UTexture2D* pTexture = GetTexture2DFromArray(OutArray, InvalidImageFormat);
		if (pTexture) {
			FString PathName = pTexture->GetPathName();
			s_mapHashPath[UrlHash] = PathName;
			//UE_LOG(LogImageDownloader, Log, TEXT("from http %s"), *PathName);
			OnSuccess.Broadcast(pTexture, this);
			return;
		}

	}
    if (HttpResponse.IsValid())
    {
        UE_LOG(LogTemp, Warning, TEXT("HttpResponse error code %d, url=[%s]"), HttpResponse->GetResponseCode(), *HttpResponse->GetURL());
    }
	OnFail.Broadcast(nullptr, this);
}
