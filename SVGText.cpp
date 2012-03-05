#include <String.h>
#include <Font.h>

#include "SVGText.h"
#include "SVGDefs.h"
#include "SVGView.h"
#include "SVGColors.h"
#include "PointIterator.h"
#include "DistanceIterator.h"
#include "TransformIterator.h"

#include "UnitHandler.h"

BSVGTextSpan::BSVGTextSpan(BSVGView *parent)
	:	BSVGPath(parent)
{
}

BSVGTextSpan::BSVGTextSpan(BSVGView *parent, SVGState state)
	:	BSVGPath(parent)
{
}

BSVGTextSpan::~BSVGTextSpan()
{
}

BSVGTextSpan::BSVGTextSpan(BMessage *data)
	:	BSVGPath(data)
{
	fParent = NULL;
	
	UNARCHIVELIST(fOffsetX, data, FindFloat, "_offset_x", float);
	UNARCHIVELIST(fOffsetY, data, FindFloat, "_offset_y", float);
	UNARCHIVELIST(fOffsetDX, data, FindFloat, "_offset_dx", float);
	UNARCHIVELIST(fOffsetDY, data, FindFloat, "_offset_dy", float);
	UNARCHIVELIST(fOffsetR, data, FindFloat, "_offset_r", float);
}

BArchivable *
BSVGTextSpan::Instantiate(BMessage *data)
{
	if (validate_instantiation(data, "BSVGTextSpan"))
		return new BSVGTextSpan(data);
	return NULL;
}

status_t
BSVGTextSpan::Archive(BMessage *data, bool deep) const
{
	BSVGPath::Archive(data, deep);
	
	ARCHIVELIST(fOffsetX, data, AddFloat, "_offset_x");
	ARCHIVELIST(fOffsetY, data, AddFloat, "_offset_y");
	ARCHIVELIST(fOffsetDX, data, AddFloat, "_offset_dx");
	ARCHIVELIST(fOffsetDY, data, AddFloat, "_offset_dy");
	ARCHIVELIST(fOffsetR, data, AddFloat, "_offset_r");
	
	ADDREPCLASS("BSVGTextSpan");
	ADDREPTYPE(B_SVG_TEXTSPAN);
	return B_OK;
}

void
BSVGTextSpan::HandleAttribute(attribute_s *attr)
{
	if (!attr)
		return;
	
	if (attr->id.Compare("x") == 0
		|| attr->id.Compare("y") == 0
		|| attr->id.Compare("dx") == 0
		|| attr->id.Compare("dy") == 0
		|| attr->id.Compare("rotate") == 0) {
		
		BString value = attr->value;
		value.ReplaceAll(",", " ");
		char *list = value.LockBuffer(value.Length());
		char *tok = strtok(list, " ,\t\n");
		
		offset_t target = B_OFFSET_X;
		if (attr->id[0] == 'x') target = B_OFFSET_X;
		else if (attr->id[0] == 'y') target = B_OFFSET_Y;
		else if (attr->id[1] == 'x') target = B_OFFSET_DX;
		else if (attr->id[1] == 'y') target = B_OFFSET_DY;
		else if (attr->id[0] == 'r') target = B_OFFSET_R;
		
		FloatList *offsetlist = GetOffsetList(target);
		
		attribute_s valuestyle;
		while (tok) {
			float *val = new float;
			valuestyle.value.SetTo(tok);
			if (UnitHandler::Handle(&valuestyle, val, &fState, fState.general_viewport.right))
				offsetlist->AddItem(val);
			
			tok = strtok(NULL, " ,\t\n");
		}
		
		value.UnlockBuffer(0);
		
	} else if (attr->id.Compare("startOffset") == 0)
		UnitHandler::Handle(attr, &fState.start_offset);
	else
		BSVGElement::HandleAttribute(attr);
}

void
BSVGTextSpan::GetBounds(BRect *bounds)
{
	for (int i = 0; i < fElements.CountItems(); i++)
		((BSVGTextSpan *)fElements.ItemAt(i))->GetBounds(bounds);
}	

void
BSVGTextSpan::PrepareFont()
{
	for (int i = 0; i < fElements.CountItems(); i++) {
		BSVGElement *element = fElements.ItemAt(i);
		switch (element->Type()) {
			case B_SVG_STRING: ((BSVGString *)element)->PrepareFont(); break;
			case B_SVG_TEXT:
			case B_SVG_TEXTPATH:
			case B_SVG_TEXTSPAN: ((BSVGTextSpan *)element)->PrepareFont(); break;
			default: break;
		}
	}
}

