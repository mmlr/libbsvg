#include <Application.h>
#include <Dragger.h>
#include <Entry.h>
#include <Window.h>
#include <OS.h>
#include <stdio.h>
#include <List.h>
#include "SVGView.h"

// archive macros
#define ADDREPADDON(x)	if (data->HasString("add_on")) data->ReplaceString("add_on", x); else data->AddString("add_on", x);
#define ADDREPCLASS(x)	if (data->HasString("class")) data->ReplaceString("class", x); else data->AddString("class", x);

// flags for what needs to be synced with the target
enum sync_flags {
	SET_WIDTH = 0x00000001,
	SET_HEIGHT = 0x00000002,
	SET_SAMPLESIZE = 0x00000004,
	SET_FITCONTENT = 0x00000008,
	SET_SCALETOFIT = 0x00000010,
	SET_BACKGROUND = 0x00000020,
	SET_PULSE = 0x00000040,
	SET_TRANSPARENT = 0x00000080,
	SET_QUIT = 0x00000100,
	SET_FILE = 0x00000200,
	SET_START_UPDATE = 0x00000400,
	SET_END_UPDATE = 0x00000800,
	SET_CLASS_ATTR = 0x00001000,
	SET_ELEMENT_ATTR = 0x00002000,
	SET_X_LOCATION = 0x00004000,
	SET_Y_LOCATION = 0x00008000,
	SET_CLASS_TEXT = 0x00010000,
	SET_ELEMENT_TEXT = 0x00020000
};

// class definitions; we only need an application and a window to hold the view
class SVGWin;
class SVGView;

class SVGApp : public BApplication
{
	public:
					SVGApp(int argc, char *argv[]);
virtual	void		RefsReceived(BMessage *message);

		SVGWin		*fWin;
};

class SVGWin : public BWindow
{
	public:
					SVGWin(BRect rect, int argc, char *argv[]);
		void		ResetTitle(const char *filename);
		void		MessageReceived(BMessage *message);
		SVGView		*View();

	private:
		float		fX;
		float		fY;
		float		fWidth;
		float		fHeight;
		int			fSampleSize;
		bool		fFitContent;
		bool		fScaleToFit;
		rgb_color	fBackground;
		bigtime_t	fPulseRate;
		bool		fTransparent;
		BString		fID;
		
		SVGView 	*fView;
		BDragger	*fDragger;
};

class SVGView : public BSVGView
{
	public:
					SVGView(BRect frame);
					~SVGView();

					SVGView(BMessage *data);
static	BArchivable	*Instantiate(BMessage *data);
virtual	status_t	Archive(BMessage *data, bool deep = true) const;

		void		SetID(const char *id);
		const char	*ID();

static	int32		WatchThreadFunc(void *arg);
		int32		WatchThread();

	private:
		port_id		fPort;
		BString		fID;
		bool		fReplicant;
		thread_id	fWatchThread;
};

// the main function just creates our application object and runs it
int
main(int argc, char **argv)
{
	SVGApp *app = new SVGApp(argc, argv);
	app->Run();
	delete app;
}

// app constructor that creates the window for our view
SVGApp::SVGApp(int argc, char *argv[])
	:	BApplication("application/x-vnd.mmlr.bsvgviewer")
{
	fWin = new SVGWin(BRect(100, 100, 275, 225), argc, argv);
	fWin->Show();
}

// if a file is opened from the tracker it's handled here
void
SVGApp::RefsReceived(BMessage *message)
{
	entry_ref ref;
	if (message->FindRef("refs", &ref) != B_OK)
		return;
	
	fWin->Lock();
	fWin->View()->Clear();
	fWin->View()->LoadFromFile(&ref);
	fWin->ResetTitle(ref.name);
	fWin->Unlock();
}

