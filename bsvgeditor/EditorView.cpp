#include "EditorView.h"
#include "EditorWindow.h"
#include "DrawIterator.h"
#include "GetPointsIterator.h"
#include "OffscreenView.h"
#include "SVGStop.h"

EditorView::EditorView(BRect frame, const char *name, uint32 resize_mask, EditorWindow *parent)
	:	BSVGView(frame, name, resize_mask),
		fPoints(20, false),
		fEditorWindow(parent),
		fViewBitmap(NULL)
{
	SetViewCenter(BPoint(0, 0), true);
	SetFocusPoint(BPoint(0, 0), true, true);
	
	fGotPoints = false;
	fTrackMouse = false;
}

EditorView::~EditorView()
{
}

void
EditorView::GetPoints()
{
	fPoints.MakeEmpty();
	
	BSVGPath *path = fEditorWindow->CurrentPath();
	if (path) {
		BShape *shape = path->Shape();
		if (shape) {
			GetPointsIterator iterator(fPoints);
			iterator.Iterate(shape);
		}
	}
	
	BSVGGradient *grad = fEditorWindow->CurrentGradient();
	if (grad) {
		BSVGRadialGradient *radgrad = dynamic_cast<BSVGRadialGradient *>(grad);
		if (radgrad)
			fGradientStart = radgrad->Focal();
		else
			fGradientStart = grad->Start();
		
		fGradientEnd = grad->End();
		
		fPoints.AddItem(&fGradientStart);
		fPoints.AddItem(&fGradientEnd);
	}
}

BObjectList<BPoint> *
EditorView::Points()
{
	return &fPoints;
}

void
EditorView::SetTrackPoint(BPoint *point)
{
	fTrackPoint = point;
}

BPoint *
EditorView::TrackPoint()
{
	return fTrackPoint;
}

void
EditorView::SetTracking(bool tracking)
{
	fTrackMouse = tracking;
}

bool
EditorView::IsTracking()
{
	return fTrackMouse;
}

void
EditorView::MouseDown(BPoint where)
{
	BPoint pos;
	uint32 buttons;
	fEditorWindow->Offscreen()->GetMouse(&pos, &buttons);
	
	GetPoints();
	
	where.x /= Scale();
	where.y /= Scale();
	
	bool found = false;
	BRect rect(where, where);
	rect.InsetBy(-3, -3);
	for (int i = 0; i < fPoints.CountItems(); i++) {
		if (rect.Contains(*fPoints.ItemAt(i))) {
			found = true;
			fTrackPoint = fPoints.ItemAt(i);
			break;
		}
	}
	
	if (found)
		SetTracking(true);
	else
		fEditorWindow->MouseDown(where, buttons);
}

void
EditorView::MouseMoved(BPoint where, uint32 code, const BMessage *message)
{
	if (!fTrackMouse)
		return;
	
	where.x /= Scale();
	where.y /= Scale();
	
	if (*fTrackPoint == where)
		return;
	
	BSVGGradient *grad = fEditorWindow->CurrentGradient();
	if (grad) {
		if (fTrackPoint == &fGradientStart) {
			BSVGRadialGradient *radgrad = dynamic_cast<BSVGRadialGradient *>(grad);
			if (radgrad)
				radgrad->SetFocal(where);
			else
				grad->SetStart(where);
		} else if (fTrackPoint == &fGradientEnd) {
			grad->SetEnd(where);
		}
	}
	
	BaseTool *tool = fEditorWindow->Tool();
	if (tool && tool->MouseMoved(fTrackPoint, where))
		Invalidate();
}

void
EditorView::MouseUp(BPoint where)
{
	fTrackMouse = false;
}

