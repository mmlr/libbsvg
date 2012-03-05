#include <Bitmap.h>
#include <Rect.h>
#include <Shape.h>
#include <StopWatch.h>
#include <SupportDefs.h>
#include <View.h>

#include "SVGGradient.h"
#include "SVGStop.h"
#include "SVGView.h"
#include "Matrix.h"
#include "ObjectList.h"

#include "UnitHandler.h"

// compare function for sortlist
int
StopCompare(const BSVGElement *item1, const BSVGElement *item2)
{
	const BSVGStop *stop1 = (const BSVGStop *)item1;
	const BSVGStop *stop2 = (const BSVGStop *)item2;
	
	if (!stop1 || !stop2)
		return 0;
	
	if (stop1->Offset() > stop2->Offset())
		return 1;
	else if (stop1->Offset() < stop2->Offset())
		return -1;
	
	return 0;
}

BSVGGradient::BSVGGradient(BSVGView *parent)
	:	BSVGPaintServer(parent),
		fStart(0, 0),
		fEnd(1, 0),
		fStartX(false),
		fStartY(false),
		fEndX(false),
		fEndY(false),
		fSpreadMethod(B_SPREAD_PAD)
{
}

BSVGGradient::BSVGGradient(BSVGView *parent, BRect frame, BPoint start, BPoint end)
	:	BSVGPaintServer(parent),
		fStart(start),
		fEnd(end),
		fStartX(false),
		fStartY(false),
		fEndX(false),
		fEndY(false),
		fSpreadMethod(B_SPREAD_PAD)
{
}

BSVGGradient::~BSVGGradient()
{
	if (fLink != "")
		fElements.MakeEmpty(false);
}

BSVGGradient::BSVGGradient(BMessage *data)
	:	BSVGPaintServer(data)
{
	data->FindPoint("_start", &fStart);
	data->FindPoint("_end", &fEnd);
	data->FindInt32("_spreadmethod", (int32 *)&fSpreadMethod);
	data->FindBool("_startx", &fStartX);
	data->FindBool("_starty", &fStartY);
	data->FindBool("_endx", &fEndX);
	data->FindBool("_endy", &fEndY);
}
	
BArchivable *
BSVGGradient::Instantiate(BMessage *data)
{
	if (validate_instantiation(data, "BSVGGradient"))
		return new BSVGGradient(data);
	return NULL;
}

status_t
BSVGGradient::Archive(BMessage *data, bool deep) const
{
	BSVGPaintServer::Archive(data, deep);
	
	data->AddPoint("_start", fStart);
	data->AddPoint("_end", fEnd);
	data->AddInt32("_spreadmethod", (int32)fSpreadMethod);
	data->AddBool("_startx", fStartX);
	data->AddBool("_starty", fStartY);
	data->AddBool("_endx", fEndX);
	data->AddBool("_endy", fEndY);
	
	ADDREPCLASS("BSVGGradient");
	ADDREPTYPE(B_SVG_GRADIENT);
	
	return B_OK;
}

void
BSVGGradient::HandleAttribute(attribute_s *attr)
{
	if (!attr)
		return;
	
	if (attr->id.Compare("gradientTransform") == 0) {
			fState.general_transform *= BSVGView::HandleTransformation(attr->value);
	
	} else if (attr->id.Compare("gradientUnits") == 0) {
		if (attr->value.Compare("objectBoundingBox") == 0)
			SetServerUnits(B_OBJECT_BOUNDING_BOX);
		else if (attr->value.Compare("userSpaceOnUse") == 0)
			SetServerUnits(B_USERSPACE_ON_USE);
	
	} else if (attr->id.Compare("spreadMethod") == 0) {
		if (attr->value.Compare("pad") == 0)
			SetSpreadMethod(B_SPREAD_PAD);
		else if (attr->value.Compare("reflect") == 0)
			SetSpreadMethod(B_SPREAD_REFLECT);
		else if (attr->value.Compare("repeat") == 0)
			SetSpreadMethod(B_SPREAD_REPEAT);
		
	} else {
		BSVGPaintServer::HandleAttribute(attr);
	}
}

void
BSVGGradient::SetVector(BPoint start, BPoint end)
{
	fStartX = true;
	fStartY = true;
	fStart = start;
	fEndX = true;
	fEndY = true;
	fEnd = end;
}

