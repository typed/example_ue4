// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.
#include "SUTRichTextBlock.h"
#include "UAERichTextBlockLayout.h"
#include "RichTextLayoutMarshaller.h"
#include "Framework/Text/IRichTextMarkupParser.h"
#include "Types/SlateConstants.h"
#include "Regex.h"

DECLARE_CYCLE_STAT( TEXT("OnPaint SUTRichTextBlock"), STAT_SlateOnPaint_SUTRichTextBlock, STATGROUP_Slate );

TSharedRef< FUTRichTextMarkupParser > FUTRichTextMarkupParser::Create()
{
	return MakeShareable(new FUTRichTextMarkupParser());
}

void FUTRichTextMarkupParser::Process(TArray<FTextLineParseResults>& Results, const FString& Input, FString& Output)
{
#if UE_ENABLE_ICU
	TArray<FTextRange> LineRanges;
	FTextRange::CalculateLineRangesFromString(Input, LineRanges);
	ParseLineRanges(Input, LineRanges, Results);
	HandleEscapeSequences(Input, Results, Output);
#else
	Output = TEXT("WARNING: Rich text formatting is disabled.");
	FTextLineParseResults FakeLineParseResults(FTextRange(0, Output.Len()));
	FTextRunParseResults FakeRunParseResults(TEXT(""), FTextRange(0, Output.Len()));
	FakeLineParseResults.Runs.Add(FakeRunParseResults);
	Results.Add(FakeLineParseResults);
#endif
}

