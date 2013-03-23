#include "SVGShapes.h"
#include "SVGDataCreator.h"

#include "UnitHandler.h"

BSVGRect::BSVGRect(BSVGView *parent)
	:	BSVGPath(parent),
		fX(0),
		fY(0),
		fWidth(0),
		fHeight(0),
		fRX(0),
		fRY(0),
		fRXSet(false),
		fRYSet(false)
{
}

BSVGRect::BSVGRect(BMessage *data)
	:	BSVGPath(data)
{
	data->FindFloat("_x", &fX);
	data->FindFloat("_y", &fY);
	data->FindFloat("_width", &fWidth);
	data->FindFloat("_height", &fHeight);
	data->FindFloat("_rx", &fRX);
	data->FindFloat("_ry", &fRY);
	data->FindBool("_rxset", &fRXSet);
	data->FindBool("_ryset", &fRYSet);
	
	FINALIZEIF(B_SVG_RECT);
}
	
BArchivable *
BSVGRect::Instantiate(BMessage *data)
{
	if (validate_instantiation(data, "BSVGRect"))
		return new BSVGRect(data);
	return NULL;
}

status_t
BSVGRect::Archive(BMessage *data, bool deep) const
{
	BSVGPath::Archive(data, deep);
	
	data->AddFloat("_x", fX);
	data->AddFloat("_y", fY);
	data->AddFloat("_width", fWidth);
	data->AddFloat("_height", fHeight);
	data->AddFloat("_rx", fRX);
	data->AddFloat("_ry", fRY);
	data->AddBool("_rxset", fRXSet);
	data->AddBool("_ryset", fRYSet);
		
	ADDREPCLASS("BSVGRect");
	ADDREPTYPE(B_SVG_RECT);
	
	return B_OK;
}

void
BSVGRect::HandleAttribute(attribute_s *attr)
{
	if (!attr)
		return;
	
	float val = 0;
	UnitHandler::Handle(attr, &val, &fState, fState.general_viewport.Width());
	
	if (attr->id.Compare("x") == 0)
		SetX(val);
	else if (attr->id.Compare("y") == 0)
		SetY(val);
	else if (attr->id.Compare("width") == 0)
		SetWidth(val);
	else if (attr->id.Compare("height") == 0)
		SetHeight(val);
	else if (attr->id.Compare("rx") == 0)
		SetRX(val);
	else if (attr->id.Compare("ry") == 0)
		SetRY(val);
	else
		BSVGPath::HandleAttribute(attr);
}

void
BSVGRect::FinalizeShape()
{
	if (fRXSet && fRX > fWidth / 2)
		fRX = fWidth / 2;
	if (fRYSet && fRY > fHeight / 2)
		fRY = fHeight / 2;
	
	if (fRXSet && !fRYSet)
		fRY = fRX;
	else if (!fRXSet && fRYSet)
		fRX = fRY;
	
	BRect rect(fX, fY, fX + fWidth, fY + fHeight);
	if (fRXSet || fRYSet) {
		BPoint radii(fRX, fRY);
		BRect centers = rect;
		centers.InsetBy(fRX, fRY);
		
		fShape.MoveTo(BPoint(rect.left + fRX, rect.top));
		
		fShape.LineTo(BPoint(rect.right - fRX, rect.top));
		RoundArcTo(centers.RightTop(), radii, 0, -90, 90);
		
		fShape.LineTo(BPoint(rect.right, rect.bottom - fRY));
		RoundArcTo(centers.RightBottom(), radii, 0, 0, 90);
		
		fShape.LineTo(BPoint(rect.left + fRX, rect.bottom));
		RoundArcTo(centers.LeftBottom(), radii, 0, 90, 90);
		
		fShape.LineTo(BPoint(rect.left, rect.top + fRY));
		RoundArcTo(centers.LeftTop(), radii, 0, 180, 90);
	} else {
		fShape.MoveTo(rect.LeftTop());
		fShape.LineTo(rect.RightTop());
		fShape.LineTo(rect.RightBottom());
		fShape.LineTo(rect.LeftBottom());
		fShape.LineTo(rect.LeftTop());
	}
	fShape.Close();
}

