/*****************************************************************************/
// SVGSettingsView
// Written by Michael Lotz
//
// SVGSettingsView.cpp
//
// This BView based object displays information about the SVGTranslator.
//
//
// Copyright (c) 2003 OpenBeOS Project
//
// Permission is hereby granted, free of charge, to any person obtaining a
// copy of this software and associated documentation files (the "Software"),
// to deal in the Software without restriction, including without limitation
// the rights to use, copy, modify, merge, publish, distribute, sublicense, 
// and/or sell copies of the Software, and to permit persons to whom the 
// Software is furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included 
// in all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
// OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL 
// THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING 
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
// DEALINGS IN THE SOFTWARE.
/*****************************************************************************/

#include <stdio.h>
#include <string.h>
#include <Alert.h>
#include <InterfaceKit.h>
#include <String.h>
#include "SVGSettingsView.h"
#include "SVGTranslator.h"

// ---------------------------------------------------------------
// Constructor
//
// Sets up the view settings
//
// Preconditions:
//
// Parameters:
//
// Postconditions:
//
// Returns:
// ---------------------------------------------------------------
SVGSettingsView::SVGSettingsView(const BRect &frame, const char *name,
	uint32 resize, uint32 flags, TranslatorSettings *settings)
	:	BView(frame, name, resize, flags)
{
	fSettings = settings;
	
	SetViewColor(220,220,220,0);
	
	float itemSpacing = 0;
	
	BRect frame(20, 38, 200, 70);
	fScaleToFit = new BCheckBox(frame, "", "Scale To Fit",
		new BMessage(M_SVG_SET_SCALE_TO_FIT));
	fScaleToFit->SetViewColor(ViewColor());
	AddChild(fScaleToFit);
	fScaleToFit->ResizeToPreferred();
	
	frame.OffsetBy(fScaleToFit->Bounds().Width() + itemSpacing + 4, 0);
	fFitContent = new BCheckBox(frame, "", "Fit Content",
		new BMessage(M_SVG_SET_FIT_CONTENT));
	AddChild(fFitContent);
	fFitContent->ResizeToPreferred();
	
	frame.OffsetBy(-fScaleToFit->Bounds().Width() - itemSpacing - 4,
					fFitContent->Bounds().Height() + itemSpacing);
	float divider = StringWidth("Default Width:   ") + 5;
	fSampleSize = new BTextControl(frame, "", "Sample Size", "4",
		new BMessage(M_SVG_SET_SAMPLE_SIZE));
	fSampleSize->SetDivider(divider);
	AddChild(fSampleSize);
		
	frame.OffsetBy(0, fSampleSize->Bounds().Height() + itemSpacing);
	fDefaultWidth = new BTextControl(frame, "", "Default Width:", "256",
		new BMessage(M_SVG_SET_DEFAULT_WIDTH));
	fDefaultWidth->SetDivider(divider);
	AddChild(fDefaultWidth);
	
	frame.OffsetBy(0, fDefaultWidth->Bounds().Height() + itemSpacing);
	fDefaultHeight = new BTextControl(frame, "", "Default Height:", "256",
		new BMessage(M_SVG_SET_DEFAULT_HEIGHT));
	fDefaultHeight->SetDivider(divider);
	AddChild(fDefaultHeight);
	
	frame.OffsetBy(0, fDefaultHeight->Bounds().Height() + itemSpacing);
	fBackColor = new BColorControl(
		frame.LeftTop(), B_CELLS_16x16, 1, "Back Color",
		new BMessage(M_SVG_SET_BACK_COLOR));
	fBackColor->ResizeToPreferred();
	AddChild(fBackColor);
	
	frame.OffsetBy(0, fBackColor->Bounds().Height() + itemSpacing);
	fBackAlpha = new BTextControl(frame, "", "Alpha", "255",
		new BMessage(M_SVG_SET_BACK_COLOR));
	fBackAlpha->SetDivider(divider);
	AddChild(fBackAlpha);
	
	fScaleToFit->SetValue(fSettings->SetGetBool(SVG_TRANSLATOR_EXT_SCALE_TO_FIT));
	fFitContent->SetValue(fSettings->SetGetBool(SVG_TRANSLATOR_EXT_FIT_CONTENT));
	
	BString text;
	text << fSettings->SetGetInt32(SVG_TRANSLATOR_EXT_SAMPLESIZE);
	fSampleSize->SetText(text.String());
	
	text.SetTo("");
	text << fSettings->SetGetInt32(SVG_TRANSLATOR_EXT_WIDTH);
	fDefaultWidth->SetText(text.String());
	
	text.SetTo("");
	text << fSettings->SetGetInt32(SVG_TRANSLATOR_EXT_HEIGHT);
	fDefaultHeight->SetText(text.String());
	
	int32 colorint = fSettings->SetGetInt32(SVG_TRANSLATOR_EXT_BACK_COLOR);
	rgb_color color;
	color.red = colorint >> 16 & 0x000000ff;
	color.green = colorint >> 8 & 0x000000ff;
	color.blue = colorint >> 0 & 0x000000ff;
	fBackColor->SetValue(color);
	
	int32 alpha = colorint >> 24 & 0x000000ff;
	BString alphastring;
	alphastring << alpha;
	fBackAlpha->SetText(alphastring.String());
}