void
BSVGGradient::SetStart(BPoint start)
{
	fStartX = true;
	fStartY = true;
	fStart = start;
}

void
BSVGGradient::SetEnd(BPoint end)
{
	fEndX = true;
	fEndY = true;
	fEnd = end;
}

BPoint
BSVGGradient::Start() const
{
	return fStart;
}

BPoint
BSVGGradient::End() const
{
	return fEnd;
}

void
BSVGGradient::SetSpreadMethod(spread_t method)
{
	fSpreadMethod = method;
}

spread_t
BSVGGradient::SpreadMethod() const
{
	return fSpreadMethod;
}

void
BSVGGradient::AddStop(BSVGStop *stop)
{
	AddElement(stop);
}

BSVGStop *
BSVGGradient::StopAt(int index)
{
	if (index >= fElements.CountItems())
		return NULL;
	
	return (BSVGStop *)fElements.ItemAt(index);
}

BSVGStop *
BSVGGradient::RemoveStopAt(int index)
{
	return (BSVGStop *)fElements.RemoveItemAt(index);
}

bool
BSVGGradient::RemoveStop(BSVGStop *stop)
{
	return fElements.RemoveItem(stop);
}

int32
BSVGGradient::CountStops()
{
	return fElements.CountItems();
}

void
BSVGGradient::ClearStops()
{
	fElements.MakeEmpty();
}

void
BSVGGradient::SortStops()
{
	fElements.SortItems(&StopCompare);
}

bool
BSVGGradient::IsFullyOpaque() const
{
	for (int i = 0; i < fElements.CountItems(); i++) {
		if (fElements.ItemAt(i)->Type() == B_SVG_STOP
			&& !((BSVGStop *)fElements.ItemAt(i))->IsOpaque())
				return false;
	}
	
	return true;
}

void
BSVGGradient::Recalculate()
{
	// not implemented in the base class
}

void
BSVGGradient::ApplyPaint(BView *view)
{
	// not implemented in base class
}

void
BSVGGradient::ApplyPaint(BBitmap *bitmap)
{
	// not implemented in base class
}

void
BSVGGradient::PrepareRendering(SVGState *inherit)
{
	BSVGPaintServer::PrepareRendering(inherit);
	fRenderState.general_transform = fState.general_transform;
}

void
BSVGGradient::Render(BView *view)
{
	// not done as gradients are not directly rendered
}

void
BSVGGradient::Render(BWindow *window)
{
	// not done as gradients are not directly rendered
}

void
BSVGGradient::Render(BBitmap *bitmap)
{
	// not done as gradients are not directly rendered
}

void
BSVGGradient::RenderCommon()
{
	// not done as gradients are not directly rendered
}

BSVGElement *
BSVGGradient::ResolveLink()
{
	if (fLink == "" || fStatus == B_STATUS_RESOLVED)
		return NULL;
	
	BSVGElement *link = fParent->FindElement(fLink, B_SVG_LINEARGRAD | B_SVG_RADIALGRAD);
	
	if (!link)
		return NULL;
	
	fRenderState.Inherit(link->fRenderState, true);
	fRenderState.Merge(fState, B_MULTIPLY_BY_SOURCE);
	
	for (int i = 0; i < link->fElements.CountItems(); i++)
		fElements.AddItem(link->fElements.ItemAt(i));
	
	fStatus = B_STATUS_RESOLVED;
	return link;
}

/************* specific functions for BSVGLinearGradient
*/

BSVGLinearGradient::BSVGLinearGradient(BSVGView *parent)
	:	BSVGGradient(parent)
{
}

BSVGLinearGradient::BSVGLinearGradient(BSVGView *parent, BRect frame, BPoint start, BPoint end)
	:	BSVGGradient(parent, frame, start, end)
{
}

BSVGLinearGradient::BSVGLinearGradient(BMessage *data)
	:	BSVGGradient(data)
{
}
	
BArchivable *
BSVGLinearGradient::Instantiate(BMessage *data)
{
	if (validate_instantiation(data, "BSVGLinearGradient"))
		return new BSVGLinearGradient(data);
	return NULL;
}

status_t
BSVGLinearGradient::Archive(BMessage *data, bool deep) const
{
	BSVGGradient::Archive(data, deep);
	
	ADDREPCLASS("BSVGLinearGradient");
	ADDREPTYPE(B_SVG_LINEARGRAD);
	
	return B_OK;
}

