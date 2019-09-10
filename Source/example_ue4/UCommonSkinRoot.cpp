// Fill out your copyright notice in the Description page of Project Settings.

#include "UCommonSkinRoot.h"
#include "Runtime/UMG/Public/Components/CanvasPanel.h"
#include "Runtime/UMG/Public/Components/CanvasPanelSlot.h"

DEFINE_LOG_CATEGORY(LogUCommonSkinRoot);

UCommonSkinRoot::UCommonSkinRoot(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer)
    , CanvasPanelRoot(nullptr)
{
}

bool UCommonSkinRoot::Initialize()
{
    if (!Super::Initialize())
        return false;
    InitWidgetPtr();
    return true;
}

void UCommonSkinRoot::NativeConstruct()
{
    Super::NativeConstruct();
    InitWidgetPtr();
}

void UCommonSkinRoot::InitWidgetPtr()
{
    CanvasPanelRoot = Cast<UCanvasPanel>(GetWidgetFromName(FName(TEXT("CanvasPanelRoot"))));
    ensure(CanvasPanelRoot.IsValid());
}

void UCommonSkinRoot::OnWidgetRebuilt()
{
    Super::OnWidgetRebuilt();
    InitWidgetPtr();
}

UUserWidget* UCommonSkinRoot::LoadSkin(FString __WidgetPath)
{
    TWeakObjectPtr<UUserWidget> widget = GetAndCreateWidgetFromPool(__WidgetPath);
    if (!widget.IsValid())
        return nullptr;
    TWeakObjectPtr<UUserWidget> widget_src = Cast<UUserWidget>(CanvasPanelRoot->GetChildAt(0));
    if (widget_src.IsValid() && widget_src == widget)
        return widget_src.Get();
    CanvasPanelRoot->ClearChildren();
    CanvasPanelRoot->AddChild(widget.Get());
    UCanvasPanelSlot* cps = Cast<UCanvasPanelSlot>(widget->Slot);
    if (cps) {
        FMargin mar(0, 0, 0, 0);
        FAnchors ach(0, 0, 1, 1);
        cps->SetAnchors(ach);
        cps->SetOffsets(mar);
    }
    return widget.Get();
}

TWeakObjectPtr<UUserWidget> UCommonSkinRoot::GetAndCreateWidgetFromPool(const FString& __WidgetPath)
{
    TWeakObjectPtr<UUserWidget>* ptr_widget = SkinPool.Find(__WidgetPath);
    if (ptr_widget && (*ptr_widget).IsValid())
        return *ptr_widget;
    TSubclassOf<UUserWidget> uclass = ::LoadClass<UUserWidget>(nullptr, *__WidgetPath);
    if (uclass == nullptr)
        return nullptr;
    UUserWidget* widget = CreateWidget<UUserWidget>(GetWorld(), uclass);
    if (widget == nullptr) {
        //UE_LOG(LogUCommonSkinRoot, Error, TEXT("UCommonSkinRoot CreateUserWidget Widget == nullptr"));
        return nullptr;
    }
    //UE_LOG(LogUCommonSkinRoot, Log, TEXT("UCommonSkinRoot CreateUserWidget Widget:%x Name: %s"), widget, *(widget->GetName()));
    SkinPool.Add(__WidgetPath, widget);
    return widget;
}