void
BSVGRect::RecreateData()
{
	fHeaderData.SetTo("<rect");
	AddID(fHeaderData);
	fState.AddToString(&fHeaderData);
	
	fHeaderData << " x=\"" << fX << "\"";
	fHeaderData << " y=\"" << fY << "\"";
	fHeaderData << " width=\"" << fWidth << "\"";
	fHeaderData << " height=\"" << fHeight << "\"";
	
	if (fRXSet)
		fHeaderData << " rx=\"" << fRX << "\"";
	if (fRYSet)
		fHeaderData << " ry=\"" << fRY << "\"";
	
	fHeaderData << " />";
	fFooterData.SetTo("");
}	

void
BSVGRect::SetX(float x)
{
	fX = x;
}

void
BSVGRect::SetY(float y)
{
	fY = y;
}

void
BSVGRect::SetWidth(float width)
{
	fWidth = width;
}

void
BSVGRect::SetHeight(float height)
{
	fHeight = height;
}

void
BSVGRect::SetRX(float rx)
{
	fRX = rx;
	fRXSet = true;
}

void
BSVGRect::SetRY(float ry)
{
	fRY = ry;
	fRYSet = true;
}

// **************************************************************************

BSVGEllipse::BSVGEllipse(BSVGView *parent)
	:	BSVGPath(parent),
		fCenter(0, 0),
		fRX(0),
		fRY(0),
		fRXSet(false),
		fRYSet(false)
{
}

BSVGEllipse::BSVGEllipse(BMessage *data)
	:	BSVGPath(data)
{
	data->FindPoint("_center", &fCenter);
	data->FindFloat("_rx", &fRX);
	data->FindFloat("_ry", &fRY);
	data->FindBool("_rxset", &fRXSet);
	data->FindBool("_ryset", &fRYSet);
	
	FINALIZEIF(B_SVG_ELLIPSE);
}
	
BArchivable *
BSVGEllipse::Instantiate(BMessage *data)
{
	if (validate_instantiation(data, "BSVGEllipse"))
		return new BSVGEllipse(data);
	return NULL;
}

status_t
BSVGEllipse::Archive(BMessage *data, bool deep) const
{
	BSVGPath::Archive(data, deep);
	
	data->AddPoint("_center", fCenter);
	data->AddFloat("_rx", fRX);
	data->AddFloat("_ry", fRY);
	data->AddBool("_rxset", fRXSet);
	data->AddBool("_ryset", fRYSet);
		
	ADDREPCLASS("BSVGEllipse");
	ADDREPTYPE(B_SVG_ELLIPSE);
	
	return B_OK;
}

void
BSVGEllipse::HandleAttribute(attribute_s *attr)
{
	if (!attr)
		return;
	
	float val = 0;
	UnitHandler::Handle(attr, &val, &fState, fState.general_viewport.Width());
	
	if (attr->id.Compare("cx") == 0)
		SetCX(val);
	else if (attr->id.Compare("cy") == 0)
		SetCY(val);
	else if (attr->id.Compare("rx") == 0)
		SetRX(val);
	else if (attr->id.Compare("ry") == 0)
		SetRY(val);
	else
		BSVGPath::HandleAttribute(attr);
}

void
BSVGEllipse::FinalizeShape()
{
	if (fRXSet && !fRYSet)
		fRY = fRX;
	else if (!fRXSet && fRYSet)
		fRX = fRY;
	
	
	fShape.MoveTo(BPoint(fCenter.x + fRX, fCenter.y));
	RoundArcTo(fCenter, BPoint(fRX, fRY), 0, 0, 360);
	fShape.Close();
}

void
BSVGEllipse::RecreateData()
{
	fHeaderData.SetTo("<ellipse");
	AddID(fHeaderData);
	fState.AddToString(&fHeaderData);
	
	fHeaderData << " cx=\"" << fCenter.x << "\"";
	fHeaderData << " cy=\"" << fCenter.y << "\"";
	if (fRXSet)
		fHeaderData << " rx=\"" << fRX << "\"";
	if (fRYSet)
		fHeaderData << " ry=\"" << fRY << "\"";
	
	fHeaderData << " />";
	fFooterData.SetTo("");
}

void
BSVGEllipse::SetCX(float cx)
{
	fCenter.x = cx;
}

