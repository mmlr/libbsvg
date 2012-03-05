#ifndef _EDITOR_VIEW_H_
#define _EDITOR_VIEW_H_

#include "SVGView.h"
#include "SVGPath.h"
#include "EditorWindow.h"

class EditorWindow;

class EditorView : public BSVGView
{
	public:
							EditorView(BRect frame, const char *name, uint32 resize_mask, EditorWindow *parent);
							~EditorView();
					
		void				GetPoints();
		BObjectList<BPoint>	*Points();
					
virtual void				MouseDown(BPoint where);
virtual	void				MouseMoved(BPoint where, uint32 code, const BMessage *message);
virtual	void				MouseUp(BPoint where);

virtual	void				MessageReceived(BMessage *message);

		void				AddAndLock();
		void				RemoveAndUnlock();

virtual	void				Invalidate();
virtual	void				Draw(BRect rect);

virtual	void				SetTrackPoint(BPoint *point);
		BPoint				*TrackPoint();
		
virtual	void				SetTracking(bool tracking);
		bool				IsTracking();

		void				SetViewBitmap(const BBitmap *bitmap, BRect srcRect, BRect dstRect, uint32 followFlags = B_FOLLOW_TOP | B_FOLLOW_LEFT, uint32 options = B_TILE_BITMAP);
		void				ClearViewBitmap();

	private:
		bool				fGotPoints;
		BObjectList<BPoint>	fPoints;
		
		BPoint				fGradientStart;
		BPoint				fGradientEnd;
		
		BPoint				*fTrackPoint;
		bool				fTrackMouse;
		
		EditorWindow		*fEditorWindow;
		
		BBitmap				*fViewBitmap;
		BRect				fViewBitmapSrc;
		uint32				fViewBitmapFollow;
		uint32				fViewBitmapOpt;
};

#endif