void FUTRichTextMarkupParser::ParseLineRanges(const FString& Input, const TArray<FTextRange>& LineRanges, TArray<FTextLineParseResults>& LineParseResultsArray) const
{
	// Special regular expression pattern for matching rich text markup elements. IE: <ElementName AttributeName="AttributeValue">Content</>
	FRegexPattern ElementRegexPattern(TEXT("<([\\w\\d\\.]+)((?: (?:[\\w\\d\\.]+=(?>\".*?\")))+)?(?:(?:/>)|(?:>(.*?)</>))"));
	FRegexMatcher ElementRegexMatcher(ElementRegexPattern, Input);

	// Parse line ranges, creating line parse results and run parse results.
	for (int32 i = 0; i < LineRanges.Num(); ++i)
	{
		FTextLineParseResults LineParseResults;
		LineParseResults.Range = LineRanges[i];

		// Limit the element regex matcher to the current line.
		ElementRegexMatcher.SetLimits(LineParseResults.Range.BeginIndex, LineParseResults.Range.EndIndex);

		// Iterate through the line, each time trying to find a match for the element regex, adding it as a run and any intervening text as another run.
		int32 LastRunEnd = LineParseResults.Range.BeginIndex;
		while (ElementRegexMatcher.FindNext())
		{
			int32 ElementBegin = ElementRegexMatcher.GetMatchBeginning();
			int32 ElementEnd = ElementRegexMatcher.GetMatchEnding();

			FTextRange OriginalRange(ElementBegin, ElementEnd);

			// Capture Group 1 is the element name.
			int32 ElementNameBegin = ElementRegexMatcher.GetCaptureGroupBeginning(1);
			int32 ElementNameEnd = ElementRegexMatcher.GetCaptureGroupEnding(1);

			// Name
			FString ElementName = Input.Mid(ElementNameBegin, ElementNameEnd - ElementNameBegin);

			// Capture Group 2 is the attribute list.
			int32 AttributeListBegin = ElementRegexMatcher.GetCaptureGroupBeginning(2);
			int32 AttributeListEnd = ElementRegexMatcher.GetCaptureGroupEnding(2);

			// Capture Group 3 is the content.
			int32 ElementContentBegin = ElementRegexMatcher.GetCaptureGroupBeginning(3);
			int32 ElementContentEnd = ElementRegexMatcher.GetCaptureGroupEnding(3);

			FTextRange ContentRange(ElementContentBegin, ElementContentEnd);

			TMap<FString, FTextRange> Attributes;

			if (AttributeListBegin != INDEX_NONE && AttributeListEnd != INDEX_NONE)
			{
				FRegexPattern AttributeRegexPattern(
					TEXT("([\\w\\d\\.]+)=(?>\"(.*?)\")")
					);

				FRegexMatcher AttributeRegexMatcher(AttributeRegexPattern, Input);
				AttributeRegexMatcher.SetLimits(AttributeListBegin, AttributeListEnd);

				// Iterate through the attribute list, each time trying to find a match for the attribute regex.
				while (AttributeRegexMatcher.FindNext())
				{
					// Capture Group 1 is the attribute key.
					int32 AttributeKeyBegin = AttributeRegexMatcher.GetCaptureGroupBeginning(1);
					int32 AttributeKeyEnd = AttributeRegexMatcher.GetCaptureGroupEnding(1);

					// Capture Group 2 is the attribute value.
					int32 AttributeValueBegin = AttributeRegexMatcher.GetCaptureGroupBeginning(2);
					int32 AttributeValueEnd = AttributeRegexMatcher.GetCaptureGroupEnding(2);

					// Attribute
					Attributes.Add(Input.Mid(AttributeKeyBegin, AttributeKeyEnd - AttributeKeyBegin), FTextRange(AttributeValueBegin, AttributeValueEnd));
				}
			}

			// Add intervening run to line.
			FTextRange InterveningRunRange(LastRunEnd, ElementBegin);
			if (!InterveningRunRange.IsEmpty())
			{
				FTextRunParseResults InterveningRunParseResults(FString(), InterveningRunRange);
				//LastRunEnd = ElementBegin;
				LineParseResults.Runs.Add(InterveningRunParseResults);
			}

			// Add element run to line.
			FTextRunParseResults RunParseResults(ElementName, OriginalRange, ContentRange);
			RunParseResults.MetaData = Attributes;
			LineParseResults.Runs.Add(RunParseResults);
			LastRunEnd = ElementEnd;
		}

		// Add dangling run to line.
		FTextRange InterveningRunRange(LastRunEnd, LineParseResults.Range.EndIndex);
		if (!InterveningRunRange.IsEmpty())
		{
			FTextRunParseResults InterveningRunParseResults(FString(), InterveningRunRange);
			//LastRunEnd = LineParseResults.Range.EndIndex;
			LineParseResults.Runs.Add(InterveningRunParseResults);
		}

		// Add blank, empty run if none are present.
		if (LineParseResults.Runs.Num() == 0)
		{
			FTextRunParseResults EmptyRunParseResults(FString(), LineParseResults.Range);
			LastRunEnd = EmptyRunParseResults.OriginalRange.EndIndex;
			LineParseResults.Runs.Add(EmptyRunParseResults);
		}

		LineParseResultsArray.Add(LineParseResults);
	}
}

