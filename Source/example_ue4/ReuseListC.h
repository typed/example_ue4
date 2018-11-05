// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Runtime/UMG/Public/UMG.h"
#include "ReuseListC.generated.h"

/**
 * 
 */

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnUpdateItemDelegate, UUserWidget*, Widget, int32, Idx);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnScrollItemDelegate, int32, BeginIdx, int32, EndIdx);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCreateItemDelegate, UUserWidget*, widget);

UCLASS()
class EXAMPLE_UE4_API UReuseListC : public UUserWidget
{
	GENERATED_BODY()
	
public:
    UReuseListC(const FObjectInitializer& ObjectInitializer);

    UPROPERTY(BlueprintAssignable)
    FOnUpdateItemDelegate OnUpdateItem;

    UPROPERTY(BlueprintAssignable)
    FOnScrollItemDelegate OnScrollItem;

    UPROPERTY(BlueprintAssignable)
    FOnCreateItemDelegate OnCreateItem;

    UFUNCTION(BlueprintCallable)
    virtual void Reload(int32 __ItemCount, int32 __ItemHeight, int32 __ItemWidth, int32 __Style, UClass* __Class, int32 __PaddingX, int32 __PaddingY, bool __ReloadJumpBegin = true);

    UFUNCTION(BlueprintCallable)
    virtual void Refresh();

    UFUNCTION(BlueprintCallable)
    virtual void RefreshOne(int32 __Idx);

    UFUNCTION(BlueprintCallable)
    virtual void JumpByIdx(int32 __Idx, int32 __Style);

    UFUNCTION(BlueprintCallable)
    virtual void Clear();

    UFUNCTION(BlueprintCallable)
    virtual int32 GetCurrentBegin();

    UFUNCTION(BlueprintCallable)
    virtual int32 GetCurrentEnd();

    virtual bool Initialize();

protected:

    virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime);

    virtual void ScrollUpdate(float __Offset);
    virtual void UpdateContentSize(UWidget* widget);
    virtual void RemoveNotUsed();
    virtual void DoReload();
    virtual UUserWidget* NewItem();
    virtual void ReleaseItem(UUserWidget* __Item);
    virtual void Update();
    virtual void DoJump();

private:

    TWeakObjectPtr<UScrollBox> ScrollBoxList;
    TWeakObjectPtr<UCanvasPanel> CanvasPanelBg;
    TWeakObjectPtr<USizeBox> SizeBoxBg;
    TWeakObjectPtr<UCanvasPanel> CanvasPanelList;
    
    FVector2D ViewSize;
    FVector2D ContentSize;
    TWeakObjectPtr<UClass> ItemClass;
    int32 ItemCacheNum;
    int32 ItemCount;
    int32 ItemHeight;
    int32 MaxPos;
    int32 Offset;
    TMap<int32, TWeakObjectPtr<UUserWidget> > ItemMap;
    int32 OffsetEnd;
    TArray< TWeakObjectPtr<UUserWidget> > ItemPool;
    int32 Style;
    int32 ItemWidth;
    int32 BIdx;
    int32 EIdx;
    int32 ColNum;
    int32 RowNum;
    int32 CurLine;
    int32 PaddingX;
    int32 PaddingY;
    int32 JumpIdx;
    int32 JumpIdxStyle;
    bool ReloadJumpBegin;
    bool NeedJump;

};