// here is where it all happens - the BSVGView is created and set
SVGWin::SVGWin(BRect rect, int argc, char *argv[])
	:	BWindow(rect, "SVGWindow", B_TITLED_WINDOW, B_QUIT_ON_WINDOW_CLOSE),
		fX(100),
		fY(100),
		fWidth(175),
		fHeight(125),
		fSampleSize(2),
		fFitContent(true),
		fScaleToFit(true),
		fPulseRate(0),
		fTransparent(false),
		fID("")
{
	fBackground.red = 50;
	fBackground.green = 100;
	fBackground.blue = 150;
	fBackground.alpha = 255;
	
	BString send_to = "";
	int32 flags = 0;
	char filename[B_PATH_NAME_LENGTH + B_FILE_NAME_LENGTH + 1];
	char buffer[B_PATH_NAME_LENGTH + B_FILE_NAME_LENGTH + 1];
	BList elementattrs;
	BList classattrs;
	BList elementtexts;
	BList classtexts;
	
	for (int i = 1; i < argc; i++) {
		BString arg = argv[i];
		
		if (arg.ICompare("-", 1) != 0) {
			strncpy(filename, argv[i], sizeof(filename) - 1);
			flags |= SET_FILE;
		} else if (arg == "--help") {
			printf("usage: %s [<svg-file>] [options]\n", argv[0]);
			printf("where options are:\n");
			printf("  -x,  --x (float)\t\tSet x location on screen\n");
			printf("  -y,  --y (float)\t\tSet y location on screen\n");
			printf("  -w,  --width (float)\t\tSet width on screen\n");
			printf("  -h,  --height (float)\t\tSet height on screen\n");
			printf("  -ss, --samplesize (1..5)\tSet the samplesize from 1 to 5 (1 = disabled)\n");
			printf("  -bg, --background r,g,b[,a]\tSet the background color (0..255 each)\n");
			printf("  -sf, --scaletofit (0,1)\tEnable / disable scale to fit\n");
			printf("  -fc, --fitcontent (0,1)\tEnable / disable fit content\n");
			printf("  -i=, --id=(string)\t\tSet the id of the given view (without quotes)\n");
			printf("\n");
			printf("If you give your SVGView an ID you can use bsvgviewer to \"remote control\"\n");
			printf("the instance. To do this call bsvgviewer with the same arguments as if you\n");
			printf("would a new instance, but provide an argument \"--id (string)\" (same as when\n");
			printf("setting the id but without a \"=\"). The arguments will then be forwarded to\n");
			printf("the named instance.\n");
			printf("\n");
			printf("Example:\n");
			printf("bsvgviewer --id test --width 100 /boot/home/Desktop/test.svg\n");
			printf("\n");
			printf("This will forward it's argument to the SVGView with the id \"test\", telling\n");
			printf("it to resize itself to a width of 100 and to load the file \"test.svg\".\n");
			printf("\n");
			printf("Advanced remote controlling:\n");
			printf("When remote controlling an instance of a SVGView you can adjust it's content\n");
			printf("further by manipulating individual attributes or text. This provides a scripting\n");
			printf("interface to change the content dynamically at runtime.\n");
			printf("\n");
			printf("The syntax is as follows:\n");
			printf("--classattr \"<class to access>:<attribute name>=<new value>\"\n");
			printf("--elementattr \"<id to access>:<attribute name>=<new value>\"\n");
			printf("--classtext \"<class to access>:<new text>\"\n");
			printf("--elementtext \"<id to access>:<new text>\"\n");
			printf("\n");
			printf("Or their respective shorthand version -ca, -ea, -ct and -et.\n");
			printf("\n");
			printf("The class versions will set the attribute/text on all elements of a given class\n");
			printf("while the element version will direct the change to a single element with the\n");
			printf("specified id. Both the id and class are specified within the SVG file using the\n");
			printf("class and id attribute.\n");
			printf("\n");
			printf("Example:\n");
			printf("Given the following SVG file:\n");
			printf("<svg xmlns=\"http://www.w3.org/2000/svg\" width=\"128\" height=\"128\">\n");
			printf("\t<defs>\n");
			printf("\t\t<path id=\"path1\" d=\"M77 63 L39 126 L113 126\" />\n");
			printf("\t\t<rect id=\"path2\" x=\"20\" y=\"20\" width=\"110\" height=\"110\" />\n");
			printf("\t\t<circle id=\"path3\" cx=\"64\" cy=\"64\" r=\"40\" />\n");
			printf("\t</defs>\n");
			printf("\t<use id=\"link\" xlink:href=\"#path1\" />\n");
			printf("\t<text id=\"one\" class=\"text\" x=\"15\" y=\"15\">\n");
			printf("\t\tabcd\n");
			printf("\t</text>\n");
			printf("\t<text id=\"two\" class=\"text\" x=\"85\" y=\"15\">\n");
			printf("\t\tdefg\n");
			printf("\t</text>\n");
			printf("</svg>\n");
			printf("\n");
			printf("You could choose which path to display by changing the xlink:href attribute of\n");
			printf("the use element. This element has the id \"link\" so you can access it like this:\n");
			printf("--elementattr \"link:xlink:href=#path2\"\n");
			printf("\n");
			printf("If you wanted to change the text displayed you could change both strings at the\n");
			printf("same time using the --classtext command (as both are of the class \"text\") or\n");
			printf("you could address them individually by using --elementtext:\n");
			printf("--classtext \"text:hello\"\n");
			printf("--elementtext \"two:world\"\n");
			printf("\n");
			printf("You can also add attributes (opacity for example) by using --elementattr. If\n");
			printf("an attribute does not yet exist it will be aded automatically. To change\n");
			printf("multiple attributes or texts at once you can provide more than one argument.\n");
			exit(0);
		} else if (arg == "--x"
				|| arg == "-x") {
			float x;
			if (sscanf(argv[i + 1], "%f", &x) == 1) {
				fX = x;
				flags |= SET_X_LOCATION;
				i++;
			}
		} else if (arg == "--y"
				|| arg == "-y") {
			float y;
			if (sscanf(argv[i + 1], "%f", &y) == 1) {
				fY = y;
				flags |= SET_Y_LOCATION;
				i++;
			}
		} else if (arg == "--width"
				|| arg == "-w") {
			float width;
			if (sscanf(argv[i + 1], "%f", &width) == 1) {
				fWidth = width;
				flags |= SET_WIDTH;
				i++;
			}
		} else if (arg == "--height"
				|| arg == "-h") {
			float height;
			if (sscanf(argv[i + 1], "%f", &height) == 1) {
				fHeight = height;
				flags |= SET_HEIGHT;
				i++;
			}
		} else if (arg == "--samplesize"
				|| arg == "-ss") {
			int samplesize;
			if (sscanf(argv[i + 1], "%d", &samplesize) == 1) {
				fSampleSize = samplesize;
				flags |= SET_SAMPLESIZE;
				i++;
			}
		} else if (arg == "--fitcontent"
				|| arg == "-fc") {
			bool fitcontent;
			if (sscanf(argv[i + 1], "%d", (int *)&fitcontent) == 1) {
				fFitContent = fitcontent;
				flags |= SET_FITCONTENT;
				i++;
			}
		} else if (arg == "--scaletofit"
				|| arg == "-sf") {
			bool scaletofit = 1;
			if (sscanf(argv[i + 1], "%d", (int *)&scaletofit) == 1) {
				fScaleToFit = scaletofit;
				flags |= SET_SCALETOFIT;
				i++;
			}
		} else if (arg == "--background"
				|| arg == "-bg") {
			rgb_color bg;
			if (sscanf(argv[i + 1], "%u,%u,%u,%u", &bg.red, &bg.green, &bg.blue, &bg.alpha) == 4) {
				fBackground = bg;
				flags |= SET_BACKGROUND;
				i++;
			} else if (sscanf(argv[i + 1], "%u,%u,%u", &bg.red, &bg.green, &bg.blue) == 3) {
				bg.alpha = 255;
				fBackground = bg;
				flags |= SET_BACKGROUND;
				i++;
			}
		} else if (arg == "--pulse") {
			bigtime_t rate;
			if (sscanf(argv[i + 1], "%lld", &rate) == 1) {
				fPulseRate = rate;
				flags |= SET_PULSE;
				i++;
			}
		} else if (arg == "--transparent") {
			fTransparent = true;
			flags |= SET_TRANSPARENT;
		
		} else if (arg.ICompare("--id=", 5) == 0
				|| arg.ICompare("-i=", 3) == 0) {
			fID = argv[i];
			fID.RemoveFirst("--id=");
			fID.RemoveFirst("-i=");
		
		} else if (arg == "--id"
				|| arg == "-i") {
			send_to = argv[++i];
		
		} else if (arg == "--quit") {
			flags |= SET_QUIT;
		
		} else if (arg == "--classattr"
				|| arg == "-ca") {
			BString *attr = new BString(argv[++i]);
			attr->Truncate(sizeof(buffer) - 1);
			classattrs.AddItem((void *)attr);
			flags |= SET_CLASS_ATTR;
		
		} else if (arg == "--elementattr"
				|| arg == "-ea") {
			BString *attr = new BString(argv[++i]);
			attr->Truncate(sizeof(buffer) - 1);
			elementattrs.AddItem((void *)attr);
			flags |= SET_ELEMENT_ATTR;
		
		} else if (arg == "--classtext"
				|| arg == "-ct") {
			BString *attr = new BString(argv[++i]);
			attr->Truncate(sizeof(buffer) - 1);
			classtexts.AddItem((void *)attr);
			flags |= SET_CLASS_TEXT;
		
		} else if (arg == "--elementtext"
				|| arg == "-et") {
			BString *attr = new BString(argv[++i]);
			attr->Truncate(sizeof(buffer) - 1);
			elementtexts.AddItem((void *)attr);
			flags |= SET_ELEMENT_TEXT;
		
		} else
			printf("unknown argument: \"%s\"\n", argv[i]);
	}
	
	// we are configuring an existing window / replicant
	if (send_to.Length() > 0) {
		send_to.Prepend("svgview:");
		send_to.Truncate(B_OS_NAME_LENGTH - 1);
		port_id port = find_port(send_to.String());
		if (port < 0) {
			send_to.Prepend("rep:");
			send_to.Truncate(B_OS_NAME_LENGTH - 1);
			port = find_port(send_to.String());
		}
		
		if (port >= 0) {
			write_port(port, SET_START_UPDATE, NULL, 0);
			
			if (flags & SET_WIDTH)
				write_port(port, SET_WIDTH, &fWidth, sizeof(fWidth));
			if (flags & SET_HEIGHT)
				write_port(port, SET_HEIGHT, &fHeight, sizeof(fHeight));
			if (flags & SET_SAMPLESIZE)
				write_port(port, SET_SAMPLESIZE, &fSampleSize, sizeof(fSampleSize));
			if (flags & SET_FITCONTENT)
				write_port(port, SET_FITCONTENT, &fFitContent, sizeof(fFitContent));
			if (flags & SET_SCALETOFIT)
				write_port(port, SET_SCALETOFIT, &fScaleToFit, sizeof(fScaleToFit));
			if (flags & SET_BACKGROUND)
				write_port(port, SET_BACKGROUND, &fBackground, sizeof(fBackground));
			if (flags & SET_PULSE)
				write_port(port, SET_PULSE, &fPulseRate, sizeof(fPulseRate));
			if (flags & SET_TRANSPARENT)
				write_port(port, SET_TRANSPARENT, &fTransparent, sizeof(fTransparent));
			if (flags & SET_X_LOCATION)
				write_port(port, SET_X_LOCATION, &fX, sizeof(fX));
			if (flags & SET_Y_LOCATION)
				write_port(port, SET_Y_LOCATION, &fY, sizeof(fY));
			if (flags & SET_CLASS_ATTR) {
				while (classattrs.CountItems() > 0) {
					BString *attr = (BString *)classattrs.RemoveItem((int32)0);
					strcpy(buffer, attr->String());
					write_port(port, SET_CLASS_ATTR, buffer, sizeof(buffer));
					delete attr;
				}
			}
			if (flags & SET_ELEMENT_ATTR) {
				while (elementattrs.CountItems() > 0) {
					BString *attr = (BString *)elementattrs.RemoveItem((int32)0);
					strcpy(buffer, attr->String());
					write_port(port, SET_ELEMENT_ATTR, buffer, sizeof(buffer));
					delete attr;
				}
			}
			if (flags & SET_CLASS_TEXT) {
				while (classtexts.CountItems() > 0) {
					BString *attr = (BString *)classtexts.RemoveItem((int32)0);
					strcpy(buffer, attr->String());
					write_port(port, SET_CLASS_TEXT, buffer, sizeof(buffer));
					delete attr;
				}
			}
			if (flags & SET_ELEMENT_TEXT) {
				while (elementtexts.CountItems() > 0) {
					BString *attr = (BString *)elementtexts.RemoveItem((int32)0);
					strcpy(buffer, attr->String());
					write_port(port, SET_ELEMENT_TEXT, buffer, sizeof(buffer));
					delete attr;
				}
			}
			if (flags & SET_FILE)
				write_port(port, SET_FILE, filename, sizeof(filename));
			if (flags & SET_QUIT)
				write_port(port, SET_QUIT, NULL, 0);
			
			write_port(port, SET_END_UPDATE, NULL, 0);
		} else {
			printf("id not found.\n");
			exit(-1);
		}
		
		Hide();
		PostMessage(B_QUIT_REQUESTED);
		return;
	}
	
	MoveTo(fX, fY);
	ResizeTo(fWidth, fHeight);
	
	fView = new SVGView(BRect(0, 0, fWidth, fHeight));
	
	if (fID.Length() > 0)
		fView->SetID(fID.String());
	
	// set the basic settings of the BSVGView
	if (!fTransparent) {
		fView->SetViewColor(fBackground);
		fView->SetPulseRate(fPulseRate);
	} else {
		fView->SetViewColor(0x77, 0x74, 0x77, 0x00); // B_TRANSPARENT_MAGIC_RGBA32
		fView->SetPulseRate(fPulseRate > 0 ? fPulseRate : 10 * 1000 * 1000); // 10 sec
	}
	fView->SetSampleSize(fSampleSize);
	fView->SetScaleToFit(fScaleToFit);
	fView->SetFitContent(fFitContent);
	
	// load the file if we got one as argument
	if (flags & SET_FILE) {
		if (fView->LoadFromFile(filename) == B_OK)
			ResetTitle(filename);
	}
	
	// add the dragger to make it a possible replcant
	rect.OffsetTo(0, 0);
	rect.Set(rect.right - 7, rect.bottom - 7, rect.right, rect.bottom);
	rect.OffsetTo(rect.right - 7, rect.bottom - 7);
	fDragger = new BDragger(rect, fView, B_FOLLOW_RIGHT | B_FOLLOW_BOTTOM);
	fView->AddChild(fDragger);

	AddChild(fView);
}