void
EditorView::MessageReceived(BMessage *message)
{
	switch (message->what) {
		case M_VIEW_SELECTED: {
			if (fEditorWindow->fLastEditing == M_SOURCE_TAB) {
				BString data = fEditorWindow->Source()->Text();
				Clear();
				LoadFromString(data);
			}
			
			fEditorWindow->fLastEditing = M_DRAW_TAB;
			
			if (!fEditorWindow->IsHidden()) {
				if (fEditorWindow->CurrentPath())
					fEditorWindow->Settings()->SetTo(fEditorWindow->CurrentPath());
				if (fEditorWindow->CurrentGradient())
					fEditorWindow->Settings()->SetTo(fEditorWindow->CurrentGradient());
				if (fEditorWindow->CurrentStop())
					fEditorWindow->Settings()->SetTo(fEditorWindow->CurrentStop());
				
				Invalidate();
			}
		} break;
		default: fEditorWindow->MessageReceived(message);
	}
}

void
EditorView::SetViewBitmap(const BBitmap *bitmap, BRect srcRect, BRect dstRect, uint32 followFlags, uint32 options)
{
	fViewBitmap = (BBitmap *)bitmap;
	fViewBitmapSrc = srcRect;
	fViewBitmapFollow = followFlags;
	fViewBitmapOpt = options;
}

void
EditorView::ClearViewBitmap()
{
	fViewBitmap = NULL;
}

void
EditorView::AddAndLock()
{
	fEditorWindow->Offscreen()->AddChild(this);
	fEditorWindow->Offscreen()->Lock();
}

void
EditorView::RemoveAndUnlock()
{
	fEditorWindow->Offscreen()->Unlock();
	fEditorWindow->Offscreen()->RemoveChild(this);
	fEditorWindow->Offscreen()->UpdateOffscreen();
}

void
EditorView::Invalidate()
{
	if (fEditorWindow->TabView()->Selection() != M_DRAW_TAB)
		return;
	
	AddAndLock();
	Draw(BRect(0, 0, 0, 0));
	RemoveAndUnlock();
}

void
EditorView::Draw(BRect rect)
{
	if (fViewBitmap) {
		SetDrawingMode(B_OP_COPY);
		SetHighColor(255, 255, 255);
		FillRect(Frame().OffsetToSelf(0, 0));
		BRect frame = Frame();
		frame.InsetBy(frame.Width() * 0.1, frame.Height() * 0.1);
		DrawBitmap(fViewBitmap, fViewBitmapSrc, (frame.IsValid() ? frame : Frame()));
	}
	
	Redraw(true);
	
	BSVGPath *path = fEditorWindow->CurrentPath();
	if (path && path->Type() == B_SVG_PATH) {
		BShape *shape = path->TransformedShape();
		if (shape) {
			DrawIterator iterator(*this);
			iterator.Iterate(shape);
		}
	}
	
	BSVGGradient *grad = fEditorWindow->CurrentGradient();
	if (grad) {
		BSVGRadialGradient *radgrad = dynamic_cast<BSVGRadialGradient *>(grad);
		BPoint pt[2];
		
		SetHighColor(0, 0, 255);
		pt[0] = (radgrad ? radgrad->Focal() : grad->Start());
		pt[0] = grad->Transformation() * pt[0];
		pt[0].x *= Scale(); pt[0].y *= Scale();
		StrokeEllipse(pt[0], 2, 2);
		
		SetHighColor(0, 255, 0);
		pt[1] = grad->End();
		pt[1] = grad->Transformation() * pt[1];
		pt[1].x *= Scale(); pt[1].y *= Scale();
		StrokeEllipse(pt[1], 2, 2);
		
		BSVGStop *stop = fEditorWindow->CurrentStop();
		if (stop) {
			SetHighColor(255, 0, 0);
			StrokeLine(pt[0], pt[1]);
			
			BPoint delta = pt[1] - pt[0];
			delta.x *= stop->Offset(); delta.y *= stop->Offset();
			
			StrokeEllipse(delta + pt[0], 2, 2);
			MovePenTo(0, 0);
		}
	}
}