void FUTRichTextMarkupParser::HandleEscapeSequences(const FString& Input, TArray<FTextLineParseResults>& LineParseResultsArray, FString& ConcatenatedUnescapedLines) const
{
	// Modify original string to handle escape sequences that need to be replaced while updating run ranges.
	for (int32 i = 0; i < LineParseResultsArray.Num(); ++i)
	{
		FTextLineParseResults& LineParseResults = LineParseResultsArray[i];

		// Adjust begin indices for previous substitutions.
		LineParseResults.Range.BeginIndex = ConcatenatedUnescapedLines.Len();

		for (int32 j = 0; j < LineParseResults.Runs.Num(); ++j)
		{
			FTextRunParseResults& RunParseResults = LineParseResults.Runs[j];

			struct FUnescapeHelper
			{
				TArray<FString> EscapeSequences;
				TArray<FString> UnescapedCharacters;

				FUnescapeHelper()
				{
					EscapeSequences.Add(TEXT("quot"));	UnescapedCharacters.Add(TEXT("\""));
					EscapeSequences.Add(TEXT("lt"));		UnescapedCharacters.Add(TEXT("<"));
					EscapeSequences.Add(TEXT("gt"));		UnescapedCharacters.Add(TEXT(">"));
					EscapeSequences.Add(TEXT("amp"));		UnescapedCharacters.Add(TEXT("&"));
				}

			} static const UnescapeHelper;

			struct FEscapeSequenceRegexPatternString
			{
				// Generate a regular expression pattern string that matches each of the escape sequences as alternatives, each in its own capture group.
				static FString Get(const TArray<FString>& EscapeSequences)
				{
					FString EscapeSequenceRegexPatternString;

					for (const FString& EscapeSequence : EscapeSequences)
					{
						// Add alternation operator to regex.
						if (!(EscapeSequenceRegexPatternString.IsEmpty()))
						{
							EscapeSequenceRegexPatternString += TEXT("|");
						}

						// Add capture group for escape sequence.
						EscapeSequenceRegexPatternString += TEXT("(");
						EscapeSequenceRegexPatternString += TEXT("&");
						EscapeSequenceRegexPatternString += EscapeSequence;
						EscapeSequenceRegexPatternString += TEXT(";");
						EscapeSequenceRegexPatternString += TEXT(")");
					}

					return EscapeSequenceRegexPatternString;
				}
			};

			FRegexPattern EscapeSequenceRegexPattern(
				FEscapeSequenceRegexPatternString::Get(UnescapeHelper.EscapeSequences)
				);

			FRegexMatcher EscapeSequenceRegexMatcher(EscapeSequenceRegexPattern, Input);

			TArray<int32*> IndicesToUpdate;
			IndicesToUpdate.Add(&RunParseResults.OriginalRange.BeginIndex);
			for (TPair<FString, FTextRange>& Pair : RunParseResults.MetaData)
			{
				IndicesToUpdate.Add(&Pair.Value.BeginIndex);
				IndicesToUpdate.Add(&Pair.Value.EndIndex);
			}
			if (RunParseResults.ContentRange.BeginIndex != INDEX_NONE && RunParseResults.ContentRange.EndIndex != INDEX_NONE)
			{
				IndicesToUpdate.Add(&RunParseResults.ContentRange.BeginIndex);
				IndicesToUpdate.Add(&RunParseResults.ContentRange.EndIndex);
			}
			IndicesToUpdate.Add(&RunParseResults.OriginalRange.EndIndex);

			{
				const auto GetUnescapedString = [&]()
				{
					int32 LastCopiedIndex = EscapeSequenceRegexMatcher.GetBeginLimit();
					while (EscapeSequenceRegexMatcher.FindNext())
					{
						// Copy intervening characters between the end of the last copy and the beginning of this match.
						ConcatenatedUnescapedLines += Input.Mid(LastCopiedIndex, EscapeSequenceRegexMatcher.GetMatchBeginning() - LastCopiedIndex);
						LastCopiedIndex = EscapeSequenceRegexMatcher.GetMatchBeginning();

						// Identify which escape sequence was captured based on which capture group has a valid range.
						for (int32 k = 0; k < UnescapeHelper.EscapeSequences.Num(); ++k)
						{
							const int32 GroupOrdinal = 1 + k; // Groups are accessed by ordinal, not index.
							const int32 EscapeSequenceBeginIndex = EscapeSequenceRegexMatcher.GetCaptureGroupBeginning(GroupOrdinal);
							const int32 EscapeSequenceEndIndex = EscapeSequenceRegexMatcher.GetCaptureGroupEnding(GroupOrdinal);

							// Lookup and copy unescaped character in place of the escape sequence.
							if (EscapeSequenceBeginIndex != INDEX_NONE && EscapeSequenceEndIndex != INDEX_NONE)
							{
								ConcatenatedUnescapedLines += UnescapeHelper.UnescapedCharacters[k];
								break;
							}
						}

						LastCopiedIndex = EscapeSequenceRegexMatcher.GetMatchEnding();
					}

					// Copy intervening characters between the end of the last copy and the end of the run.
					ConcatenatedUnescapedLines += Input.Mid(LastCopiedIndex, EscapeSequenceRegexMatcher.GetEndLimit() - LastCopiedIndex);
				};

				int32 k;
				for (k = 0; k + 1 < IndicesToUpdate.Num(); ++k)
				{
					EscapeSequenceRegexMatcher.SetLimits(*(IndicesToUpdate[k]), *(IndicesToUpdate[k + 1]));
					*(IndicesToUpdate[k]) = ConcatenatedUnescapedLines.Len();
					GetUnescapedString();
				}
				*(IndicesToUpdate[k]) = ConcatenatedUnescapedLines.Len();
			}
		}

		// Adjust end indices for previous substitutions.
		LineParseResults.Range.EndIndex = ConcatenatedUnescapedLines.Len();
	}
}


