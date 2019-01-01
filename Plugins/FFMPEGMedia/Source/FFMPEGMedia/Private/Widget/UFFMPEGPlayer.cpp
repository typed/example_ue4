// Fill out your copyright notice in the Description page of Project Settings.

#include "UFFMPEGPlayer.h"
#include "FFMPEGMediaPrivate.h"
#include "HAL/PlatformFilemanager.h"
#include "FileHelper.h"
#include "Paths.h"
#include "LambdaFunctionRunnable.h"
#include "Runtime/Engine/Classes/Engine/Texture2D.h"
#include "Runtime/UMG/Public/Components/Image.h"

extern  "C" {
#include "libavformat/avformat.h"
#include "libavcodec/avcodec.h"
#include "libavutil/imgutils.h"
#include "libavutil/hwcontext.h"
#include "libavutil/time.h"
#include "libswscale/swscale.h"
#include "libswresample/swresample.h"
}

static int c_ff_input_buffer_padding_size = 32;

static int ReadFileCallback(void* opaque, uint8_t* buf, int buf_size)
{
    UFFMPEGPlayer* player = static_cast<UFFMPEGPlayer*>(opaque);
    if (player->m_pFileHandle == nullptr) {
        return 0;
    }
    int64 Size = player->m_pFileHandle->Size();
    int64 Position = player->m_pFileHandle->Tell();
    int64 BytesToRead = buf_size;
    if (BytesToRead > Size) {
        BytesToRead = Size;
    }
    if ((Position + BytesToRead) > Size) {
        BytesToRead = Size - Position;
    }
    if (BytesToRead <= 0) {
        return AVERROR_EOF;
    }
    if (player->m_pFileHandle->Read(buf, BytesToRead)) {
        //Position = player->m_pFileHandle->Tell();
        return BytesToRead;
    }
    return AVERROR_EOF;
}

static int64_t SeekFileCallback(void *opaque, int64_t offset, int whence)
{
    UFFMPEGPlayer* player = static_cast<UFFMPEGPlayer*>(opaque);
    if (player->m_pFileHandle == nullptr) {
        return -1;
    }
    int64 Size = player->m_pFileHandle->Size();
    if (whence == AVSEEK_SIZE) {
        return Size;
    }
    else if (whence == SEEK_SET) {
        if (offset <= Size) {
            if (player->m_pFileHandle->Seek(offset)) {
                int64 pos = player->m_pFileHandle->Tell();
                return pos;
            }
        }
        return -1;
    }
    else if (whence == SEEK_CUR) {
        int64 pos = player->m_pFileHandle->Tell();
        int64 to_pos = pos + offset;
        if (to_pos <= Size) {
            if (player->m_pFileHandle->Seek(to_pos)) {
                int64 pos1 = player->m_pFileHandle->Tell();
                return pos1;
            }
        }
    }
    else if (whence == SEEK_END) {
        return -1;
    }
    return -1;
}

static int DecodeInterruptCallback(void *ctx)
{
    UFFMPEGPlayer* player = static_cast<UFFMPEGPlayer*>(ctx);
    return player->m_stopped ? 1 : 0;
}

static void LogAVErr(int err)
{
    char errbuf[128];
    const char *errbuf_ptr = errbuf;
#if PLATFORM_WINDOWS
    if (av_strerror(err, errbuf, sizeof(errbuf)) < 0)
        strerror_s(errbuf, 128, AVUNERROR(err));
#else
    if (av_strerror(err, errbuf, sizeof(errbuf)) < 0)
        errbuf_ptr = strerror(AVUNERROR(err));
#endif
    UE_LOG(LogFFMPEGMedia, Error, TEXT("avformat_open_input error %s."), UTF8_TO_TCHAR(errbuf));
}

UFFMPEGPlayer::UFFMPEGPlayer(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer)
    , m_pTexture(nullptr)
    , m_pFileHandle(nullptr)
    , m_pAVFormatContext(nullptr)
    , m_pAVCodecContext(nullptr)
    , m_pAVCodec(nullptr)
    , m_videoindex(-1)
    , m_doThread(nullptr)
    , ImageRender(nullptr)
    , m_bTextureRender(false)
{

}

bool UFFMPEGPlayer::Initialize()
{
    if (!Super::Initialize())
        return false;
    ImageRender = Cast<UImage>(GetWidgetFromName(FName(TEXT("ImageRender"))));
    ensure(ImageRender);
    return true;
}

