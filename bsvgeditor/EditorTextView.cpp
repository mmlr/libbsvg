#include "EditorTextView.h"
#include "EditorWindow.h"
#include "EditorDefs.h"
#include <stdio.h>
#include <Message.h>
#include <String.h>

EditorTextView::EditorTextView(BRect frame, const char *name, uint32 resize, EditorWindow *parent)
	:	BTextView(frame, name, frame, resize),
		fEditorWindow(parent)
{
}

void
EditorTextView::FrameResized(float new_width, float new_height)
{
	SetTextRect(BRect(0, 0, new_width, new_height));
}

void
EditorTextView::MessageReceived(BMessage *message)
{
	switch (message->what) {
		case M_VIEW_SELECTED: {
			if (fEditorWindow->fLastEditing == M_DRAW_TAB) {
				Rebuild();
			}
			
			fEditorWindow->fLastEditing = M_SOURCE_TAB;
			fEditorWindow->Settings()->SetTo(this);
		} break;
		default: BTextView::MessageReceived(message);
	}
}

void
EditorTextView::Rebuild()
{
	if (fEditorWindow->TabView()->Selection() != M_SOURCE_TAB)
		return;

	BString data;
	
	if (!fEditorWindow || fEditorWindow->View()->ExportToString(data) != B_OK) {
		SetText("Error Exporting Data");
		return;
	}
			
	SetText(data.String());
}
