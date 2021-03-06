// Fill out your copyright notice in the Description page of Project Settings.

#include "UWrapGroupBox.h"

DEFINE_LOG_CATEGORY(LogUWrapGroupBox);

UWrapGroupBox::UWrapGroupBox(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer)
    , ItemClass(nullptr)
    , LastItemClass(nullptr)
    , PreviewCount(5)
{
}

void UWrapGroupBox::Reload(int32 __ItemCount)
{
    if (ItemClass == nullptr) {
        return;
    }
    for (int32 i = 0; i < GetChildrenCount(); i++) {
        UWidget* w = GetChildAt(i);
        if (w) {
            w->SetVisibility(ESlateVisibility::Collapsed);
        }
    }
    for (int32 i = 0; i < __ItemCount; i++) {
        UUserWidget* w = Cast<UUserWidget>(GetChildAt(i));
        if (w == nullptr) {
            w = CreateWidget<UUserWidget>(GetWorld(), ItemClass);
            if (w) {
                AddChild(w);
            }
        }
        if (w) {
            w->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
            OnUpdateItem.Broadcast(w, i);
        }
    }
}

void UWrapGroupBox::Reset(TSubclassOf<UUserWidget> __ItemClass)
{
    ClearChildren();
    ItemClass = __ItemClass;
    LastItemClass = __ItemClass;
}

void UWrapGroupBox::SynchronizeProperties()
{
    Super::SynchronizeProperties();
#if WITH_EDITOR
    if (LastItemClass != ItemClass) {
        LastItemClass = ItemClass;
        ClearChildren();
    }
    Reload(PreviewCount);
#endif
}
