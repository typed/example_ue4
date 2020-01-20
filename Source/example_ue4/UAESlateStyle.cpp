// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

#include "UAESlateStyle.h"
#include "SlateBasics.h"
#include "Slate/SlateGameResources.h"
//#include "UTLoadedClassManager.h"

TSharedPtr<FSlateStyleSet> STFSlateStyle::TFSlateStyleInstance = NULL;

void STFSlateStyle::Initialize()
{
	if (!TFSlateStyleInstance.IsValid())
	{
		TFSlateStyleInstance = Create();
		FSlateStyleRegistry::UnRegisterSlateStyle(TFSlateStyleInstance.Get()->GetStyleSetName());
		FSlateStyleRegistry::RegisterSlateStyle( *TFSlateStyleInstance);
	}
}

void STFSlateStyle::Shutdown()
{
	if (TFSlateStyleInstance.IsValid())
	{
		FSlateStyleRegistry::UnRegisterSlateStyle(*TFSlateStyleInstance.Get());
	}
	
}

static bool ParsFstringToColor(FString& SrcText, FLinearColor& OutColor)
{
    if (SrcText.Len() > 0)
    {
        TArray<FString> ColorStrList;
        SrcText.ParseIntoArray(ColorStrList, TEXT(";"), true);
        if (ColorStrList.Num() == 4)
        {
            OutColor.R = FCString::Atof(*ColorStrList[0]) / 255.0f;
            OutColor.G = FCString::Atof(*ColorStrList[1]) / 255.0f;
            OutColor.B = FCString::Atof(*ColorStrList[2]) / 255.0f;
            OutColor.A = FCString::Atof(*ColorStrList[3]) / 255.0f;
            return true;
        }
    }
    return false;
}

static FVector2D IconSize[EUIIconSize::UIIconSize_Max] =
{
    FVector2D(8.0f, 8.0f),
    FVector2D(16.0f, 16.0f),
    FVector2D(22.0f, 22.0f),
    FVector2D(64.0f, 64.0f)
};

bool STFSlateStyle::SetImageStyle(FString StyleName, int ImageSize, FString ImagePath, FString ImageColor)
{
    if (TFSlateStyleInstance.IsValid())
    {
        FSlateStyleSet* Style = TFSlateStyleInstance.Get();

        FLinearColor LineImageColor(FLinearColor::White);
        ParsFstringToColor(ImageColor, LineImageColor);
        UObject* pImage = //NULL;
            LoadObject<UObject>((UObject*)GetTransientPackage(), *ImagePath);
        if (pImage)
        {   
            FVector2D Size(32.0f, 32.0f);
            if (ImageSize >= 16 && ImageSize <= 128)
            {
                Size.X = (float)ImageSize;
                Size.Y = (float)ImageSize;
            }
            else if (ImageSize >= 1 && ImageSize <= (int)EUIIconSize::UIIconSize_Max)
            {
                Size = IconSize[ImageSize - 1];
            }
            FName StyleNameName = *StyleName;
            ///避免图片被垃圾回收掉
            pImage->AddToRoot();
#if 0
            FSlateImageBrush* pSlateImage = new FSlateImageBrush(TEXT(""), Size, LineImageColor);

            pSlateImage->SetResourceObject(pImage);
            
            Style->Set(StyleNameName, pSlateImage);
#else
            FInlineTextImageStyle ImgStyle = FInlineTextImageStyle().
                SetImage(FSlateImageBrush(
                    pImage
                    , Size, LineImageColor)).
                SetBaseline(0);
            Style->Set(StyleNameName, ImgStyle);
#endif
            return true;
        }   
    }

    return false;
}

bool STFSlateStyle::SetFontStyle(FString StyleName, int FontSize, FString FontPath, FString FontColor, bool UseShadow)
{
    if (TFSlateStyleInstance.IsValid())
    {
        FSlateStyleSet* Style = TFSlateStyleInstance.Get();
        
        FLinearColor LineFontColor(FLinearColor::White);
        ParsFstringToColor(FontColor, LineFontColor);
        FTextBlockStyle NormalRichTextStyle = FTextBlockStyle()
            .SetFont(
                FSlateFontInfo(
                    LoadObject<UObject>((UObject*)GetTransientPackage(), *FontPath),
                    FontSize)
                )
            .SetColorAndOpacity(FSlateColor::UseForeground())
            .SetShadowOffset(FVector2D::ZeroVector)
            .SetShadowColorAndOpacity(FLinearColor::Black)
            .SetColorAndOpacity(LineFontColor);

        if (UseShadow)
        {
            FVector2D offset(1.0f, 1.5f);
            NormalRichTextStyle.SetShadowOffset(offset);
        }
        FName StyleNameName = *StyleName;
        Style->Set(StyleNameName, NormalRichTextStyle);

        return true;
     }

    return false;
}

