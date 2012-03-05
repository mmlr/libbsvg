#include "OffscreenView.h"
#include "EditorView.h"

OffscreenView::OffscreenView(EditorView *target)
	:	BView(target->Frame(), "OffscreenView", B_FOLLOW_ALL, B_WILL_DRAW | B_FRAME_EVENTS),
		fOffscreen(NULL)
{
	fTarget = target;
	fOffscreen = new BBitmap(target->Frame().OffsetToSelf(0, 0), B_RGBA32, true);
}

OffscreenView::~OffscreenView()
{
}

void
OffscreenView::FrameResized(float new_width, float new_height)
{
	BRect rect(0, 0, new_width, new_height);
	delete fOffscreen;
	fOffscreen = new BBitmap(rect, B_RGBA32, true);
	fTarget->ResizeTo(new_width, new_height);
	fTarget->FrameResized(new_width, new_height);
	fTarget->Invalidate();
}

void
OffscreenView::AddChild(BView *view)
{
	fOffscreen->AddChild(view);
}

void
OffscreenView::RemoveChild(BView *view)
{
	fOffscreen->RemoveChild(view);
}

void
OffscreenView::Lock()
{
	fOffscreen->Lock();
}

void
OffscreenView::Unlock()
{
	fOffscreen->Unlock();
}

void
OffscreenView::UpdateOffscreen()
{
	SetDrawingMode(B_OP_COPY);
	DrawBitmap(fOffscreen);
}

void
OffscreenView::Invalidate()
{
	UpdateOffscreen();
}

void
OffscreenView::Draw(BRect rect)
{
	SetDrawingMode(B_OP_COPY);
	DrawBitmap(fOffscreen, rect, rect);
}

void
OffscreenView::MouseDown(BPoint where)
{
	fTarget->MouseDown(where);
	if (fTarget->IsTracking())
		SetMouseEventMask(B_POINTER_EVENTS, B_LOCK_WINDOW_FOCUS | B_NO_POINTER_HISTORY);
}

void
OffscreenView::MouseMoved(BPoint where, uint32 code, const BMessage *message)
{
	if (fTarget->IsTracking())
		fTarget->MouseMoved(where, code, message);
}

void
OffscreenView::MouseUp(BPoint where)
{
	fTarget->MouseUp(where);
}

void
OffscreenView::MessageReceived(BMessage *message)
{
	fTarget->MessageReceived(message);
}
