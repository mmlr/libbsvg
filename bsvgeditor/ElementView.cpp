#include "ElementView.h"
#include "EditorWindow.h"
#include <String.h>

ElementView::ElementView(BRect frame, const char *name)
	:	BOutlineListView(frame, name, B_SINGLE_SELECTION_LIST, B_FOLLOW_ALL)
{
}

void
ElementView::KeyDown(const char *bytes, int32 numBytes)
{
	EditorWindow *win = (EditorWindow *)Window();
	if (!win || !win->KeyDown(bytes, numBytes))
		BOutlineListView::KeyDown(bytes, numBytes);
}
