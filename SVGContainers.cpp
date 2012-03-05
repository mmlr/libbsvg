#include <Bitmap.h>
#include <Message.h>
#include "SVGContainers.h"
#include "SVGView.h"
#include "UnitHandler.h"

BSVGDocument::BSVGDocument()
	:	BSVGElement()
{
}

BSVGDocument::BSVGDocument(BSVGView *parent)
	:	BSVGElement(parent)
{
}

BSVGDocument::BSVGDocument(BSVGView *parent, const BString &data)
	:	BSVGElement(parent, data)
{
}

BSVGDocument::BSVGDocument(BMessage *data)
	:	BSVGElement(data)
{
}

BArchivable *
BSVGDocument::Instantiate(BMessage *data)
{
	if (validate_instantiation(data, "BSVGDocument"))
		return new BSVGDocument(data);
	return NULL;
}

status_t
BSVGDocument::Archive(BMessage *data, bool deep) const
{
	BSVGElement::Archive(data, deep);
	ADDREPCLASS("BSVGDocument");
	ADDREPTYPE(B_SVG_DOCUMENT);
	return B_OK;
}	

void
BSVGDocument::HandleAttribute(attribute_s *attr)
{
}

void
BSVGDocument::RecreateData()
{
	fHeaderData.SetTo("");
	fHeaderData << "<?xml version=\"1.0\"?>\n";
	fHeaderData << "<!-- Generator: libbsvg " << EXPORT_LIB_VERSION << " - a native SVG-Library for BeOS -->\n";
	fHeaderData << "<!DOCTYPE svg PUBLIC \"-//W3C//DTD SVG 1.1//EN\" ";
	fHeaderData << "\"http://www.w3.org/Graphics/SVG/1.1/DTD/svg11.dtd\">";
	fFooterData.SetTo("");
	
	for (int i = 0; i < fElements.CountItems(); i++)
		fElements.ItemAt(i)->RecreateData();
}

void
BSVGDocument::CollectData(BString &into, int32 indentlevel)
{
	into << fHeaderData << "\n";
	for (int i = 0; i < fElements.CountItems(); i++)
		fElements.ItemAt(i)->CollectData(into, indentlevel);
}

// *********************************************

BSVGRoot::BSVGRoot()
	:	BSVGElement(),
		fTranslate(0, 0)
{
}

BSVGRoot::BSVGRoot(BSVGView *parent)
	:	BSVGElement(parent),
		fTranslate(0, 0)
{
}

BSVGRoot::BSVGRoot(BSVGView *parent, const BString &data)
	:	BSVGElement(parent, data),
		fTranslate(0, 0)
{
}

BSVGRoot::BSVGRoot(BMessage *data)
	:	BSVGElement(data),
		fTranslate(0, 0)
{
	data->FindPoint("_translate", &fTranslate);
}

BArchivable *
BSVGRoot::Instantiate(BMessage *data)
{
	if (validate_instantiation(data, "BSVGRoot"))
		return new BSVGRoot(data);
	return NULL;
}

status_t
BSVGRoot::Archive(BMessage *data, bool deep) const
{
	BSVGElement::Archive(data, deep);
	
	data->AddPoint("_translate", fTranslate);
	
	ADDREPCLASS("BSVGRoot");
	ADDREPTYPE(B_SVG_ROOT);
	return B_OK;
}	

void
BSVGRoot::HandleAttribute(attribute_s *attr)
{
	if (!attr)
		return;
	
	float val = 0;
	UnitHandler::Handle(attr, &val, &fState, fState.general_viewport.right);
	
	if (attr->id.Compare("width") == 0)
		fState.general_viewport.right = val;
	else if (attr->id.Compare("height") == 0)
		fState.general_viewport.bottom = val;
	else if (attr->id.Compare("x") == 0)
		fTranslate.x = val;
	else if (attr->id.Compare("y") == 0)
		fTranslate.y = val;
	else
		BSVGElement::HandleAttribute(attr);
}

void
BSVGRoot::RecreateData()
{
	fHeaderData.SetTo("<svg xmlns=\"http://www.w3.org/2000/svg\" version=\"1.1\"");
	
	if (fState.general_viewport.source == SOURCE_SET) {
		fHeaderData << " width=\"" << fState.general_viewport.Width();
		fHeaderData << "\" height=\"" << fState.general_viewport.Height() << "\"";
	}
	
	BSVGElement::RecreateData();
	if (fElements.CountItems() > 0)
		fFooterData.SetTo("</svg>");
}