void UFFMPEGPlayer::NativeDestruct()
{
    Stop();
    m_pTexture = nullptr;
}

bool UFFMPEGPlayer::Play(FString Url)
{
    Stop();

    int err = 0;
    FString FilePath = FPaths::ProjectContentDir() + Url;
    IPlatformFile& PlatformFile = FPlatformFileManager::Get().GetPlatformFile();

    m_pFileHandle = PlatformFile.OpenRead(*FilePath);
    if (m_pFileHandle == nullptr) {
        UE_LOG(LogFFMPEGMedia, Error, TEXT("PlatformFile.OpenRead error FilePath=%s"), *FilePath);
        return false;
    }

    UE_LOG(LogFFMPEGMedia, Display, TEXT("PlatformFile.OpenRead ok FilePath=%s FileSize=%d"), *FilePath, (uint32)m_pFileHandle->Size());

    m_pAVFormatContext = avformat_alloc_context();

    m_pAVFormatContext->interrupt_callback.callback = DecodeInterruptCallback;
    m_pAVFormatContext->interrupt_callback.opaque = this;
    
    const int ioBufferSize = 32768;
    unsigned char * ioBuffer = (unsigned char *)av_malloc(ioBufferSize + c_ff_input_buffer_padding_size);
    AVIOContext* pIOContext = avio_alloc_context(ioBuffer, ioBufferSize, 0, this, ReadFileCallback, NULL, SeekFileCallback);
    m_pAVFormatContext->pb = pIOContext;
    m_pAVFormatContext->flags = AVFMT_FLAG_CUSTOM_IO;
    err = avformat_open_input(&m_pAVFormatContext, "", NULL, NULL);
    if (err < 0) {
        LogAVErr(err);
        Stop();
        return false;
    }

    err = avformat_find_stream_info(m_pAVFormatContext, NULL);
    if (err < 0) {
        LogAVErr(err);
        Stop();
        return false;
    }

    m_videoindex = av_find_best_stream(m_pAVFormatContext, AVMEDIA_TYPE_VIDEO, -1, -1, NULL, 0);
    if (m_videoindex == -1) {
        UE_LOG(LogFFMPEGMedia, Error, TEXT("Didn't find a video stream."));
        return false;
    }

    m_pAVCodecContext = avcodec_alloc_context3(NULL);
    if (m_pAVCodecContext == nullptr) {
        UE_LOG(LogFFMPEGMedia, Error, TEXT("avcodec_alloc_context3 m_pAVCodecContext == nullptr."));
        return false;
    }

    err = avcodec_parameters_to_context(m_pAVCodecContext, m_pAVFormatContext->streams[m_videoindex]->codecpar);
    if (err < 0) {
        UE_LOG(LogFFMPEGMedia, Error, TEXT("avcodec_parameters_to_context error."));
        return false;
    }

    //m_pAVCodecContext->pkt_timebase = m_pAVFormatContext->streams[m_videoindex]->time_base;

    //m_pAVCodec = avcodec_find_decoder(m_pAVCodecContext->codec_id);
    //if (m_pAVCodec == NULL) {
    //    UE_LOG(LogFFMPEGMedia, Error, TEXT("Codec not found."));
    //    return false;
    //}
    //err = avcodec_open2(m_pAVCodecContext, m_pAVCodec, NULL);
    //if (err < 0) {
    //    UE_LOG(LogFFMPEGMedia, Error, TEXT("Could not open codec."));
    //    return false;
    //}

    AVPixelFormat fmt = AV_PIX_FMT_BGRA;

    //Output Info-----------------------------
    //printf("--------------- File Information ----------------\n");
    //av_dump_format(m_pAVFormatContext, 0, TCHAR_TO_UTF8(*FilePath), 0);
    //printf("-------------------------------------------------\n");

    if (m_pTexture == nullptr || (m_pTexture->GetSurfaceWidth() != m_pAVCodecContext->width || m_pTexture->GetSurfaceHeight() != m_pAVCodecContext->height)) {
        m_pTexture = UTexture2D::CreateTransient(m_pAVCodecContext->width, m_pAVCodecContext->height, PF_B8G8R8A8);
    }
    ImageRender->SetBrushFromTexture(m_pTexture);

    m_stopped = false;

    m_doThread = LambdaFunctionRunnable::RunThreaded(TEXT("DoThread"), [this] {
        DoThread();
    });

    return true;
}

