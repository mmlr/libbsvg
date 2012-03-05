/*****************************************************************************/
// SVGTranslator
// Written by Michael Lotz
//
// SVGTranslator.cpp
//
// This BTranslator based object is for rasterizing SVG images.
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

#include <OS.h>
#include <stdio.h>
#include <string.h>
#include <String.h>

#include "SVGView.h"
#include "SVGDefs.h"
#include "SVGTranslator.h"
#include "SVGSettingsView.h"

// The input formats that this translator supports.
translation_format gInputFormats[] = {
	{ SVG_FORMAT_CODE, B_TRANSLATOR_BITMAP, SVG_IN_QUALITY, SVG_IN_CAPABILITY,
		"image/svg+xml", "SVG image" }
};

// The output formats that this translator supports.
translation_format gOutputFormats[] = {
};

// Long translator info
char translatorInfo[] = SVG_TRANSLATOR_INFO;

// Short name
char translatorName[] = SVG_TRANSLATOR_NAME;

// Translator version
int32 translatorVersion = SVG_TRANSLATOR_VERSION;

// Default settings for the Translator
TranSetting gDefaultSettings[] = {
	{ B_TRANSLATOR_EXT_HEADER_ONLY, TRAN_SETTING_BOOL, false },
	{ B_TRANSLATOR_EXT_DATA_ONLY, TRAN_SETTING_BOOL, false },
	{ SVG_TRANSLATOR_EXT_SAMPLESIZE, TRAN_SETTING_INT32, 4},
	{ SVG_TRANSLATOR_EXT_WIDTH, TRAN_SETTING_INT32, 256 },
	{ SVG_TRANSLATOR_EXT_HEIGHT, TRAN_SETTING_INT32, 256 },
	{ SVG_TRANSLATOR_EXT_SCALE_TO_FIT, TRAN_SETTING_BOOL, true },
	{ SVG_TRANSLATOR_EXT_FIT_CONTENT, TRAN_SETTING_BOOL, true },
	{ SVG_TRANSLATOR_EXT_BACK_COLOR, TRAN_SETTING_INT32, 0xffffffff }
};

// ---------------------------------------------------------------
// make_nth_translator
//
// Creates a SVGTranslator object to be used by BTranslatorRoster
//
// Preconditions:
//
// Parameters: n,		The translator to return. Since
//						SVGTranslator only publishes one
//						translator, it only returns a
//						SVGTranslator if n == 0
//
//             you, 	The image_id of the add-on that
//						contains code (not used).
//
//             flags,	Has no meaning yet, should be 0.
//
// Postconditions:
//
// Returns: NULL if n is not zero,
//          a new SVGTranslator if n is zero
// ---------------------------------------------------------------
BTranslator *
make_nth_translator(int32 n, image_id you, uint32 flags, ...)
{
	if (!n)
		return new SVGTranslator();
	else
		return NULL;
}

// ---------------------------------------------------------------
// Constructor
//
// Sets up the version info and the name of the translator so that
// these values can be returned when they are requested.
//
// Preconditions:
//
// Parameters:
//
// Postconditions:
//
// Returns:
// ---------------------------------------------------------------
SVGTranslator::SVGTranslator()
	: BaseTranslator("SVG Image", SVG_TRANSLATOR_INFO, SVG_TRANSLATOR_VERSION,
		gInputFormats, sizeof(gInputFormats) / sizeof(translation_format),
		gOutputFormats, sizeof(gOutputFormats) / sizeof(translation_format),
		"SVGTranslator_Settings",
		gDefaultSettings, sizeof(gDefaultSettings) / sizeof(TranSetting),
		B_TRANSLATOR_BITMAP, SVG_FORMAT_CODE)
{
}

// ---------------------------------------------------------------
// Destructor
//
// Does nothing
//
// Preconditions:
//
// Parameters:
//
// Postconditions:
//
// Returns:
// ---------------------------------------------------------------
SVGTranslator::~SVGTranslator()
{
}

