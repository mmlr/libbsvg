#ifndef _OFFSCREEN_VIEW_H_
#define _OFFSCREEN_VIEW_H_

#include <View.h>
#include <Bitmap.h>

class EditorView;
class EditorWindow;

class OffscreenView : public BView {
	public:
						OffscreenView(EditorView *target);
						~OffscreenView();
						
virtual	void			Draw(BRect rect);
virtual	void			Invalidate();

virtual void			MouseDown(BPoint where);
virtual	void			MouseMoved(BPoint where, uint32 code, const BMessage *message);
virtual	void			MouseUp(BPoint where);

virtual	void			MessageReceived(BMessage *message);

virtual	void			FrameResized(float new_width, float new_height);

virtual	void			AddChild(BView *view);
virtual	void			RemoveChild(BView *view);

		void			UpdateOffscreen();
		void			Lock();
		void			Unlock();

	private:
		BBitmap			*fOffscreen;
		EditorView		*fTarget;
};

#endif