void SUTRichTextBlock::Construct( const FArguments& InArgs )
{
	BoundText = InArgs._Text;
	HighlightText = InArgs._HighlightText;

	TextStyle = *InArgs._TextStyle;
	TAttribute<FMargin> Padding = InArgs._Padding.IsSet() ? InArgs._Padding : FMargin(4.0f,2.0f);
	WrapTextAt = InArgs._WrapTextAt;
	AutoWrapText = InArgs._AutoWrapText;
	Margin = InArgs._Margin;
	LineHeightPercentage = InArgs._LineHeightPercentage;
	Justification = InArgs._Justification;
	ScrollOffset = FVector2D::ZeroVector;	

	{
		TSharedPtr<IRichTextMarkupParser> Parser = InArgs._Parser;
		if (!Parser.IsValid())
		{
			Parser = FUTRichTextMarkupParser::Create();
		}

		TSharedRef<FRichTextLayoutMarshaller> Marshaller = FRichTextLayoutMarshaller::Create(Parser, nullptr, InArgs._Decorators, InArgs._DecoratorStyleSet);
		for (const TSharedRef< ITextDecorator >& Decorator : InArgs.InlineDecorators)
		{
			Marshaller->AppendInlineDecorator(Decorator);
		}

		TextLayoutCache = FUTRichTextBlockLayout::Create(TextStyle, Marshaller, nullptr);
	}

	TSharedPtr<SWidget> HScrollBarWidget;
	HScrollBar = InArgs._HScrollBar;
	if (!HScrollBar.IsValid())
	{
		// Create and use our own scrollbar
		HScrollBar = SNew(SScrollBar)
			.Style(&InArgs._Style->ScrollBarStyle)
			.Orientation(Orient_Horizontal)
			.AlwaysShowScrollbar(InArgs._AlwaysShowScrollbars)
			.Thickness(FVector2D(5.0f, 5.0f));
		HScrollBarWidget = HScrollBar;
		HScrollBar->SetOnUserScrolled(FOnUserScrolled::CreateSP(this, &SUTRichTextBlock::OnHScrollBarMoved));
	}
	else
	{
		// User provided an external scrollbar, use a null widget
		HScrollBarWidget = SNullWidget::NullWidget;
	}


	TSharedPtr<SWidget> VScrollBarWidget;
	VScrollBar = InArgs._VScrollBar;
	if (!VScrollBar.IsValid())
	{
		// Create and use our own scrollbar
		VScrollBar = SNew(SScrollBar)
			.Style(&InArgs._Style->ScrollBarStyle)
			.Orientation(Orient_Vertical)
			.AlwaysShowScrollbar(InArgs._AlwaysShowScrollbars)
			.Thickness(FVector2D(5.0f, 5.0f));
		VScrollBarWidget = VScrollBar;
		VScrollBar->SetOnUserScrolled(FOnUserScrolled::CreateSP(this, &SUTRichTextBlock::OnVScrollBarMoved));
	}
	else
	{
		// User provided an external scrollbar, use a null widget
		VScrollBarWidget = SNullWidget::NullWidget;
	}

	if (TextLayoutCache.IsValid())
	{
		TextLayoutCache->SetIgnoreSnapping(InArgs._IgnoreSnapping.Get());
		TextLayoutCache->SetIgnoreStyleSet(InArgs._IgnoreStyleSet.Get());
	}
}