bool STFSlateStyle::SetLinkStyle(FString StyleName, int FontSize, FString FontPath, FString FontColor, bool ShowUnderline)
{
    if (TFSlateStyleInstance.IsValid())
    {
        FSlateStyleSet* Style = TFSlateStyleInstance.Get();

        FLinearColor LineFontColor(FLinearColor::White);
        ParsFstringToColor(FontColor, LineFontColor);

        FTextBlockStyle NormalRichTextStyle = FTextBlockStyle()
            .SetFont(
                FSlateFontInfo(
                    LoadObject<UObject>((UObject*)GetTransientPackage(), *FontPath),
                    FontSize)
            ).SetColorAndOpacity(FSlateColor::UseForeground())
            .SetShadowOffset(FVector2D::ZeroVector)
            .SetShadowColorAndOpacity(FLinearColor::Black)
            .SetColorAndOpacity(LineFontColor);

        ///超链默认不显示下划线
        FButtonStyle DarkHyperlinkButton = FButtonStyle()
            .SetNormal(FSlateNoResource())
            .SetPressed(FSlateNoResource())
            .SetHovered(FSlateNoResource());

        
        if (ShowUnderline)
        {
            static const FMargin NormalFMargin(0, 0, 0, 3 / 16.0f);
            static const FString  PathHyperLinkNormalBrush = TEXT("/Game/UMG/Texture/TF_HyperlinkDotted.TF_HyperlinkDotted");
            static const FString PathHyperLinkHoveredrBrush = TEXT("/Game/UMG/Texture/TF_HyperlinkUnderline.TF_HyperlinkUnderline");

            UObject* pImage = 
                LoadObject<UObject>((UObject*)GetTransientPackage(), *PathHyperLinkNormalBrush);
            FSlateBorderBrush SlateImageNormal(TEXT(""), NormalFMargin, LineFontColor);
            if (pImage)
            {
                ///避免图片被垃圾回收掉
                pImage->AddToRoot();
                SlateImageNormal.SetResourceObject(pImage);
                DarkHyperlinkButton.
                    SetNormal(SlateImageNormal);
            }

            pImage = 
                LoadObject<UObject>((UObject*)GetTransientPackage(), *PathHyperLinkHoveredrBrush);
            FSlateBorderBrush SlateImageHovered(TEXT(""), NormalFMargin, LineFontColor);
            if (pImage)
            {
                ///避免图片被垃圾回收掉
                pImage->AddToRoot();
                SlateImageHovered.SetResourceObject(pImage);
                DarkHyperlinkButton.
                    SetHovered(SlateImageHovered);
            }   
        }

        FHyperlinkStyle DarkHyperlink = FHyperlinkStyle()
            .SetUnderlineStyle(DarkHyperlinkButton)
            .SetTextStyle(NormalRichTextStyle)
            .SetPadding(FMargin(0.0f));
        FName StyleNameName = *StyleName;

        Style->Set(StyleNameName, DarkHyperlink);

        return true;
    }

    return false;
}

FName STFSlateStyle::GetStyleSetName()
{
	static FName StyleSetName(TEXT("UAEWindowsStyle"));
	return StyleSetName;
}

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION

TSharedRef< FSlateStyleSet > STFSlateStyle::Create()
{
	TSharedRef< FSlateStyleSet > Style = MakeShareable( new FSlateStyleSet("SlateStyle") );

	Style->SetContentRoot(FPaths::ProjectContentDir() / TEXT("RestrictedAssets/UI"));
	Style->SetCoreContentRoot(FPaths::EngineContentDir() / TEXT("Slate"));

	return Style;
}

END_SLATE_FUNCTION_BUILD_OPTIMIZATION

#undef IMAGE_BRUSH
#undef BOX_BRUSH
#undef BORDER_BRUSH
#undef TTF_FONT
#undef OTF_FONT


void STFSlateStyle::Reload()
{
	FSlateApplication::Get().GetRenderer()->ReloadTextureResources();
}

const ISlateStyle& STFSlateStyle::Get()
{
	return * TFSlateStyleInstance;
}