void
BSVGLinearGradient::HandleAttribute(attribute_s *attr)
{
	if (!attr)
		return;
	
	float val = 0;
	UnitHandler::Handle(attr, &val, &fState, 1);
	
	if (attr->id.Compare("x1") == 0)
		SetStart(BPoint(val, fStart.y));
	else if (attr->id.Compare("y1") == 0)
		SetStart(BPoint(fStart.x, val));
	else if (attr->id.Compare("x2") == 0)
		SetEnd(BPoint(val, fEnd.y));
	else if (attr->id.Compare("y2") == 0)
		SetEnd(BPoint(fEnd.x, val));
	else
		BSVGGradient::HandleAttribute(attr);
}

void
BSVGLinearGradient::RecreateData()
{
	BSVGPaintServer::RecreateData();
	fHeaderData.ReplaceFirst("<paintserver", "<linearGradient");
	fHeaderData.ReplaceLast("transform", "gradientTransform");
	fHeaderData.ReplaceLast("serverUnits", "gradientUnits");
	
	if (fSpreadMethod != B_SPREAD_PAD) {
		fHeaderData << " spreadMethod=\"";
		switch (fSpreadMethod) {
			case B_SPREAD_PAD: fHeaderData << "pad"; break;
			case B_SPREAD_REFLECT: fHeaderData << "reflect"; break;
			case B_SPREAD_REPEAT: fHeaderData << "repeat"; break;
		}
		fHeaderData << "\"";
	}
	
	fHeaderData << " x1=\"" << fStart.x << "\" y1=\"" << fStart.y;
	fHeaderData << "\" x2=\"" << fEnd.x << "\" y2=\"" << fEnd.y << "\">";
	
	for (int i = 0; i < fElements.CountItems(); i++)
		fElements.ItemAt(i)->RecreateData();
	
	fFooterData.SetTo("</linearGradient>");
}

void
BSVGLinearGradient::Recalculate()
{
	fRenderMatrix = CoordinateSystem() * fRenderState.general_transform;
	fRenderEnd = fRenderMatrix * fEnd;
	fRenderStart = fRenderMatrix * fStart;
	fRenderEnd += fOffset;
	fRenderStart += fOffset;
	
	BPoint vector = fRenderEnd - fRenderStart;
	if (vector.x == 0) {
		fHorizontal = true;
		return;
	}
	
	fHorizontal = false;
	float slope = vector.y / vector.x;
	float top_diff = fRenderStart.y - fBounds.top;
	float bottom_diff = fRenderStart.y - fBounds.bottom;
	fIntersections[0] = BPoint(top_diff * slope + fRenderStart.x, fBounds.top);
	fIntersections[1] = BPoint(bottom_diff * slope + fRenderStart.x, fBounds.bottom);
	top_diff = fRenderEnd.y - fBounds.top;
	bottom_diff = fRenderEnd.y - fBounds.bottom;
	fIntersections[2] = BPoint(top_diff * slope + fRenderEnd.x, fBounds.top);
	fIntersections[3] = BPoint(bottom_diff * slope + fRenderEnd.x, fBounds.bottom);
	
	if (fabs(fIntersections[0].x - fIntersections[2].x) > 20000)
		fHorizontal = true;
}

BSVGElement *
BSVGLinearGradient::ResolveLink()
{
	BSVGElement *link = BSVGGradient::ResolveLink();
	
	if (!link)
		return NULL;
	
	if (link->Type() != Type())
		return NULL;
	
	BSVGLinearGradient *grad = (BSVGLinearGradient *)link;
	
	if (grad->fStartX && !fStartX)
		fStart.x = grad->fStart.x;
	if (grad->fStartY && !fStartY)
		fStart.y = grad->fStart.y;
	
	if (grad->fEndX && !fEndX)
		fEnd.x = grad->fEnd.x;
	if (grad->fEndY && !fEndY)
		fEnd.y = grad->fEnd.y;
	
	return link;
}

void
BSVGLinearGradient::ApplyPaint(BView *view)
{
	BSVGGradient::ApplyPaint(view);
}

