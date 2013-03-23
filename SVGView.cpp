#include <Alert.h>
#include <Bitmap.h>
#include <Entry.h>
#include <File.h>
#include <FilePanel.h>
#include <fs_attr.h>
#include <MenuItem.h>
#include <Node.h>
#include <Path.h>
#include <Picture.h>
#include <PopUpMenu.h>
#include <Roster.h>
#include <StopWatch.h>
#include <zlib.h>

#include "CSSParser.h"
#include "expat.h"
#include "StyleParser.h"
#include "SVGStop.h"
#include "SVGColors.h"
#include "SVGShapes.h"
#include "SVGView.h"
#include "SVGContainers.h"
#include "SVGDefs.h"
#include "SVGText.h"
#include "SVGDataCreator.h"
#include "TransformIterator.h"
#include "UnitHandler.h"
#include "EvenOddIterator.h"

static sem_id fXMLHandlerLock = -1;

BSVGView::BSVGView(BRect frame, const char *name, uint32 resizeMask, uint32 flags)
	:	BView(frame, name, resizeMask, flags | B_SUBPIXEL_PRECISE),
		fParser(NULL),
		fCSSParser(NULL),
		fScaleToFit(false),
		fFitContent(false),
		fSuperSampling(true),
		fSampleSize(2),
		fScale(1),
		fFocusPoint(0.5, 0.5),
		fViewCenter(0.5, 0.5),
		fGraphicsWidth(-1),
		fGraphicsHeight(-1),
		fViewBounds(frame.OffsetToCopy(0, 0)),
		fViewColor(BView::ViewColor()),
		fGradient(NULL),
		fText(NULL),
		fTextBuffer(""),
		fOffscreenBitmap(NULL),
		fTempBitmap(NULL),
		fTempView(NULL),
		fEvenOdd(NULL),
		fOuterFirst(true),
		fOuterBounds(0, 0, 0, 0),
		fInsideCSS(false),
		fUpdateNeeded(false),
		fTransparentHack(0),
		fPulseRate(0),
		fPulseThread(0),
		fDropsEnabled(true),
		fMenusEnabled(true),
		fContextMenu(NULL),
		fLocked(false)
{
	fDocument = new BSVGDocument(this);
	fDocument->State()->Reset(SOURCE_SET);
	
	// specific init neccessary
	SetMenusEnabled(fMenusEnabled);
	SetPulseRate(fPulseRate);
	
	fUpdateNeeded = true;
	Redraw();
}

BSVGView::~BSVGView()
{
	delete fDocument;
	delete fOffscreenBitmap;
	
	if (fPulseThread)
		kill_thread(fPulseThread);
}

BSVGView::BSVGView(BMessage *data)
	:	BView(data),
		fParser(NULL),
		fCSSParser(NULL),
		fScaleToFit(false),
		fFitContent(false),
		fSuperSampling(true),
		fSampleSize(2),
		fScale(1),
		fFocusPoint(0.5, 0.5),
		fViewCenter(0.5, 0.5),
		fGraphicsWidth(-1),
		fGraphicsHeight(-1),
		fViewBounds(0, 0, 0, 0),
		fViewColor(BView::ViewColor()),
		fGradient(NULL),
		fText(NULL),
		fTextBuffer(""),
		fOffscreenBitmap(NULL),
		fTempBitmap(NULL),
		fTempView(NULL),
		fEvenOdd(NULL),
		fOuterFirst(true),
		fOuterBounds(0, 0, 0, 0),
		fInsideCSS(false),
		fUpdateNeeded(false),
		fTransparentHack(0),
		fPulseRate(0),
		fPulseThread(0),
		fDropsEnabled(true),
		fMenusEnabled(true),
		fContextMenu(NULL),
		fLocked(false)
{
	data->FindFloat("_scale", &fScale);
	data->FindPoint("_focuspoint", &fFocusPoint);
	data->FindPoint("_viewcenter", &fViewCenter);
	data->FindBool("_scaletofit", &fScaleToFit);
	data->FindBool("_fitcontent", &fFitContent);
	data->FindFloat("_graphwidth", &fGraphicsWidth);
	data->FindFloat("_graphheight", &fGraphicsHeight);
	
	SVGColor color;
	data->FindInt32("_backcolor", (int32 *)&color.color);
	fViewColor = color.color;
	
	data->FindBool("_supersampling", &fSuperSampling);
	data->FindInt32("_samplesize", (int32 *)&fSampleSize);

	data->FindBool("_outerfirst", &fOuterFirst);
	data->FindRect("_outerbounds", &fOuterBounds);
	
	data->FindInt64("_pulserate", &fPulseRate);
	SetPulseRate(fPulseRate);
	
	data->FindRect("_innerframe", &fInnerFrame);
	fOffscreenBitmap = new BBitmap(fInnerFrame, B_RGBA32, true);
	
	data->FindBool("_dropsenabled", &fDropsEnabled);
	data->FindBool("_menusenabled", &fMenusEnabled);
	SetMenusEnabled(fMenusEnabled);
	
	BMessage msg;
	if (data->FindMessage("_document", &msg) == B_OK) {
		fDocument = new BSVGDocument(&msg);
		BSVGView *temp = this;
		fDocument->PromoteDownMember((void *)&fDocument->fParent, &temp);
	} else {
		fDocument = new BSVGDocument(this);
	}
	
	fDocument->State()->Reset(SOURCE_SET);
	
	fViewBounds = Bounds().OffsetToCopy(0, 0);
	
	fUpdateNeeded = true;
	Redraw();
}
	
BArchivable *
BSVGView::Instantiate(BMessage *data)
{
	if (validate_instantiation(data, "BSVGView"))
		return new BSVGView(data);
	return NULL;
}

status_t
BSVGView::Archive(BMessage *data, bool deep) const
{
	BView::Archive(data, deep);
	
	data->AddFloat("_scale", fScale);
	data->AddPoint("_focuspoint", fFocusPoint);
	data->AddPoint("_viewcenter", fViewCenter);
	data->AddBool("_scaletofit", fScaleToFit);
	data->AddBool("_fitcontent", fFitContent);
	data->AddFloat("_graphwidth", fGraphicsWidth);
	data->AddFloat("_graphheight", fGraphicsHeight);
	
	SVGColor color = fViewColor;
	data->AddInt32("_backcolor", color.Value());
	
	data->AddBool("_supersampling", fSuperSampling);
	data->AddInt32("_samplesize", fSampleSize);
	
	data->AddBool("_outerfirst", fOuterFirst);
	data->AddRect("_outerbounds", fOuterBounds);
	
	data->AddRect("_innerframe", fInnerFrame);
	
	data->AddInt64("_pulserate", fPulseRate);
	data->AddBool("_dropsenabled", fDropsEnabled);
	data->AddBool("_menusenabled", fMenusEnabled);
	
	if (deep) {
		BMessage msg;
		fDocument->Archive(&msg, deep);
		data->AddMessage("_document", &msg);
	}
	
	//ADDREPADDON("application/libbsvg");
	ADDREPCLASS("BSVGView");
	
	return B_OK;
}

status_t
BSVGView::LoadFromFile(const char *name)
{
	BFile file(name, B_READ_ONLY);
	return ParseXMLFile(&file);
}

status_t
BSVGView::LoadFromFile(BEntry *entry)
{
	BFile file(entry, B_READ_ONLY);
	return ParseXMLFile(&file);
}

status_t
BSVGView::LoadFromFile(entry_ref *ref)
{
	BFile file(ref, B_READ_ONLY);
	return ParseXMLFile(&file);
}

status_t
BSVGView::LoadFromFile(BFile *file)
{
	return ParseXMLFile(file);
}

status_t
BSVGView::LoadFromAttribute(const char *filename, const char *attribute)
{
	BNode node(filename);
	return ParseXMLNode(&node, attribute);
}

status_t
BSVGView::LoadFromAttribute(BEntry *entry, const char *attribute)
{
	BNode node(entry);
	return ParseXMLNode(&node, attribute);
}

status_t
BSVGView::LoadFromAttribute(entry_ref *ref, const char *attribute)
{
	BNode node(ref);
	return ParseXMLNode(&node, attribute);
}

status_t
BSVGView::LoadFromAttribute(BNode *in_node, const char *attribute)
{
	BNode node(*in_node);
	return ParseXMLNode(&node, attribute);
}

status_t
BSVGView::LoadFromPositionIO(BPositionIO *positionio)
{
	return ParseXMLData(positionio);
}

status_t
BSVGView::LoadFromBuffer(char *buffer, off_t length)
{
	return ParseXMLBuffer(buffer, length);
}

status_t
BSVGView::LoadFromString(BString &data)
{
	int32 len = data.Length();
	char *buffer = data.LockBuffer(len);
	status_t result = ParseXMLBuffer(buffer, len);
	data.UnlockBuffer();
	return result;
}

