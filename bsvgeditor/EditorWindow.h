#ifndef _EDITOR_WINDOW_H_
#define _EDITOR_WINDOW_H_

#include <Window.h>
#include <ListView.h>
#include <Menu.h>
#include <MenuBar.h>
#include <MenuItem.h>
#include <Box.h>
#include <FilePanel.h>
#include <Messenger.h>
#include "EditorApp.h"
#include "EditorView.h"
#include "ObjectList.h"
#include "EditorDefs.h"
#include "EditorTools.h"
#include "SettingsView.h"
#include "ElementView.h"
#include "OffscreenView.h"
#include "EditorTabView.h"
#include "EditorPreView.h"
#include "EditorTextView.h"

class EditorApp;
class EditorView;
class EditorPreView;
class EditorTextView;
class BaseTool;
class SettingsView;

class EditorWindow : public BWindow
{
	public:
						EditorWindow(BRect frame, const char *title);
						~EditorWindow();
						
virtual bool			MouseDown(BPoint where, uint32 buttons);
virtual void			MessageReceived(BMessage *message);
virtual	bool			KeyDown(const char *bytes, int32 numBytes);
						
		void			CreateElement(element_t type = B_SVG_PATH);
		void			DeleteElement();
		void			ClearElements();
		void			AddRecursive(BSVGElement *element, BListItem *under);
		void			RefreshElements();
		
		void			OpenFile();
		void			OpenBackground();
		void			ClearBackground();
		void			ExportFile();
		
		BSVGElement		*CurrentElement();
		BSVGPath		*CurrentPath();
		BSVGGradient	*CurrentGradient();
		BSVGStop		*CurrentStop();

inline	EditorTabView	*TabView() { return fTabView; };
inline	EditorView		*View() { return fView; };
inline	EditorPreView	*PreView() { return fPreView; };
inline	EditorTextView	*Source() { return fSourceView; };
inline	OffscreenView	*Offscreen() { return fOffscreenView; };
inline	ElementView		*Elements() { return fElements; };
inline	SettingsView	*Settings() { return fSettings; };
		
inline	BaseTool		*Tool() { return fTool; };
		
		editor_tab		fLastEditing;

	private:
		EditorTabView	*fTabView;
		EditorView		*fView;
		OffscreenView	*fOffscreenView;
		EditorTextView	*fSourceView;
		EditorPreView	*fPreView;
		SettingsView	*fSettings;
		ElementView		*fElements;
		
		BMenuBar		*fMenu;
		BMenu			*fFileMenu;
		BMenu			*fHelpMenu;
		BMenu			*fToolMenu;
		BMenu			*fElementMenu;
		
		BFilePanel		*fFilePanel;
		
		BaseTool		*fTool;
		bool			fRelative;
};

#endif
