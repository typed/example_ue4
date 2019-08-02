// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.
#pragma once

#include "CoreMinimal.h"
#include "Styling/ISlateStyle.h"

enum EUIIconSize
{
	UIIconSize_None,
	UIIconSize_8X8,
	UIIconSize_16X16,
	UIIconSize_32X32,
	UIIconSize_64X64,
	UIIconSize_Max,
};

class STFSlateStyle 
{
public:
	static void Initialize();
	static void Shutdown();
	static void Reload();

    static bool SetImageStyle(FString StyleName, int ImageSize, FString ImagePath, FString ImageColor);
    static bool SetFontStyle(FString StyleName, int FontSize, FString FontPath, FString FontColor, bool UseShadow);
    static bool SetLinkStyle(FString StyleName, int FontSize, FString FontPath, FString FontColor, bool ShowUnderline);
	
	static const ISlateStyle& Get();
	static FName GetStyleSetName();

private:
	static TSharedRef<class FSlateStyleSet> Create();
	static TSharedPtr<class FSlateStyleSet> TFSlateStyleInstance;
};