void
BSVGLinearGradient::ApplyPaint(BBitmap *bitmap)
{
	ResolveLink();
	Recalculate();
	
	int count = fElements.CountItems();
	BObjectList<BSVGStop> stops(count, false);
	for (int i = 0; i < count; i++) {
		if (fElements.ItemAt(i)->Type() == B_SVG_STOP)
			stops.AddItem((BSVGStop *)fElements.ItemAt(i));
	}
	
	count = stops.CountItems();
	if (count <= 0)
		return;
	
	float bitmapwidth = fBounds.Width();
	float bitmapheight = fBounds.Height();
	float position = 0, to_length = 0, way;
	float step_red, step_green, step_blue, step_alpha;
	float red = 0, green = 0, blue = 0, alpha = 255;
	BSVGStop *stop = stops.ItemAt(0);
	rgb_color next_color = stop->fRenderState.stop_color.Color();
	next_color.alpha = (uint8)(stop->fRenderState.stop_opacity() * 255);
	
	BPoint diff = fIntersections[1] - fIntersections[0];
	int bufferwidth = 0;
	int progress = 1;
	
	if (fHorizontal) {
		bufferwidth = (int)(fabs(fRenderEnd.y - fRenderStart.y) + 0.5);
		progress = (fRenderEnd.y - fRenderStart.y < 0 ? -1 : 1);
		diff.x += 1;
	} else {
		bufferwidth = (int)(fabs(fIntersections[0].x - fIntersections[2].x) + 0.5);
		progress = (fRenderEnd.x - fRenderStart.x < 0 ? -1 : 1);
		diff.y += 1;
	}
	
	uint32 buffer[bufferwidth];
	uint32 revbuffer[bufferwidth];
	position = stop->Offset() * bufferwidth;
	int index = (int)position;
	uint32 *whichbuffer = (progress < 0 ? revbuffer : buffer);
	
	// fill begin of buffer if stops start > 0
	if (index > 0) {
		uint32 color = (next_color.alpha << 24) + (next_color.red << 16) + (next_color.green << 8) + next_color.blue;
		fast_memset(index, color, whichbuffer);
	}
	
	// generate a buffer with all the colors of the gradient
	for (int i = 1; i < count; i++) {
		stop = stops.ItemAt(i);
		
		red = next_color.red;
		green = next_color.green;
		blue = next_color.blue;
		alpha = next_color.alpha;
		
		next_color = stop->fRenderState.stop_color.Color();
		next_color.alpha = (uint8)(stop->fRenderState.stop_opacity() * 255);
		to_length = stop->Offset() * bufferwidth;
		way = to_length - position;
		
		step_red = (next_color.red - red) / way;
		step_green = (next_color.green - green) / way;
		step_blue = (next_color.blue - blue) / way;
		step_alpha = (next_color.alpha - alpha) / way;
		
		way = 0;
		for (; position < to_length; position++) {
			whichbuffer[index++] = ((uint8)alpha << 24) + ((uint8)red << 16) + ((uint8)green << 8) + (uint8)blue;
			red += step_red;
			green += step_green;
			blue += step_blue;
			alpha += step_alpha;
		}
	}
	
	// fill rest of the buffer if stops end < 1
	if (bufferwidth - index > 0)
		fast_memset(bufferwidth - index, whichbuffer[index - 1], whichbuffer + index);
	
	// reverse buffers
	if (whichbuffer == buffer) {
		for (int i = 0; i < bufferwidth; i++)
			revbuffer[bufferwidth - i - 1] = buffer[i];
	} else {
		for (int i = 0; i < bufferwidth; i++)
			buffer[bufferwidth - i - 1] = revbuffer[i];
	}
	
	uint32 *bits = (uint32 *)bitmap->Bits();
	uint32 row = bitmap->BytesPerRow() / 4;
	uint32 rowwidth = MIN((uint32)ceilf(bitmapwidth), row);
	uint32 bitsmax = (uint32)ceilf(bitmapheight) * row;
	
	// special handling of a exactly horizontal case
	if (fHorizontal) {
		float distance = fabs(fRenderStart.y - fRenderEnd.y);
		int startrow = (int)(MIN(fRenderStart.y, fRenderEnd.y) - fBounds.top);
		uint32 *line = bits + startrow * row;
		
		// fill space above the gradient
		int fill_count = 0;
		switch (fSpreadMethod) {
			case B_SPREAD_PAD:
				fill_count = line - bits;
				if (fill_count > 0)
					fast_memset(fill_count, buffer[0], bits);
				break;
			case B_SPREAD_REFLECT:
			case B_SPREAD_REPEAT:
				whichbuffer = (fSpreadMethod == B_SPREAD_REFLECT && (startrow / (int)distance) % 2 == 0 ? revbuffer : buffer);
				count = abs(startrow) / (int)distance;
				startrow = startrow % (int)distance;
				
				// first
				for (int i = 0; i < startrow; i++)
					fast_memset(rowwidth, whichbuffer[bufferwidth - startrow + i], bits + i * row);
				if (fSpreadMethod == B_SPREAD_REFLECT)
					whichbuffer = (whichbuffer == revbuffer ? buffer : revbuffer);
				
				// middle
				for (int i = 0; i < count; i++) {
					for (int j = 0; j < distance; j++, startrow++)
						if (startrow >= 0 && startrow * row <= bitsmax - row)
							fast_memset(rowwidth, whichbuffer[j], bits + startrow * row);
					if (fSpreadMethod == B_SPREAD_REFLECT)
						whichbuffer = (whichbuffer == revbuffer ? buffer : revbuffer);
				}
				break;
		}
		
		// draw gradient itself
		for (int i = 0; i < distance; i++) {
			if (line >= bits && line <= bits + bitsmax)
				fast_memset(rowwidth, buffer[i], line);
			line += row;
		}
		
		// and fill below the gradient
		switch (fSpreadMethod) {
			case B_SPREAD_PAD:
				fill_count = bits + bitsmax - line;
				if (fill_count > 0)
					fast_memset(fill_count, buffer[bufferwidth - 1], line);
				break;
			case B_SPREAD_REFLECT:
			case B_SPREAD_REPEAT:
				startrow += (int)distance;
				count = ((int)bitmapheight - startrow) / (int)distance;
				if (fSpreadMethod == B_SPREAD_REFLECT)
					whichbuffer = (whichbuffer == revbuffer ? buffer : revbuffer);
				
				// middle
				for (int i = 0; i < count; i++) {
					for (int j = 0; j < distance; j++, startrow++)
						if (startrow >= 0 && startrow * row <= bitsmax - row)
							fast_memset(rowwidth, whichbuffer[j], bits + startrow * row);
					if (fSpreadMethod == B_SPREAD_REFLECT)
						whichbuffer = (whichbuffer == revbuffer ? buffer : revbuffer);
				}
				
				// last
				for (int i = startrow; i < bitmapheight; i++)
					if (i >= 0)
						fast_memset(rowwidth, whichbuffer[i - startrow], bits + i * row);
				
				break;
		}
		return;
	}
	
	float min = fIntersections[0].x;
	for (int i = 1; i < 4; i++)
		min = MIN(min, fIntersections[i].x);
	
	// if we start at 0.5 we don't have to round location
	float location = 0.5;
	float offset = min - fBounds.left;
	if (diff.x < 0)
		location += (int)fabs(diff.x);
	location += offset;
	float step = diff.x / diff.y;
	
	uint32 *line = bits;
	uint32 *bitmaplocation;
	uint32 *bufferlocation;
	int copycount;
	
	// for each line we just draw the same buffer just offset by step
	for (int i = 0; i <= (int)bitmapheight; i++, location += step, line += row) {
		int intloc = (int)location;
		
		// spread method specific stuff before actual gradient
		switch (fSpreadMethod) {
			case B_SPREAD_PAD:
				// out of range handling
				if (intloc < -bufferwidth) {
					fast_memset((int)bitmapwidth, buffer[bufferwidth - 1], line);
					continue;
				} else if (intloc > bitmapwidth) {
					fast_memset((int)bitmapwidth, buffer[0], line);
					continue;
				}
				
				fast_memset(MAX(intloc, 0), buffer[0], line);
				break;
			case B_SPREAD_REFLECT:
			case B_SPREAD_REPEAT:
				// out of range handling
				if (intloc < -bufferwidth || intloc > bitmapwidth) {
					int ourloc = abs(intloc) % bufferwidth;
					int count = ((int)bitmapwidth - ourloc) / bufferwidth + 1;
					whichbuffer = (fSpreadMethod == B_SPREAD_REFLECT && (intloc / bufferwidth) % 2 == 0 ? revbuffer : buffer);
					if (intloc < 0) {
						ourloc = (bufferwidth - ourloc);
						if (fSpreadMethod == B_SPREAD_REFLECT)
							whichbuffer = (whichbuffer == revbuffer ? buffer : revbuffer);
					}
					
					// first
					memcpy(line, whichbuffer + (bufferwidth - ourloc), ourloc * 4);
					if (fSpreadMethod == B_SPREAD_REFLECT)
						whichbuffer = (whichbuffer == revbuffer ? buffer : revbuffer);
					
					// middle
					for (int i = 0; i < count; i++) {
						memcpy(line + ourloc + i * bufferwidth, whichbuffer, MAX(0, (int32)(rowwidth - ourloc)) * 4);
						if (fSpreadMethod == B_SPREAD_REFLECT)
							whichbuffer = (whichbuffer == revbuffer ? buffer : revbuffer);
					}
					
					// last
					ourloc += bufferwidth * count;
					memcpy(line + ourloc, whichbuffer, MAX(0, (int32)(rowwidth - ourloc)) * 4);
					continue;
				}
				
				if (location > 0) {
					uint32 *whichbuffer = (fSpreadMethod == B_SPREAD_REFLECT ? revbuffer : buffer);
					int count = intloc / bufferwidth;
					for (int i = 1; i <= count; i++) {
						memcpy(line + intloc - bufferwidth * i, whichbuffer, bufferwidth * 4);
						if (fSpreadMethod == B_SPREAD_REFLECT)
							whichbuffer = (whichbuffer == revbuffer ? buffer : revbuffer);
					}
					float modwidth = intloc % bufferwidth;
					memcpy(line, whichbuffer + (bufferwidth - (int)modwidth), (int)modwidth * 4);
				}
				break;
		}
		
		if (location <= 0) {
			bitmaplocation = line;
			bufferlocation = buffer + intloc * -1;
			copycount = MIN(bufferwidth + intloc, (int)bitmapwidth);
		} else {
			bitmaplocation = line + intloc;
			bufferlocation = buffer;
			copycount = MIN(bufferwidth, (int)bitmapwidth - intloc);
		}
		
		// do the actual gradient
		memcpy(bitmaplocation, bufferlocation, copycount * 4);
		
		// spread method specific stuff after actual gradient
		switch (fSpreadMethod) {
			case B_SPREAD_PAD:
				fast_memset(MAX(0, line + rowwidth - (bitmaplocation + copycount)), buffer[bufferwidth - 1], bitmaplocation + copycount);
				break;
			case B_SPREAD_REFLECT:
			case B_SPREAD_REPEAT: 
				whichbuffer = (fSpreadMethod == B_SPREAD_REFLECT ? revbuffer : buffer);
				int ourloc = bitmaplocation - line + copycount;
				int count = ((int)bitmapwidth - ourloc) / bufferwidth;
				for (int i = 0; i < count; i++) {
					memcpy(line + ourloc + i * bufferwidth, whichbuffer, bufferwidth * 4);
					if (fSpreadMethod == B_SPREAD_REFLECT)
						whichbuffer = (whichbuffer == revbuffer ? buffer : revbuffer);
				}
				ourloc += bufferwidth * count;
				memcpy(line + ourloc, whichbuffer, MAX(0, (rowwidth - ourloc)) * 4);
				break;
		}
	}
}