void
BSVGRoot::PrepareRendering(SVGState *inherit)
{
	if (inherit)
		fRenderState.Inherit(*inherit, true);
	else
		fRenderState.Reset();
	
	fRenderState.Merge(fState, B_MULTIPLY_BY_SOURCE);
	fRenderState.general_transform.TranslateBy(fTranslate);
	
	for (int i = 0; i < fElements.CountItems(); i++)
		fElements.ItemAt(i)->PrepareRendering(&fRenderState);
}

// *********************************************

BSVGGroup::BSVGGroup()
	:	BSVGElement()
{
}

BSVGGroup::BSVGGroup(BSVGView *parent)
	:	BSVGElement(parent)
{
}

BSVGGroup::BSVGGroup(BSVGView *parent, const BString &data)
	:	BSVGElement(parent, data)
{
}

BSVGGroup::BSVGGroup(BMessage *data)
	:	BSVGElement(data)
{
}

BArchivable *
BSVGGroup::Instantiate(BMessage *data)
{
	if (validate_instantiation(data, "BSVGGroup"))
		return new BSVGGroup(data);
	return NULL;
}

status_t
BSVGGroup::Archive(BMessage *data, bool deep) const
{
	BSVGElement::Archive(data, deep);
	ADDREPCLASS("BSVGGroup");
	ADDREPTYPE(B_SVG_GROUP);
	return B_OK;
}	

void
BSVGGroup::RecreateData()
{
	fHeaderData.SetTo("<g");
	BSVGElement::RecreateData();
	if (fElements.CountItems() > 0)
		fFooterData.SetTo("</g>");
}

// *********************************************

BSVGDefs::BSVGDefs()
	:	BSVGElement()
{
}

BSVGDefs::BSVGDefs(BSVGView *parent)
	:	BSVGElement(parent)
{
}

BSVGDefs::BSVGDefs(BSVGView *parent, const BString &data)
	:	BSVGElement(parent, data)
{
}

BSVGDefs::BSVGDefs(BMessage *data)
	:	BSVGElement(data)
{
}

BArchivable *
BSVGDefs::Instantiate(BMessage *data)
{
	if (validate_instantiation(data, "BSVGDefs"))
		return new BSVGDefs(data);
	return NULL;
}

status_t
BSVGDefs::Archive(BMessage *data, bool deep) const
{
	BSVGElement::Archive(data, deep);
	ADDREPCLASS("BSVGDefs");
	ADDREPTYPE(B_SVG_DEFS);
	return B_OK;
}	

void
BSVGDefs::CollectBounds(BRect &bounds, bool *first)
{
	// as def elements are not rendered directly they have no bounds either
}

void
BSVGDefs::RecreateData()
{
	fHeaderData.SetTo("<defs");
	BSVGElement::RecreateData();
	if (fElements.CountItems() > 0)
		fFooterData.SetTo("</defs>");
	else
		fFooterData.SetTo("");
}

void
BSVGDefs::Render(BView *view)
{
	// do not render any def elements
}

void
BSVGDefs::Render(BWindow *window)
{
	// do not render any def elements
}

void
BSVGDefs::Render(BBitmap *bitmap)
{
	// do not render any def elements
}

void
BSVGDefs::RenderCommon()
{
	// do not render any def elements
}

// *********************************************

BSVGPattern::BSVGPattern()
	:	BSVGPaintServer(),
		fContentUnits(B_USERSPACE_ON_USE),
		fX(0),
		fY(0),
		fWidth(0),
		fHeight(0)
{
}

BSVGPattern::BSVGPattern(BSVGView *parent)
	:	BSVGPaintServer(parent),
		fContentUnits(B_USERSPACE_ON_USE),
		fX(0),
		fY(0),
		fWidth(0),
		fHeight(0)
{
}

BSVGPattern::BSVGPattern(BSVGView *parent, const BString &data)
	:	BSVGPaintServer(parent, data),
		fContentUnits(B_USERSPACE_ON_USE),
		fX(0),
		fY(0),
		fWidth(0),
		fHeight(0)
{
}

BSVGPattern::BSVGPattern(BMessage *data)
	:	BSVGPaintServer(data),
		fContentUnits(B_USERSPACE_ON_USE),
		fX(0),
		fY(0),
		fWidth(0),
		fHeight(0)
{
	data->FindInt32("_contentunits", (int32 *)&fContentUnits);
	data->FindFloat("_x", &fX);
	data->FindFloat("_y", &fY);
	data->FindFloat("_width", &fWidth);
	data->FindFloat("_height", &fHeight);
}

BArchivable *
BSVGPattern::Instantiate(BMessage *data)
{
	if (validate_instantiation(data, "BSVGPattern"))
		return new BSVGPattern(data);
	return NULL;
}

