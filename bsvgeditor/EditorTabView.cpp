#include "EditorTabView.h"
#include "EditorDefs.h"
#include <stdio.h>
#include <Message.h>

EditorTabView::EditorTabView(BRect frame, const char *name)
	:	BTabView(frame, name)
{
}
						
void
EditorTabView::FrameResized(float new_width, float new_height)
{
	BTabView::FrameResized(new_width, new_height);
	
	for (int i = 0; i < CountTabs(); i++)
		if (i != Selection())
			TabAt(i)->View()->ResizeTo(new_width, new_height);
}

void
EditorTabView::Select(int32 tabIndex)
{
	BTabView::Select(tabIndex);
	TabAt(tabIndex)->View()->MessageReceived(new BMessage(M_VIEW_SELECTED));
}
