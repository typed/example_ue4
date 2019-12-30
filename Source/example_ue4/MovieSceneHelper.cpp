// Fill out your copyright notice in the Description page of Project Settings.

#include "MovieSceneHelper.h"
#include "Runtime/MovieScene/Public/MovieScene.h"
#include "Runtime/UMG/Public/Animation/WidgetAnimation.h"
#include "Runtime/UMG/Public/Animation/MovieScene2DTransformSection.h"
#include "Runtime/MovieSceneTracks/Public/Tracks/MovieScenePropertyTrack.h"
#include "Runtime/MovieSceneTracks/Public/Sections/MovieSceneVectorSection.h"

static const FMovieSceneBinding* FindMovieSceneBinding(UWidgetAnimation* InAnimation, const FString& InName)
{
    if (InAnimation == nullptr || !InAnimation->IsValidLowLevel())
        return nullptr;
    UMovieScene* ms = InAnimation->GetMovieScene();
    if (ms == nullptr || !ms->IsValidLowLevel())
        return nullptr;
    const TArray<FMovieSceneBinding>& ar_msb = ms->GetBindings();
    for (int32 i = 0; i < ar_msb.Num(); i++) {
        const FMovieSceneBinding& msb = ar_msb[i];
        if (msb.GetName() == InName) {
            return &msb;
        }
    }
    return nullptr;
}

static UMovieSceneTrack* FindMovieSceneTrack(const FMovieSceneBinding* InBinding, const FString& InName)
{
    if (InBinding == nullptr)
        return nullptr;
    const TArray<UMovieSceneTrack*>& ar_track = InBinding->GetTracks();
    for (int32 j = 0; j < ar_track.Num(); j++) {
        UMovieScenePropertyTrack* mst = Cast<UMovieScenePropertyTrack>(ar_track[j]);
        if (mst && mst->GetPropertyName().ToString() == InName) {
            return mst;
        }
    }
    return nullptr;
}

static UMovieSceneSection* FindMovieSceneSection(UMovieSceneTrack* InTrack, int32 section)
{
    if (InTrack == nullptr || !InTrack->IsValidLowLevel())
        return nullptr;
    const TArray<UMovieSceneSection*>& amss = InTrack->GetAllSections();
    if (amss.IsValidIndex(section))
        return amss[section];
    return nullptr;
}

static UMovieSceneSection* FindMovieSceneSection(UWidgetAnimation* InAnimation, const FString& InBindingName, EMovieSceneHelperProperty InProperty, int32 section)
{
    const FMovieSceneBinding* msb = FindMovieSceneBinding(InAnimation, InBindingName);
    if (msb == nullptr)
        return nullptr;
    FString InTrackName;
    switch (InProperty) {
    case EMovieSceneHelperProperty::TranslationX:
    case EMovieSceneHelperProperty::TranslationY:
    case EMovieSceneHelperProperty::Rotation:
    case EMovieSceneHelperProperty::ScaleX:
    case EMovieSceneHelperProperty::ScaleY:
        InTrackName = TEXT("RenderTransform");
        break;
    case EMovieSceneHelperProperty::PivotX:
    case EMovieSceneHelperProperty::PivotY:
        InTrackName = TEXT("RenderTransformPivot");
        break;
    }
    UMovieSceneTrack* mst = FindMovieSceneTrack(msb, InTrackName);
    if (mst == nullptr)
        return nullptr;
    return FindMovieSceneSection(mst, section);
}

static FRichCurve* FindRichCurve(UMovieSceneSection* mss, EMovieSceneHelperProperty InProperty)
{
    if (mss == nullptr)
        return nullptr;
    FRichCurve* p_rc = nullptr;
    if (mss->IsA(UMovieScene2DTransformSection::StaticClass())) {
        UMovieScene2DTransformSection* ms2dts = Cast<UMovieScene2DTransformSection>(mss);
        if (ms2dts) {
            switch (InProperty) {
            case EMovieSceneHelperProperty::TranslationX:
                p_rc = &ms2dts->GetTranslationCurve(EAxis::X);
                break;
            case EMovieSceneHelperProperty::TranslationY:
                p_rc = &ms2dts->GetTranslationCurve(EAxis::Y);
                break;
            case EMovieSceneHelperProperty::Rotation:
                p_rc = &ms2dts->GetRotationCurve();
                break;
            case EMovieSceneHelperProperty::ScaleX:
                p_rc = &ms2dts->GetScaleCurve(EAxis::X);
                break;
            case EMovieSceneHelperProperty::ScaleY:
                p_rc = &ms2dts->GetScaleCurve(EAxis::Y);
                break;
            }
        }
    }
    else if (mss->IsA(UMovieSceneVectorSection::StaticClass())) {
        UMovieSceneVectorSection* ms2dts = Cast<UMovieSceneVectorSection>(mss);
        if (ms2dts) {
            switch (InProperty) {
            case EMovieSceneHelperProperty::PivotX:
                p_rc = &ms2dts->GetCurve(0);
                break;
            case EMovieSceneHelperProperty::PivotY:
                p_rc = &ms2dts->GetCurve(1);
                break;
            }
        }
    }
    return p_rc;
}

void UMovieSceneHelper::UpdateAniKeyValue_Float(UWidgetAnimation* InAnimation, const FString& InBindingName,
    EMovieSceneHelperProperty InProperty, int32 section, float InTime, float v)
{
    UMovieSceneSection* mss = FindMovieSceneSection(InAnimation, InBindingName, InProperty, section);
    if (mss == nullptr)
        return;
    FRichCurve* p_rc = FindRichCurve(mss, InProperty);
    if (p_rc == nullptr)
        return;
    FKeyHandle kh = p_rc->FindKey(InTime);
    if (p_rc->IsKeyHandleValid(kh)) {
        p_rc->GetKey(kh).Value = v;
        mss->Modify();
    }
}

void UMovieSceneHelper::RemoveAniKey(UWidgetAnimation* InAnimation, const FString& InBindingName,
    EMovieSceneHelperProperty InProperty, int32 section, float InTime)
{
    UMovieSceneSection* mss = FindMovieSceneSection(InAnimation, InBindingName, InProperty, section);
    if (mss == nullptr)
        return;
    FRichCurve* p_rc = FindRichCurve(mss, InProperty);
    if (p_rc == nullptr)
        return;
    FKeyHandle kh = p_rc->FindKey(InTime);
    if (p_rc->IsKeyHandleValid(kh)) {
        p_rc->DeleteKey(kh);
        mss->Modify();
    }
}

void UMovieSceneHelper::UpdateOrAddAniKey_Float(UWidgetAnimation* InAnimation, const FString& InBindingName,
    EMovieSceneHelperProperty InProperty, int32 section, float InTime, float v)
{
    UMovieSceneSection* mss = FindMovieSceneSection(InAnimation, InBindingName, InProperty, section);
    if (mss == nullptr)
        return;
    FRichCurve* p_rc = FindRichCurve(mss, InProperty);
    if (p_rc == nullptr)
        return;
    p_rc->UpdateOrAddKey(InTime, v);
    mss->Modify();
}