void
BSVGTextSpan::GetCharpos(CharposList *charpos, FloatList *chunklengths, int32 *index, bool *first)
{
	int32 oldindex = *index;
	*index = 0;
	for (int i = 0; i < fElements.CountItems(); i++)
		((BSVGTextSpan *)fElements.ItemAt(i))->GetCharpos(charpos, chunklengths, index, first);
	*index = oldindex;
}

void
BSVGTextSpan::AddToShape(CharposList *charpos, FloatList *chunklengths, BPoint *absolute, BPoint *relative, float *rotation, progress_t progress)
{
	for (int i = 0; i < fElements.CountItems(); i++)
		((BSVGTextSpan *)fElements.ItemAt(i))->AddToShape(charpos, chunklengths, absolute, relative, rotation, progress);
}

void
BSVGTextSpan::RecreateData()
{
	fHeaderData.SetTo("<tspan");
	AddID(fHeaderData);
	fState.AddToString(&fHeaderData);
	
	BString names[5] = { "x", "y", "dx", "dy", "rotate" };
	offset_t lists[5] = { B_OFFSET_X, B_OFFSET_Y, B_OFFSET_DX, B_OFFSET_DY, B_OFFSET_R };
	FloatList *list = NULL;
	for (int i = 0; i < 5; i++) {
		list = GetOffsetList(lists[i]);
		if (!list)
			continue;
		
		int count = list->CountItems();
		if (count <= 0)
			continue;
		
		fHeaderData << " " << names[i] << "=\"";
		
		for (int j = 0; j < count - 1; j++)
			fHeaderData << *list->ItemAt(j) << " ";
		fHeaderData << *list->ItemAt(count - 1) << "\"";
	}
	
	fHeaderData << ">";
	
	for (int i = 0; i < fElements.CountItems(); i++) {
		BSVGElement *element = fElements.ItemAt(i);
		element->RecreateData();
	}
	
	fFooterData.SetTo("</tspan>");
}

BSVGString *
BSVGTextSpan::FirstText()
{
	for (int i = 0; i < fElements.CountItems(); i++) {
		BSVGElement *element = fElements.ItemAt(i);
		if (element->Type() == B_SVG_STRING) {
			return (BSVGString *)element;
		} else {
			BSVGString *result = ((BSVGTextSpan *)element)->FirstText();
			if (result)
				return result;
		}
	}
	
	return NULL;
}

BSVGString *
BSVGTextSpan::LastText()
{
	for (int i = fElements.CountItems() - 1; i >= 0; i--) {
		BSVGElement *element = fElements.ItemAt(i);
		if (element->Type() == B_SVG_STRING) {
			return (BSVGString *)element;
		} else {
			BSVGString *result = ((BSVGTextSpan *)element)->LastText();
			if (result)
				return result;
		}
	}
	
	return NULL;
}

BSVGTextSpan *
BSVGTextSpan::LastTextContainer()
{
	for (int i = fElements.CountItems() - 1; i >= 0; i--) {
		BSVGTextSpan *span = ((BSVGTextSpan *)fElements.ItemAt(i))->LastTextContainer();
		if (span)
			return span;
	}
	
	return this;
}

void
BSVGTextSpan::AddChar(char chr)
{
	BString str = "";
	str << chr;
	AddString(str);
}

void
BSVGTextSpan::AddString(const char *string)
{
	AddString(BString(string));
}

void
BSVGTextSpan::AddString(const BString &string)
{
	if (LastContainer() != this)
		((BSVGTextSpan *)LastContainer())->AddString(string);
	else
		AddSpan(new BSVGString(fParent, string));
}

void
BSVGTextSpan::AddSpan(BSVGTextSpan *span)
{
	span->CloseElement();
	span->SetParentElement(this);
	LastContainer()->AddElement(span);
}

BString
BSVGTextSpan::WholeText()
{
	BString result = "";
	for (int i = 0; i < fElements.CountItems(); i++)
		result << ((BSVGTextSpan *)fElements.ItemAt(i))->WholeText();
	
	return result;
}

void
BSVGTextSpan::StripInterSpaces(bool *kept_last)
{
	for (int i = 0; i < fElements.CountItems(); i++) {
		BSVGTextSpan *span = (BSVGTextSpan *)fElements.ItemAt(i);
		span->StripInterSpaces(kept_last);
		if (span != fElements.ItemAt(i))
			i--;
	}
}