void
BSVGEllipse::SetCY(float cy)
{
	fCenter.y = cy;
}

void
BSVGEllipse::SetRX(float rx)
{
	fRX = rx;
	fRXSet = true;
}

void
BSVGEllipse::SetRY(float ry)
{
	fRY = ry;
	fRYSet = true;
}

// **************************************************************************


BSVGCircle::BSVGCircle(BSVGView *parent)
	:	BSVGEllipse(parent)
{
}

BSVGCircle::BSVGCircle(BMessage *data)
	:	BSVGEllipse(data)
{
	FINALIZEIF(B_SVG_CIRCLE);
}
	
BArchivable *
BSVGCircle::Instantiate(BMessage *data)
{
	if (validate_instantiation(data, "BSVGCircle"))
		return new BSVGCircle(data);
	return NULL;
}

status_t
BSVGCircle::Archive(BMessage *data, bool deep) const
{
	BSVGEllipse::Archive(data, deep);
	
	ADDREPCLASS("BSVGCircle");
	ADDREPTYPE(B_SVG_CIRCLE);
	
	return B_OK;
}

void
BSVGCircle::HandleAttribute(attribute_s *attr)
{
	if (!attr)
		return;
	
	float val = 0;
	UnitHandler::Handle(attr, &val, &fState, fState.general_viewport.Width());
	
	if (attr->id.Compare("r") == 0)
		SetR(val);
	else
		BSVGEllipse::HandleAttribute(attr);
}

void
BSVGCircle::RecreateData()
{
	fHeaderData.SetTo("<circle");
	AddID(fHeaderData);
	fState.AddToString(&fHeaderData);
	
	fHeaderData << " cx=\"" << fCenter.x << "\"";
	fHeaderData << " cy=\"" << fCenter.y << "\"";
	fHeaderData << " r=\"" << fRX << "\"";
	
	fHeaderData << " />";
	fFooterData.SetTo("");
}

void
BSVGCircle::SetR(float r)
{
	fRX = r;
	fRY = r;
	fRXSet = true;
	fRYSet = true;
}

// **************************************************************************

BSVGLine::BSVGLine(BSVGView *parent)
	:	BSVGPath(parent),
		fPoint1(0, 0),
		fPoint2(0, 0)
{
}

BSVGLine::BSVGLine(BMessage *data)
	:	BSVGPath(data)
{
	data->FindPoint("_point1", &fPoint1);
	data->FindPoint("_point2", &fPoint2);
	
	FINALIZEIF(B_SVG_LINE);
}
	
BArchivable *
BSVGLine::Instantiate(BMessage *data)
{
	if (validate_instantiation(data, "BSVGLine"))
		return new BSVGLine(data);
	return NULL;
}

status_t
BSVGLine::Archive(BMessage *data, bool deep) const
{
	BSVGPath::Archive(data, deep);
	
	data->AddPoint("_point1", fPoint1);
	data->AddPoint("_point2", fPoint2);
	
	ADDREPCLASS("BSVGLine");
	ADDREPTYPE(B_SVG_LINE);
	
	return B_OK;
}

void
BSVGLine::HandleAttribute(attribute_s *attr)
{
	if (!attr)
		return;
	
	float val = 0;
	UnitHandler::Handle(attr, &val, &fState, fState.general_viewport.Width());
	
	if (attr->id.Compare("x1") == 0)
		SetX1(val);
	else if (attr->id.Compare("y1") == 0)
		SetY1(val);
	else if (attr->id.Compare("x2") == 0)
		SetX2(val);
	else if (attr->id.Compare("y2") == 0)
		SetY2(val);
	else
		BSVGPath::HandleAttribute(attr);
}

void
BSVGLine::FinalizeShape()
{
	fShape.MoveTo(fPoint1);
	fShape.LineTo(fPoint2);
}

void
BSVGLine::RecreateData()
{
	fHeaderData.SetTo("<line");
	AddID(fHeaderData);
	fState.AddToString(&fHeaderData);
	
	fHeaderData << " x1=\"" << fPoint1.x << "\"";
	fHeaderData << " y1=\"" << fPoint1.y << "\"";
	fHeaderData << " x2=\"" << fPoint2.x << "\"";
	fHeaderData << " y2=\"" << fPoint2.y << "\"";
	
	fHeaderData << " />";
	fFooterData.SetTo("");
}