/************* specific functions for BSVGRadialGradient
*/

BSVGRadialGradient::BSVGRadialGradient(BSVGView *parent)
	:	BSVGGradient(parent),
		fFocal(0.5, 0.5),
		fFocalX(false),
		fFocalY(false)
{
	fStart = BPoint(0.5, 0.5);
	fEnd = BPoint(1.0, 0.5);
}

BSVGRadialGradient::BSVGRadialGradient(BSVGView *parent, BRect frame, BPoint start, BPoint end)
	:	BSVGGradient(parent, frame, start, end)
{
}

BSVGRadialGradient::BSVGRadialGradient(BMessage *data)
	:	BSVGGradient(data)
{
	data->FindPoint("_focal", &fFocal);
}
	
BArchivable *
BSVGRadialGradient::Instantiate(BMessage *data)
{
	if (validate_instantiation(data, "BSVGRadialGradient"))
		return new BSVGRadialGradient(data);
	return NULL;
}

status_t
BSVGRadialGradient::Archive(BMessage *data, bool deep) const
{
	BSVGGradient::Archive(data, deep);
	data->AddPoint("_focal", fFocal);
	
	ADDREPCLASS("BSVGRadialGradient");
	ADDREPTYPE(B_SVG_RADIALGRAD);
	
	return B_OK;
}