void
BSVGTextSpan::OverlapOffset(offset_t type)
{
	BSVGTextSpan *span = NULL;
	FloatList *spanlist = NULL;
	FloatList *ourlist = NULL;
	int length = 0;
	int index = 0;
	int overlap = 0;
	
	for (int i = 0; i < fElements.CountItems(); i++) {
		span = (BSVGTextSpan *)fElements.ItemAt(i);
		
		if (span->Type() == B_SVG_STRING) {
			index += ((BSVGString *)span)->Length();
		} else {
			spanlist = span->GetOffsetList(type);
			ourlist = GetOffsetList(type);
			length = span->WholeText().Length();
			
			// how many offsets to overlap?
			overlap = MIN(ourlist->CountItems() - index, length);
			for (int j = spanlist->CountItems(); j < overlap; j++)
				spanlist->AddItem(ourlist->RemoveItemAt(index));
			index += overlap;
		}
		
		span->OverlapOffset(type);
	}
}

void
BSVGTextSpan::OverlapOffsets()
{
	OverlapOffset(B_OFFSET_X);
	OverlapOffset(B_OFFSET_Y);
	OverlapOffset(B_OFFSET_DX);
	OverlapOffset(B_OFFSET_DY);
	OverlapOffset(B_OFFSET_R);
}

BSVGTextSpan *
BSVGTextSpan::TextParent()
{
	BSVGElement *parent = ParentElement();
	if ((parent->Type() & B_SVG_TEXT_KIND) != 0)
		return (BSVGTextSpan *)parent;
	
	return NULL;
}

BSVGText *
BSVGTextSpan::ParentText()
{
	BSVGElement *parent = FindParentElement(B_SVG_TEXT);
	if (parent)
		return (BSVGText *)parent;
	
	return NULL;
}

BSVGTextSpan *
BSVGTextSpan::ParentSpan()
{
	BSVGElement *parent = FindParentElement(B_SVG_TEXTSPAN);
	if (parent)
		return (BSVGTextSpan *)parent;
	
	return NULL;
}

BSVGTextPath *
BSVGTextSpan::ParentPath()
{
	BSVGElement *parent = FindParentElement(B_SVG_TEXTPATH);
	if (parent)
		return (BSVGTextPath *)parent;
	
	return NULL;
}

FloatList *
BSVGTextSpan::GetOffsetList(offset_t which)
{
	switch (which) {
		case B_OFFSET_X: return &fOffsetX;
		case B_OFFSET_Y: return &fOffsetY;
		case B_OFFSET_DX: return &fOffsetDX;
		case B_OFFSET_DY: return &fOffsetDY;
		case B_OFFSET_R: return &fOffsetR;
	}
	
	return NULL;
}

void
BSVGTextSpan::PrepareRendering(SVGState *inherit)
{
	BSVGElement::PrepareRendering(inherit);
}

void
BSVGTextSpan::Render(BView *view)
{
	BSVGPath::Render(view);
	BSVGElement::Render(view);
}

void
BSVGTextSpan::Render(BWindow *window)
{
	BSVGPath::Render(window);
	BSVGElement::Render(window);
}

void
BSVGTextSpan::Render(BBitmap *bitmap)
{
	BSVGPath::Render(bitmap);
	BSVGElement::Render(bitmap);
}

// ***************************************

BSVGString::BSVGString(BSVGView *parent, const BString &text)
	:	BSVGTextSpan(parent),
		fText(text)
{
}

BSVGString::BSVGString(BSVGView *parent, const char *text)
	:	BSVGTextSpan(parent),
		fText(text ? text : "")
{
}

BSVGString::BSVGString(BMessage *data)
	:	BSVGTextSpan(data)
{
	data->FindString("_text", &fText);
}

BArchivable *
BSVGString::Instantiate(BMessage *data)
{
	if (validate_instantiation(data, "BSVGString"))
		return new BSVGString(data);
	return NULL;
}

status_t
BSVGString::Archive(BMessage *data, bool deep) const
{
	BSVGTextSpan::Archive(data, deep);
	
	data->AddString("_text", fText);
	
	ADDREPCLASS("BSVGString");
	ADDREPTYPE(B_SVG_STRING);
	return B_OK;
}

