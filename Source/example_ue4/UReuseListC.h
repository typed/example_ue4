// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Runtime/UMG/Public/UMG.h"
#include "UReuseListC.generated.h"

/**
 * 
 */

UENUM(BlueprintType)
enum class EReuseListStyle : uint8
{
    Vertical,
    Horizontal,
    VerticalGrid,
};

UENUM(BlueprintType)
enum class EReuseListJumpStyle : uint8
{
    Middle,
    Begin,
    End,
};

UCLASS()
class EXAMPLE_UE4_API UReuseListC : public UUserWidget
{
	GENERATED_BODY()
	
public:

    DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnUpdateItemDelegate, UUserWidget*, Widget, int32, Idx);
    DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnScrollItemDelegate, int32, BeginIdx, int32, EndIdx);
    DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCreateItemDelegate, UUserWidget*, widget);
    DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnDestroyItemDelegate, UUserWidget*, widget);

    UReuseListC(const FObjectInitializer& ObjectInitializer);

    virtual bool Initialize();

    UPROPERTY(BlueprintAssignable)
    FOnUpdateItemDelegate OnUpdateItem;

    UPROPERTY(BlueprintAssignable)
    FOnScrollItemDelegate OnScrollItem;

    UPROPERTY(BlueprintAssignable)
    FOnCreateItemDelegate OnCreateItem;

    UPROPERTY(BlueprintAssignable)
    FOnDestroyItemDelegate OnDestroyItem;

    UFUNCTION(BlueprintCallable)
    virtual void Reload(int32 __ItemCount);

    UFUNCTION(BlueprintCallable)
    virtual void Refresh();

    UFUNCTION(BlueprintCallable)
    virtual void RefreshOne(int32 __Idx);

    UFUNCTION(BlueprintCallable)
    virtual void JumpByIdx(int32 __Idx, EReuseListJumpStyle __Style);

    UFUNCTION(BlueprintCallable)
    virtual void Clear();

    UFUNCTION(BlueprintCallable)
    virtual int32 GetCurrentBegin();

    UFUNCTION(BlueprintCallable)
    virtual int32 GetCurrentEnd();

    UFUNCTION(BlueprintCallable)
    virtual void ClearCache();

    //~ Begin UWidget Interface
    virtual void SynchronizeProperties() override;
    //~ End UWidget Interface

protected:

    UPROPERTY(EditAnywhere, Category = Property)
    int32 ItemCacheNum;

    UPROPERTY(EditAnywhere, Category = Property)
    int32 ItemWidth;

    UPROPERTY(EditAnywhere, Category = Property)
    int32 ItemHeight;

    UPROPERTY(EditAnywhere, Category = Property)
    int32 PaddingX;

    UPROPERTY(EditAnywhere, Category = Property)
    int32 PaddingY;

    UPROPERTY(EditAnywhere, Category = Property)
    EReuseListStyle Style;

    UPROPERTY(EditAnywhere, Category = Property)
    UWidgetBlueprintGeneratedClass* ItemClass;

    UPROPERTY(EditAnywhere, Category = Property)
    int32 TestCount;

    /** Function called after the underlying SWidget is constructed. */
    virtual void OnWidgetRebuilt();

    virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime);

    virtual void ScrollUpdate(float __Offset);
    virtual void UpdateContentSize(UWidget* widget);
    virtual void RemoveNotUsed();
    virtual void DoReload();
    virtual UUserWidget* NewItem();
    virtual void ReleaseItem(UUserWidget* __Item);
    virtual void Update();
    virtual void DoJump();

    UFUNCTION()
    void OnCallReload();

private:

    bool IsValidClass() const;

    UScrollBox* ScrollBoxList;
    UCanvasPanel* CanvasPanelBg;
    USizeBox* SizeBoxBg;
    UCanvasPanel* CanvasPanelList;
    
    FVector2D ViewSize;
    FVector2D ContentSize;
    
    int32 ItemCount;
    int32 MaxPos;
    int32 Offset;
    TMap<int32, UUserWidget*> ItemMap;
    int32 OffsetEnd;
    TArray<UUserWidget*> ItemPool;
    int32 BIdx;
    int32 EIdx;
    int32 ColNum;
    int32 RowNum;
    int32 CurLine;
    int32 JumpIdx;
    EReuseListJumpStyle JumpStyle;
    bool NeedJump;

};