#include "EditorPreView.h"
#include <Message.h>
#include <String.h>

EditorPreView::EditorPreView(BRect frame, const char *name, uint32 resize, EditorWindow *parent)
	:	BSVGView(frame, name, resize),
		fEditorWindow(parent)
{
}

void
EditorPreView::Rebuild()
{
	if (fEditorWindow->TabView()->Selection() != M_PREVIEW_TAB)
		return;
	
	SetScale(fEditorWindow->View()->Scale());
	if (fEditorWindow->fLastEditing == M_DRAW_TAB) {
		BString data;
		if (fEditorWindow->View()->ExportToString(data) != B_OK)
			return;
		
		Clear();
		LoadFromString(data);
	} else if (fEditorWindow->fLastEditing == M_SOURCE_TAB) {
		BString data = fEditorWindow->Source()->Text();
		Clear();
		LoadFromString(data);
	}
}

void
EditorPreView::MessageReceived(BMessage *message)
{
	switch (message->what) {
		case M_VIEW_SELECTED: {
			Rebuild();
			fEditorWindow->Settings()->SetTo(this);
		} break;
		default: BSVGView::MessageReceived(message);
	}
}
