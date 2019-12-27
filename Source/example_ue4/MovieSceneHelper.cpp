// Fill out your copyright notice in the Description page of Project Settings.

#include "MovieSceneHelper.h"
#include "Runtime/MovieScene/Public/MovieScene.h"
#include "Runtime/UMG/Public/Animation/WidgetAnimation.h"
#include "Runtime/UMG/Public/Animation/MovieScene2DTransformSection.h"
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
        UMovieSceneTrack* mst = ar_track[j];
        if (mst->GetDisplayName().ToString() == InName) {
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

void UMovieSceneHelper::UpdateAniKeyValue_Float(UWidgetAnimation* InAnimation, const FString& InBindingName,
    const FString& InTrackName, EMovieSceneHelperProperty InProperty, float InTime, float v, int32 section)
{
    const FMovieSceneBinding* msb = FindMovieSceneBinding(InAnimation, InBindingName);
    if (msb == nullptr)
        return;
    UMovieSceneTrack* mst = FindMovieSceneTrack(msb, InTrackName);
    if (mst == nullptr)
        return;
    UMovieSceneSection* mss = FindMovieSceneSection(mst, section);
    if (mss == nullptr)
        return;
    if (mss->IsA(UMovieScene2DTransformSection::StaticClass())) {
        UMovieScene2DTransformSection* ms2dts = Cast<UMovieScene2DTransformSection>(mss);
        if (ms2dts) {
            FRichCurve* p_rc = nullptr;
            if (InProperty == EMovieSceneHelperProperty::TranslationX) {
                p_rc = &ms2dts->GetTranslationCurve(EAxis::X);
            }
            else if (InProperty == EMovieSceneHelperProperty::TranslationY) {
                p_rc = &ms2dts->GetTranslationCurve(EAxis::Y);
            }
            else if (InProperty == EMovieSceneHelperProperty::Rotation) {
                p_rc = &ms2dts->GetRotationCurve();
            }
            else if (InProperty == EMovieSceneHelperProperty::ScaleX) {
                p_rc = &ms2dts->GetScaleCurve(EAxis::X);
            }
            else if (InProperty == EMovieSceneHelperProperty::ScaleY) {
                p_rc = &ms2dts->GetScaleCurve(EAxis::Y);
            }
            if (p_rc) {
                FKeyHandle kh = p_rc->FindKey(InTime);
                if (p_rc->IsKeyHandleValid(kh)) {
                    p_rc->GetKey(kh).Value = v;
                    ms2dts->Modify();
                }
            }
        }
    }
    else if (mss->IsA(UMovieSceneVectorSection::StaticClass())) {
        UMovieSceneVectorSection* ms2dts = Cast<UMovieSceneVectorSection>(mss);
        if (ms2dts) {
            FRichCurve* p_rc = nullptr;
            if (InProperty == EMovieSceneHelperProperty::PivotX) {
                p_rc = &ms2dts->GetCurve(0);
            }
            else if (InProperty == EMovieSceneHelperProperty::PivotY) {
                p_rc = &ms2dts->GetCurve(1);
            }
            if (p_rc) {
                FKeyHandle kh = p_rc->FindKey(InTime);
                if (p_rc->IsKeyHandleValid(kh)) {
                    p_rc->GetKey(kh).Value = v;
                    ms2dts->Modify();
                }
            }
        }
    }
}

void UMovieSceneHelper::RemoveAniKey(UWidgetAnimation* InAnimation, const FString& InBindingName,
    const FString& InTrackName, EMovieSceneHelperProperty InProperty, float InTime, int32 section)
{
    const FMovieSceneBinding* msb = FindMovieSceneBinding(InAnimation, InBindingName);
    if (msb == nullptr)
        return;
    UMovieSceneTrack* mst = FindMovieSceneTrack(msb, InTrackName);
    if (mst == nullptr)
        return;
    UMovieSceneSection* mss = FindMovieSceneSection(mst, section);
    if (mss == nullptr)
        return;
    if (mss->IsA(UMovieScene2DTransformSection::StaticClass())) {
        UMovieScene2DTransformSection* ms2dts = Cast<UMovieScene2DTransformSection>(mss);
        if (ms2dts) {
            FRichCurve* p_rc = nullptr;
            if (InProperty == EMovieSceneHelperProperty::TranslationX) {
                p_rc = &ms2dts->GetTranslationCurve(EAxis::X);
            }
            else if (InProperty == EMovieSceneHelperProperty::TranslationY) {
                p_rc = &ms2dts->GetTranslationCurve(EAxis::Y);
            }
            else if (InProperty == EMovieSceneHelperProperty::Rotation) {
                p_rc = &ms2dts->GetRotationCurve();
            }
            else if (InProperty == EMovieSceneHelperProperty::ScaleX) {
                p_rc = &ms2dts->GetScaleCurve(EAxis::X);
            }
            else if (InProperty == EMovieSceneHelperProperty::ScaleY) {
                p_rc = &ms2dts->GetScaleCurve(EAxis::Y);
            }
            if (p_rc) {
                FKeyHandle kh = p_rc->FindKey(InTime);
                if (p_rc->IsKeyHandleValid(kh)) {
                    p_rc->DeleteKey(kh);
                    ms2dts->Modify();
                }
            }
        }
    }
    else if (mss->IsA(UMovieSceneVectorSection::StaticClass())) {
        UMovieSceneVectorSection* ms2dts = Cast<UMovieSceneVectorSection>(mss);
        if (ms2dts) {
            FRichCurve* p_rc = nullptr;
            if (InProperty == EMovieSceneHelperProperty::PivotX) {
                p_rc = &ms2dts->GetCurve(0);
            }
            else if (InProperty == EMovieSceneHelperProperty::PivotY) {
                p_rc = &ms2dts->GetCurve(1);
            }
            if (p_rc) {
                FKeyHandle kh = p_rc->FindKey(InTime);
                if (p_rc->IsKeyHandleValid(kh)) {
                    p_rc->DeleteKey(kh);
                    ms2dts->Modify();
                }
            }
        }
    }
}

void UMovieSceneHelper::UpdateOrAddAniKey_Float(UWidgetAnimation* InAnimation, const FString& InBindingName,
    const FString& InTrackName, EMovieSceneHelperProperty InProperty, float InTime, float v, int32 section)
{
    const FMovieSceneBinding* msb = FindMovieSceneBinding(InAnimation, InBindingName);
    if (msb == nullptr)
        return;
    UMovieSceneTrack* mst = FindMovieSceneTrack(msb, InTrackName);
    if (mst == nullptr)
        return;
    UMovieSceneSection* mss = FindMovieSceneSection(mst, section);
    if (mss == nullptr)
        return;
    if (mss->IsA(UMovieScene2DTransformSection::StaticClass())) {
        UMovieScene2DTransformSection* ms2dts = Cast<UMovieScene2DTransformSection>(mss);
        if (ms2dts) {
            FRichCurve* p_rc = nullptr;
            if (InProperty == EMovieSceneHelperProperty::TranslationX) {
                p_rc = &ms2dts->GetTranslationCurve(EAxis::X);
            }
            else if (InProperty == EMovieSceneHelperProperty::TranslationY) {
                p_rc = &ms2dts->GetTranslationCurve(EAxis::Y);
            }
            else if (InProperty == EMovieSceneHelperProperty::Rotation) {
                p_rc = &ms2dts->GetRotationCurve();
            }
            else if (InProperty == EMovieSceneHelperProperty::ScaleX) {
                p_rc = &ms2dts->GetScaleCurve(EAxis::X);
            }
            else if (InProperty == EMovieSceneHelperProperty::ScaleY) {
                p_rc = &ms2dts->GetScaleCurve(EAxis::Y);
            }
            if (p_rc) {
                p_rc->UpdateOrAddKey(InTime, v);
                ms2dts->Modify();
            }
        }
    }
    else if (mss->IsA(UMovieSceneVectorSection::StaticClass())) {
        UMovieSceneVectorSection* ms2dts = Cast<UMovieSceneVectorSection>(mss);
        if (ms2dts) {
            FRichCurve* p_rc = nullptr;
            if (InProperty == EMovieSceneHelperProperty::PivotX) {
                p_rc = &ms2dts->GetCurve(0);
            }
            else if (InProperty == EMovieSceneHelperProperty::PivotY) {
                p_rc = &ms2dts->GetCurve(1);
            }
            if (p_rc) {
                p_rc->UpdateOrAddKey(InTime, v);
                ms2dts->Modify();
            }
        }
    }
}