// Fill out your copyright notice in the Description page of Project Settings.

#include "UTextBlockEllipsis.h"
#include "Runtime/UMG/Public/Components/TextBlock.h"
#include "Runtime/UMG/Public/Blueprint/WidgetBlueprintLibrary.h"
#include "Runtime/Engine/Classes/Kismet/KismetMathLibrary.h"
#include "UAERichTextBlock.h"

DEFINE_LOG_CATEGORY(LogUTextBlockEllipsis);

UTextBlockEllipsis::UTextBlockEllipsis(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer)
    , NeedBuildString(false)
    , Ticked(false)
{

}

bool UTextBlockEllipsis::Initialize()
{
    //UE_LOG(LogUTextBlockEllipsis, Log, TEXT("UTextBlockEllipsis::Initialize"));
    if (!Super::Initialize())
        return false;
    InitWidgetPtr();
    SyncProp();
#if WITH_EDITOR
    UWorld* wld = GetWorld();
    if (wld && !wld->IsGameWorld()) {
        TWeakObjectPtr<UTextBlockEllipsis> self = this;
        wld->GetTimerManager().SetTimer(OnTickTimerHandle, FTimerDelegate::CreateLambda([=]() {
            if (self.IsValid()) {
                self->OnMyTick();
            }
        }), 0.001f, true);
    }
#endif
    return true;
}

void UTextBlockEllipsis::NativeConstruct()
{
    //UE_LOG(LogUTextBlockEllipsis, Log, TEXT("UTextBlockEllipsis::NativeConstruct"));
    Super::NativeConstruct();
    InitWidgetPtr();
    SyncProp();
}

void UTextBlockEllipsis::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
    Super::NativeTick(MyGeometry, InDeltaTime);
    Ticked = true;
    OnMyTick();
}

void UTextBlockEllipsis::OnMyTick()
{
    const FVector2D& lzSz = GetCachedGeometry().GetLocalSize();
    if (!ViewSize.Equals(lzSz, 0.0001f)) {
        ViewSize = lzSz;
        NeedBuildString = true;
    }
    if (NeedBuildString) {
        NeedBuildString = false;
        BuildString();
    }
}

void UTextBlockEllipsis::ReleaseSlateResources(bool bReleaseChildren)
{
    //UE_LOG(LogUTextBlockEllipsis, Log, TEXT("UTextBlockEllipsis::ReleaseSlateResources"));
    Super::ReleaseSlateResources(bReleaseChildren);
#if WITH_EDITOR
    UWorld* wld = GetWorld();
    if (wld && !wld->IsGameWorld()) {
        if (OnTickTimerHandle.IsValid()) {
            wld->GetTimerManager().ClearTimer(OnTickTimerHandle);
        }
    }
#endif
}

void UTextBlockEllipsis::SynchronizeProperties()
{
    //UE_LOG(LogUTextBlockEllipsis, Log, TEXT("UTextBlockEllipsis::SynchronizeProperties"));
    Super::SynchronizeProperties();
    NeedBuildString = true;
}

#if WITH_EDITOR
void UTextBlockEllipsis::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
    //UE_LOG(LogUTextBlockEllipsis, Log, TEXT("UTextBlockEllipsis::PostEditChangeProperty"));
    Super::PostEditChangeProperty(PropertyChangedEvent);
    SyncProp();
}
#endif

void UTextBlockEllipsis::OnWidgetRebuilt()
{
    //UE_LOG(LogUTextBlockEllipsis, Log, TEXT("UTextBlockEllipsis::OnWidgetRebuilt"));
    Super::OnWidgetRebuilt();
    InitWidgetPtr();
    SyncProp();
}

void UTextBlockEllipsis::InitWidgetPtr()
{
    TextBlockMain = nullptr;
    RichTextBlockMain = nullptr;
    TextBlockMain = Cast<UTextBlock>(GetWidgetFromName(FName(TEXT("TextBlockMain"))));
    if (!TextBlockMain.IsValid()) {
        RichTextBlockMain = Cast<UUTRichTextBlock>(GetWidgetFromName(FName(TEXT("RichTextBlockMain"))));
    }
}

void UTextBlockEllipsis::SyncProp()
{
    NeedBuildString = true;
}

void UTextBlockEllipsis::SetText(FString InText)
{
    Content = InText;
    if (!Ticked) {
        NeedBuildString = true;
        return;
    }
    BuildString();
}

FString UTextBlockEllipsis::GetText() const
{
    return Content;
}

void UTextBlockEllipsis::BuildString()
{
    FVector2D lsz = GetCachedGeometry().GetLocalSize();
    if (lsz.Equals(FVector2D::ZeroVector, 0.0001f)) {
        ViewSize = lsz;
        return;
    }
    if (TextBlockMain.IsValid()) {
        BuildString_Normal();
    }
    else if (RichTextBlockMain.IsValid()) {
        BuildString_Rich();
    }
}

