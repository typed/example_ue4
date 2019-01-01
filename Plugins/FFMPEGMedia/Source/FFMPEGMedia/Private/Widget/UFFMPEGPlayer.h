// Fill out your copyright notice in the Description page of Project Settings.

/**************************************************************************
Author: levingong
Date: 2018-11-15
Description: FFMPEGPlayer UMG
**************************************************************************/

#pragma once

#include "CoreMinimal.h"
#include "Runtime/UMG/Public/Blueprint/UserWidget.h"
#include "UFFMPEGPlayer.generated.h"

struct AVIOContext;
struct AVFormatContext;
struct AVCodecContext;
struct AVCodec;
struct AVFrame;
struct AVPacket;
struct SwsContext;
class IFileHandle;
class UTexture2D;
class FRunnableThread;
class UImage;

/**
 * 
 */
UCLASS()
class UFFMPEGPlayer : public UUserWidget
{
	GENERATED_BODY()
	
public:

    UFFMPEGPlayer(const FObjectInitializer& ObjectInitializer);

    bool Initialize();

    UFUNCTION(BlueprintCallable)
    bool Play(FString Url);

    UFUNCTION(BlueprintCallable)
    void Stop();

    void NativeDestruct();
    void NativeTick(const FGeometry& MyGeometry, float InDeltaTime);

    void DoThread();



    bool                    m_stopped;
    UTexture2D*             m_pTexture;
    IFileHandle*            m_pFileHandle;
    AVFormatContext*        m_pAVFormatContext;
    AVCodecContext*         m_pAVCodecContext;
    AVCodec*                m_pAVCodec;
    int                     m_videoindex;
    FRunnableThread*        m_doThread;

    UImage*                 ImageRender;
    TArray<uint8>	        m_dataBuffer;
    bool                    m_bTextureRender;
	
};