void
BSVGString::GetBounds(BRect *bounds)
{
	*bounds = *bounds | fShape.Bounds();
}


void
BSVGString::PrepareFont()
{	
	BSVGTextSpan *parent = TextParent();
	if (!parent)
		return;
	
	SVGState state = *parent->RenderState();
	fFont = be_plain_font;
	font_family family;
	memset(family, 0, B_FONT_FAMILY_LENGTH + 1);
	strncpy(family, state.font_family.String(), B_FONT_FAMILY_LENGTH);
	
	fFont.SetFamilyAndStyle(family, NULL);
	fFont.SetSize(state.font_size());
}

void
BSVGString::GetCharpos(CharposList *charpos, FloatList *chunklengths, int32 *index, bool *first)
{
	BSVGTextSpan *parent = TextParent();
	if (!parent)
		return;
	
	int textlen = fText.Length();
	BPoint *escapes = new BPoint[textlen];
	BPoint *offsets = new BPoint[textlen];
	edge_info *edges = new edge_info[textlen];
	
	escapement_delta delta;
	delta.nonspace = 0;
	delta.space = 0;
	
	fFont.SetSize(100); // for better quality
	fFont.GetEscapements(fText.String(), textlen, &delta, escapes, offsets);
	fFont.GetEdges(fText.String(), textlen, edges);
	
	FloatList *offsetx = parent->GetOffsetList(B_OFFSET_X);
	FloatList *offsety = parent->GetOffsetList(B_OFFSET_Y);
	FloatList *offsetdx = parent->GetOffsetList(B_OFFSET_DX);
	FloatList *offsetdy = parent->GetOffsetList(B_OFFSET_DY);
	FloatList *offsetr = parent->GetOffsetList(B_OFFSET_R);
	
	for (int i = 0; i < textlen; i++) {
		charpos_s *pos = new charpos_s();
		
		if (*first) {
			pos->absolutex = true;
			pos->absolutey = true;
			*first = false;
		}
		
		if (offsetx->CountItems() > *index) {
			pos->position.x = *offsetx->ItemAt(*index);
			pos->absolutex = true;
		}
		
		if (offsety->CountItems() > *index) {
			pos->position.y = *offsety->ItemAt(*index);
			pos->absolutey = true;
		}
		
		if (pos->absolutex || pos->absolutey) {
			pos->anchor = fRenderState.text_anchor();
			chunklengths->AddItem(new float(0));
		}
		
		if (offsetdx->CountItems() > *index) {
			pos->relativex = true;
			pos->offset.x = *offsetdx->ItemAt(*index);
		} else
			pos->charoffset.x = offsets[i].x;
		
		if (offsetdy->CountItems() > *index) {
			pos->relativey = true;
			pos->offset.y = *offsetdy->ItemAt(*index);
		} else
			pos->charoffset.y = offsets[i].y;
		
		if (offsetr->CountItems() > *index)
			pos->rotation = *offsetr->ItemAt(*index);
		
		float kerning = (edges[i].left + -edges[i].right) * fRenderState.font_size();
		//pos->charwidth -= kerning;
		
		if (fRenderState.font_kerning.source != SOURCE_UNSET)
			pos->spacing += fRenderState.font_kerning();
		else
			pos->spacing += kerning;
		
		if (fText.String()[i] <= 32)
			pos->spacing += fRenderState.font_wordspacing();
		else
			pos->spacing += fRenderState.font_letterspacing();
		
		pos->charwidth += escapes[i].x * fRenderState.font_size();
		
		(*index)++;
		*chunklengths->LastItem() += pos->charwidth + pos->spacing;
		charpos->AddItem(pos);
	}
	
	delete [] escapes;
	delete [] offsets;
}

