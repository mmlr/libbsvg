#include "npplat.h"
#include <String.h>
#include <Window.h>
#include "SVGView.h"

class PluginView : public BSVGView {
	public:
						PluginView();
virtual					~PluginView();
		void			Load();
		void			SetBuffer(char *buffer, int32 length, const char *url);

	private:
		BString			fBuffer;
		int32			fBufferLength;
		BString			fFileName;
};

struct PluginData {
	bool		fHidden;
	uint16		fMode;
	
	PluginView 	*fView;
	char		*fBuffer;
	int32		fBufferLength;
};

typedef struct _PluginInstance
{
	PluginData	*data;
} PluginInstance;

typedef struct _MimeTypeElement
{
	PluginInstance *pinst;
	struct _MimeTypeElement *next;
} MimeTypeElement;
