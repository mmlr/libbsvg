#include <Bitmap.h>
#include <View.h>
#include "SVGPaintServer.h"
#include "SVGElement.h"

BSVGPaintServer::BSVGPaintServer()
	:	BSVGElement(),
		fBounds(0, 0, 0, 0),
		fOffset(0, 0),
		fCoordinateSystem(),
		fUnits(B_OBJECT_BOUNDING_BOX)
{
}

BSVGPaintServer::BSVGPaintServer(BSVGView *parent)
	:	BSVGElement(parent),
		fBounds(0, 0, 0, 0),
		fOffset(0, 0),
		fCoordinateSystem(),
		fUnits(B_OBJECT_BOUNDING_BOX)
{
}

BSVGPaintServer::BSVGPaintServer(BSVGView *parent, const BString &data)
	:	BSVGElement(parent, data),
		fBounds(0, 0, 0, 0),
		fOffset(0, 0),
		fCoordinateSystem(),
		fUnits(B_OBJECT_BOUNDING_BOX)
{
}

BSVGPaintServer::~BSVGPaintServer()
{
}

BSVGPaintServer::BSVGPaintServer(BMessage *data)
	:	BSVGElement(data),
		fBounds(0, 0, 0, 0),
		fOffset(0, 0),
		fCoordinateSystem(),
		fUnits(B_OBJECT_BOUNDING_BOX)
{
	data->FindRect("_pbounds", &fBounds);
	data->FindPoint("_poffset", &fOffset);
	data->FindInt32("_units", (int32 *)&fUnits);
	
	for (int i = 0; i < 3; i++)
		for (int j = 0; j < 3; j++)
			data->FindFloat("_coordinatesystem", i * 3 + j, &fCoordinateSystem.matrix[i][j]);
}

BArchivable *
BSVGPaintServer::Instantiate(BMessage *data)
{
	if (validate_instantiation(data, "BSVGPaintServer"))
		return new BSVGPaintServer(data);
	return NULL;
}

status_t
BSVGPaintServer::Archive(BMessage *data, bool deep) const
{
	BSVGElement::Archive(data, deep);
	
	data->AddRect("_pbounds", fBounds);
	data->AddPoint("_poffset", fOffset);
	data->AddInt32("_units", (int32)fUnits);
	
	for (int i = 0; i < 3; i++)
		for (int j = 0; j < 3; j++)
			data->AddFloat("_coordinatesystem", fCoordinateSystem.matrix[i][j]);
	
	ADDREPCLASS("BSVGPaintServer");
	ADDREPTYPE(B_SVG_PAINTSERVER);
	return B_OK;
}	

void
BSVGPaintServer::RecreateData()
{
	fHeaderData.SetTo("<paintserver");
	AddID(fHeaderData);
	fState.AddToString(&fHeaderData);
	
	if (fUnits != B_OBJECT_BOUNDING_BOX)
		fHeaderData << " serverUnits=\"userSpaceOnUse\"";
	
	for (int i = 0; i < fElements.CountItems(); i++)
		fElements.ItemAt(i)->RecreateData();
	
	if (fElements.CountItems() > 0)
		fFooterData.SetTo("</paintserver>");
	else
		fFooterData.SetTo("");
}

void
BSVGPaintServer::SetBounds(BRect bounds)
{
	fBounds = bounds;
}

BRect
BSVGPaintServer::Bounds()
{
	return fBounds;
}

void
BSVGPaintServer::SetOffset(BPoint offset)
{
	fOffset = offset;
}

BPoint
BSVGPaintServer::Offset()
{
	return fOffset;
}

void
BSVGPaintServer::SetCoordinateSystem(const Matrix2D &system)
{
	fCoordinateSystem = system;
}

Matrix2D
BSVGPaintServer::CoordinateSystem() const
{
	return fCoordinateSystem;
}

void
BSVGPaintServer::SetServerUnits(units_t units)
{
	fUnits = units;
}

units_t
BSVGPaintServer::ServerUnits() const
{
	return fUnits;
}

bool
BSVGPaintServer::IsFullyOpaque() const
{
	return false;
}

void
BSVGPaintServer::ApplyPaint(BView *view)
{
	// create a bitmap where the gradient is rendered into
	BBitmap bitmap(BRect(0, 0, fBounds.Width(), fBounds.Height()), B_RGBA32);
	ApplyPaint(&bitmap);
	
	if (view->LockLooper()) {
		view->SetDrawingMode(B_OP_COPY);
		view->DrawBitmap(&bitmap, fBounds.LeftTop());
		view->UnlockLooper();
	}
}

void
BSVGPaintServer::ApplyPaint(BBitmap *bitmap)
{
	memset(bitmap->Bits(), 128, bitmap->BitsLength());
}
