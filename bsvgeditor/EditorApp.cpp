#include "EditorApp.h"
#include <TranslationUtils.h>
#include <Bitmap.h>
#include <Path.h>

EditorApp::EditorApp(int argc, char **argv)
	:	BApplication("application/x-vnd.mmlr.bsvgeditor")
{
	fWindows = new BObjectList<EditorWindow>(4, true);
	
	if (argc >= 2) {
		BEntry entry(argv[1]);
		if (entry.InitCheck() != B_OK)
			return;
		
		entry_ref ref;
		entry.GetRef(&ref);
		
		LoadInNewWindow(ref);
	}
}

EditorApp::~EditorApp()
{
}

void
EditorApp::ReadyToRun()
{
	if (!fWindows->IsEmpty())
		return;
	
	EditorWindow *window = new EditorWindow(BRect(100, 100, 1000, 800), "bsvgeditor");
	window->CreateElement(B_SVG_ROOT);
	window->CreateElement(B_SVG_PATH);
	window->Show();
	fWindows->AddItem(window);
}

void
EditorApp::LoadInNewWindow(entry_ref &ref)
{
	BString name = "bsvgeditor -> ";
	name << ref.name;
	EditorWindow *window = new EditorWindow(BRect(100, 100, 1000, 800), name.String());
	
	window->Lock();
	if (window->View()->LoadFromFile(&ref) != B_OK) {
		delete window;
		return;
	}
	
	window->RefreshElements();
	window->Unlock();
	window->Show();
	window->Elements()->Select(0);
	
	fWindows->AddItem(window);
}	

void
EditorApp::RefsReceived(BMessage *message)
{
	entry_ref ref;
	if (message->FindRef("refs", &ref) != B_OK)
		return;
	
	LoadInNewWindow(ref);
}

void
EditorApp::MessageReceived(BMessage *message)
{
	switch (message->what) {
		case B_SIMPLE_DATA:
		case M_OPEN_FILE_REFS: {
			entry_ref ref;
			if (message->FindRef("refs", &ref) != B_OK)
				break;
			
			LoadInNewWindow(ref);
		} break;
		
		case M_OPEN_BACKGROUND_REFS: {
			if (!fRequest)
				break;
			
			entry_ref ref;
			if (message->FindRef("refs", &ref) != B_OK)
				break;
			
			BBitmap *bitmap = BTranslationUtils::GetBitmap(&ref);
			if (!bitmap)
				break;
			
			fRequest->Lock();
			fRequest->View()->SetViewColor(255, 255, 255, 0);
			fRequest->View()->SetViewBitmap(bitmap, bitmap->Bounds(), fRequest->View()->Frame().OffsetToSelf(0, 0), B_FOLLOW_ALL);
			fRequest->View()->Invalidate();
			fRequest->Unlock();
			fRequest = NULL;
		} break;
		
		case M_EXPORT_FILE_REFS: {
			if (!fRequest)
				break;
			
			entry_ref dir;
			BString name;
			if (message->FindRef("directory", &dir) != B_OK
				|| message->FindString("name", &name) != B_OK)
				break;
			
			BPath path(&dir);
			path.Append(name.String());
			fRequest->View()->ExportToFile(path.Path());
			fRequest = NULL;
		} break;
		
		default: BApplication::MessageReceived(message);
	}
}

void
EditorApp::SetRequest(EditorWindow *window)
{
	fRequest = window;
}

EditorWindow *
EditorApp::Request()
{
	return fRequest;
}

void
EditorApp::ClearRequest()
{
	fRequest = NULL;
}

void
EditorApp::RemoveWindow(EditorWindow *window)
{
	fWindows->RemoveItem(window);
	
	if (fWindows->IsEmpty()) {
		LockLooper();
		Quit();
		UnlockLooper();
	}
}