status_t
BSVGPattern::Archive(BMessage *data, bool deep) const
{
	BSVGPaintServer::Archive(data, deep);
	
	data->AddInt32("_contentunits", (int32)fContentUnits);
	data->AddFloat("_x", fX);
	data->AddFloat("_y", fY);
	data->AddFloat("_width", fWidth);
	data->AddFloat("_height", fHeight);
	
	ADDREPCLASS("BSVGPattern");
	ADDREPTYPE(B_SVG_PATTERN);
	return B_OK;
}	

void
BSVGPattern::CollectBounds(BRect &bounds, bool *first)
{
	// as pattern elements are not rendered directly they have no bounds either
}

void
BSVGPattern::RecreateData()
{
	BSVGPaintServer::RecreateData();
	fHeaderData.ReplaceFirst("<paintserver", "<pattern");
	fHeaderData.ReplaceLast("transform", "patternTransform");
	fHeaderData.ReplaceLast("serverUnits", "patternUnits");
	
	if (fContentUnits != B_USERSPACE_ON_USE)
		fHeaderData << " patternContentUnits=\"objectBoundingBox\"";
	
	fHeaderData << " x=\"" << fX << "\"";
	fHeaderData << " y=\"" << fY << "\"";
	fHeaderData << " width=\"" << fWidth << "\"";
	fHeaderData << " height=\"" << fHeight << "\">";
	
	fFooterData.SetTo("</pattern>");
}

void
BSVGPattern::HandleAttribute(attribute_s *attr)
{
	if (!attr)
		return;
	
	float val = 0;
	UnitHandler::Handle(attr, &val, &fState, fState.general_viewport.right);
	
	if (attr->id.Compare("patternTransform") == 0) {
			fState.general_transform *= BSVGView::HandleTransformation(attr->value);
	
	} else if (attr->id.Compare("patternUnits") == 0) {
		if (attr->value.Compare("objectBoundingBox") == 0)
			SetServerUnits(B_OBJECT_BOUNDING_BOX);
		else if (attr->value.Compare("userSpaceOnUse") == 0)
			SetServerUnits(B_USERSPACE_ON_USE);
	
	} else if (attr->id.Compare("patternContentUnits") == 0) {
		if (attr->value.Compare("objectBoundingBox") == 0)
			SetContentUnits(B_OBJECT_BOUNDING_BOX);
		else if (attr->value.Compare("userSpaceOnUse") == 0)
			SetContentUnits(B_USERSPACE_ON_USE);
	
	} else if (attr->id.Compare("x") == 0) {
		fX = val;
	
	} else if (attr->id.Compare("y") == 0) {
		fY = val;
	
	} else if (attr->id.Compare("width") == 0) {
		fWidth = val;
	
	} else if (attr->id.Compare("height") == 0) {
		fHeight = val;
	
	} else {
		BSVGPaintServer::HandleAttribute(attr);
	}
}

void
BSVGPattern::SetContentUnits(units_t units)
{
	fContentUnits = units;
}

units_t
BSVGPattern::ContentUnits() const
{
	return fContentUnits;
}

void
BSVGPattern::PrepareRendering(SVGState *inherit)
{
	if (inherit)
		fRenderState.Inherit(*inherit, true);
	else
		fRenderState.Reset();
	
	fRenderState.Merge(fState, B_MULTIPLY_BY_SOURCE);
	// don't prepare yet, prepare when referenced
}

void
BSVGPattern::Render(BView *view)
{
	// do not render any pattern elements
}

void
BSVGPattern::Render(BWindow *window)
{
	// do not render any pattern elements
}

void
BSVGPattern::Render(BBitmap *bitmap)
{
	// do not render any pattern elements
}

void
BSVGPattern::RenderCommon()
{
	// do not render any pattern elements
}

void
BSVGPattern::ApplyPaint(BView *view)
{
	BSVGPaintServer::ApplyPaint(view);
}

void
BSVGPattern::ApplyPaint(BBitmap *bitmap)
{
	if (!fParent)
		return;
	
	fRenderMatrix = CoordinateSystem() * fState.general_transform;
	
	BRect bounds = BRect(fX, fY, fX + fWidth, fY + fHeight);
	bounds.SetLeftTop(fRenderMatrix * bounds.LeftTop());
	bounds.SetRightBottom(fRenderMatrix * bounds.RightBottom());
	bounds.SetLeftTop(bounds.LeftTop() + fOffset);
	bounds.SetRightBottom(bounds.RightBottom() + fOffset);
	BPoint offset = bounds.LeftTop();
	bounds.OffsetTo(BPoint(0, 0));
	
	BBitmap *tile = fParent->NeedTempBitmap(bounds);
	memset(tile->Bits(), 0xff, tile->BitsLength());
	
	uint32 *bits = (uint32 *)tile->Bits();
	uint32 row = tile->BytesPerRow() / 4;
	for (int y = (int)bounds.top; y < bounds.bottom; y++) {
		for (int x = (int)bounds.left; x < bounds.right; x++) {
			bits[y * row + x] = 0xffff0000;
		}
	}

	fRenderState.general_transform = fRenderMatrix;
	fRenderState.general_transform.matrix[0][2] = 0;
	fRenderState.general_transform.matrix[1][2] = 0;
	
	tile->Lock();
	for (int i = 0; i < fElements.CountItems(); i++) {
		fElements.ItemAt(i)->PrepareRendering(&fRenderState);
		fElements.ItemAt(i)->Render(tile);
	}
	tile->Unlock();
}