void
BSVGString::AddToShape(CharposList *charpos, FloatList *chunklengths, BPoint *absolute, BPoint *relative, float *rotation, progress_t progress)
{
	BSVGTextPath *textpath = ParentPath();
	if (textpath) {
		// we are part of a textpath
		BSVGPath *path = textpath->ResolvedPath();
		if (!path)
			return;
		
		int textlen = fText.Length();
		if (textlen <= 0)
			return;
		
		BShape *shapes[textlen];
		for (int i = 0; i < textlen; i++)
			shapes[i] = new BShape();
		
		fFont.SetSize(100); // for better quality
		fFont.GetGlyphShapes(fText.String(), textlen, shapes);
		
		bool invisible[textlen];
		PointList points;
		FloatList rotations;
		FloatList widths;
		
		// get the points and rotations
		float startpoint = absolute->x;
		float midpoint = startpoint;
		float endpoint = midpoint;
		FloatList offsets;
		
		/**************************************/
	
		for (int i = 0; i < textlen; i++) {
			charpos_s *pos = charpos->ItemAt(i);
			if (!pos)
				continue;
		
			// this may be a beginning chunk
			if (pos->absolutex || pos->absolutey) {
				float *chunklength = chunklengths->RemoveItemAt(0);
				
				if (chunklength) {
					switch (pos->anchor) {
						case B_ANCHOR_START: break; // leave as is
						case B_ANCHOR_MIDDLE: startpoint += *chunklength / 2;
						case B_ANCHOR_END: startpoint -= *chunklength;
					}
					
					float length = 0;
					if (fRenderState.start_offset.unit == PE_UNIT) {
						DistanceIterator iterator(&length);
						iterator.Iterate(path->Shape());
					}
								
					startpoint += fRenderState.ResolveValue(fRenderState.start_offset(), length);
					
					delete chunklength;
				}
			}
			
			midpoint = startpoint + pos->charwidth / 2;
			
			if (midpoint >= 0) {
				endpoint = startpoint + pos->charwidth;
				offsets.AddItem(new float(startpoint));
				offsets.AddItem(new float(midpoint));
				offsets.AddItem(new float(endpoint));
				widths.AddItem(new float(pos->charwidth));
				invisible[i] = false;
			} else {
				offsets.AddItem(new float(0));
				offsets.AddItem(new float(0));
				offsets.AddItem(new float(0));
				widths.AddItem(new float(0));
				invisible[i] = true;
			}
			
			startpoint += pos->charwidth + pos->spacing;
		}
		if (startpoint >= 0)
			offsets.AddItem(new float(startpoint));
		
		absolute->x = startpoint;
		
		if (offsets.CountItems() > 0) {
			PointIterator iterator(&offsets, &points);
			iterator.Iterate(path->Shape());
		}
		
		for (int i = 0; i < points.CountItems() - 3; i += 3) {
			// i + 0 = startpoint; i + 1 = midpoint; i + 2 = endpoint;
			BPoint delta = *points.ItemAt(i + 2) - *points.ItemAt(i + 0);
			float angle = BSVGPath::VectorsToAngle(BPoint(1, 0), delta);
			rotations.AddItem(new float(isnan(angle) ? 0 : angle));
		}
		points.RemoveLast();
		
		int count = MIN(textlen, points.CountItems() / 3);
		for (int i = 0; i < count; i++) {
			charpos_s *pos = charpos->RemoveItemAt(0);
			
			if (pos->relativex)
				relative->x += pos->offset.x;
			if (pos->relativey)
				relative->y += pos->offset.y;
			
			for (int j = 0; j < 3; j++)
				*points.ItemAt(i * 3 + j) += *relative + pos->charoffset;
			
			*rotation += pos->rotation;
		}
		
		/****************************************/
		
		Matrix2D matrix;
		float rot = 0;
		fShape.Clear();
		
		for (int i = 0; i < count; i++) {
			if (invisible[i]) {
				delete shapes[i];
				shapes[i] = NULL;
				continue;
			}
			
			matrix.Reset();
			
			matrix.TranslateBy(*points.ItemAt((i) * 3 + 1));
			rot = *rotations.ItemAt(i);
			if (rot != 0) matrix.RotateBy(rot);
			matrix.TranslateBy(BPoint(-(*widths.ItemAt(i)) / 2, 0));
			
			// for better quality
			float font_size = fRenderState.font_size() / 100;
			matrix.ScaleBy(BPoint(font_size, font_size));
			
			BShape temp_shape;
			TransformIterator transformer(&matrix, &temp_shape);
			transformer.Iterate(shapes[i]);
			
			fShape.AddShape(&temp_shape);
			
			delete shapes[i];
			shapes[i] = NULL;
		}
		return;
	}
	
	// we are not part of a textpath
	int textlen = fText.Length();
	if (textlen <= 0)
		return;
	
	BShape *shapes[textlen];
	for (int i = 0; i < textlen; i++)
		shapes[i] = new BShape();
	
	fFont.SetSize(100); // for better quality
	fFont.GetGlyphShapes(fText.String(), textlen, shapes);
	
	PointList points;
	FloatList rotations;
	FloatList widths;
	
	// get the points and rotations
	for (int i = 0; i < textlen; i++) {
		charpos_s *pos = charpos->RemoveItemAt(0);
		if (!pos)
			continue;
		
		// this may be a beginning chunk
		if (pos->absolutex || pos->absolutey) {
			float *chunklength = chunklengths->RemoveItemAt(0);
			
			if (chunklength) {
				if (pos->absolutex)
					absolute->x = pos->position.x;
				if (pos->absolutey)
					absolute->y = pos->position.y;
				
				switch (pos->anchor) {
					case B_ANCHOR_START: break; // leave as is
					case B_ANCHOR_MIDDLE: absolute->x += *chunklength / 2;
					case B_ANCHOR_END: absolute->x -= *chunklength;
				}
				
				delete chunklength;
			}
		}
		
		if (pos->relativex)
			relative->x += pos->offset.x;
		if (pos->relativey)
			relative->y += pos->offset.y;
		
		BPoint point = *absolute + *relative + pos->charoffset;
		*rotation += pos->rotation;
		
		points.AddItem(new BPoint(point));
		rotations.AddItem(new float(*rotation));
		
		if (*rotation != 0) {
			Matrix2D matrix;
			matrix.RotateBy(*rotation);
			*absolute += matrix * BPoint(pos->charwidth + pos->spacing, 0);
		} else {
			*absolute += BPoint(pos->charwidth + pos->spacing, 0);
		}
		
		delete pos;
	}
	
	Matrix2D matrix;
	float rot = 0;
	int count = MIN(textlen, points.CountItems());
	fShape.Clear();
	
	for (int i = 0; i < count; i++) {
		matrix.Reset();
		matrix.TranslateBy(*points.ItemAt(i));
		rot = *rotations.ItemAt(i);
		if (rot != 0) matrix.RotateBy(rot);
		
		// for better quality
		float font_size = fRenderState.font_size() / 100;
		matrix.ScaleBy(BPoint(font_size, font_size));
		
		BShape temp_shape;
		TransformIterator transformer(&matrix, &temp_shape);
		transformer.Iterate(shapes[i]);
		
		fShape.AddShape(&temp_shape);
		
		delete shapes[i];
		shapes[i] = NULL;
	}
}