void
SVGWin::ResetTitle(const char *filename)
{
	if (!filename)
		return;
	
	BString name = filename;
	int index = name.FindLast("/");
	if (index >= 0)
		name.Remove(0, index + 1);
	
	BString title = "SVGWindow -> ";
	title << name;
	
	SetTitle(title.String());
}

// files that are droped over the window are handeled here
void
SVGWin::MessageReceived(BMessage *message)
{
	switch (message->what) {
		/*case B_SIMPLE_DATA:	{
			entry_ref ref;
			if (message->FindRef("refs", &ref) != B_OK)
				return;
			
			fView->Clear();
			fView->LoadFromFile(&ref);
			ResetTitle(ref.name);
		} break;*/
		
		default: BWindow::MessageReceived(message);
	}
}

SVGView *
SVGWin::View()
{
	return fView;
}

SVGView::SVGView(BRect frame)
	:	BSVGView(frame, "SVGView", B_FOLLOW_ALL),
		fPort(-1),
		fID(""),
		fReplicant(false),
		fWatchThread(-1)
{
}

SVGView::~SVGView()
{
	if (fPort >= 0) {
		delete_port(fPort);
		fPort = -1;
	}
	
	if (fWatchThread >= 0)
		kill_thread(fWatchThread);
}

SVGView::SVGView(BMessage *data)
	:	BSVGView(data),
		fPort(-1),
		fID(""),
		fReplicant(true),
		fWatchThread(-1)
{
	BString id;
	if (data->FindString("svgview:id", &id) == B_OK && id.Length() > 0)
		SetID(id.String());
}

