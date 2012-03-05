#ifndef _EDITOR_APP_H_
#define _EDITOR_APP_H_

#include <Application.h>
#include "EditorWindow.h"
#include "ObjectList.h"

class EditorWindow;

class EditorApp : public BApplication
{
	public:
						EditorApp(int argc, char **argv);
virtual					~EditorApp();

		void			LoadInNewWindow(entry_ref &ref);
virtual	void			ReadyToRun();
virtual	void			RefsReceived(BMessage *message);
virtual	void			MessageReceived(BMessage *message);

		void			SetRequest(EditorWindow *window);
		EditorWindow	*Request();
		void			ClearRequest();
		
		void			RemoveWindow(EditorWindow *window);
						
	private:
		
		BObjectList<EditorWindow>	*fWindows;
		EditorWindow				*fRequest; // the window that is requesting something
};

#endif