void
BSVGRadialGradient::HandleAttribute(attribute_s *attr)
{
	if (!attr)
		return;
	
	float val = 0;
	UnitHandler::Handle(attr, &val, &fState, 1);
	
	if (attr->id.Compare("fx") == 0)
		SetFocal(BPoint(val, fFocal.y));
	else if (attr->id.Compare("fy") == 0)
		SetFocal(BPoint(fFocal.x, val));
	else if (attr->id.Compare("cx") == 0)
		SetStart(BPoint(val, fStart.y));
	else if (attr->id.Compare("cy") == 0)
		SetStart(BPoint(fStart.x, val));
	else if (attr->id.Compare("r") == 0)
		SetEnd(BPoint(fStart.x + val, fStart.y));
	else
		BSVGGradient::HandleAttribute(attr);
}

void
BSVGRadialGradient::RecreateData()
{
	BSVGPaintServer::RecreateData();
	fHeaderData.ReplaceFirst("<paintserver", "<radialGradient");
	fHeaderData.ReplaceLast("transform", "gradientTransform");
	fHeaderData.ReplaceLast("serverUnits", "gradientUnits");
	
	fHeaderData << " cx=\"" << fStart.x << "\" cy=\"" << fStart.y;
	fHeaderData << "\" fx=\"" << fFocal.x << "\" fy=\"" << fFocal.y;
	fHeaderData << "\" r=\"" << fEnd.x - fStart.x << "\">";
	
	for (int i = 0; i < fElements.CountItems(); i++)
		fElements.ItemAt(i)->RecreateData();
	
	fFooterData.SetTo("</radialGradient>");
}

