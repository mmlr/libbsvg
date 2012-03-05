#ifndef _EDITOR_TOOLS_H_
#define _EDITOR_TOOLS_H_

#include <Message.h>
#include "EditorWindow.h"

class EditorWindow;

class BaseTool {
	public:
						BaseTool(EditorWindow *target);
virtual					~BaseTool();
						
		// handled?
virtual	bool			MessageReceived(BMessage *message);
virtual	bool			MouseDown(BPoint where, uint32 buttons = 1);
virtual	bool			MouseMoved(BPoint *track, BPoint where);

		EditorWindow	*fWindow;
		BPoint			fLast;
};

class ToolMoveTo : public BaseTool {
	public:
						ToolMoveTo(EditorWindow *target);						
virtual	bool			MouseDown(BPoint where, uint32 buttons = 1);
};

class ToolLineTo : public BaseTool {
	public:
						ToolLineTo(EditorWindow *target);
virtual	bool			MouseDown(BPoint where, uint32 buttons = 1);
};

class ToolHLineTo : public BaseTool {
	public:
						ToolHLineTo(EditorWindow *target);
virtual	bool			MouseDown(BPoint where, uint32 buttons = 1);
virtual	bool			MouseMoved(BPoint *track, BPoint where);
};

class ToolVLineTo : public BaseTool {
	public:
						ToolVLineTo(EditorWindow *target);
virtual	bool			MouseDown(BPoint where, uint32 buttons = 1);
virtual	bool			MouseMoved(BPoint *track, BPoint where);
};

class ToolCurveTo : public BaseTool {
	public:
						ToolCurveTo(EditorWindow *target);
virtual	bool			MouseDown(BPoint where, uint32 buttons = 1);
};

class ToolShortCurveTo : public BaseTool {
	public:
						ToolShortCurveTo(EditorWindow *target);
virtual	bool			MouseDown(BPoint where, uint32 buttons = 1);
};

class ToolQuadBezierTo : public BaseTool {
	public:
						ToolQuadBezierTo(EditorWindow *target);
virtual	bool			MouseDown(BPoint where, uint32 buttons = 1);
};

class ToolShortBezierTo : public BaseTool {
	public:
						ToolShortBezierTo(EditorWindow *target);
virtual	bool			MouseDown(BPoint where, uint32 buttons = 1);
};

class ToolEllipticalArcTo : public BaseTool {
	public:
						ToolEllipticalArcTo(EditorWindow *target);
virtual	bool			MouseDown(BPoint where, uint32 buttons = 1);
};

class ToolClose : public BaseTool {
	public:
						ToolClose(EditorWindow *target);
virtual	bool			MouseDown(BPoint where, uint32 buttons = 1);
};

#endif