void
BSVGString::RecreateData()
{
	fHeaderData.SetTo(fText);
	fFooterData.SetTo("");
}

BSVGString *
BSVGString::FirstText()
{
	return this;
}

BSVGString *
BSVGString::LastText()
{
	return this;
}

BSVGTextSpan *
BSVGString::LastTextContainer()
{
	return NULL;
}

void
BSVGString::SetText(const char *text)
{
	if (!text)
		ClearText();	
	fText.SetTo(text);
}

void
BSVGString::SetText(const BString &text)
{
	fText.SetTo(text);
}

void
BSVGString::ClearText()
{
	fText.SetTo("");
}

BString
BSVGString::Text()
{
	return fText;
}

int32
BSVGString::Length()
{
	return fText.Length();
}

BString
BSVGString::WholeText()
{
	return fText;
}

void
BSVGString::StripInterSpaces(bool *kept_last)
{
	if (*kept_last && fText.FindFirst(" ") == 0)
		fText.RemoveFirst(" ");
	
	int32 length = fText.Length();
	if (length <= 0) {
		delete RemoveSelf();
		return;
	}
	
	*kept_last = fText.FindLast(" ") == length - 1;
}

// ***************************************
BSVGTextPath::BSVGTextPath(BSVGView *parent)
	:	BSVGTextSpan(parent)
{
}

BSVGTextPath::BSVGTextPath(BSVGView *parent, BString path)
	:	BSVGTextSpan(parent)
{
}

BSVGTextPath::BSVGTextPath(BMessage *data)
	:	BSVGTextSpan(data)
{
}

BArchivable *
BSVGTextPath::Instantiate(BMessage *data)
{
	if (validate_instantiation(data, "BSVGTextPath"))
		return new BSVGTextPath(data);
	return NULL;
}

status_t
BSVGTextPath::Archive(BMessage *data, bool deep) const
{
	BSVGTextSpan::Archive(data, deep);
	ADDREPCLASS("BSVGTextPath");
	ADDREPTYPE(B_SVG_TEXTPATH);
	return B_OK;
}

void
BSVGTextPath::FinalizeShape()
{
}