void
BSVGLine::SetX1(float x1)
{
	fPoint1.x = x1;
}

void
BSVGLine::SetY1(float y1)
{
	fPoint1.y = y1;
}

void
BSVGLine::SetX2(float x2)
{
	fPoint2.x = x2;
}

void
BSVGLine::SetY2(float y2)
{
	fPoint2.y = y2;
}

// **************************************************************************

BSVGPolyline::BSVGPolyline(BSVGView *parent)
	:	BSVGPath(parent)
{
}

BSVGPolyline::BSVGPolyline(BMessage *data)
	:	BSVGPath(data)
{
	FINALIZEIF(B_SVG_POLYLINE);
}

BArchivable *
BSVGPolyline::Instantiate(BMessage *data)
{
	if (validate_instantiation(data, "BSVGPolyline"))
		return new BSVGPolyline(data);
	return NULL;
}

status_t
BSVGPolyline::Archive(BMessage *data, bool deep) const
{
	BSVGPath::Archive(data, deep);
	
	ADDREPCLASS("BSVGPolyline");
	ADDREPTYPE(B_SVG_POLYLINE);
	
	return B_OK;
}

void
BSVGPolyline::HandleAttribute(attribute_s *attr)
{
	if (!attr)
		return;
	
	if (attr->id.Compare("points") == 0)
		SetHeaderData(attr->value);
	else
		BSVGPath::HandleAttribute(attr);
}

void
BSVGPolyline::FinalizeShape()
{
	if (fHeaderData.Length() <= 0)
		return;
	
	BString buffer = fHeaderData.String();
	char *data = buffer.LockBuffer(buffer.Length());
	
	BPoint pt;
	
	if (sscanf(data, "%f%*c%f", &pt.x, &pt.y) != 2)
		return;
	
	fShape.MoveTo(pt);
	
	char *token = strtok(data, " ,");
	while (token != NULL) {
		if (sscanf(token, "%f", &pt.x) != 1)
			break;
		
		token = strtok(NULL, " ,");
		if (sscanf(token, "%f", &pt.y) != 1)
			break;
		
		token = strtok(NULL, " ,");
		fShape.LineTo(pt);
	}
	
	buffer.UnlockBuffer(0);
}

void
BSVGPolyline::RecreateData()
{
	fHeaderData.SetTo("<polyline");
	AddID(fHeaderData);
	fState.AddToString(&fHeaderData);
	
	fHeaderData << " points=\"";
	BSVGPolyDataCreator creator(fHeaderData);
	creator.Iterate(&fShape);
	
	fHeaderData << "\" />";
	fFooterData.SetTo("");
}

// **************************************************************************

BSVGPolygon::BSVGPolygon(BSVGView *parent)
	:	BSVGPolyline(parent)
{
}

BSVGPolygon::BSVGPolygon(BMessage *data)
	:	BSVGPolyline(data)
{
	FINALIZEIF(B_SVG_POLYGON);
}
	
BArchivable *
BSVGPolygon::Instantiate(BMessage *data)
{
	if (validate_instantiation(data, "BSVGPolygon"))
		return new BSVGPolygon(data);
	return NULL;
}

status_t
BSVGPolygon::Archive(BMessage *data, bool deep) const
{
	BSVGPolyline::Archive(data, deep);
	
	ADDREPCLASS("BSVGPolygon");
	ADDREPTYPE(B_SVG_POLYGON);
	
	return B_OK;
}

void
BSVGPolygon::HandleAttribute(attribute_s *attr)
{
	if (!attr)
		return;
	
	BSVGPolyline::HandleAttribute(attr);
}

void
BSVGPolygon::FinalizeShape()
{
	BSVGPolyline::FinalizeShape();
	Close();
}

void
BSVGPolygon::RecreateData()
{
	fHeaderData.SetTo("<polygon");
	AddID(fHeaderData);
	fState.AddToString(&fHeaderData);
	
	fHeaderData << " points=\"";
	BSVGPolyDataCreator creator(fHeaderData);
	creator.Iterate(&fShape);
	
	fHeaderData << "\" />";
	fFooterData.SetTo("");
}
