#include <stdio.h>
#include <strings.h>
#include "npapi.h"
#include "svgplugin.h"

/***********************************************************************
 *
 * Implementations of plugin API functions
 *
 ***********************************************************************/

#define DEBUG_PRINT(x)		//printf(x);
#define	PLUGIN_NAME			"SVGPlugin"
#define	PLUGIN_DESCRIPTION	PLUGIN_NAME " based on libbsvg"
#define	PLUGIN_MIME			"image/svg+xml"
#define	PLUGIN_EXT			"svg"
#define	MIME_SUPPORT		PLUGIN_MIME";"PLUGIN_EXT";"PLUGIN_NAME
#define	INITINST			if (instance == NULL) return NPERR_INVALID_INSTANCE_ERROR; \
							PluginInstance *_inst = (PluginInstance *)instance->pdata; \
							PluginData *inst = _inst->data; \
							if (inst == NULL) return NPERR_INVALID_INSTANCE_ERROR;

PluginView::PluginView()
	:	BSVGView(BRect(0, 0, 1, 1), "SVGPluginView", B_FOLLOW_NONE, B_WILL_DRAW),
		fBuffer(NULL),
		fBufferLength(0),
		fFileName(NULL)
{
	SetViewColor(255, 255, 255);
	SetScaleToFit(true);
	SetFitContent(true);
}

PluginView::~PluginView()
{
	fBuffer.SetTo("");
	fFileName.SetTo("");
}

void
PluginView::SetBuffer(char *buffer, int32 length, const char *url)
{
	if (!buffer)
		return;
	
	fBuffer.SetTo(buffer, length);
	fBufferLength = length;
	
	fFileName.SetTo(url);
	int where = fFileName.FindLast("/");
	if (where >= 0)
		fFileName.Remove(0, where + 1);
	
	Load();
}

void
PluginView::Load()
{
	if (!Window()->Lock())
		return;
	
	Clear();
	
	char *buffer = fBuffer.LockBuffer(fBufferLength);
	LoadFromBuffer(buffer, fBufferLength);
	fBuffer.UnlockBuffer(fBufferLength);
	FrameResized(Bounds().Width(), Bounds().Height());
	
	Window()->Unlock();
}

char *
NPP_GetMIMEDescription(void)
{
	DEBUG_PRINT("SVGPlugin: getmimedescription\n");
	return (MIME_SUPPORT);
}

NPError
NPP_GetValue(NPP instance, NPPVariable variable, void *value)
{
	DEBUG_PRINT("SVGPlugin: getvalue\n");
	NPError err = NPERR_NO_ERROR;
	
	switch (variable) {
		case NPPVpluginNameString: *((char **)value) = PLUGIN_NAME; break;
		case NPPVpluginDescriptionString: *((char **)value) = PLUGIN_DESCRIPTION; break;
		default: err = NPERR_GENERIC_ERROR; break;
	}
	
	return err;
}

NPError
NPP_Initialize(void)
{
	DEBUG_PRINT("SVGPlugin: initialize\n");
	return NPERR_NO_ERROR;
}

void
NPP_Shutdown(void)
{
	DEBUG_PRINT("SVGPlugin: shutdown\n");
}

NPError 
NPP_New(NPMIMEType pluginType, NPP instance, uint16 mode, int16 argc, char *argn[], char *argv[], NPSavedData *saved)
{
	DEBUG_PRINT("SVGPlugin: new\n");
	
	if (instance == NULL)
		return NPERR_INVALID_INSTANCE_ERROR;
	
	instance->pdata = new PluginInstance;
	PluginInstance *_inst = (PluginInstance *)instance->pdata;
	_inst->data = new PluginData;
	PluginData *inst = _inst->data;
	
	if (inst == NULL)
        return NPERR_OUT_OF_MEMORY_ERROR;
	
	inst->fMode = mode;
	inst->fBuffer = NULL;
	inst->fBufferLength = 0;
	inst->fView = new PluginView();
	
	return NPERR_NO_ERROR;
}

NPError 
NPP_Destroy(NPP instance, NPSavedData **save)
{
	DEBUG_PRINT("SVGPlugin: destroy\n");
	INITINST;
	
	if (inst) {
		if (inst->fView) {
			BWindow *win = inst->fView->Window();
			win->Lock();
			inst->fView->RemoveSelf();
			delete inst->fView;
			win->Unlock();
		}
    }
	
    return NPERR_NO_ERROR;
}


NPError 
NPP_SetWindow(NPP instance, NPWindow *window)
{
	DEBUG_PRINT("SVGPlugin: setwindow\n");
	INITINST;
	
	BView *bview = (BView *)window->window;
	if (!bview)
		return NPERR_INVALID_PARAM;

	bview->Window()->Lock();
	inst->fView->RemoveSelf();
	inst->fView->ResizeTo(bview->Bounds().Width(), bview->Bounds().Height());
	bview->AddChild(inst->fView);
	bview->Window()->Unlock();
	
	return NPERR_NO_ERROR;
}


NPError 
NPP_NewStream(NPP instance, NPMIMEType type, NPStream *stream, NPBool seekable, uint16 *stype)
{
	DEBUG_PRINT("SVGPlugin: newstream\n");
	if (stream->end <= 0)
		return NPERR_INVALID_PARAM;
	
	INITINST;
	inst->fBufferLength = stream->end;
	inst->fBuffer = new char[inst->fBufferLength];
	memset(inst->fBuffer, 0, inst->fBufferLength);
	
	return NPERR_NO_ERROR;
}


int32 
NPP_WriteReady(NPP instance, NPStream *stream)
{
	DEBUG_PRINT("SVGPlugin: writeready\n");
	return stream->end;
}


int32 
NPP_Write(NPP instance, NPStream *stream, int32 offset, int32 len, void *buffer)
{
	DEBUG_PRINT("SVGPlugin: write\n");
	INITINST;
		
	if (!inst->fBuffer || offset + len > inst->fBufferLength)
		return -1;
	
	memcpy(inst->fBuffer + offset, buffer, len);
	
	return len;
}


NPError 
NPP_DestroyStream(NPP instance, NPStream *stream, NPError reason)
{
	DEBUG_PRINT("SVGPlugin: destroystream\n");
	INITINST;	
	
#if 0	
	BString path("/boot/home/Desktop/");
	BString url(stream->url);
	BString name("");
	url.MoveInto(name, url.FindLast("/") + 1, url.Length() - url.FindLast("/") - 1);
	
	path << name;
	FILE *file = fopen(path.String(), "w");
	fwrite(inst->fBuffer, inst->fBufferLength, 1, file);
	fclose(file);
#endif
	
	inst->fView->SetBuffer(inst->fBuffer, inst->fBufferLength, stream->url);
	inst->fBuffer = NULL;
	inst->fBufferLength = 0;
	
	return NPERR_NO_ERROR;
}


void 
NPP_StreamAsFile(NPP instance, NPStream *stream, const char *fname)
{
	DEBUG_PRINT("SVGPlugin: streamasfile\n");
}


void
NPP_URLNotify(NPP instance, const char *url, NPReason reason, void *notifyData)
{
	DEBUG_PRINT("SVGPlugin: urlnotify\n");
}


void 
NPP_Print(NPP instance, NPPrint* printInfo)
{
	DEBUG_PRINT("SVGPlugin: print\n");
	if(printInfo == NULL)
		return;
	
	if (instance != NULL) {
		if (printInfo->mode == NP_FULL) {
			/* Do the default*/
			printInfo->print.fullPrint.pluginPrinted = FALSE;
		} else {
			/* If not fullscreen, we must be embedded */
		}
	}
}