status_t
BSVGView::ExportToFile(const char *name)
{
	BFile file(name, B_WRITE_ONLY | B_CREATE_FILE);
	return ExportToFile(&file);
}

status_t
BSVGView::ExportToFile(BEntry *entry)
{
	BFile file(entry, B_WRITE_ONLY | B_CREATE_FILE);
	return ExportToFile(&file);
}

status_t
BSVGView::ExportToFile(entry_ref *ref)
{
	BFile file(ref, B_WRITE_ONLY | B_CREATE_FILE);
	return ExportToFile(&file);
}

status_t
BSVGView::ExportToFile(BFile *file)
{
	BString data;
	
	status_t res = ExportToString(data);
	if (res != B_OK)
		return res;
	
	file->Write(data.String(), data.Length());
	return B_OK;
}

status_t
BSVGView::ExportToAttribute(const char *filename, const char *attribute)
{
	BNode node(filename);
	return ExportToAttribute(&node, attribute);
}

status_t
BSVGView::ExportToAttribute(BEntry *entry, const char *attribute)
{
	BNode node(entry);
	return ExportToAttribute(&node, attribute);
}

status_t
BSVGView::ExportToAttribute(entry_ref *ref, const char *attribute)
{
	BNode node(ref);
	return ExportToAttribute(&node, attribute);
}

status_t
BSVGView::ExportToAttribute(BNode *node, const char *attribute)
{
	if (!node || node->InitCheck() != B_OK)
		return B_ERROR;
	
	BString data;
	status_t res = ExportToString(data);
	if (res != B_OK)
		return res;
	
	node->WriteAttr(attribute, 'zICO', 0, data.String(), data.Length());
	
	return B_OK;
}

status_t
BSVGView::ExportToPositionIO(BPositionIO *positionio)
{
	if (!positionio)
		return B_ERROR;
	
	BString data;
	status_t res = ExportToString(data);
	if (res != B_OK)
		return res;
	
	positionio->Write(data.String(), data.Length());
	
	return B_OK;
}

status_t
BSVGView::ExportToString(BString &data)
{
	fDocument->RecreateData();
	fDocument->CollectData(data);
	return B_OK;
}

void
BSVGView::SavePanel()
{
	BFilePanel *panel = new BFilePanel(B_SAVE_PANEL);
	panel->SetTarget(this);
	panel->SetMessage(new BMessage('save'));
	panel->SetSaveText("untitled.svg");
	panel->Show();
}

void
BSVGView::ShowSource()
{
	BString tempname = "/boot/var/tmp/svgsource-";
	tempname << rand() % 1000 << ".tmp";
	BFile file(tempname.String(), B_READ_WRITE | B_CREATE_FILE);
	ExportToFile(&file);
	
	BString attribute("text/plain");
	file.WriteAttrString("BEOS:TYPE", &attribute);
	
	BEntry entry(tempname.String());
	entry_ref ref;
	entry.GetRef(&ref);
	BRoster().Launch(&ref);
}

void
BSVGView::AddElement(BSVGElement *element)
{
	if (!fDocument->IsOpen())
		fDocument->OpenElement();
	fDocument->LastContainer()->AddElement(element);
	fUpdateNeeded = true;
	Redraw();
}

BSVGElement *
BSVGView::RemoveElement(const char *id, bool deep)
{
	return fDocument->RemoveElement(id, deep);
	fUpdateNeeded = true;
	Redraw();
}

BSVGElement *
BSVGView::RemoveElement(BSVGElement *element, bool deep)
{
	return fDocument->RemoveElement(element, deep);
	fUpdateNeeded = true;
	Redraw();
}

BSVGElement *
BSVGView::RemoveElementAt(int index, bool deep)
{
	return fDocument->RemoveElementAt(index, deep);
	fUpdateNeeded = true;
	Redraw();
}

BSVGElement *
BSVGView::ElementAt(int index, bool deep)
{
	return fDocument->ElementAt(index, deep);
}

int32
BSVGView::CountElements(bool deep)
{
	return fDocument->CountElements(deep);
}

BSVGElement *
BSVGView::FindElement(const char *id, int32 of_type)
{
	return FindElement(BString(id), of_type);
}

BSVGElement *
BSVGView::FindElement(const BString &id, int32 of_type)
{
	return fDocument->FindElement(id, of_type);
}

int32
BSVGView::FindElements(const char *_class, BSVGElement *elements[])
{
	return FindElements(BString(_class), elements);
}

int32
BSVGView::FindElements(const BString &_class, BSVGElement *elements[])
{
	ElementList list;
	if (fDocument->FindElements(_class, list) != B_OK || list.CountItems() <= 0) {
		*elements = NULL;
		return 0;
	}
	
	elements = new BSVGElement *[list.CountItems()];
	for (int i = 0; i < list.CountItems(); i++)
		elements[i] = list.ItemAt(i);
	
	return list.CountItems();
}

void
BSVGView::ClearElements()
{
	delete fDocument;
	fDocument = new BSVGDocument(this);
	fDocument->State()->Reset(SOURCE_SET);
}


void
BSVGView::SetAttributeByClass(const BString &_class, const BString &attribute, const BString &value)
{
	ElementList list(10, false);
	if (fDocument->FindElements(_class, list) != B_OK)
		return;
	
	for (int i = 0; i < list.CountItems(); i++) {
		attribute_s *attr = new attribute_s(attribute, value);
		list.ItemAt(i)->AddAttribute(attr, true);
	}
	
	fUpdateNeeded = true;
	Redraw();
}

void
BSVGView::SetAttributeByID(const BString &id, const BString &attribute, const BString &value)
{
	BSVGElement *element = fDocument->FindElement(id);
	if (!element)
		return;
	
	attribute_s *attr = new attribute_s(attribute, value);
	element->AddAttribute(attr, true);
	
	fUpdateNeeded = true;
	Redraw();
}

void
BSVGView::SetTextByClass(const BString &_class, const BString &text)
{
	ElementList list(10, false);
	if (fDocument->FindElements(_class, list) != B_OK)
		return;
	
	for (int i = 0; i < list.CountItems(); i++) {
		element_t type = list.ItemAt(i)->Type();
		if (type == B_SVG_TEXTSPAN
			|| type == B_SVG_STRING
			|| type == B_SVG_TEXTPATH
			|| type == B_SVG_TEXT) {
			BSVGTextSpan *element = (BSVGTextSpan *)list.ItemAt(i);
			if (element) {
				BSVGString *string = element->FirstText();
				if (string) {
					string->SetText(text);
					element->FinalizeShape();
					element->CloseContainer();
					element->PrepareRendering();
					element->RenderCommon();
				} else {
					element->AddString(text);
					element->FinalizeShape();
					element->CloseContainer();
					element->PrepareRendering();
					element->RenderCommon();
				}
			}
		}
	}
	
	fUpdateNeeded = true;
	Redraw();
}

void
BSVGView::SetTextByID(const BString &id, const BString &text)
{
	BSVGElement *found = fDocument->FindElement(id);
	if (!found)
		return;
	
	element_t type = found->Type();
	if (type == B_SVG_TEXTSPAN
		|| type == B_SVG_STRING
		|| type == B_SVG_TEXTPATH
		|| type == B_SVG_TEXT) {
		BSVGTextSpan *element = (BSVGTextSpan *)found;
		if (element) {
			BSVGString *string = element->FirstText();
			if (string) {
				string->SetText(text);
				element->FinalizeShape();
				element->CloseContainer();
				element->PrepareRendering();
				element->RenderCommon();
			} else {
				element->AddString(text);
				element->FinalizeShape();
				element->CloseContainer();
				element->PrepareRendering();
				element->RenderCommon();
			}
		}
	}
	
	fUpdateNeeded = true;
	Redraw();
}

void
BSVGView::Clear()
{
	ClearElements();
	
	fGraphicsWidth = -1;
	fGraphicsHeight = -1;
	fOuterBounds = BRect(0, 0, 0, 0);
	fOuterFirst = true;
	
	fGradient = NULL;
	fText = NULL;
	fTextBuffer.SetTo("");
	
	fUpdateNeeded = true;
	Redraw();
}

void
BSVGView::UpdateInnerFrame()
{
	BRect bounds(0, 0, 0, 0);
	if (fFitContent || fGraphicsWidth < 0 || fGraphicsHeight < 0) {
		bool first = true;
		fDocument->State()->general_transform.Reset();
		fDocument->PrepareRendering();
		fDocument->CollectBounds(fOuterBounds, &first);
		bounds = fOuterBounds.OffsetToCopy(0, 0);
	} else {
		bounds.left = 0;
		bounds.top = 0;
		bounds.right = fGraphicsWidth;
		bounds.bottom = fGraphicsHeight;
	}
	
	if (fScaleToFit) {
		fScale = MIN(fViewBounds.right / bounds.Width(), fViewBounds.bottom / bounds.Height());
		bounds.left *= fScale;
		bounds.top *= fScale;
		bounds.right *= fScale;
		bounds.bottom *= fScale;
	}
	
	fInnerFrame = bounds;
}