int32 SUTRichTextBlock::OnPaint( const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyClippingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& InWidgetStyle, bool bParentEnabled) const
{
	SCOPE_CYCLE_COUNTER( STAT_SlateOnPaint_SUTRichTextBlock );

	// OnPaint will also update the text layout cache if required
	LayerId = TextLayoutCache->OnPaint(
		FUTRichTextBlockLayout::FWidgetArgs(BoundText, HighlightText, WrapTextAt, AutoWrapText, Margin, LineHeightPercentage, Justification), 
		Args.WithNewParent(this), AllottedGeometry, MyClippingRect, OutDrawElements, LayerId, InWidgetStyle, 
		ShouldBeEnabled(bParentEnabled));

	FGeometry BarPosition(FVector2D(AllottedGeometry.GetDrawSize().X - 20.f,0.f), AllottedGeometry.AbsolutePosition, FVector2D(20.f, AllottedGeometry.Size.Y), 1.0f);
	VScrollBar->OnPaint(Args, BarPosition, MyClippingRect, OutDrawElements, LayerId, InWidgetStyle, bParentEnabled);
	BarPosition = FGeometry(FVector2D(0.f,AllottedGeometry.GetDrawSize().Y - 20.f), AllottedGeometry.AbsolutePosition, FVector2D(AllottedGeometry.Size.X,20.f), 1.0f);
	HScrollBar->OnPaint(Args, BarPosition, MyClippingRect, OutDrawElements, LayerId, InWidgetStyle, bParentEnabled);

	return LayerId;
}

FVector2D SUTRichTextBlock::ComputeDesiredSize(float) const
{
	// todo: jdale - The scale needs to be passed to ComputeDesiredSize
	//const float Scale = CachedScale;

	// ComputeDesiredSize will also update the text layout cache if required
	const FVector2D TextSize = TextLayoutCache->ComputeDesiredSize(
		FUTRichTextBlockLayout::FWidgetArgs(BoundText, HighlightText, WrapTextAt, AutoWrapText, Margin, LineHeightPercentage, Justification), 
		/*Scale, */TextStyle
		);

	return TextSize;
}

FChildren* SUTRichTextBlock::GetChildren()
{
	return &ChildSlot;//TextLayoutCache->GetChildren();
}

void SUTRichTextBlock::OnArrangeChildren(const FGeometry& AllottedGeometry, FArrangedChildren& ArrangedChildren) const
{
	TextLayoutCache->ArrangeChildren(AllottedGeometry, ArrangedChildren);
}

void SUTRichTextBlock::SetText(const TAttribute<FText>& InTextAttr)
{
	BoundText = InTextAttr;
}

void SUTRichTextBlock::SetHighlightText(const TAttribute<FText>& InHighlightText)
{
	HighlightText = InHighlightText;
}

void SUTRichTextBlock::Refresh()
{
	TextLayoutCache->DirtyLayout();
}

void SUTRichTextBlock::OnHScrollBarMoved(const float InScrollOffsetFraction)
{
	ScrollOffset.X = FMath::Clamp<float>(InScrollOffsetFraction, 0.0, 1.0) * GetDesiredSize().X;
}

void SUTRichTextBlock::OnVScrollBarMoved(const float InScrollOffsetFraction)
{
	ScrollOffset.Y = FMath::Clamp<float>(InScrollOffsetFraction, 0.0, 1.0) * GetDesiredSize().Y;
}

