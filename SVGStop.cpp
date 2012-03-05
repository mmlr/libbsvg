#include "SVGStop.h"
#include "UnitHandler.h"

BSVGStop::BSVGStop()
{
}

BSVGStop::BSVGStop(BSVGStop *stop)
{
	SetOffset(stop->Offset());
	SetStopColor(stop->StopColor());
	SetStopOpacity(stop->StopOpacity());
}

BSVGStop::BSVGStop(rgb_color color, float offset, float opacity)
{
	SetStopColor(color);
	SetOffset(offset);
	SetStopOpacity(opacity);
}

BSVGStop::BSVGStop(uchar r, uchar g, uchar b, float offset, float opacity)
{
	SetStopColor(r, g, b);
	SetOffset(offset);
	SetStopOpacity(opacity);
}

BSVGStop::~BSVGStop()
{
}

BSVGStop::BSVGStop(BMessage *data)
	:	BSVGElement(data)
{
	data->FindFloat("_offset", &fOffset);
}
	
BArchivable *
BSVGStop::Instantiate(BMessage *data)
{
	if (validate_instantiation(data, "BSVGStop"))
		return new BSVGStop(data);
	return NULL;
}

status_t
BSVGStop::Archive(BMessage *data, bool deep) const
{
	BSVGElement::Archive(data, deep);
	
	data->AddFloat("_offset", fOffset);
	
	ADDREPCLASS("BSVGStop");
	ADDREPTYPE(B_SVG_STOP);
	
	return B_OK;
}

void
BSVGStop::HandleAttribute(attribute_s *attr)
{
	if (!attr)
		return;
	
	if (attr->id.Compare("offset") == 0) {
		float val;
		UnitHandler::Handle(attr, &val, &fState, 1);
		SetOffset(val);
	} else {
		BSVGElement::HandleAttribute(attr);
	}
}

void
BSVGStop::RecreateData()
{
	fHeaderData.SetTo("<stop");
	AddID(fHeaderData);
	fState.AddToString(&fHeaderData);
	
	fHeaderData << " offset=\"" << fOffset << "\" />";
	fFooterData.SetTo("");
}

void
BSVGStop::SetOffset(float offset)
{
	fOffset = offset;
}

float
BSVGStop::Offset() const
{
	return fOffset;
}

bool
BSVGStop::IsOpaque() const
{
	return (fRenderState.stop_opacity.value >= 1.0);
}