//#define HIT_STATISTIC
#ifdef HIT_STATISTIC
static int fBitmapMissCount = 0;
static int fBitmapHitCount = 0;
static int fViewMissCount = 0;
static int fViewHitCount = 0;
static int fEvenOddMissCount = 0;
static int fEvenOddHitCount = 0;
#endif

BBitmap *
BSVGView::NeedTempBitmap(BRect frame)
{
	BRect current(0, 0, 0, 0);
	if (fTempBitmap)
		current = fTempBitmap->Bounds();
	
	// check if bitmap is large enough and only recreate if not
	if (current.right < frame.right || current.bottom < frame.bottom) {
		if (fTempView) {
			fTempBitmap->Lock();
			fTempView->RemoveSelf();
			fTempBitmap->Unlock();
			delete fTempView;
			fTempView = NULL;
		}
		
		current.right = MAX(current.right, frame.right);
		current.bottom = MAX(current.bottom, frame.bottom);
		delete fTempBitmap;
		fTempBitmap = new BBitmap(current, B_RGBA32, true);
#ifndef HIT_STATISTIC
	}
#else
		fBitmapMissCount++;
	} else
		fBitmapHitCount++;
#endif
	
	return fTempBitmap;
}

BView *
BSVGView::NeedTempView(BRect frame)
{
	BRect current(0, 0, 0, 0);
	if (fTempView && fTempView->LockLooper()) {
		current = fTempView->Bounds();
		fTempView->UnlockLooper();
	}
	
	// check if view is large enough and only recreate if not
	if (current.right < frame.right || current.bottom < frame.bottom) {
		if (fTempView) {
			fTempBitmap->Lock();
			fTempView->RemoveSelf();
			fTempBitmap->Unlock();
			delete fTempView;
		}
		
		current.right = MAX(current.right, frame.right);
		current.bottom = MAX(current.bottom, frame.bottom);
		fTempView = new BView(current, "tempview", 0, B_SUBPIXEL_PRECISE);
		fTempBitmap->AddChild(fTempView);
#ifndef HIT_STATISTIC
	}
#else
		fViewMissCount++;
	} else
		fViewHitCount++;
#endif
	
	return fTempView;
}

EvenOddIterator *
BSVGView::NeedEvenOdd(BRect frame)
{
	BRect current(0, 0, 0, 0);
	if (fEvenOdd)
		current = fEvenOdd->Bounds();
	
	// check if iterator is large enough and only recreate if not
	if (current.right < frame.right || current.bottom < frame.bottom) {
		current.right = MAX(current.right, frame.right);
		current.bottom = MAX(current.bottom, frame.bottom);
		delete fEvenOdd;
		fEvenOdd = new EvenOddIterator(current);
#ifndef HIT_STATISTIC
	}
#else
		fEvenOddMissCount++;
	} else
		fEvenOddHitCount++;
#endif
	
	return fEvenOdd;
}

extern bigtime_t creationtime;

void
BSVGView::PrepareTempBitmap()
{
#ifdef HIT_STATISTIC
	fBitmapMissCount = 0;
	fBitmapHitCount = 0;
	fViewMissCount = 0;
	fViewHitCount = 0;
	fEvenOddMissCount = 0;
	fEvenOddHitCount = 0;
	creationtime = 0;
#endif
	
	fTempBitmap = NULL;
	fTempView = NULL;
	fEvenOdd = NULL;
	
	BRect frame(0, 0, fOuterBounds.Width() * fScale * CLAMP(fSampleSize, 1, 5), fOuterBounds.Height() * fScale * CLAMP(fSampleSize, 1, 5));
	NeedTempBitmap(frame);
	NeedTempView(frame);
	NeedEvenOdd(frame);
}

void
BSVGView::FreeTempBitmap()
{
	if (fTempBitmap)
		fTempBitmap->Lock();
	
	if (fTempView)
		fTempView->RemoveSelf();
	
	delete fTempView;
	delete fTempBitmap;
	delete fEvenOdd;
	
	fTempView = NULL;
	fTempBitmap = NULL;
	fEvenOdd = NULL;
	
#ifdef HIT_STATISTIC
	printf("statistics:\n");
	printf("bitmapmiss: %d\nbitmaphit: %d\n", fBitmapMissCount, fBitmapHitCount);
	printf("viewmiss: %d\nviewhit: %d\n", fViewMissCount, fViewHitCount);
	printf("evenoddmiss: %d\nevenoddhit: %d\n", fEvenOddMissCount, fEvenOddHitCount);
	printf("creationtime: %lld\n", creationtime);
#endif
}

#define WEIGHTED