void UFFMPEGPlayer::Stop()
{
    m_stopped = true;
    m_doThread = nullptr;
    m_bTextureRender = false;
    if (m_pAVCodecContext) {
        avcodec_free_context(&m_pAVCodecContext);
        m_pAVCodecContext = nullptr;
    }
    if (m_pAVFormatContext) {
        avformat_close_input(&m_pAVFormatContext);
        avformat_free_context(m_pAVFormatContext);
        m_pAVFormatContext = nullptr;
    }
    if (m_pFileHandle) {
        delete m_pFileHandle;
        m_pFileHandle = nullptr;
    }
}

void UFFMPEGPlayer::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
    Super::NativeTick(MyGeometry, InDeltaTime);
    //FVector2D szView = MyGeometry.GetLocalSize();


    if (m_pTexture && m_bTextureRender) {
        void* TextureData = m_pTexture->PlatformData->Mips[0].BulkData.Lock(LOCK_READ_WRITE);
        FMemory::Memcpy(TextureData, m_dataBuffer.GetData(), m_dataBuffer.Num());
        m_pTexture->PlatformData->Mips[0].BulkData.Unlock();
        m_pTexture->UpdateResource();
    }
}

static int decode(AVCodecContext *avctx, AVFrame *frame, int *got_frame, AVPacket *pkt)
{
    int ret;

    *got_frame = 0;

    if (pkt) {
        ret = avcodec_send_packet(avctx, pkt);
        // In particular, we don't expect AVERROR(EAGAIN), because we read all
        // decoded frames with avcodec_receive_frame() until done.
        if (ret < 0)
            return ret == AVERROR_EOF ? 0 : ret;
    }

    ret = avcodec_receive_frame(avctx, frame);
    if (ret < 0 && ret != AVERROR(EAGAIN) && ret != AVERROR_EOF)
        return ret;
    if (ret >= 0)
        *got_frame = 1;

    return 0;
}

void UFFMPEGPlayer::DoThread()
{
    SwsContext* pSwsContext = nullptr;
    //AVPacket* pAVPacket = av_packet_alloc();
    //AVFrame* pAVFrame = av_frame_alloc();
    AVPixelFormat fmt = AV_PIX_FMT_BGRA;
    int ret;
    int got_frame;
    for (;;) {
        if (m_stopped) {
            break;
        }
        AVFrame frm;
        AVPacket pkt;
        ret = av_read_frame(m_pAVFormatContext, &pkt);
        if (ret < 0) {
            continue;
        }
        if (pkt.stream_index == m_videoindex) {
            //decode(m_pAVCodecContext, &frm, &got_frame, &pkt);
            ret = avcodec_decode_video2(m_pAVCodecContext, &frm, &got_frame, &pkt);
            if (got_frame == 1) {

                if (pkt.stream_index == m_videoindex) {

                    int size = av_image_get_buffer_size(fmt, frm.width, frm.height, 1);

                    int bufSize = size + 1;

                    if (m_dataBuffer.Num() != bufSize) {
                        m_dataBuffer.Reset();
                        m_dataBuffer.AddUninitialized(bufSize);
                        //check(m_dataBuffer.Num() == bufSize);
                    }

                    int pitch[4] = { 0, 0, 0, 0 };
                    ret = av_image_fill_linesizes(pitch, fmt, frm.width);

                    uint8_t* data[4] = { 0 };
                    ret = av_image_fill_pointers(data, fmt, frm.height, m_dataBuffer.GetData(), pitch);

                    pSwsContext = sws_getContext(
                        frm.width, frm.height, (AVPixelFormat)frm.format,
                        frm.width, frm.height, fmt,
                        SWS_BICUBIC, NULL, NULL, NULL);

                    ret = sws_scale(pSwsContext, frm.data, frm.linesize, 0, frm.height, data, pitch);
                    //UE_LOG(LogFFMPEGMedia, Error, TEXT("sws_scale ret=%d."), ret);

                    m_bTextureRender = true;

                }
            }
            //av_packet_unref(&pkt);
        }
    }

//out:
    //av_packet_free(&pAVPacket);
    //av_frame_free(&pAVFrame);
}