BArchivable *
SVGView::Instantiate(BMessage *data)
{
	if (validate_instantiation(data, "SVGView"))
		return new SVGView(data);
	return NULL;
}

status_t
SVGView::Archive(BMessage *data, bool deep) const
{
	BSVGView::Archive(data, deep);
	
	data->AddString("svgview:id", fID);
	
	ADDREPCLASS("SVGView");
	ADDREPADDON("application/x-vnd.mmlr.bsvgviewer");
	
	return B_OK;
}

void
SVGView::SetID(const char *_id)
{
	if (!_id)
		return;
	
	fID = _id;
	BString id = fID;
	id.Prepend("svgview:");
	
	if (fReplicant)
		id.Prepend("rep:");
	
	id.Truncate(B_OS_NAME_LENGTH - 1);
	fPort = create_port(B_PATH_NAME_LENGTH + B_FILE_NAME_LENGTH + 1, id.String());
	
	// spawn monitoring thread
	if (fPort) {
		fWatchThread = spawn_thread(WatchThreadFunc, "WatchThread", B_LOW_PRIORITY, this);
		resume_thread(fWatchThread);
	}
}

const char *
SVGView::ID()
{
	return fID.String();
}

int32
SVGView::WatchThreadFunc(void *arg)
{
	SVGView *view = (SVGView *)arg;
	if (view->WatchThread() == SET_QUIT)
		delete view;
	return B_OK;
}

