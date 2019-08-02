// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.

#include "UAERichTextBlock.h"
#include "UAESlateStyle.h"
#include "SHyperlink.h"
#include "UObject/ConstructorHelpers.h"
#include "Engine/Font.h"
#if WITH_LUA_PLUGIN
#include "LuaStateWrapper.h"
#endif

#define LOCTEXT_NAMESPACE "UMG"


UUTRichTextBlock::UUTRichTextBlock(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
    static ConstructorHelpers::FObjectFinder<UFont> RobotoFontObj(TEXT("/Engine/EngineFonts/Roboto"));
    Font = FSlateFontInfo(RobotoFontObj.Object, 24, FName("Bold"));
               
	TextColor = FLinearColor::White;
	LineHeightPercentage = 1;
	bSupportHyLink = true;
	bSupportImage = false;
	HyperlinkDecoratorTag = TEXT("HyperlinkDecorator");

    HyperlinkCallBackFunctionName = TEXT("OnHyperLinkClicked");
}

//void UUTRichTextBlock::SetGameFrontendHUD(UGameFrontendHUD* InHUD)
//{
//    GameFrontendHUD = InHUD;
//}

void UUTRichTextBlock::OnAssetLinkClicked(const FSlateHyperlinkRun::FMetadata& Metadata)
{
    //if (GameFrontendHUD != NULL && GameFrontendHUD->GetLuaStateWrapper() &&
    //    !HyperlinkCallBackFunctionName.IsEmpty())
    //{
    //    if (HyperlinkCallBackTableName.IsEmpty())
    //    {
    //        GameFrontendHUD->GetLuaStateWrapper()->CallGlobalFunction(
    //            HyperlinkCallBackFunctionName, Metadata);
    //    }
    //    else
    //    {
    //        GameFrontendHUD->GetLuaStateWrapper()->CallTableFunction(
    //            HyperlinkCallBackTableName, HyperlinkCallBackFunctionName, Metadata);
    //    }
    //    
    //}
}

void UUTRichTextBlock::SetText(FText InText)
{
	ContentText = InText.ToString();
	CachContentText = ContentText;
	if (MyRichTextBlock.IsValid())
	{
		MyRichTextBlock->SetText(FText::FromString(CachContentText));
	}
	
}

FText UUTRichTextBlock::GetText()
{
	if (MyRichTextBlock.IsValid())
	{
		return MyRichTextBlock->GetText();
	}
	return FText();
}

void UUTRichTextBlock::ReleaseSlateResources(bool bReleaseChildren)
{
	Super::ReleaseSlateResources(bReleaseChildren);
	MyRichTextBlock.Reset();
}

TSharedRef<SWidget> UUTRichTextBlock::RebuildWidget()
{
    STFSlateStyle::Initialize();
    if (!ClickDelegate.IsBound())
    {
        ClickDelegate.BindUObject(this, &UUTRichTextBlock::OnAssetLinkClicked);
    }
	DefaultStyle.SetFont(Font);
	DefaultStyle.SetColorAndOpacity(TextColor);
	CachContentText = ContentText;
	if (bSupportHyLink&&bSupportImage)
	{
		MyRichTextBlock =
			SNew(SUTRichTextBlock)
			.Text(FText::FromString(CachContentText))
			.Justification(Justification)
			.AutoWrapText(AutoWrapText)
			.WrapTextAt(WrapTextAt)
			.Margin(Margin)
			.LineHeightPercentage(LineHeightPercentage)
			.TextStyle(&DefaultStyle)
			.IgnoreSnapping(bIgnoreSnapping)
			.IgnoreStyleSet(bIgnoreStyle)
			.DecoratorStyleSet(&STFSlateStyle::Get())			
			+ SRichTextBlock::ImageDecorator()
			+ SRichTextBlock::HyperlinkDecorator(*HyperlinkDecoratorTag,
                ClickDelegate
            );
	}
	else  if (bSupportImage)
	{
		MyRichTextBlock =
			SNew(SUTRichTextBlock)
			.Text(FText::FromString(CachContentText))
			.Justification(Justification)
			.AutoWrapText(AutoWrapText)
			.WrapTextAt(WrapTextAt)
			.Margin(Margin)
			.LineHeightPercentage(LineHeightPercentage)
			.TextStyle(&DefaultStyle)
			.IgnoreSnapping(bIgnoreSnapping)
			.IgnoreStyleSet(bIgnoreStyle)
			.DecoratorStyleSet(&STFSlateStyle::Get())
			+ SRichTextBlock::ImageDecorator();
	}
	else  if (bSupportHyLink)
	{
		MyRichTextBlock =
			SNew(SUTRichTextBlock)
			.Text(FText::FromString(CachContentText))
			.Justification(Justification)
			.AutoWrapText(AutoWrapText)
			.WrapTextAt(WrapTextAt)
			.Margin(Margin)
			.LineHeightPercentage(LineHeightPercentage)
			.TextStyle(&DefaultStyle)
			.IgnoreSnapping(bIgnoreSnapping)
			.IgnoreStyleSet(bIgnoreStyle)
			.DecoratorStyleSet(&STFSlateStyle::Get())
			+ SRichTextBlock::HyperlinkDecorator(*HyperlinkDecoratorTag,
                ClickDelegate
            );
	}
	 else
	 {
		 MyRichTextBlock =
			 SNew(SUTRichTextBlock)
			 .Text(FText::FromString(CachContentText))
			 .Justification(Justification)
			 .AutoWrapText(AutoWrapText)
			 .WrapTextAt(WrapTextAt)
			 .Margin(Margin)
			 .LineHeightPercentage(LineHeightPercentage)
			 .TextStyle(&DefaultStyle)
			 .IgnoreSnapping(bIgnoreSnapping)
			 .IgnoreStyleSet(bIgnoreStyle)
			 .DecoratorStyleSet(&STFSlateStyle::Get());
	 }
	return MyRichTextBlock.ToSharedRef();
}





/////////////////////////////////////////////////////

#undef LOCTEXT_NAMESPACE