void
BSVGView::ScaleOffscreen()
{
	int bytes = fOffscreenBitmap->BytesPerRow() / 4;
	int height = ((fOffscreenBitmap->Bounds().IntegerHeight() / fSampleSize)) * bytes;
	
	int line = 0;
	uint32 *bits = (uint32 *)fOffscreenBitmap->Bits();
	uint32 red = 0, green = 0, blue = 0, alpha = 0;
	uint32 *abspos = NULL;
	uint8 *pixel = new uint8[fSampleSize * fSampleSize * 4];
	
	//BStopWatch *alias = new BStopWatch("aliaswatch");
	switch (fSampleSize) {
#ifdef WEIGHTED
		case 5: {
			for (int i = 0; i < height; i += bytes, line += bytes * 5) {
				abspos = bits + line;
				for (int j = 0; j < bytes; j++) {
					memcpy(pixel     , abspos          	 , 20);
					memcpy(pixel + 20, abspos + bytes    , 20);
					memcpy(pixel + 40, abspos + bytes * 2, 20);
					memcpy(pixel + 60, abspos + bytes * 3, 20);
					memcpy(pixel + 80, abspos + bytes * 4, 20);
					
					red		= (	pixel[ 0] * 1 + pixel[ 4] * 2 + pixel[ 8] * 3 + pixel[12] * 2 + pixel[16] * 1 +
								pixel[20] * 2 + pixel[24] * 3 + pixel[28] * 4 + pixel[32] * 3 + pixel[36] * 2 +
								pixel[40] * 3 + pixel[44] * 4 + pixel[48] * 5 + pixel[52] * 4 + pixel[56] * 3 +
								pixel[60] * 2 + pixel[64] * 3 + pixel[68] * 4 + pixel[72] * 3 + pixel[76] * 2 +
								pixel[80] * 1 + pixel[84] * 2 + pixel[88] * 3 + pixel[92] * 2 + pixel[96] * 1) / 65;
					
					green	= (	pixel[ 1] * 1 + pixel[ 5] * 2 + pixel[ 9] * 3 + pixel[13] * 2 + pixel[17] * 1 +
								pixel[21] * 2 + pixel[25] * 3 + pixel[29] * 4 + pixel[33] * 3 + pixel[37] * 2 +
								pixel[41] * 3 + pixel[45] * 4 + pixel[49] * 5 + pixel[53] * 4 + pixel[57] * 3 +
								pixel[61] * 2 + pixel[65] * 3 + pixel[69] * 4 + pixel[73] * 3 + pixel[77] * 2 +
								pixel[81] * 1 + pixel[85] * 2 + pixel[89] * 3 + pixel[93] * 2 + pixel[97] * 1) / 65;
					
					blue	= (	pixel[ 2] * 1 + pixel[ 6] * 2 + pixel[10] * 3 + pixel[14] * 2 + pixel[18] * 1 +
								pixel[22] * 2 + pixel[26] * 3 + pixel[30] * 4 + pixel[34] * 3 + pixel[38] * 2 +
								pixel[42] * 3 + pixel[46] * 4 + pixel[50] * 5 + pixel[54] * 4 + pixel[58] * 3 +
								pixel[62] * 2 + pixel[66] * 3 + pixel[70] * 4 + pixel[74] * 3 + pixel[78] * 2 +
								pixel[82] * 1 + pixel[86] * 2 + pixel[90] * 3 + pixel[94] * 2 + pixel[98] * 1) / 65;
					
					alpha	= (	pixel[ 3] * 1 + pixel[ 7] * 2 + pixel[11] * 3 + pixel[15] * 2 + pixel[19] * 1 +
								pixel[23] * 2 + pixel[27] * 3 + pixel[31] * 4 + pixel[35] * 3 + pixel[39] * 2 +
								pixel[43] * 3 + pixel[47] * 4 + pixel[51] * 5 + pixel[55] * 4 + pixel[59] * 3 +
								pixel[63] * 2 + pixel[67] * 3 + pixel[71] * 4 + pixel[75] * 3 + pixel[79] * 2 +
								pixel[83] * 1 + pixel[87] * 2 + pixel[91] * 3 + pixel[95] * 2 + pixel[99] * 1) / 65;
					
					bits[i + j] = (alpha << 24) + (blue << 16) + (green << 8) + red;
					abspos += 5;
				}
			}
		} break;
		
		case 4: {
			for (int i = 0; i < height; i += bytes, line += bytes * 4) {
				abspos = bits + line;
				for (int j = 0; j < bytes; j++) {
					memcpy(pixel     , abspos            , 16);
					memcpy(pixel + 16, abspos + bytes    , 16);
					memcpy(pixel + 32, abspos + bytes * 2, 16);
					memcpy(pixel + 48, abspos + bytes * 3, 16);
					
					red		= (	pixel[ 0] * 1 + pixel[ 4] * 2 + pixel[ 8] * 2 + pixel[12] * 1 +
								pixel[16] * 2 + pixel[20] * 3 + pixel[24] * 3 + pixel[28] * 2 +
								pixel[32] * 2 + pixel[36] * 3 + pixel[40] * 3 + pixel[44] * 2 +
								pixel[48] * 1 + pixel[52] * 2 + pixel[56] * 2 + pixel[60] * 1) >> 5;
					
					green	= (	pixel[ 1] * 1 + pixel[ 5] * 2 + pixel[ 9] * 2 + pixel[13] * 1 +
								pixel[17] * 2 + pixel[21] * 3 + pixel[25] * 3 + pixel[29] * 2 +
								pixel[33] * 2 + pixel[37] * 3 + pixel[41] * 3 + pixel[45] * 2 +
								pixel[49] * 1 + pixel[53] * 2 + pixel[57] * 2 + pixel[61] * 1) >> 5;
					
					blue 	= (	pixel[ 2] * 1 + pixel[ 6] * 2 + pixel[10] * 2 + pixel[14] * 1 +
								pixel[18] * 2 + pixel[22] * 3 + pixel[26] * 3 + pixel[30] * 2 +
								pixel[34] * 2 + pixel[38] * 3 + pixel[42] * 3 + pixel[46] * 2 +
								pixel[50] * 1 + pixel[54] * 2 + pixel[58] * 2 + pixel[62] * 1) >> 5;
					
					alpha	= (	pixel[ 3] * 1 + pixel[ 7] * 2 + pixel[11] * 2 + pixel[15] * 1 +
								pixel[19] * 2 + pixel[23] * 3 + pixel[27] * 3 + pixel[31] * 2 +
								pixel[35] * 2 + pixel[39] * 3 + pixel[43] * 3 + pixel[47] * 2 +
								pixel[51] * 1 + pixel[55] * 2 + pixel[59] * 2 + pixel[63] * 1) >> 5;
					
					bits[i + j] = (alpha << 24) + (blue << 16) + (green << 8) + red;
					abspos += 4;
				}
			}
		} break;
		
		case 3: {
			for (int i = 0; i < height; i += bytes, line += bytes * 3) {
				abspos = bits + line;
				for (int j = 0; j < bytes; j++) {
					memcpy(pixel     , abspos            , 12);
					memcpy(pixel + 12, abspos + bytes    , 12);
					memcpy(pixel + 24, abspos + bytes * 2, 12);
					
					red		= ( pixel[ 0] * 1 + pixel[ 4] * 2 + pixel[ 8] * 1 +
								pixel[12] * 2 + pixel[16] * 3 + pixel[20] * 2 +
								pixel[24] * 1 + pixel[28] * 2 + pixel[32] * 1) / 15;
					
					green	= ( pixel[ 1] * 1 + pixel[ 5] * 2 + pixel[ 9] * 1 +
								pixel[13] * 2 + pixel[17] * 3 + pixel[21] * 2 +
								pixel[25] * 1 + pixel[29] * 2 + pixel[33] * 1) / 15;
					
					blue 	= ( pixel[ 2] * 1 + pixel[ 6] * 2 + pixel[10] * 1 +
								pixel[14] * 2 + pixel[18] * 3 + pixel[22] * 2 +
								pixel[26] * 1 + pixel[30] * 2 + pixel[34] * 1) / 15;
					
					alpha	= ( pixel[ 3] * 1 + pixel[ 7] * 2 + pixel[11] * 1 +
								pixel[15] * 2 + pixel[19] * 3 + pixel[23] * 2 +
								pixel[27] * 1 + pixel[31] * 2 + pixel[35] * 1) / 15;
					
					bits[i + j] = (alpha << 24) + (blue << 16) + (green << 8) + red;
					abspos += 3;
				}
			}
		} break;
#else
		case 5: {
			for (int i = 0; i < height; i += bytes, line += bytes * 5) {
				abspos = bits + line;
				for (int j = 0; j < bytes; j++) {
					memcpy(pixel     , abspos          	 , 20);
					memcpy(pixel + 20, abspos + bytes    , 20);
					memcpy(pixel + 40, abspos + bytes * 2, 20);
					memcpy(pixel + 60, abspos + bytes * 3, 20);
					memcpy(pixel + 80, abspos + bytes * 4, 20);
					
					red		= (pixel[0] + pixel[4] + pixel[ 8] + pixel[12] + pixel[16] + pixel[20] + pixel[24] + pixel[28] + pixel[32] + pixel[36] + pixel[40] + pixel[44] + pixel[48] + pixel[52] + pixel[56] + pixel[60] + pixel[64] + pixel[68] + pixel[72] + pixel[76] + pixel[80]
								+ pixel[84] + pixel[88] + pixel[92] + pixel[96]) / 25;
					green	= (pixel[1] + pixel[5] + pixel[ 9] + pixel[13] + pixel[17] + pixel[21] + pixel[25] + pixel[29] + pixel[33] + pixel[37] + pixel[41] + pixel[45] + pixel[49] + pixel[53] + pixel[57] + pixel[61] + pixel[65] + pixel[69] + pixel[73] + pixel[77] + pixel[81]
								+ pixel[85] + pixel[89] + pixel[93] + pixel[97]) / 25;
					blue	= (pixel[2] + pixel[6] + pixel[10] + pixel[14] + pixel[18] + pixel[22] + pixel[26] + pixel[30] + pixel[34] + pixel[38] + pixel[42] + pixel[46] + pixel[50] + pixel[54] + pixel[58] + pixel[62] + pixel[66] + pixel[70] + pixel[74] + pixel[78] + pixel[82]
								+ pixel[86] + pixel[90] + pixel[94] + pixel[98]) / 25;
					alpha	= (pixel[3] + pixel[7] + pixel[11] + pixel[15] + pixel[19] + pixel[23] + pixel[27] + pixel[31] + pixel[35] + pixel[39] + pixel[43] + pixel[47] + pixel[51] + pixel[55] + pixel[59] + pixel[63] + pixel[67] + pixel[71] + pixel[75] + pixel[79] + pixel[83]
								+ pixel[87] + pixel[91] + pixel[95] + pixel[99]) / 25;
					
					bits[i + j] = (alpha << 24) + (blue << 16) + (green << 8) + red;
					abspos += 5;
				}
			}
		} break;
		
		case 4: {
			for (int i = 0; i < height; i += bytes, line += bytes * 4) {
				abspos = bits + line;
				for (int j = 0; j < bytes; j++) {
					memcpy(pixel     , abspos            , 16);
					memcpy(pixel + 16, abspos + bytes    , 16);
					memcpy(pixel + 32, abspos + bytes * 2, 16);
					memcpy(pixel + 48, abspos + bytes * 3, 16);
					
					red		= (pixel[0] + pixel[4] + pixel[ 8] + pixel[12] + pixel[16] + pixel[20] + pixel[24] + pixel[28] + pixel[32] + pixel[36] + pixel[40] + pixel[44] + pixel[48] + pixel[52] + pixel[56] + pixel[60]) >> 4;
					green	= (pixel[1] + pixel[5] + pixel[ 9] + pixel[13] + pixel[17] + pixel[21] + pixel[25] + pixel[29] + pixel[33] + pixel[37] + pixel[41] + pixel[45] + pixel[49] + pixel[53] + pixel[57] + pixel[61]) >> 4;
					blue 	= (pixel[2] + pixel[6] + pixel[10] + pixel[14] + pixel[18] + pixel[22] + pixel[26] + pixel[30] + pixel[34] + pixel[38] + pixel[42] + pixel[46] + pixel[50] + pixel[54] + pixel[58] + pixel[62]) >> 4;
					alpha	= (pixel[3] + pixel[7] + pixel[11] + pixel[15] + pixel[19] + pixel[23] + pixel[27] + pixel[31] + pixel[35] + pixel[39] + pixel[43] + pixel[47] + pixel[51] + pixel[55] + pixel[59] + pixel[63]) >> 4;
					
					bits[i + j] = (alpha << 24) + (blue << 16) + (green << 8) + red;
					abspos += 4;
				}
			}
		} break;
		
		case 3: {
			for (int i = 0; i < height; i += bytes, line += bytes * 3) {
				abspos = bits + line;
				for (int j = 0; j < bytes; j++) {
					memcpy(pixel     , abspos            , 12);
					memcpy(pixel + 12, abspos + bytes    , 12);
					memcpy(pixel + 24, abspos + bytes * 2, 12);
					
					red		= (pixel[0] + pixel[4] + pixel[ 8] + pixel[12] + pixel[16] + pixel[20] + pixel[24] + pixel[28] + pixel[32]) / 9;
					green	= (pixel[1] + pixel[5] + pixel[ 9] + pixel[13] + pixel[17] + pixel[21] + pixel[25] + pixel[29] + pixel[33]) / 9;
					blue 	= (pixel[2] + pixel[6] + pixel[10] + pixel[14] + pixel[18] + pixel[22] + pixel[26] + pixel[30] + pixel[34]) / 9;
					alpha	= (pixel[3] + pixel[7] + pixel[11] + pixel[15] + pixel[19] + pixel[23] + pixel[27] + pixel[31] + pixel[35]) / 9;
					
					bits[i + j] = (alpha << 24) + (blue << 16) + (green << 8) + red;
					abspos += 3;
				}
			}
		} break;
#endif
		case 2: {
			for (int i = 0; i < height; i += bytes, line += bytes * 2) {
				abspos = bits + line;
				for (int j = 0; j < bytes; j++) {
					memcpy(pixel    , abspos        , 8);
					memcpy(pixel + 8, abspos + bytes, 8);
					
					red		= (pixel[0] + pixel[4] + pixel[ 8] + pixel[12]) >> 2;
					green	= (pixel[1] + pixel[5] + pixel[ 9] + pixel[13]) >> 2;
					blue	= (pixel[2] + pixel[6] + pixel[10] + pixel[14]) >> 2;
					alpha	= (pixel[3] + pixel[7] + pixel[11] + pixel[15]) >> 2;
					
					bits[i + j] = (alpha << 24) + (blue << 16) + (green << 8) + red;
					abspos += 2;
				}
			}
		} break;
		
		default: break; // something's wrong!
	}
	delete [] pixel;
	//delete alias;
}