void
BSVGRadialGradient::SetFocal(BPoint focal)
{
	fFocalX = true;
	fFocalY = true;
	fFocal = focal;
}

BPoint
BSVGRadialGradient::Focal() const
{
	return fFocal;
}

void
BSVGRadialGradient::Recalculate()
{
	fRenderMatrix = CoordinateSystem() * fRenderState.general_transform;
	
	fRenderStart = fRenderMatrix * fStart;
	fRenderEnd = fRenderMatrix * fEnd;
	fRenderFocal = fRenderMatrix * fFocal;
	
	fRenderEnd += fOffset;
	fRenderStart += fOffset;
	fRenderFocal += fOffset;
}

BSVGElement *
BSVGRadialGradient::ResolveLink()
{
	BSVGElement *link = BSVGGradient::ResolveLink();
	
	if (!link)
		return NULL;
	
	if (link->Type() != Type())
		return NULL;
	
	BSVGRadialGradient *grad = (BSVGRadialGradient *)link;
	
	if (grad->fStartX && !fStartX)
		fStart.x = grad->fStart.x;
	if (grad->fStartY && !fStartY)
		fStart.y = grad->fStart.y;
	
	if (grad->fEndX && !fEndX)
		fEnd.x = grad->fEnd.x;
	if (grad->fEndY && !fEndY)
		fEnd.y = grad->fEnd.y;
	
	if (grad->fFocalX && !fFocalX)
		fFocal.x = grad->fFocal.x;
	if (grad->fFocalY && !fFocalY)
		fFocal.y = grad->fFocal.y;
	
	return link;
}

void
BSVGRadialGradient::ApplyPaint(BView *view)
{
	BSVGGradient::ApplyPaint(view);
}