void UTextBlockEllipsis::BuildString_Normal()
{
    FVector2D lsz = GetCachedGeometry().GetLocalSize();

    const float lsz_Add_Y = 1.f;

    const FString Ellipsis = TEXT("...");

    //2分遍历
    int32 dir = 1;
    int32 last_dir = 0;
    const int32 len = Content.Len();
    int32 step = len / 2;
    step = step == 0 ? 1 : step;
    int32 num = step;
    int32 last_num = -1;
    bool compute_ellipsis = false;
    for (int32 i = 1;;++i) {
        if (last_num == num) {
            if (num >= Content.Len()) {
                //到尾了
                TextBlockMain->SetText(FText::FromString(Content));
                break;
            }
            else if (num <= 1) {
                //到头了
                TextBlockMain->SetText(FText::FromString(TEXT("")));
                break;
            }
            else {
                num += dir;
            }
        }
        FString tmp2 = Content.Mid(0, num);
        TextBlockMain->SetText(FText::FromString(tmp2));
        ForceLayoutPrepass();
        FVector2D sz = GetDesiredSize();
        dir = (sz.X > lsz.X || sz.Y > lsz.Y + lsz_Add_Y) ? -1 : 1;
        if (last_dir != 0 && last_dir != dir && step == 1) {
            //变方向了，结束循环
            compute_ellipsis = true;
            break;
        }
        last_num = num;
        last_dir = dir;
        step = (step / 2 <= 0 ? 1 : step / 2);
        num += dir * step;
        num = FMath::Clamp(num, 1, len);
        //UE_LOG(LogUTextBlockEllipsis, Log, TEXT("UTextBlockEllipsis::SetText %d x=%f y=%f lx=%f ly=%f"), i, sz.X, sz.Y, lsz.X, lsz.Y);
    }
    if (compute_ellipsis) {
        bool done = false;
        for (int32 i = num; i >= 0; --i) {
            FString tmp2 = Content.Mid(0, i);
            tmp2.Append(Ellipsis);
            TextBlockMain->SetText(FText::FromString(tmp2));
            ForceLayoutPrepass();
            FVector2D sz = GetDesiredSize();
            if (sz.X <= lsz.X && sz.Y <= lsz.Y + lsz_Add_Y) {
                done = true;
                break;
            }
        }
        if (!done) {
            TextBlockMain->SetText(FText::FromString(TEXT("")));
        }
    }
}

int32 UTextBlockEllipsis::Len_Rich()
{
    return Content_Rich.Len();
}

FString UTextBlockEllipsis::Mid_Rich(int32 count)
{
    int32 Offset = 0;
    int32 EndIdx = count - 1;
    FString ret;
    for (int32 i = 0; i < ArraySeqement.Num(); ++i) {
        const FSeqementItem& itm = ArraySeqement[i];
        if (itm.Begin < EndIdx) {
            if (itm.IsLabel) {
                ret.Append(itm.Content);
                ret.Append(Content_Rich.Mid(itm.Begin + 1, FMath::Min(itm.End, EndIdx) - itm.Begin));
                ret.Append("</>");
                Offset = itm.Begin;
            }
            else {
                ret.Append(itm.Content.Mid(0, FMath::Min(itm.Count, EndIdx - itm.Begin)));
            }
        }
    }
    return ret;
}

FString UTextBlockEllipsis::GetString_Rich()
{
    return Content_Rich;
}

