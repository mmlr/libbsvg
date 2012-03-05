#include "EvenOddIterator.h"
#include <Point.h>
#include <View.h>
#include <Bitmap.h>
#include <Polygon.h>
#include <StopWatch.h>
#include <stdio.h>
#include <string.h>

bigtime_t creationtime = 0;

EvenOddIterator::EvenOddIterator(BRect frame)
	:	fBitmap(NULL),
		fView(NULL),
		fOutput(NULL),
		fLocation(0, 0),
		fStart(0, 0),
		fOffset(0, 0)
{
	frame.right++;
	frame.bottom++;
	fBitmap = new BBitmap(frame, B_GRAY1, true);
	fView = new BView(frame, "bitmapdrawer", 0, 0);
	fBitmap->AddChild(fView);
}

EvenOddIterator::~EvenOddIterator()
{
	if (fBitmap)
		fBitmap->Lock();
	
	if (fView)
		fView->RemoveSelf();
	
	delete fBitmap;
	delete fView;
	
	fBitmap = NULL;
	fView = NULL;
}

void
EvenOddIterator::SetOutput(BView *output)
{
	fOutput = output;
}

void
EvenOddIterator::SetOffset(float x, float y)
{
	fOffset.Set(x, y);
}

BRect
EvenOddIterator::Bounds()
{
	return (fBitmap ? fBitmap->Bounds() : BRect(0, 0, 0, 0));
}

status_t
EvenOddIterator::IterateMoveTo(BPoint *point)
{
	fLocation = *point;
	fStart = fLocation;
	return B_OK;
}

status_t
EvenOddIterator::IterateLineTo(int32 count, BPoint *pts)
{
	for (int i = 0; i < count; i++) {
		BShape shape;
		shape.MoveTo(fLocation);
		shape.LineTo(BPoint(0, fLocation.y));	// TODO: find out why 0 works
		shape.LineTo(BPoint(0, pts[i].y));		// actually, does it?
		shape.LineTo(pts[i]);
		
		fView->FillShape(&shape);
		fLocation = pts[i];
	}
	
	return B_OK;
}

status_t
EvenOddIterator::IterateBezierTo(int32 count, BPoint *pts)
{
	for (int i = 0; i < count; i++) {
		BShape shape;
		shape.MoveTo(fLocation);
		shape.BezierTo(&pts[i * 3]);
		
		fView->FillShape(&shape);
		IterateLineTo(1, &pts[i * 3 + 2]);
	}
	return B_OK;
}

status_t
EvenOddIterator::IterateClose()
{
	IterateLineTo(1, &fStart);
	return B_OK;
}

status_t
EvenOddIterator::Iterate(BShape *shape)
{
	BRect frame = shape->Bounds();
	fLocation.Set(0, 0);
	fStart.Set(0, 0);
	
	fBitmap->Lock();
	fView->SetHighColor(255, 255, 255);
	fView->SetLowColor(0, 0, 0);
	fView->SetDrawingMode(B_OP_COPY);
	fView->FillRect(frame, B_SOLID_LOW);
	fView->SetDrawingMode(B_OP_INVERT);
	
	BShapeIterator::Iterate(shape);
	
	fView->Sync();
	fBitmap->Unlock();
	
	uint32 c = B_TRANSPARENT_MAGIC_RGBA32;
	rgb_color transparent;
	transparent.alpha = (c >> 24) & 0xff;
	transparent.red = (c >> 16) & 0xff;
	transparent.green = (c >> 8) & 0xff;
	transparent.blue = (c >> 0) & 0xff;
	
	if (fOutput->LockLooper()) {
		drawing_mode mode = fOutput->DrawingMode();
		BPoint origin = fOutput->Origin();
		fOutput->SetLowColor(transparent);
		fOutput->SetHighColor(0, 0, 0, 255);
		fOutput->SetDrawingMode(B_OP_ERASE);
		fOutput->SetOrigin(fOffset);
		fOutput->DrawBitmap(fBitmap);
		fOutput->SetDrawingMode(mode);
		fOutput->SetOrigin(origin);
		fOutput->UnlockLooper();
	}
	
	return B_OK;
}
