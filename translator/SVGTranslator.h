/*****************************************************************************/
// SVGTranslator
// Written by Michael Lotz
//
// SVGTranslator.h
//
// This BTranslator based object is for rasterizing SVG images
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

#ifndef SVG_TRANSLATOR_H
#define SVG_TRANSLATOR_H

#include <Translator.h>
#include <TranslatorFormats.h>
#include <GraphicsDefs.h>
#include <InterfaceDefs.h>
#include <DataIO.h>
#include <File.h>
#include <ByteOrder.h>
#include <fs_attr.h>
#include <TranslationDefs.h>
#include "BaseTranslator.h"

#define SVG_FORMAT_CODE 'SVG '

#define SVG_TRANSLATOR_INFO "SVG Rasterizer using libbsvg"
#define SVG_TRANSLATOR_NAME "SVG (libbsvg)"
#define SVG_TRANSLATOR_VERSION B_TRANSLATION_MAKE_VER(0,5,0)

#define SVG_IN_QUALITY 0.5
#define SVG_IN_CAPABILITY 0.4

#define BBT_OUT_QUALITY 0.5
#define BBT_OUT_CAPABILITY 0.4

// SVG Translator Settings
#define SVG_TRANSLATOR_EXT_WIDTH		"/width"
#define SVG_TRANSLATOR_EXT_HEIGHT		"/height"
#define SVG_TRANSLATOR_EXT_SCALE_TO_FIT	"/scaleToFit"
#define SVG_TRANSLATOR_EXT_FIT_CONTENT	"/fitContent"
#define SVG_TRANSLATOR_EXT_SAMPLESIZE	"/samplesize"
#define SVG_TRANSLATOR_EXT_BACK_COLOR	"/backColor"

class SVGTranslator : public BaseTranslator {
public:
	SVGTranslator();

	virtual status_t DerivedIdentify(BPositionIO *inSource,
		const translation_format *inFormat, BMessage *ioExtension,
		translator_info *outInfo, uint32 outType);
		
	virtual status_t DerivedTranslate(BPositionIO *inSource,
		const translator_info *inInfo, BMessage *ioExtension,
		uint32 outType, BPositionIO *outDestination, int32 baseType);
		
	virtual BView *NewConfigView(TranslatorSettings *settings);

protected:
	virtual ~SVGTranslator();
		// this is protected because the object is deleted by the
		// Release() function instead of being deleted directly by
		// the user
};

#endif // #ifndef SVG_TRANSLATOR_H