// ---------------------------------------------------------------
// DerivedIdentify
//
// Examines the data from inSource and determines if it is in a
// format that this translator knows how to work with.
//
// Preconditions:
//
// Parameters:	inSource,	where the data to examine is
//
//				inFormat,	a hint about the data in inSource,
//							it is ignored since it is only a hint
//
//				ioExtension,	configuration settings for the
//								translator
//
//				outInfo,	information about what data is in
//							inSource and how well this translator
//							can handle that data is stored here
//
//				outType,	The format that the user wants
//							the data in inSource to be
//							converted to
//
// Postconditions:
//
// Returns: B_NO_TRANSLATOR,	if this translator can't handle
//								the data in inSource
//
// B_ERROR,	if there was an error converting the data to the host
//			format
//
// B_BAD_VALUE, if the settings in ioExtension are bad
//
// B_OK,	if this translator understood the data and there were
//			no errors found
//
// Other errors if BPositionIO::Read() returned an error value
// ---------------------------------------------------------------
status_t
SVGTranslator::DerivedIdentify(BPositionIO *inSource,
	const translation_format *inFormat, BMessage *ioExtension,
	translator_info *outInfo, uint32 outType)
{
	char buffer[1024];
	inSource->Read(buffer, 1024);
	BString string(buffer);
	
	if (string.IFindFirst("<!DOCTYPE svg ") < 0
		&& string.IFindFirst("<path") < 0
		&& string.FindFirst("linearGradient") < 0
		&& string.FindFirst("radialGradient") < 0)
		return B_NO_TRANSLATOR;
	
	if (outInfo) {
		outInfo->type = SVG_FORMAT_CODE;
		outInfo->group = B_TRANSLATOR_BITMAP;
		outInfo->quality = SVG_IN_QUALITY;
		outInfo->capability = SVG_IN_CAPABILITY;
		strcpy(outInfo->MIME, "image/svg+xml");
		strcpy(outInfo->name, "SVG images");
	}
		
	return B_OK;
}

