#include "GradientView.h"
#include "EditorWindow.h"

GradientView::GradientView(BRect frame, const char *name)
	:	BOutlineListView(frame, name)
{
}
						
void
GradientView::KeyDown(const char *bytes, int32 numBytes)
{
	EditorWindow *win = (EditorWindow *)Window();
	if (!win || !win->KeyDown(bytes, numBytes))
		BOutlineListView::KeyDown(bytes, numBytes);
}