void
BSVGTextPath::RecreateData()
{
	BSVGTextSpan::RecreateData();
	BString replaceby = "<textPath";
	if (fLink != "")
		replaceby << " xlink:href=\"#" << fLink << "\"";
	
	fHeaderData.ReplaceFirst("<tspan", replaceby.String());
	fFooterData.SetTo("</textPath>");
}

BSVGPath *
BSVGTextPath::ResolvedPath()
{
	if (fLink == "") {
		BSVGTextPath *parent = ParentPath();
		
		if (!parent)
			return NULL;
		
		return parent->ResolvedPath();
	}
	
	BSVGElement *element = fParent->FindElement(fLink);
	if (element && element->Type() == B_SVG_PATH)
		return (BSVGPath *)element;
	
	return NULL;
}

// ***************************************
BSVGText::BSVGText(BSVGView *parent)
	:	BSVGTextSpan(parent)
{
}

BSVGText::BSVGText(BSVGView *parent, SVGState state)
	:	BSVGTextSpan(parent)
{
	SetState(state);
}

BSVGText::BSVGText(BMessage *data)
	:	BSVGTextSpan(data)
{
	FinalizeShape();
}

BArchivable *
BSVGText::Instantiate(BMessage *data)
{
	if (validate_instantiation(data, "BSVGText"))
		return new BSVGText(data);
	return NULL;
}

status_t
BSVGText::Archive(BMessage *data, bool deep) const
{
	BSVGTextSpan::Archive(data, deep);
	ADDREPCLASS("BSVGText");
	ADDREPTYPE(B_SVG_TEXT);
	return B_OK;
}

void
BSVGText::FinalizeShape()
{
	StripWhiteSpaces();
	OverlapOffsets();
}

void
BSVGText::StripWhiteSpaces()
{
	BSVGString *string = FirstText();
	BString text = "";
	
	while (string) {
		text = string->Text();
		if (text.FindFirst(" ") == 0) {
			text.RemoveFirst(" ");
			
			if (text.Length() == 0) {
				delete string->RemoveSelf();
				string = FirstText();
				continue;
			} else
				string->SetText(text);
		}
		break;
	}
	
	string = LastText();
	while (string) {
		text = string->Text();
		if (text.FindLast(" ") == text.Length() - 1) {
			text.RemoveLast(" ");
			
			if (text.Length() == 0) {
				delete string->RemoveSelf();
				string = LastText();
				continue;
			} else
				string->SetText(text);
		}
		break;
	}
	
	bool kept_last = false;
	StripInterSpaces(&kept_last);
}

void
BSVGText::RecreateData()
{
	BSVGTextSpan::RecreateData();
	fHeaderData.ReplaceFirst("<tspan", "<text");
	fFooterData.SetTo("</text>");
}

void
BSVGText::PrepareRendering(SVGState *inherit)
{
	BSVGPath::PrepareRendering(inherit);
}

void
BSVGText::Render(BView *view)
{
	RenderCommon();
	for (int i = 0; i < fElements.CountItems(); i++)
		fElements.ItemAt(i)->Render(view);
}

void
BSVGText::Render(BWindow *window)
{
	RenderCommon();
	for (int i = 0; i < fElements.CountItems(); i++)
		fElements.ItemAt(i)->Render(window);
}

void
BSVGText::Render(BBitmap *bitmap)
{
	RenderCommon();
	for (int i = 0; i < fElements.CountItems(); i++)
		fElements.ItemAt(i)->Render(bitmap);
}

void
BSVGText::RenderCommon()
{
	BPoint absolute(0, 0);
	BPoint relative(0, 0);
	float rotate = 0;
	bool first = true;
	int32 index = 0;
	CharposList charpos;
	FloatList chunklengths;
	
	// will recurse through all the sub-spans
	PrepareFont();
	GetCharpos(&charpos, &chunklengths, &index, &first);
	AddToShape(&charpos, &chunklengths, &absolute, &relative, &rotate, B_PROGRESS_RL_TB);
}

BRect
BSVGText::ShapeBounds()
{
	BRect bounds;
	GetBounds(&bounds);
	return fRenderState.general_transform.ApplyToRect(bounds);
}

void
BSVGText::CollectBounds(BRect &bounds, bool *first)
{
	if (*first) {
		bounds = ShapeBounds();
		*first = false;
	} else
		bounds = bounds | ShapeBounds();
	
	// recursion handled by ShapeBounds() so don't add children
}