int32
SVGView::WatchThread()
{
	while (fPort >= 0) {
		ssize_t size = port_buffer_size(fPort);
		
		int32 code;
		char buffer[size];
		
		ssize_t result = read_port(fPort, &code, buffer, size);
		if (result < 0)
			break;
		
		switch (code) {
			case SET_WIDTH: {
				float value = *(float *)buffer;
				if (fReplicant) {
					ResizeTo(value, Bounds().Height());
				} else {
					if (Window() && Window()->Lock()) {
						Window()->ResizeTo(value, Bounds().Height());
						Window()->Unlock();
					}
				}
			} break;
			case SET_HEIGHT: {
				float value = *(float *)buffer;
				if (fReplicant) {
					ResizeTo(Bounds().Width(), value);
				} else {
					if (Window() && Window()->Lock()) {
						Window()->ResizeTo(Bounds().Width(), value);
						Window()->Unlock();
					}
				}
			} break;
			case SET_X_LOCATION: {
				float value = *(float *)buffer;
				if (fReplicant) {
					MoveTo(value, Frame().top);
				} else {
					if (Window() && Window()->Lock()) {
						Window()->MoveTo(value, Window()->Frame().top);
						Window()->Unlock();
					}
				}
			} break;
			case SET_Y_LOCATION: {
				float value = *(float *)buffer;
				if (fReplicant) {
					MoveTo(Frame().left, value);
				} else {
					if (Window() && Window()->Lock()) {
						Window()->MoveTo(Window()->Frame().left, value);
						Window()->Unlock();
					}
				}
			} break;
			case SET_QUIT: {
				if (fReplicant) {
					UnlockLooper();
					BWindow *window = Window();
					if (window) {
						window->Lock();
						RemoveSelf();
						window->Unlock();
						fWatchThread = 0;
						return SET_QUIT;
					}
				} else {
					UnlockLooper();
					if (Window())
						Window()->PostMessage(B_QUIT_REQUESTED);
				}
			} break;
			case SET_SAMPLESIZE: {
				int value = *(int *)buffer;
				SetSampleSize(value);
			} break;
			case SET_FITCONTENT: {
				bool value = *(bool *)buffer;
				SetFitContent(value);
			} break;
			case SET_SCALETOFIT: {
				bool value = *(bool *)buffer;
				SetScaleToFit(value);
			} break;
			case SET_BACKGROUND: {
				rgb_color value = *(rgb_color *)buffer;
				SetViewColor(value);
			} break;
			case SET_PULSE: {
				bigtime_t value = *(bigtime_t *)buffer;
				SetPulseRate(value);
			} break;
			case SET_TRANSPARENT: {
				bool value = *(bool *)buffer;
				if (value) {
					SetViewColor(0x77, 0x74, 0x77, 0x00);
					bigtime_t rate = PulseRate();
					SetPulseRate(rate > 0 ? rate : 10 * 1000 * 1000); // 10 sec
				}
			} break;
			case SET_START_UPDATE: {
				LockLooper();
				LockUpdates();
			} break;
			case SET_END_UPDATE: {
				UnlockUpdates(true);
				UnlockLooper();
			} break;
			case SET_FILE: {
				Clear();
				if (LoadFromFile(buffer) == B_OK && !fReplicant && Window()) {
					SVGWin *win = (SVGWin *)Window();
					if (win)
						win->ResetTitle(buffer);
				}
			} break;
			case SET_CLASS_ATTR:
			case SET_ELEMENT_ATTR: {
				BString source = buffer;
				BString target = "";
				BString attr = "";
				BString value = "";
				
				int32 pos = source.FindFirst(":");
				if (pos < 0) break;
				source.MoveInto(target, 0, pos);
				pos = source.FindFirst("=");
				if (pos < 0) break;
				source.MoveInto(attr, 1, pos - 1);
				source.MoveInto(value, 2, source.Length() - 2);
				
				if (code == SET_CLASS_ATTR)
					SetAttributeByClass(target, attr, value);
				else
					SetAttributeByID(target, attr, value);
			} break;
			case SET_CLASS_TEXT:
			case SET_ELEMENT_TEXT: {
				BString source = buffer;
				BString target = "";
				BString text = "";
				
				int32 pos = source.FindFirst(":");
				if (pos < 0) break;
				source.MoveInto(target, 0, pos);
				source.MoveInto(text, 1, source.Length() - 1);
				
				if (code == SET_CLASS_TEXT)
					SetTextByClass(target, text);
				else
					SetTextByID(target, text);
			} break;
			default: break;
		}
	}
	
	fWatchThread = 0;
	return B_OK;
}