void UTextBlockEllipsis::DetachTextAndLabel()
{
    Content_Rich.Empty();
    ArraySeqement.Empty();
    
    FSeqementItem itm;

    FRegexPattern ElementRegexPattern(TEXT("<([\\w\\d\\.]+)((?: (?:[\\w\\d\\.]+=(?>\".*?\")))+)?(?:(?:/>)|(?:>(.*?)</>))"));
    FRegexMatcher ElementRegexMatcher(ElementRegexPattern, Content);

    int32 ElementEndLast = 0;

    while (ElementRegexMatcher.FindNext()) {

        int32 ElementBegin = ElementRegexMatcher.GetMatchBeginning();
        int32 ElementEnd = ElementRegexMatcher.GetMatchEnding();

        FTextRange OriginalRange(ElementBegin, ElementEnd);

        // Capture Group 1 is the element name.
        int32 ElementNameBegin = ElementRegexMatcher.GetCaptureGroupBeginning(1);
        int32 ElementNameEnd = ElementRegexMatcher.GetCaptureGroupEnding(1);

        // Name
        FString ElementName = Content.Mid(ElementNameBegin, ElementNameEnd - ElementNameBegin);

        // Capture Group 2 is the attribute list.
        int32 AttributeListBegin = ElementRegexMatcher.GetCaptureGroupBeginning(2);
        int32 AttributeListEnd = ElementRegexMatcher.GetCaptureGroupEnding(2);

        FString AttributeStr = Content.Mid(AttributeListBegin, AttributeListEnd - AttributeListBegin);

        // Capture Group 3 is the content.
        int32 ElementContentBegin = ElementRegexMatcher.GetCaptureGroupBeginning(3);
        int32 ElementContentEnd = ElementRegexMatcher.GetCaptureGroupEnding(3);

        // Content
        FString ContentStr = Content.Mid(ElementContentBegin, ElementContentEnd - ElementContentBegin);

        FString str = Content.Mid(ElementEndLast, ElementBegin - ElementEndLast);
        if (str.Len() > 0) {
            itm.Content = str;
            itm.Begin = FMath::Max(0, Content_Rich.Len() - 1);
            itm.End = itm.Begin + str.Len();
            itm.Count = str.Len();
            itm.IsLabel = false;
            Content_Rich.Append(str);
            ArraySeqement.Add(itm);
        }

        itm.Begin = Content_Rich.Len()-1;
        itm.End = itm.Begin + ContentStr.Len();
        itm.Count = ContentStr.Len();
        itm.Content.Empty();
        itm.Content.Append("<");
        itm.Content.Append(ElementName);
        if (AttributeStr.Len() > 0) {
            itm.Content.Append(AttributeStr);
        }
        itm.Content.Append(">");
        itm.IsLabel = true;
        ArraySeqement.Add(itm);

        Content_Rich.Append(ContentStr);

        ElementEndLast = ElementEnd;

    }

    if (ElementEndLast < Content.Len()) {
        FString str = Content.Mid(ElementEndLast);
        itm.Content = str;
        itm.Begin = FMath::Max(0, Content_Rich.Len() - 1);
        itm.End = itm.Begin + str.Len();
        itm.Count = str.Len();
        itm.IsLabel = false;
        Content_Rich.Append(str);
        ArraySeqement.Add(itm);
    }

}

void UTextBlockEllipsis::BuildString_Rich()
{
    FVector2D lsz = GetCachedGeometry().GetLocalSize();

    DetachTextAndLabel();

    const float lsz_Add_Y = 1.f;

    const FString Ellipsis = TEXT("...");

    //2分遍历
    int32 dir = 1;
    int32 last_dir = 0;
    const int32 len = Len_Rich();
    int32 step = len / 2;
    step = step == 0 ? 1 : step;
    int32 num = step;
    int32 last_num = -1;
    bool compute_ellipsis = false;
    for (int32 i = 1;; ++i) {
        if (last_num == num) {
            if (num >= Len_Rich()) {
                //到尾了
                RichTextBlockMain->SetText(FText::FromString(Content));
                break;
            }
            else if (num <= 1) {
                //到头了
                RichTextBlockMain->SetText(FText::FromString(TEXT("")));
                break;
            }
            else {
                num += dir;
            }
        }
        FString tmp2 = Mid_Rich(num);
        RichTextBlockMain->SetText(FText::FromString(tmp2));
        ForceLayoutPrepass();
        FVector2D sz = GetDesiredSize();
        dir = (sz.X > lsz.X || sz.Y > lsz.Y + lsz_Add_Y) ? -1 : 1;
        if (last_dir != 0 && last_dir != dir && step == 1) {
            //变方向了，结束循环
            compute_ellipsis = true;
            break;
        }
        last_num = num;
        last_dir = dir;
        step = (step / 2 <= 0 ? 1 : step / 2);
        num += dir * step;
        num = FMath::Clamp(num, 1, len);
        //UE_LOG(LogUTextBlockEllipsis, Log, TEXT("UTextBlockEllipsis::SetText %d x=%f y=%f lx=%f ly=%f"), i, sz.X, sz.Y, lsz.X, lsz.Y);
    }
    if (compute_ellipsis) {
        bool done = false;
        for (int32 i = num; i >= 0; --i) {
            FString tmp2 = Mid_Rich(i);
            tmp2.Append(Ellipsis);
            RichTextBlockMain->SetText(FText::FromString(tmp2));
            ForceLayoutPrepass();
            FVector2D sz = GetDesiredSize();
            if (sz.X <= lsz.X && sz.Y <= lsz.Y + lsz_Add_Y) {
                done = true;
                break;
            }
        }
        if (!done) {
            RichTextBlockMain->SetText(FText::FromString(TEXT("")));
        }
    }
}

void UTextBlockEllipsis::Test()
{
    SetText(TEXT("阿萨<r a1=\"11\" a2=\"22\">小城之春</>\n萨<g>许昌</>任务德"));
}