void
BSVGView::LockUpdates()
{
	fLocked = true;
}

void
BSVGView::UnlockUpdates(bool redraw)
{
	fLocked = false;
	Redraw();
}

void
BSVGView::Redraw(bool force)
{
	if ((!fUpdateNeeded || fLocked) && !force)
		return;
	
	if (force)
		fUpdateNeeded = true;
	
	UpdateInnerFrame();
	
	BRect bounds = fViewBounds;
	if (fSuperSampling) {
		bounds.right *= fSampleSize;
		bounds.bottom *= fSampleSize;
	}
	
	if (!fOffscreenBitmap || bounds.right > fOffscreenBitmap->Bounds().right
		|| bounds.bottom > fOffscreenBitmap->Bounds().bottom) {
		
		delete fOffscreenBitmap;
		bounds.right += fSampleSize - 1;
		bounds.bottom += fSampleSize - 1;
		fOffscreenBitmap = new BBitmap(bounds, B_RGBA32, true);
	}
	
	BSVGView::Draw(fViewBounds);
}

void
BSVGView::Draw(BRect updateRect)
{
	// this little hack will clear any background when we are transparent
	if (fViewColor.alpha != 255 && Parent()) {
		switch (fTransparentHack) {
			case 0: MoveBy(Frame().Width() + 1, 0);
					fTransparentHack++;
					return;
			case 1: MoveBy(-Frame().Width() - 1, 0);
					fTransparentHack++;
					return;
			case 2: fTransparentHack = 0;
					break;
		}
	}
	
	if (fUpdateNeeded) {
		// clear the background
		uint32 backcolor = (fViewColor.alpha << 24) + (fViewColor.red << 16) + (fViewColor.green << 8) + fViewColor.blue;
		uint32 *bits = (uint32 *)fOffscreenBitmap->Bits();
		uint32 length = fOffscreenBitmap->BitsLength() / 4;
		fast_memset(length, backcolor, bits);
		
		// use the root transformation to scale and move everything
		Matrix2D matrix;
		
		// extra scale for supersampling
		float scale = CLAMP(fSampleSize, 1, 5);
		matrix.ScaleBy(BPoint(scale, scale));
		
		// focus point and actual scale
		matrix.TranslateBy(BPoint(fViewBounds.Width() * fViewCenter.x, fViewBounds.Height() * fViewCenter.y));
		matrix.ScaleBy(BPoint(fScale, fScale));
		matrix.TranslateBy(BPoint(-fOuterBounds.Width() * fFocusPoint.x, -fOuterBounds.Height() * fFocusPoint.y));
		
		// handling of out of bounds graphics
		if (fFitContent)
			matrix.TranslateBy(BPoint(-fOuterBounds.left, -fOuterBounds.top));
		
		fDocument->SetTransformation(matrix);
		
		// prepare and render into the bitmap
		//BStopWatch *drawtime = new BStopWatch("drawtime");
		PrepareTempBitmap();
		fOffscreenBitmap->Lock();
		//fRoot->DumpTree();
		fDocument->PrepareRendering();
		fDocument->Render(fOffscreenBitmap);
		fOffscreenBitmap->Unlock();
		FreeTempBitmap();
		//delete drawtime;
		
		if (fSuperSampling)
			ScaleOffscreen();
		
		fSourceRect.Set(0, 0, fOffscreenBitmap->Bounds().IntegerWidth() / fSampleSize - 1, fOffscreenBitmap->Bounds().IntegerHeight() / fSampleSize - 1);
		fUpdateNeeded = false;
	}
	
	if (fViewColor.alpha == 255) {
		SetDrawingMode(B_OP_COPY);
	} else {
		SetDrawingMode(B_OP_ALPHA);
		SetBlendingMode(B_PIXEL_ALPHA, B_ALPHA_COMPOSITE);
	}
	
	BRect destination = fSourceRect.OffsetToCopy(fSourceOrigin);
	DrawBitmap(fOffscreenBitmap, fSourceRect, destination);
}

void
BSVGView::FrameResized(float new_width, float new_height)
{
	fViewBounds.right = new_width;
	fViewBounds.bottom = new_height;
	fUpdateNeeded = true;
	Redraw();
}

void
BSVGView::MessageReceived(BMessage *message)
{
	if (!message)
		return;
	
	switch (message->what) {
		case B_ABOUT_REQUESTED: {
			(new BAlert("About SVGView", "SVGView rendered by libbsvg.", "OK"))->Go();
		} break;
		case B_SIMPLE_DATA:	{
			if (!fDropsEnabled) {
				BView::MessageReceived(message);
				return;
			}
			
			entry_ref ref;
			if (message->FindRef("refs", &ref) != B_OK)
				return;
			
			Clear();
			LoadFromFile(&ref);
		} break;
		case 'save': {
			entry_ref dir;
			BString name;
			if (message->FindRef("directory", &dir) != B_OK
				|| message->FindString("name", &name) != B_OK)
				break;
			
			BPath path(&dir);
			path.Append(name.String());
			
			ExportToFile(path.Path());
		} break;
		default: BView::MessageReceived(message); break;
	}
}

void
BSVGView::MouseDown(BPoint where)
{
	if (!fMenusEnabled || !fContextMenu) {
		BView::MouseDown(where);
		return;
	}
	
	BPoint pt;
	uint32 buttons;
	GetMouse(&pt, &buttons);
	
	if (buttons & B_SECONDARY_MOUSE_BUTTON) {
		BMenuItem *item = NULL;
		item = fContextMenu->FindItem('scal');
		if (item) item->SetMarked(ScaleToFit());
		item = fContextMenu->FindItem('fitc');
		if (item) item->SetMarked(FitContent());
		item = fContextMenu->FindItem('sup1');
		if (item) item->SetMarked(SampleSize() <= 1);
		item = fContextMenu->FindItem('sup2');
		if (item) item->SetMarked(SampleSize() == 2);
		item = fContextMenu->FindItem('sup3');
		if (item) item->SetMarked(SampleSize() == 3);
		item = fContextMenu->FindItem('sup4');
		if (item) item->SetMarked(SampleSize() == 4);
		item = fContextMenu->FindItem('sup5');
		if (item) item->SetMarked(SampleSize() >= 5);
		
		item = fContextMenu->Go(ConvertToScreen(where));
		if (!item)
			return;
		
		switch (item->Message()->what) {
			case 'scal': SetScaleToFit(!ScaleToFit()); break;
			case 'fitc': SetFitContent(!FitContent()); break;
			case 'sup1': SetSampleSize(1); break;
			case 'sup2': SetSampleSize(2); break;
			case 'sup3': SetSampleSize(3); break;
			case 'sup4': SetSampleSize(4); break;
			case 'sup5': SetSampleSize(5); break;
			case 'save': SavePanel(); return;
			case 'ssrc': ShowSource(); return;
			case 'zoin': ZoomIn(where, true); return;
			case 'zout': ZoomOut(where, true); return;
			case 'zres': ResetZoom(); return;
			// may be a user added menuitem
			default: MessageReceived(item->Message()); return;
		}
		
		// we only get here if a redraw is needed
		Redraw();
	}
}