// ---------------------------------------------------------------
// DerivedTranslate
//
// Translates the data in inSource to the type outType and stores
// the translated data in outDestination.
//
// Preconditions:
//
// Parameters:	inSource,	the data to be translated
// 
//				inInfo,	hint about the data in inSource (not used)
//
//				ioExtension,	configuration options for the
//								translator
//
//				outType,	the type to convert inSource to
//
//				outDestination,	where the translated data is
//								put
//
//				baseType, indicates whether inSource is in the
//				          bits format, not in the bits format or
//				          is unknown
//
// Postconditions:
//
// Returns: B_BAD_VALUE, if the options in ioExtension are bad
//
// B_NO_TRANSLATOR, if this translator doesn't understand the data
//
// B_ERROR, if there was an error allocating memory or converting
//          data
//
// B_OK, if all went well
// ---------------------------------------------------------------
status_t
SVGTranslator::DerivedTranslate(BPositionIO *inSource,
	const translator_info *inInfo, BMessage *ioExtension, uint32 outType,
	BPositionIO *outDestination, int32 baseType)
{
	if (baseType != 0)
		return B_NO_TRANSLATOR;
	
	int32 width = 256;
	int32 height = 256;
	bool scale = true;
	bool fit = true;
	int32 color = 0xffffffff;
	bool header = true;
	bool data = true;
	int32 samplesize = 4;
	
	fSettings->LoadSettings();
	
	if (!ioExtension
		|| ioExtension->FindInt32(SVG_TRANSLATOR_EXT_WIDTH, &width) != B_OK)
		width = fSettings->SetGetInt32(SVG_TRANSLATOR_EXT_WIDTH);
	if (!ioExtension
		|| ioExtension->FindInt32(SVG_TRANSLATOR_EXT_HEIGHT, &height) != B_OK)
		height = fSettings->SetGetInt32(SVG_TRANSLATOR_EXT_HEIGHT);
	if (!ioExtension
		|| ioExtension->FindBool(SVG_TRANSLATOR_EXT_SCALE_TO_FIT, &scale) != B_OK)
		scale = fSettings->SetGetBool(SVG_TRANSLATOR_EXT_SCALE_TO_FIT);
	if (!ioExtension
		|| ioExtension->FindBool(SVG_TRANSLATOR_EXT_FIT_CONTENT, &fit) != B_OK)
		fit = fSettings->SetGetBool(SVG_TRANSLATOR_EXT_FIT_CONTENT);
	if (!ioExtension
		|| ioExtension->FindInt32(SVG_TRANSLATOR_EXT_SAMPLESIZE, &samplesize) != B_OK)
		samplesize = fSettings->SetGetInt32(SVG_TRANSLATOR_EXT_SAMPLESIZE);
	if (!ioExtension
		|| ioExtension->FindInt32(SVG_TRANSLATOR_EXT_BACK_COLOR, &color) != B_OK)
		color = fSettings->SetGetInt32(SVG_TRANSLATOR_EXT_BACK_COLOR);
	if (!ioExtension
		|| ioExtension->FindBool(B_TRANSLATOR_EXT_HEADER_ONLY, &header) != B_OK)
		header = fSettings->SetGetBool(B_TRANSLATOR_EXT_HEADER_ONLY);
	if (!ioExtension
		|| ioExtension->FindBool(B_TRANSLATOR_EXT_DATA_ONLY, &data) != B_OK)
		data = fSettings->SetGetBool(B_TRANSLATOR_EXT_DATA_ONLY);
	
	if (header)
		data = false;
	else if (data)
		header = false;
	else {
		data = true;
		header = true;
	}
	
	BRect bounds(0, 0, width - 1, height - 1);
	BBitmap *output = new BBitmap(bounds, B_RGBA32, true);
	bounds.right += 1; bounds.bottom += 1;
	uint32 *bits = (uint32 *)output->Bits();
	int length = output->BitsLength();
	
	BSVGView *view = NULL;
	if (data) {
		view = new BSVGView(bounds, "BSVGView", B_FOLLOW_NONE);
		view->SetScaleToFit(scale);
		view->SetFitContent(fit);
		view->SetSampleSize(samplesize);
		view->SetViewColor(color >> 16 & 0x000000ff, color >> 8 & 0x000000ff,
							color >> 0 & 0x000000ff, color >> 24 & 0x000000ff);
		fast_memset(length / 4, color, bits);
		output->AddChild(view);
		output->Lock();
		
		if (view->LoadFromPositionIO(inSource) != B_OK) {
			output->RemoveChild(view);
			output->Unlock();
			delete output;
			delete view;
			return B_ERROR;
		}
		
		view->Draw(bounds);
		output->RemoveChild(view);
		output->Unlock();
		delete view;
	}
	
	// Write out the data to outDestination
	// Construct and write Be bitmap header
	TranslatorBitmap bitsHeader;
	if (header) {
		bounds.right -= 1; bounds.bottom -= 1;
		bitsHeader.magic = B_TRANSLATOR_BITMAP;
		bitsHeader.bounds = bounds;
		bitsHeader.rowBytes = output->BytesPerRow();
		bitsHeader.colors = B_RGBA32;
		bitsHeader.dataSize = length;
		if (swap_data(B_UINT32_TYPE, &bitsHeader,
			sizeof(TranslatorBitmap), B_SWAP_HOST_TO_BENDIAN) != B_OK) {
			delete output;
			return B_ERROR;
		}
	}
	
	if (header)
		outDestination->Write(&bitsHeader, sizeof(TranslatorBitmap));
	if (data)
		outDestination->Write(bits, length);
	
	delete output;
	
	return B_OK;
}

BView *
SVGTranslator::NewConfigView(TranslatorSettings *settings)
{
	return new SVGSettingsView(BRect(0, 0, SVG_VIEW_WIDTH, SVG_VIEW_HEIGHT),
		"SVGTranslator Settings", B_FOLLOW_ALL, B_WILL_DRAW, settings);
}
