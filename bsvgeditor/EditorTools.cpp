#include "EditorTools.h"

BaseTool::BaseTool(EditorWindow *target)
	:	fWindow(target),
		fLast(0, 0)
{
}

BaseTool::~BaseTool()
{
}

bool
BaseTool::MessageReceived(BMessage *message)
{
	return false;
}

bool
BaseTool::MouseDown(BPoint where, uint32 buttons)
{
	BSVGPath *path = fWindow->CurrentPath();
	if (!path)
		return false;
	
	path->ApplyTransformation();
	fWindow->View()->Invalidate();
	
	if (fLast != fWindow->CurrentPath()->LastLocation()) {
		fLast = fWindow->CurrentPath()->LastLocation();
		fWindow->View()->GetPoints();
		int32 count = fWindow->View()->Points()->CountItems();
		BPoint *track = fWindow->View()->Points()->ItemAt(count - 1);
		if (track != NULL) {
			fWindow->View()->SetTrackPoint(track);
			fWindow->View()->SetTracking(true);
		}
	}
	return true;
}

bool
BaseTool::MouseMoved(BPoint *track, BPoint where)
{
	if (*track != where)
		*track = where;
	
	return true;
}

ToolMoveTo::ToolMoveTo(EditorWindow *target)
	:	BaseTool(target)
{
}

bool
ToolMoveTo::MouseDown(BPoint where, uint32 buttons)
{
	BSVGPath *path = fWindow->CurrentPath();
	if (!path)
		return false;
	
	path->MoveTo(where);
	return BaseTool::MouseDown(where, buttons);
}

ToolLineTo::ToolLineTo(EditorWindow *target)
	: BaseTool(target)
{
}

bool
ToolLineTo::MouseDown(BPoint where, uint32 buttons)
{
	BSVGPath *path = fWindow->CurrentPath();
	if (!path)
		return false;
	
	path->LineTo(where);
	return BaseTool::MouseDown(where, buttons);
}

ToolHLineTo::ToolHLineTo(EditorWindow *target)
	:	BaseTool(target)
{
}

bool
ToolHLineTo::MouseDown(BPoint where, uint32 buttons)
{
	BSVGPath *path = fWindow->CurrentPath();
	if (!path)
		return false;
	
	path->HLineTo(where.x);
	path->ApplyTransformation();
	fWindow->View()->Invalidate();
	
	if (fLast.y != fWindow->CurrentPath()->LastLocation().y) {
		fLast.y = fWindow->CurrentPath()->LastLocation().y;
		fWindow->View()->GetPoints();
		int32 count = fWindow->View()->Points()->CountItems();
		BPoint *track = fWindow->View()->Points()->ItemAt(count - 1);
		if (track != NULL) {
			fWindow->View()->SetTrackPoint(track);
			fWindow->View()->SetTracking(true);
		}
	}
	return true;
}

bool
ToolHLineTo::MouseMoved(BPoint *track, BPoint where)
{
	if (track->x != where.x)
		track->x = where.x;
	
	return true;
}

ToolVLineTo::ToolVLineTo(EditorWindow *target)
	:	BaseTool(target)
{
}

bool
ToolVLineTo::MouseDown(BPoint where, uint32 buttons)
{
	BSVGPath *path = fWindow->CurrentPath();
	if (!path)
		return false;
	
	path->VLineTo(where.y);
	path->ApplyTransformation();
	fWindow->View()->Invalidate();
	
	if (fLast.x != fWindow->CurrentPath()->LastLocation().x) {
		fLast.x = fWindow->CurrentPath()->LastLocation().x;
		fWindow->View()->GetPoints();
		int32 count = fWindow->View()->Points()->CountItems();
		BPoint *track = fWindow->View()->Points()->ItemAt(count - 1);
		if (track != NULL) {
			fWindow->View()->SetTrackPoint(track);
			fWindow->View()->SetTracking(true);
		}
	}
	return true;
}

bool
ToolVLineTo::MouseMoved(BPoint *track, BPoint where)
{
	if (track->y != where.y)
		track->y = where.y;
	
	return true;
}

ToolCurveTo::ToolCurveTo(EditorWindow *target)
	:	BaseTool(target)
{
}

bool
ToolCurveTo::MouseDown(BPoint where, uint32 buttons)
{
	BSVGPath *path = fWindow->CurrentPath();
	if (!path)
		return false;
	
	BPoint ctrl1, ctrl2;
	ctrl1 = path->LastLocation();
	
	BPoint delta = where - ctrl1;
	if (delta.x != 0)
		delta.x /= 3;
	if (delta.y != 0)
		delta.y /= 3;
	
	ctrl1 += delta;
	ctrl2 = ctrl1 + delta;
	
	path->CurveTo(ctrl1, ctrl2, where);
	return BaseTool::MouseDown(where, buttons);
}

ToolShortCurveTo::ToolShortCurveTo(EditorWindow *target)
	:	BaseTool(target)
{
}

bool
ToolShortCurveTo::MouseDown(BPoint where, uint32 buttons)
{
	BSVGPath *path = fWindow->CurrentPath();
	if (!path)
		return false;
	
	BPoint ctrl;
	ctrl = path->LastLocation();
	
	BPoint delta = where - ctrl;
	if (delta.x != 0)
		delta.x /= 3;
	if (delta.y != 0)
		delta.y /= 3;
	
	ctrl += delta;
	ctrl += delta;
	
	path->ShortCurveTo(ctrl, where);
	return BaseTool::MouseDown(where, buttons);
}

ToolQuadBezierTo::ToolQuadBezierTo(EditorWindow *target)
	:	BaseTool(target)
{
}

bool
ToolQuadBezierTo::MouseDown(BPoint where, uint32 buttons)
{
	BSVGPath *path = fWindow->CurrentPath();
	if (!path)
		return false;
	
	BPoint ctrl;
	ctrl = path->LastLocation();
	
	BPoint delta = where - ctrl;
	if (delta.x != 0)
		delta.x /= 2;
	if (delta.y != 0)
		delta.y /= 2;
	
	ctrl += delta;
	
	path->QuadBezierTo(ctrl, where);
	return BaseTool::MouseDown(where, buttons);
}

ToolShortBezierTo::ToolShortBezierTo(EditorWindow *target)
	:	BaseTool(target)
{
}

bool
ToolShortBezierTo::MouseDown(BPoint where, uint32 buttons)
{
	BSVGPath *path = fWindow->CurrentPath();
	if (!path)
		return false;
	
	path->ShortBezierTo(where);
	return BaseTool::MouseDown(where, buttons);
}

ToolEllipticalArcTo::ToolEllipticalArcTo(EditorWindow *target)
	:	BaseTool(target)
{
}

bool
ToolEllipticalArcTo::MouseDown(BPoint where, uint32 buttons)
{
	BSVGPath *path = fWindow->CurrentPath();
	if (!path)
		return false;
	
	path->EllipticalArcTo(BPoint(50, 50), 0, 0, 0, where);
	return BaseTool::MouseDown(where, buttons);
}

ToolClose::ToolClose(EditorWindow *target)
	:	BaseTool(target)
{
	BSVGPath *path = fWindow->CurrentPath();
	if (!path)
		return;
	
	path->Close();
	path->ApplyTransformation();
	fWindow->View()->Invalidate();
}

bool
ToolClose::MouseDown(BPoint where, uint32 buttons)
{
	BSVGPath *path = fWindow->CurrentPath();
	if (!path)
		return false;
	
	path->Close();
	return BaseTool::MouseDown(where, buttons);
}