void
BSVGView::SetScale(float scale)
{
	fUpdateNeeded = fUpdateNeeded || (fScale != scale);
	fScale = scale;
	
	if (fScaleToFit)
		fScaleToFit = false;
	
	Redraw();
}

float
BSVGView::Scale() const
{
	return fScale;
}

void
BSVGView::SetScaleToFit(bool enabled)
{
	fUpdateNeeded = fUpdateNeeded || (fScaleToFit != enabled);
	fScaleToFit = enabled;
	Redraw();
}

bool
BSVGView::ScaleToFit() const
{
	return fScaleToFit;
}

void
BSVGView::SetFitContent(bool enabled)
{
	fUpdateNeeded = fUpdateNeeded || (fFitContent != enabled);
	fFitContent = enabled;
	Redraw();
}

bool
BSVGView::FitContent() const
{
	return fFitContent;
}

void
BSVGView::SetSuperSampling(bool enabled)
{
	fUpdateNeeded = fUpdateNeeded || (fSuperSampling != enabled);
	fSuperSampling = enabled;
	Redraw();
}

bool
BSVGView::SuperSampling() const
{
	return fSuperSampling;
}

void
BSVGView::SetSampleSize(uint32 samplesize)
{
	fUpdateNeeded = fUpdateNeeded || (fSampleSize != CLAMP(samplesize, 1, 5));
	fSampleSize = CLAMP(samplesize, 1, 5);
	fSuperSampling = (fSampleSize != 1);
	Redraw();
}

uint32
BSVGView::SampleSize() const
{
	return fSampleSize;
}

void
BSVGView::SetFocusPoint(BPoint focus, bool in_views_system, bool percentage)
{
	if (!percentage) {
		if (in_views_system) {
			focus.x -= fViewBounds.Width() * fViewCenter.x;
			focus.y -= fViewBounds.Height() * fViewCenter.y;
			focus.x /= fScale;
			focus.y /= fScale;
			focus.x += fFocusPoint.x * fOuterBounds.Width();
			focus.y += fFocusPoint.y * fOuterBounds.Height();
		}
		
		focus.x = focus.x / fOuterBounds.Width();
		focus.y = focus.y / fOuterBounds.Height();
	}
	
	fUpdateNeeded = fUpdateNeeded || (fFocusPoint != focus);
	fFocusPoint = focus;
	Redraw();
}

BPoint
BSVGView::FocusPoint(bool as_percentage) const
{
	if (as_percentage)
		return fFocusPoint;
	
	return BPoint(fFocusPoint.x * fOuterBounds.Width(), fFocusPoint.y * fOuterBounds.Height());
}

void
BSVGView::SetViewCenter(BPoint center, bool percentage)
{
	if (!percentage) {
		center.x = center.x / fViewBounds.Width();
		center.y = center.y / fViewBounds.Height();
	}
	
	fUpdateNeeded = fUpdateNeeded || (fViewCenter != center);
	fViewCenter = center;
	Redraw();
}

BPoint
BSVGView::ViewCenter(bool as_percentage) const
{
	if (as_percentage)
		return fViewCenter;
	
	return BPoint(fViewCenter.x * fViewBounds.Width(), fViewCenter.y * fViewBounds.Height());
}

void
BSVGView::SetViewColor(rgb_color color)
{
	fUpdateNeeded = true;
	fViewColor = color;
	BView::SetViewColor(color);
	Redraw();
}

void
BSVGView::SetViewColor(uchar r, uchar g, uchar b, uchar a)
{
	fUpdateNeeded = true;
	SVGColor color(r, g, b, a);
	fViewColor = color.color;
	BView::SetViewColor(r, g, b, a);
	Redraw();
}

rgb_color
BSVGView::ViewColor() const
{
	return fViewColor;
}

void
BSVGView::SetPulseRate(bigtime_t rate)
{
	fPulseRate = rate;
	
	if (fPulseRate > 0 && !fPulseThread) {
		fPulseThread = spawn_thread(PulseThreadFunc, "BSVGView::PulseThread", B_LOW_PRIORITY, this);
		resume_thread(fPulseThread);
	}
}

bigtime_t
BSVGView::PulseRate() const
{
	return fPulseRate;
}

void
BSVGView::ZoomIn(BPoint focus, bool in_views_system, bool percentage)
{
	SetFocusPoint(focus, in_views_system, percentage);
	SetScale(fScale *= 1.1);
	fUpdateNeeded = true;
	Redraw();
}

void
BSVGView::ZoomOut(BPoint focus, bool in_views_system, bool percentage)
{
	SetFocusPoint(focus, in_views_system, percentage);
	SetScale(fScale *= 0.9);
	fUpdateNeeded = true;
	Redraw();
}

void
BSVGView::ResetZoom()
{
	fUpdateNeeded = fUpdateNeeded || (fFocusPoint != BPoint(0.5, 0.5) || fScale != 1.0);
	fFocusPoint.Set(0.5, 0.5);
	fScale = 1.0;
	Redraw();
}

void
BSVGView::SetDropsEnabled(bool enabled)
{
	fDropsEnabled = enabled;
}

bool
BSVGView::DropsEnabled() const
{
	return fDropsEnabled;
}

void
BSVGView::SetMenusEnabled(bool enabled)
{
	fMenusEnabled = enabled;
	
	if (enabled && !fContextMenu) {
		fContextMenu = new BPopUpMenu("fContextMenu", false, false);
		fContextMenu->AddItem(new BMenuItem("Scale To Fit", new BMessage('scal')));
		fContextMenu->AddItem(new BMenuItem("Fit Content", new BMessage('fitc')));
		fContextMenu->AddItem(new BSeparatorItem());
		fContextMenu->AddItem(new BMenuItem("Zoom In", new BMessage('zoin')));
		fContextMenu->AddItem(new BMenuItem("Zoom Out", new BMessage('zout')));
		fContextMenu->AddItem(new BMenuItem("Reset Zoom", new BMessage('zres')));
		fContextMenu->AddItem(new BSeparatorItem());
		fContextMenu->AddItem(new BMenuItem("No Antialiasing", new BMessage('sup1')));
		fContextMenu->AddItem(new BMenuItem("Antialias 2 Times", new BMessage('sup2')));
		fContextMenu->AddItem(new BMenuItem("Antialias 3 Times", new BMessage('sup3')));
		fContextMenu->AddItem(new BMenuItem("Antialias 4 Times", new BMessage('sup4')));
		fContextMenu->AddItem(new BMenuItem("Antialias 5 Times", new BMessage('sup5')));
		fContextMenu->AddItem(new BSeparatorItem());
		fContextMenu->AddItem(new BMenuItem("View SVG Source", new BMessage('ssrc')));
		fContextMenu->AddItem(new BMenuItem("Export File To Disk", new BMessage('save')));
	}
}

bool
BSVGView::MenusEnabled() const
{
	return fMenusEnabled;
}

BPopUpMenu *
BSVGView::ContextMenu()
{
	return fContextMenu;
}

float
BSVGView::GraphicsWidth() const
{
	return fGraphicsWidth;
}

float
BSVGView::GraphicsHeight() const
{
	return fGraphicsHeight;
}

status_t
BSVGView::InitializeXMLParser()
{
	if (fXMLHandlerLock < B_OK)
		fXMLHandlerLock = create_sem(1, "XMLHandlerLock");
	
	acquire_sem(fXMLHandlerLock);
	
	fParser = XML_ParserCreate(NULL);
	fCSSParser = new CSSParser();
	fInsideCSS = false;
	
	if (!fParser)
		return B_ERROR;
	
	XML_SetUserData(fParser, this);
	XML_SetStartElementHandler(fParser, XMLHandlerStart);
	XML_SetEndElementHandler(fParser, XMLHandlerEnd);
	XML_SetCharacterDataHandler(fParser, XMLHandlerText);
	
	return B_OK;
}

status_t
BSVGView::UninitializeXMLParser()
{
	if (fParser) {
		XML_ParserFree(fParser);
		fParser = NULL;
	}
	
	if (fCSSParser) {
		delete fCSSParser;
		fCSSParser = NULL;
	}
	
	release_sem(fXMLHandlerLock);
	
	return B_OK;
}

status_t
BSVGView::ParseXMLData(BPositionIO *data)
{
	data->Seek(0, SEEK_END);
	off_t streamsize = data->Position();
	char *buffer = new char[streamsize];
	data->Seek(0, SEEK_SET);
	
	size_t len = data->Read(buffer, streamsize);
	status_t result = ParseXMLBuffer(buffer, len);
	delete [] buffer;
	return result;
}

