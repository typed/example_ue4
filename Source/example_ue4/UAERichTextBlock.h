// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "SlateBasics.h"
#include "Fonts/SlateFontInfo.h"
#include "Components/Widget.h"
#include "SUTRichTextBlock.h"
#include "Framework/Text/SlateHyperlinkRun.h"
//#include "GameFrontendHUD.h"
#include "UAERichTextBlock.generated.h"

UCLASS(meta = (DisplayName = "UUTRichTextBlock (String)"), ClassGroup = UserInterface)
class UUTRichTextBlock : public UWidget
{
	GENERATED_BODY()

public:
	UUTRichTextBlock(const FObjectInitializer& ObjectInitializer);
	
	// UVisual interface
	virtual void ReleaseSlateResources(bool bReleaseChildren) override;

	UFUNCTION(BlueprintCallable, Category = "Widget")
	void SetText(FText InText);

	UFUNCTION(BlueprintCallable, Category = "Widget")
	FText GetText();

    //UFUNCTION(BlueprintCallable, Category = "Widget")
    //void SetGameFrontendHUD(UGameFrontendHUD* InHUD);

protected:
	/** The text to display */
	UPROPERTY(EditAnywhere, Category=Content, meta=( MultiLine="true" ))
	FString ContentText;

	FString CachContentText;
	/** The default font for the text. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Appearance)
	FSlateFontInfo Font;

	/**/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Appearance)
	bool bSupportHyLink = false;

	/**/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Appearance)
	bool bSupportImage = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Appearance)
	bool bIgnoreSnapping = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Appearance)
	bool bIgnoreStyle = false;

	/** The default color for the text. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Appearance)
	FLinearColor TextColor;

	/** How the text should be aligned with the margin. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Appearance)
	TEnumAsByte<ETextJustify::Type> Justification;

	/** True if we're wrapping text automatically based on the computed horizontal space for this widget */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Appearance)
	bool AutoWrapText = false;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Appearance)
	FScrollBarStyle ScrollBarStyle;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Appearance)
	FMargin HScrollBarPadding;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Appearance)
	FMargin VScrollBarPadding;


	/** Whether text wraps onto a new line when it's length exceeds this width; if this value is zero or negative, no wrapping occurs. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Appearance, AdvancedDisplay)
	float WrapTextAt = 0.0f;

	/** The amount of blank space left around the edges of text area. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Appearance, AdvancedDisplay)
	FMargin Margin;

	/** The amount to scale each lines height by. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Appearance, AdvancedDisplay)
	float LineHeightPercentage = 0.0f;

	/** Spacing between hyper's border and the content. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Appearance, AdvancedDisplay)
	FMargin HyperlinkPadding;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Appearance, AdvancedDisplay)
	FString HyperlinkDecoratorTag;


    /** 超链点击回调的函数名 **/
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Appearance, AdvancedDisplay)
    FString HyperlinkCallBackFunctionName;

    /** 如果此项有设置，则调此lua table内定义的函数;如果此项为空，那么将调lua全局函数 **/
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Appearance, AdvancedDisplay)
    FString HyperlinkCallBackTableName;
	

protected:
	FTextBlockStyle DefaultStyle;

	/** Native Slate Widget */
	TSharedPtr<SUTRichTextBlock> MyRichTextBlock;

	// UWidget interface
	virtual TSharedRef<SWidget> RebuildWidget() override;

    ///点击回调
    void OnAssetLinkClicked(const FSlateHyperlinkRun::FMetadata& Metadata);

    FSlateHyperlinkRun::FOnClick ClickDelegate;

    //UPROPERTY()
    //UGameFrontendHUD* GameFrontendHUD = NULL;
};
