#ifndef EDITOR_PRE_VIEW_H_
#define EDITOR_PRE_VIEW_H_

#include "SVGView.h"
#include "EditorWindow.h"

class BMessage;

class EditorPreView : public BSVGView
{
	public:
						EditorPreView(BRect frame, const char *name, uint32 resize, EditorWindow *parent);
virtual	void			MessageReceived(BMessage *message);
		void			Rebuild();

	private:
		EditorWindow	*fEditorWindow;
};

#endif