status_t
BSVGView::ParseXMLFile(BFile *file)
{
	if (!file->IsReadable())
		return B_ERROR;
	
	off_t filesize = 0;
	file->GetSize(&filesize);
	char *buffer = new char[filesize];
	
	size_t len = file->Read(buffer, filesize);
	status_t result = ParseXMLBuffer(buffer, len);
	delete [] buffer;
	return result;
}

status_t
BSVGView::ParseXMLNode(BNode *node, const char *attr)
{
	if (!node || node->InitCheck() != B_OK)
		return B_ERROR;
	
	attr_info info;
	if (node->GetAttrInfo(attr, &info) != B_OK || info.size <= 0)
		return B_ERROR;
	
	char *buffer = new char[info.size];
	
	if (node->ReadAttr(attr, info.type, 0, buffer, info.size) != info.size)
		return B_ERROR;
	
	status_t result = ParseXMLBuffer(buffer, info.size);
	
	delete [] buffer;
	return result;
}

status_t
BSVGView::ParseXMLBuffer(char *buffer, off_t size)
{
	status_t result = B_OK;
	
	if ((result = InitializeXMLParser()) != B_OK)
		return result;
	
	if (size >= 2 && (uchar)buffer[0] == 0x1f && (uchar)buffer[1] == 0x8b) {
		// seems to be a gziped file (svgz)
		const char *filename = "/var/tmp/svgunzip.tmp";
		BFile file(filename, B_READ_WRITE | B_CREATE_FILE);
		file.Write(buffer, size);
		
		uint32 outsize = *(uint32 *)(buffer + size - 4);
		uint8 *outbuffer = new uint8[outsize];
		
		gzFile gzfile = gzopen(filename, "rb");
		uint32 gz_result = gzread(gzfile, outbuffer, outsize);
		if (gz_result != outsize) {
			printf("Cannot decompress source file\n");
			delete outbuffer;
			return B_ERROR;
		}
		
		LockUpdates();
		if (!XML_Parse(fParser, (char *)outbuffer, outsize, 0)) {
			fprintf(stderr, "ParseXMLNode: Parse error at line %d:\n%s\n",
			XML_GetCurrentLineNumber(fParser),
			XML_ErrorString(XML_GetErrorCode(fParser)));
			UninitializeXMLParser();
			UnlockUpdates();
			return B_ERROR;
		}
		UnlockUpdates();
		
		delete outbuffer;
	} else {
		// seems to be a normal file (svg)
		LockUpdates();
		if (!XML_Parse(fParser, buffer, size, 0)) {
			fprintf(stderr, "ParseXMLNode: Parse error at line %d:\n%s\n",
			XML_GetCurrentLineNumber(fParser),
			XML_ErrorString(XML_GetErrorCode(fParser)));
			UninitializeXMLParser();
			UnlockUpdates();
			return B_ERROR;
		}
		UnlockUpdates();
	}
	
	if ((result = UninitializeXMLParser()) != B_OK)
		return result;
	
	fUpdateNeeded = true;
	Redraw();
	
	/*printf("TreeDump:\n");
	fDocument->DumpTree();*/
	
	return B_OK;
}

Matrix2D
BSVGView::HandleTransformation(BString &value)
{
	float a, b, c, d, e, f;
	Matrix2D transform;
	
	value.ReplaceAll(",", " ");
	const char *pos = value.String();
	const char *length = (const char *)(pos + value.Length());
	
	char type[11];
	memset(type, 0, 11);
	while (pos < length) {
		a = 0; b = 0; c = 0; d = 0; e = 0; f = 0;
		if (strstr(pos, "(") > 0) {
			strncpy(type, pos, 10);
			pos = strstr(pos, "(") + 1;
		}
		int32 count = sscanf(pos, "%f %f %f %f %f %f", &a, &b, &c, &d, &e, &f);
		switch (count) {
			case 0: a = 0; // fall through
			case 1: b = 0;
			case 2: c = 0;
			case 3: d = 0;
			case 4: e = 0;
			case 5: f = 0;
		}
		
		if (strstr(type, "matrix") > 0) {
			Matrix2D temp(a, c, e, b, d, f);
			transform *= temp;
		} else if (strstr(type, "translate") > 0) {
			transform.TranslateBy(BPoint(a, b));
		} else if (strstr(type, "scale") > 0) {
			if (count == 1)
				b = a;
			transform.ScaleBy(BPoint(a, b));
		} else if (strstr(type, "rotate") > 0) {
			if (count > 1) {
				transform.TranslateBy(BPoint(b, c));
				transform.RotateBy(a);
				transform.TranslateBy(BPoint(-b, -c));
			} else
				transform.RotateBy(a);
		} else if (strstr(type, "skewX") > 0) {
			transform.SkewBy(BPoint(a, 0));
		} else if (strstr(type, "skewY") > 0) {
			transform.SkewBy(BPoint(0, a));
		}
		
		pos = strstr(pos, ")");
		if (!pos)
			break;
		else
			pos++;
	}
	
	return transform;
}

void
BSVGView::XMLHandlerStart(void *userData, const char *name, const char **attr)
{
	BSVGView *view = (BSVGView *)userData;	
	BString nam(name);
	/*printf("<name=\"%s\">\n", name);
	for (int i = 0; attr[i]; i += 2) {
		printf("\t<attr=\"%s\">\n", attr[i]);
		printf("\t\t<val=\"%s\">\n", attr[i + 1]);
	}*/
	
	// handle anything non svg here (css for example)
	if (nam.Compare("style") == 0) {
		// probably text/css
		for (int i = 0; attr[i]; i += 2) {
			if (strcmp(attr[i], "type") == 0 && strcmp(attr[i + 1], "text/css") == 0) {
				view->fInsideCSS = true;
				break;
			}
		}
	}
	
	element_t type = B_SVG_ELEMENT;
	if (nam.Compare("svg") == 0) type = B_SVG_ROOT;
	else if (nam.Compare("g") == 0) type = B_SVG_GROUP;
	else if (nam.Compare("defs") == 0) type = B_SVG_DEFS;
	else if (nam.Compare("pattern") == 0) type = B_SVG_PATTERN;
	else if (nam.Compare("use") == 0) type = B_SVG_USE;
	else if (nam.Compare("path") == 0) type = B_SVG_PATH;
	else if (nam.Compare("rect") == 0) type = B_SVG_RECT;
	else if (nam.Compare("circle") == 0) type = B_SVG_CIRCLE;
	else if (nam.Compare("ellipse") == 0) type = B_SVG_ELLIPSE;
	else if (nam.Compare("line") == 0) type = B_SVG_LINE;
	else if (nam.Compare("polyline") == 0) type = B_SVG_POLYLINE;
	else if (nam.Compare("polygon") == 0) type = B_SVG_POLYGON;
	else if (nam.Compare("text") == 0) type = B_SVG_TEXT;
	else if (nam.Compare("tspan") == 0) type = B_SVG_TEXTSPAN;
	else if (nam.Compare("textPath") == 0) type = B_SVG_TEXTPATH;
	else if (nam.Compare("linearGradient") == 0) type = B_SVG_LINEARGRAD;
	else if (nam.Compare("radialGradient") == 0) type = B_SVG_RADIALGRAD;
	else if (nam.Compare("stop") == 0) type = B_SVG_STOP;
	else return;
	
	switch (type) {
		case B_SVG_DOCUMENT: return; // should only have one at any given time
		case B_SVG_ROOT: {
			BSVGRoot *element = new BSVGRoot(view);
			view->fDocument->OpenContainer(element);
			
			element->AddAttribute(view->fCSSParser->MatchElement(element, attr));
			for (int i = 0; attr[i]; i += 2)
				element->AddAttribute(new attribute_s(attr[i], attr[i + 1]));
			
			element->HandleAttributes();
			
			// svg elemts always establish a new viewport
			element->State()->general_viewport.source = SOURCE_SET;
			
			// ToDo: reenable this
			/*if (view->fInheritStates->CountItems() == 0) {
				// this is the root, setup the initial viewport
				element->State()->general_viewport.right = view->Frame().Width();
				element->State()->general_viewport.bottom = view->Frame().Height();
			}*/
		} break;
		
		case B_SVG_STOP: {
			if (!view->fGradient)
				return;
			
			BSVGStop *element = new BSVGStop();
			view->fGradient->AddStop(element);
			
			element->AddAttribute(view->fCSSParser->MatchElement(element, attr));
			for (int i = 0; attr[i]; i += 2)
				element->AddAttribute(new attribute_s(attr[i], attr[i + 1]));
			
			element->HandleAttributes();
		} break;
		
		case B_SVG_GROUP:
		case B_SVG_DEFS:
		case B_SVG_PATTERN: {
			BSVGElement *element = NULL;
			switch (type) {
				case B_SVG_GROUP: element = view->fDocument->OpenContainer(new BSVGGroup(view)); break;
				case B_SVG_DEFS: element = view->fDocument->OpenContainer(new BSVGDefs(view)); break;
				case B_SVG_PATTERN: element = view->fDocument->OpenContainer(new BSVGPattern(view)); break;
				default: break;
			}
			
			element->AddAttribute(view->fCSSParser->MatchElement(element, attr));
			for (int i = 0; attr[i]; i += 2)
				element->AddAttribute(new attribute_s(attr[i], attr[i + 1]));
			
			element->HandleAttributes();
		} break;
		
		case B_SVG_PATH:
		case B_SVG_RECT:
		case B_SVG_CIRCLE:
		case B_SVG_ELLIPSE:
		case B_SVG_LINE:
		case B_SVG_POLYLINE:
		case B_SVG_POLYGON: {
			BSVGPath *element = NULL;
			switch (type) {
				case B_SVG_PATH: element = new BSVGPath(view); break;
				case B_SVG_RECT: element = new BSVGRect(view); break;
				case B_SVG_CIRCLE: element = new BSVGCircle(view); break;
				case B_SVG_ELLIPSE: element = new BSVGEllipse(view); break;
				case B_SVG_LINE: element = new BSVGLine(view); break;
				case B_SVG_POLYLINE: element = new BSVGPolyline(view); break;
				case B_SVG_POLYGON: element = new BSVGPolygon(view); break;
				default: break;
			}
			
			if (!element)
				return;
			
			view->AddElement(element);
			
			element->AddAttribute(view->fCSSParser->MatchElement(element, attr));
			for (int i = 0; attr[i]; i += 2)
				element->AddAttribute(new attribute_s(attr[i], attr[i + 1]));
			
			element->HandleAttributes();
			element->FinalizeShape();
		} break;
		
		case B_SVG_LINEARGRAD:
		case B_SVG_RADIALGRAD: {
			BSVGGradient *element = NULL;
			if (type == B_SVG_LINEARGRAD)
				element = new BSVGLinearGradient(view);
			else if (type == B_SVG_RADIALGRAD)
				element = new BSVGRadialGradient(view);
			else
				return;
			
			view->fGradient = element;
			view->AddElement(element);
			
			element->AddAttribute(view->fCSSParser->MatchElement(element, attr));
			for (int i = 0; attr[i]; i += 2)
				element->AddAttribute(new attribute_s(attr[i], attr[i + 1]));
			
			element->HandleAttributes();
		} break;
		
		case B_SVG_TEXT:
		case B_SVG_TEXTSPAN:
		case B_SVG_TEXTPATH: {
			view->FlushText();
			
			BSVGTextSpan *element = NULL;
			if (type == B_SVG_TEXT) {
				element = new BSVGText(view);
				view->AddElement(element);
				view->fText = (BSVGText *)element;
			} else if (type == B_SVG_TEXTSPAN) {
				element = new BSVGTextSpan(view);
				view->fText->LastContainer()->AddElement(element);
			} else if (type == B_SVG_TEXTPATH) {
				element = new BSVGTextPath(view);
				view->fText->LastContainer()->AddElement(element);
			}
			
			element->AddAttribute(view->fCSSParser->MatchElement(element, attr));
			for (int i = 0; attr[i]; i += 2)
				element->AddAttribute(new attribute_s(attr[i], attr[i + 1]));
			
			element->HandleAttributes();
		} break;
		
		case B_SVG_USE: {
			BSVGUse *element = new BSVGUse(view);
			view->AddElement(element);
			
			element->AddAttribute(view->fCSSParser->MatchElement(element, attr));
			for (int i = 0; attr[i]; i += 2)
				element->AddAttribute(new attribute_s(attr[i], attr[i + 1]));
			
			element->HandleAttributes();
			element->CloseElement();
		}
		
		case B_SVG_ELEMENT:
		case B_SVG_GRADIENT:
		case B_SVG_STRING:
		case B_SVG_PAINTSERVER: break;
	}
}