// ---------------------------------------------------------------
// Destructor
//
// Releases the SVGTranslator settings
//
// Preconditions:
//
// Parameters:
//
// Postconditions:
//
// Returns:
// ---------------------------------------------------------------
SVGSettingsView::~SVGSettingsView()
{
	fSettings->Release();
}

void
SVGSettingsView::AllAttached()
{
	fScaleToFit->SetTarget(this);
	fFitContent->SetTarget(this);
	fSampleSize->SetTarget(this);
	fDefaultWidth->SetTarget(this);
	fDefaultHeight->SetTarget(this);
	fBackColor->SetTarget(this);
	fBackAlpha->SetTarget(this);
	BView::AllAttached();
}

// ---------------------------------------------------------------
// Draw
//
// Draws information about the SVGTranslator to this view.
//
// Preconditions:
//
// Parameters: area,	not used
//
// Postconditions:
//
// Returns:
// ---------------------------------------------------------------
void
SVGSettingsView::Draw(BRect area)
{
	SetFont(be_bold_font);
	font_height fh;
	GetFontHeight(&fh);
	float xbold, ybold;
	xbold = fh.descent + 1;
	ybold = fh.ascent + fh.descent * 2 + fh.leading;
	
	char title[] = "SVG Rasterizer based on libbsvg";
	DrawString(title, BPoint(xbold, ybold));
	
	SetFont(be_plain_font);
	font_height plainh;
	GetFontHeight(&plainh);
	float yplain;
	yplain = plainh.ascent + plainh.descent * 2 + plainh.leading;
	
	char detail[100];
	sprintf(detail, "Version %d.%d.%d %s © Michael Lotz",
		static_cast<int>(B_TRANSLATION_MAJOR_VER(SVG_TRANSLATOR_VERSION)),
		static_cast<int>(B_TRANSLATION_MINOR_VER(SVG_TRANSLATOR_VERSION)),
		static_cast<int>(B_TRANSLATION_REVSN_VER(SVG_TRANSLATOR_VERSION)),
		__DATE__);
	DrawString(detail, BPoint(xbold, yplain + ybold));
}

void
SVGSettingsView::MessageReceived(BMessage *msg)
{
	switch (msg->what) {
		case M_SVG_SET_SCALE_TO_FIT: {
			bool enabled = fScaleToFit->Value();
			fSettings->SetGetBool(SVG_TRANSLATOR_EXT_SCALE_TO_FIT, &enabled);
			fSettings->SaveSettings();
		} break;
		case M_SVG_SET_FIT_CONTENT: {
			bool enabled = fFitContent->Value();
			fSettings->SetGetBool(SVG_TRANSLATOR_EXT_FIT_CONTENT, &enabled);
			fSettings->SaveSettings();
		} break;
		case M_SVG_SET_SAMPLE_SIZE: {
			int32 samplesize;
			if (sscanf(fSampleSize->Text(), "%ld", &samplesize) != 1)
				break;
			
			fSettings->SetGetInt32(SVG_TRANSLATOR_EXT_SAMPLESIZE, &samplesize);
			fSettings->SaveSettings();
		} break;
		case M_SVG_SET_DEFAULT_WIDTH: {
			int32 width;
			if (sscanf(fDefaultWidth->Text(), "%ld", &width) != 1)
				break;
			
			fSettings->SetGetInt32(SVG_TRANSLATOR_EXT_WIDTH, &width);
			fSettings->SaveSettings();
		} break;
		case M_SVG_SET_DEFAULT_HEIGHT: {
			int32 height;
			if (sscanf(fDefaultHeight->Text(), "%ld", &height) != 1)
				break;
			
			fSettings->SetGetInt32(SVG_TRANSLATOR_EXT_HEIGHT, &height);
			fSettings->SaveSettings();
		} break;
		case M_SVG_SET_BACK_COLOR: {
			rgb_color color;
			color = fBackColor->ValueAsColor();
			int32 alpha;
			if (sscanf(fBackAlpha->Text(), "%ld", &alpha) != 1)
				break;
			
			if (alpha < 0 || alpha > 255)
				alpha = 255;
			
			int32 colorint = (alpha << 24) + (color.red << 16)
								+ (color.green << 8) + color.blue;
			fSettings->SetGetInt32(SVG_TRANSLATOR_EXT_BACK_COLOR, &colorint);
			fSettings->SaveSettings();
		} break;
		default: BView::MessageReceived(msg);
	}
}