// *********************************************

BSVGUse::BSVGUse()
	:	BSVGGroup(),
		fTranslate(0, 0)
{
}

BSVGUse::BSVGUse(BSVGView *parent)
	:	BSVGGroup(parent),
		fTranslate(0, 0)
{
}

BSVGUse::BSVGUse(BSVGView *parent, const BString &data)
	:	BSVGGroup(parent, data),
		fTranslate(0, 0)
{
}

BSVGUse::BSVGUse(BMessage *data)
	:	BSVGGroup(data),
		fTranslate(0, 0)
{
	data->FindPoint("_translate", &fTranslate);
}

BSVGUse::~BSVGUse()
{
	fElements.MakeEmpty(false);
}

BArchivable *
BSVGUse::Instantiate(BMessage *data)
{
	if (validate_instantiation(data, "BSVGUse"))
		return new BSVGUse(data);
	return NULL;
}

status_t
BSVGUse::Archive(BMessage *data, bool deep) const
{
	BSVGElement::Archive(data, deep);
	
	data->AddPoint("_translate", fTranslate);
	
	ADDREPCLASS("BSVGUse");
	ADDREPTYPE(B_SVG_USE);
	return B_OK;
}	

void
BSVGUse::HandleAttribute(attribute_s *attr)
{
	if (!attr)
		return;
	
	float val = 0;
	UnitHandler::Handle(attr, &val, &fState, fState.general_viewport.right);
	
	if (attr->id.Compare("x") == 0)
		fTranslate.x = val;
	else if (attr->id.Compare("y") == 0)
		fTranslate.y = val;
	else
		BSVGElement::HandleAttribute(attr);
}

void
BSVGUse::RecreateData()
{
	fHeaderData.SetTo("<use");
	AddID(fHeaderData);
	fState.AddToString(&fHeaderData);
	
	if (fLink != "")
		fHeaderData << " xlink:href=\"#" << fLink << "\"";
	
	if (fTranslate.x != 0 || fTranslate.y != 0) {
		fHeaderData << " x=\"" << fTranslate.x << "\"";
		fHeaderData << " y=\"" << fTranslate.y << "\"";
	}
	
	fHeaderData << " />";
	fFooterData.SetTo("");
}

void
BSVGUse::CollectData(BString &into, int32 indentlevel)
{
	if (fHeaderData != "") {
		INDENT(into, indentlevel);
		into << fHeaderData << "\n";
	}
	
	if (fFooterData != "") {
		INDENT(into, indentlevel);
		into << fFooterData << "\n";
	}
}

void
BSVGUse::PrepareRendering(SVGState *inherit)
{
	if (inherit)
		fRenderState.Inherit(*inherit, true);
	else
		fRenderState.Reset();
	
	fRenderState.Merge(fState, B_MULTIPLY_BY_SOURCE);
	fRenderState.general_transform.TranslateBy(fTranslate);
	
	if (fLink == "" || !fParent)
		return;
	
	BSVGElement *link = fParent->FindElement(fLink);
	
	if (link)
		fElements.AddItem(link);
}

void
BSVGUse::Render(BView *view)
{
	for (int i = 0; i < fElements.CountItems(); i++) {
		fElements.ItemAt(i)->PrepareRendering(&fRenderState);
		fElements.ItemAt(i)->Render(view);
	}
	
	fElements.MakeEmpty(false);
}

void
BSVGUse::Render(BWindow *window)
{
	for (int i = 0; i < fElements.CountItems(); i++) {
		fElements.ItemAt(i)->PrepareRendering(&fRenderState);
		fElements.ItemAt(i)->Render(window);
	}
	
	fElements.MakeEmpty(false);
}

void
BSVGUse::Render(BBitmap *bitmap)
{
	for (int i = 0; i < fElements.CountItems(); i++) {
		fElements.ItemAt(i)->PrepareRendering(&fRenderState);
		fElements.ItemAt(i)->Render(bitmap);
	}
	
	fElements.MakeEmpty(false);
}

void
BSVGUse::RenderCommon()
{
	BSVGElement::RenderCommon();
}