void
BSVGView::XMLHandlerEnd(void *userData, const char *name)
{
	BSVGView *view = (BSVGView *)userData;
	BString nam(name);
	
	// anything non svg here
	if (nam.Compare("style") == 0) {
		// ending css parsing
		view->fInsideCSS = false;
		view->fCSSParser->Parse();
	}
	
	element_t type = B_SVG_ELEMENT;
	if (nam.Compare("svg") == 0) type = B_SVG_ROOT;
	else if (nam.Compare("g") == 0) type = B_SVG_GROUP;
	else if (nam.Compare("defs") == 0) type = B_SVG_DEFS;
	else if (nam.Compare("pattern") == 0) type = B_SVG_PATTERN;
	else if (nam.Compare("path") == 0) type = B_SVG_PATH;
	else if (nam.Compare("rect") == 0) type = B_SVG_RECT;
	else if (nam.Compare("circle") == 0) type = B_SVG_CIRCLE;
	else if (nam.Compare("ellipse") == 0) type = B_SVG_ELLIPSE;
	else if (nam.Compare("line") == 0) type = B_SVG_LINE;
	else if (nam.Compare("polyline") == 0) type = B_SVG_POLYLINE;
	else if (nam.Compare("polygon") == 0) type = B_SVG_POLYGON;
	else if (nam.Compare("text") == 0) type = B_SVG_TEXT;
	else if (nam.Compare("tspan") == 0) type = B_SVG_TEXTSPAN;
	else if (nam.Compare("textPath") == 0) type = B_SVG_TEXTPATH;
	else if (nam.Compare("linearGradient") == 0) type = B_SVG_LINEARGRAD;
	else if (nam.Compare("radialGradient") == 0) type = B_SVG_RADIALGRAD;
	else if (nam.Compare("stop") == 0) type = B_SVG_STOP;
	else if (nam.Compare("use") == 0) type = B_SVG_USE;
	else return;
	
	switch (type) {
		case B_SVG_ROOT:
		case B_SVG_GROUP:
		case B_SVG_DEFS:
		case B_SVG_PATTERN:
		case B_SVG_TEXTSPAN:
		case B_SVG_TEXTPATH: {
			view->FlushText();
			view->fDocument->CloseContainer();
		} break;
		
		case B_SVG_LINEARGRAD:
		case B_SVG_RADIALGRAD: {
			view->fGradient->SortStops();
			view->fGradient = NULL;
		} break;
		
		case B_SVG_TEXT: {
			view->FlushText();
			view->fText->FinalizeShape();
			view->fText->CloseContainer();
			
			view->fText->PrepareRendering();
			view->fText->RenderCommon();
			view->fText = NULL;
		} break;
		
		case B_SVG_ELEMENT:
		case B_SVG_DOCUMENT:
		case B_SVG_USE:
		case B_SVG_PATH:
		case B_SVG_RECT:
		case B_SVG_CIRCLE:
		case B_SVG_ELLIPSE:
		case B_SVG_LINE:
		case B_SVG_POLYLINE:
		case B_SVG_POLYGON:
		case B_SVG_STOP:
		case B_SVG_GRADIENT:
		case B_SVG_STRING:
		case B_SVG_PAINTSERVER: break;
	}
}

void
BSVGView::XMLHandlerText(void *userData, const char *text, int len)
{
	BSVGView *view = (BSVGView *)userData;
	
	// this is slow, but necessary as BString can't handle unicode chars here
	char buffer[len + 1];
	memset(buffer, 0, len + 1);
	int offset = 0;
	for (int i = 0; i < len; i++){
		if (text[i] > 0) {
			buffer[i - offset] = text[i];
		} else {
			offset++;
		}
	}
	
	// we may be parsing a css string
	if (view->fInsideCSS) {
		view->fCSSParser->AddData(buffer);
	} else {	
		view->fTextBuffer.Append(buffer);
	}
}

void
BSVGView::StripWhiteSpaces(BString *string)
{
	string->RemoveSet("\n\r");
	string->ReplaceAll("\t", " ");
	
	while (string->FindFirst("  ") >= 0)
		string->ReplaceAll("  ", " ");
}

void
BSVGView::FlushText()
{
	if (!fText) {
		fTextBuffer.SetTo("");
		return;
	}
	
	StripWhiteSpaces(&fTextBuffer);
	if (fTextBuffer.Length() <= 0)
		return;
	
	fText->AddString(fTextBuffer);
	fTextBuffer.SetTo("");
}

int32
BSVGView::PulseThreadFunc(void *data)
{
	BSVGView *view = (BSVGView *)data;
	if (!view)
		return -1;
	
	bigtime_t time_remaining = 0, start_time = 0;
	while (view->fPulseRate) {
		if (view->LockLooper()) {
			view->Redraw(true);
			view->UnlockLooper();
		}
		
		start_time = system_time();
		time_remaining = view->fPulseRate;
		while (time_remaining > 1000000) {
			snooze(1000000);
			time_remaining = view->fPulseRate - (system_time() - start_time);
		}
		
		if (time_remaining > 0)
			snooze(time_remaining);
	}
	
	view->fPulseThread = 0;
	return 0;
}