FReply SUTRichTextBlock::OnMouseWheel(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
	if (VScrollBar->IsNeeded())
	{
		const float PreviousScrollOffset = ScrollOffset.Y;

		const float ScrollAmount = -MouseEvent.GetWheelDelta() * GetGlobalScrollAmount();
		ScrollOffset.Y += ScrollAmount;

		const float ContentSize = TextLayoutCache->TextLayout->GetSize().Y;
		const float ScrollMin = 0.0f;
		const float ScrollMax = ContentSize - MyGeometry.Size.Y;
		ScrollOffset.Y = FMath::Clamp(ScrollOffset.Y, ScrollMin, ScrollMax);

		return (PreviousScrollOffset != ScrollOffset.Y) ? FReply::Handled() : FReply::Unhandled();
	}

	return FReply::Unhandled();
}

void SUTRichTextBlock::Tick(const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime)
{
	if (!TextLayoutCache.IsValid() || !TextLayoutCache->TextLayout.IsValid())
	{
		return;
	}

	// If we're auto-wrapping, we need to hide the scrollbars until the first valid auto-wrap has been performed
	// If we don't do this, then we can get some nasty layout shuffling as the scrollbars appear for one frame and then vanish again
	const EVisibility ScrollBarVisiblityOverride = (AutoWrapText.Get() && TextLayoutCache->CachedSize.IsZero()) ? EVisibility::Collapsed : EVisibility::Visible;

	{
		// Need to account for the caret width too
		const float ContentSize = TextLayoutCache->TextLayout->GetSize().X;
		const float VisibleSize = AllottedGeometry.Size.X;

		// If this text box has no size, do not compute a view fraction because it will be wrong and causes pop in when the size is available
		const float ViewFraction = (VisibleSize > 0.0f && ContentSize > 0.0f) ? VisibleSize / ContentSize : 1;
		const float ViewOffset = (ContentSize > 0.0f && ViewFraction < 1.0f) ? FMath::Clamp<float>(ScrollOffset.X / ContentSize, 0.0f, 1.0f - ViewFraction) : 0.0f;

		// Update the scrollbar with the clamped version of the offset
		ScrollOffset.X = ViewOffset * ContentSize;

		if (HScrollBar.IsValid())
		{
			HScrollBar->SetState(ViewOffset, ViewFraction);
			HScrollBar->SetUserVisibility(ScrollBarVisiblityOverride);
			if (!HScrollBar->IsNeeded())
			{
				// We cannot scroll, so ensure that there is no offset
				ScrollOffset.X = 0.0f;
			}
		}
	}

	{
		const float ContentSize = TextLayoutCache->TextLayout->GetSize().Y;
		const float VisibleSize = AllottedGeometry.Size.Y;

		// If this text box has no size, do not compute a view fraction because it will be wrong and causes pop in when the size is available
		const float ViewFraction = (VisibleSize > 0.0f && ContentSize > 0.0f) ? VisibleSize / ContentSize : 1;
		const float ViewOffset = (ContentSize > 0.0f && ViewFraction < 1.0f) ? FMath::Clamp<float>(ScrollOffset.Y / ContentSize, 0.0f, 1.0f - ViewFraction) : 0.0f;

		// Update the scrollbar with the clamped version of the offset
		ScrollOffset.Y = ViewOffset * ContentSize;

		if (VScrollBar.IsValid())
		{
			VScrollBar->SetState(ViewOffset, ViewFraction);
			VScrollBar->SetUserVisibility(ScrollBarVisiblityOverride);
			if (!VScrollBar->IsNeeded())
			{
				// We cannot scroll, so ensure that there is no offset
				ScrollOffset.Y = 0.0f;
			}
		}
	}

	TextLayoutCache->SetScrollOffset( ScrollOffset);
}