#ifndef EDITOR_TAB_VIEW_H_
#define EDITOR_TAB_VIEW_H_

#include <TabView.h>

class EditorTabView : public BTabView
{
	public:
						EditorTabView(BRect frame, const char *name);
						
virtual	void			FrameResized(float new_width, float new_height);
virtual	void			Select(int32 tabIndex);
};

#endif
