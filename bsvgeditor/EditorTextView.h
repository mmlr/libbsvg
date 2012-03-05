#ifndef EDITOR_TEXT_VIEW_H_
#define EDITOR_TEXT_VIEW_H_

#include <TextView.h>
#include "EditorWindow.h"

class EditorTextView : public BTextView
{
	public:
						EditorTextView(BRect frame, const char *name, uint32 resize, EditorWindow *parent);
						
virtual	void			FrameResized(float new_width, float new_height);
virtual	void			MessageReceived(BMessage *message);
		void			Rebuild();

	private:
		EditorWindow	*fEditorWindow;
};

#endif
