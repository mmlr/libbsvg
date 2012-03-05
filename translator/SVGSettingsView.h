/*****************************************************************************/
// SVGSettingsView
// Written by Michael Lotz
//
// SVGSettingsView.h
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

#ifndef SVGSETTINGSVIEW_H
#define SVGSETTINGSVIEW_H

#include "TranslatorSettings.h"
#include <View.h>
#include <InterfaceKit.h>

#define SVG_VIEW_WIDTH 300
#define SVG_VIEW_HEIGHT 250

// Config panel messages
#define M_SVG_SET_SCALE_TO_FIT		'svgs'
#define M_SVG_SET_FIT_CONTENT		'svgf'
#define M_SVG_SET_SAMPLE_SIZE		'svgi'
#define M_SVG_SET_DEFAULT_WIDTH		'svgw'
#define M_SVG_SET_DEFAULT_HEIGHT	'svgh'
#define M_SVG_SET_BACK_COLOR		'svgc'

class SVGSettingsView : public BView {
public:
	SVGSettingsView(const BRect &frame, const char *name, uint32 resize,
		uint32 flags, TranslatorSettings *settings);
		// sets up the view
		
	~SVGSettingsView();
		// releases the SVGTranslator settings

	virtual void AllAttached();
	virtual	void Draw(BRect area);
		// draws information about the SVGTranslator
	virtual void MessageReceived(BMessage *msg);
		
private:
	TranslatorSettings	*fSettings;
		// the actual settings for the translator,
		// shared with the translator
	BCheckBox			*fScaleToFit;
	BCheckBox			*fFitContent;
	
	BTextControl		*fSampleSize;
	BTextControl		*fDefaultWidth;
	BTextControl		*fDefaultHeight;
	
	BColorControl		*fBackColor;
	BTextControl		*fBackAlpha;
};

#endif // #ifndef SVGSETTINGSVIEW_H