void
BSVGRadialGradient::ApplyPaint(BBitmap *bitmap)
{
	ResolveLink();
	Recalculate();
	
	int count = fElements.CountItems();
	BObjectList<BSVGStop> stops(count, false);
	for (int i = 0; i < count; i++) {
		if (fElements.ItemAt(i)->Type() == B_SVG_STOP)
			stops.AddItem((BSVGStop *)fElements.ItemAt(i));
	}
	
	count = stops.CountItems();
	if (count <= 0)
		return;
	
	BPoint radii(fabs(fRenderEnd.x - fRenderStart.x), fabs(fRenderEnd.x - fRenderStart.x));
	
	float position = 0, to_length = 0, way;
	float step_red, step_green, step_blue, step_alpha;
	float red = 0, green = 0, blue = 0, alpha = 255;
	BSVGStop *stop = stops.ItemAt(0);
	rgb_color next_color = stop->fRenderState.stop_color.Color();
	next_color.alpha = (uint8)(stop->fRenderState.stop_opacity() * 255);
	
	uint32 bufferwidth = (int)MAX(radii.x, radii.y);
	uint32 buffer[bufferwidth];
	position = stop->Offset() * bufferwidth;
	int index = (int)position;
	
	// fill begin of buffer if stops start > 0
	if (index > 0) {
		uint32 color = (next_color.alpha << 24) + (next_color.red << 16) + (next_color.green << 8) + next_color.blue;
		fast_memset(index, color, buffer);
	}
	
	// generate a buffer with all the colors of the gradient
	for (int i = 1; i < count; i++) {
		stop = stops.ItemAt(i);
		
		red = next_color.red;
		green = next_color.green;
		blue = next_color.blue;
		alpha = next_color.alpha;
		
		next_color = stop->fRenderState.stop_color.Color();
		next_color.alpha = (uint8)(stop->fRenderState.stop_opacity() * 255);
		to_length = stop->Offset() * bufferwidth;
		way = to_length - position;
		
		step_red = (next_color.red - red) / way;
		step_green = (next_color.green - green) / way;
		step_blue = (next_color.blue - blue) / way;
		step_alpha = (next_color.alpha - alpha) / way;
		
		for (; position < to_length; position++) {
			buffer[index++] = ((uint8)alpha << 24) + ((uint8)red << 16) + ((uint8)green << 8) + (uint8)blue;
			red += step_red;
			green += step_green;
			blue += step_blue;
			alpha += step_alpha;
		}
	}
	
	// fill rest of the buffer if stops end < 1
	if (bufferwidth - index > 0)
		fast_memset(bufferwidth - index, buffer[index - 1], buffer + index);
	
	// fill target bitmap
	uint32 *bits = (uint32 *)bitmap->Bits();
	uint32 row = bitmap->BytesPerRow() / 4;
	uint32 bitslength = bitmap->BitsLength() / 4;
	fast_memset(bitslength, buffer[bufferwidth - 1], bits);
	
	// loop vars, pointers into the bitmap
	BPoint center = fRenderFocal - fBounds.LeftTop();
	uint32 offset1, offset2;
	uint32 *row1, *row2;
	uint32 *where1, *where2, *where3, *where4, *where5, *where6, *where7, *where8;
	uint32 *max = bits + bitslength;
	uint32 *min = bits;
	uint32 color = 0;
	uint32 radius;
	uint32 counter;
	
	// do actual gradient
	for (int i = 0; i < radii.y; i++) {
		offset1 = (int)center.x - i;
		offset2 = (int)center.x + i;
		
		row1 = bits + ((int)center.y - i) * row;
		where1 = where2 = row1 + offset1;
		where3 = where4 = row1 + offset2;
		
		row2 = bits + ((int)center.y + i) * row;
		where5 = where6 = row2 + offset1;
		where7 = where8 = row2 + offset2;
		
		counter = 0;
		while (true) {
			radius = (int)sqrt((i + counter) * (i + counter++) + i * i);
			if (radius >= bufferwidth)
				break;
			
			color = buffer[radius];
			
			if (where1 <= max && where1 >= min && where1 < row1 + row && where1 >= row1)
				*where1 = color;
			where1--;
			
			if (where2 <= max && where2 >= min && offset1 >= 0 && offset1 < row)
				*where2 = color;
			where2 -= row;
			
			if (where3 <= max && where3 >= min && where3 < row1 + row && where3 >= row1)
				*where3 = color;
			where3++;
			
			if (where4 <= max && where4 >= min && offset2 >= 0 && offset2 < row)
				*where4 = color;
			where4 -= row;
			
			if (where5 <= max && where5 >= min && where5 < row2 + row && where5 >= row2)
				*where5 = color;
			where5--;
			
			if (where6 <= max && where6 >= min && offset1 >= 0 && offset1 < row)
				*where6 = color;
			where6 += row;
			
			if (where7 <= max && where7 >= min && where7 < row2 + row && where7 >= row2)
				*where7 = color;
			where7++;
			
			if (where8 <= max && where8 >= min && offset2 >= 0 && offset2 < row)
				*where8 = color;
			where8 += row;
		}
	}
}
