#include <stdio.h>

#include <Application.h>
#include <Beep.h>
#include <Bitmap.h>
#include <Directory.h>
#include <Entry.h>
#include <Font.h>
#include <Path.h>
#include <StopWatch.h>
#include <String.h>
#include <View.h>
#include <Window.h>

#include "SVGDefs.h"
#include "SVGView.h"

class SVGWin;
class SVGView;

class SVGApp : public BApplication
{
	public:
					SVGApp(int argc, char *argv[]);
		
		void		RefsReceived(BMessage *message);
		
	private:
			SVGWin	*fWin;
};

class SVGWin : public BWindow
{
	public:
					SVGWin(BRect rect, int argc, char *argv[]);
		void		MessageReceived(BMessage *message);
		SVGView		*GetView() { return fView; };
		BBitmap		*Offscreen() { return fOffscreenBitmap; };
		
		void		RenderDirectory(entry_ref &ref);
	
	private:
		SVGView			*fView;
		BSVGView		*fSVGView;
		float			fWidth;
		float			fHeight;
		int				fCountPerRow;
		int				fCountPerColumn;
		int				fSampleSize;
		rgb_color		fBackground;
		bool			fVerbose;
		BBitmap			*fOffscreenBitmap;
};

class SVGView : public BView
{
	public:
					SVGView(BRect rect, const char *title, uint32 res, SVGWin *parent)
						:	BView(rect, title, res, B_WILL_DRAW) { fParent = parent; };
		void		Draw(BRect update) { DrawBitmap(fParent->Offscreen()); };
	private:
		SVGWin		*fParent;
};

int main(int argc, char **argv)
{
	SVGApp *app = new SVGApp(argc, argv);
	app->Run();
	delete app;
}

SVGApp::SVGApp(int argc, char *argv[])
	:	BApplication("application/x-vnd.mmlr.bsvgfolderviewer")
{
	fWin = new SVGWin(BRect(100, 100, 275, 225), argc, argv);
	fWin->Show();
}

void
SVGApp::RefsReceived(BMessage *message)
{
	entry_ref ref;
	
	if (message->FindRef("refs", &ref) == B_OK) {
		fWin->Lock();
		fWin->RenderDirectory(ref);
		fWin->Unlock();
	}
}

SVGWin::SVGWin(BRect rect, int argc, char *argv[])
	:	BWindow(rect, "SVGWindow", B_TITLED_WINDOW, B_QUIT_ON_WINDOW_CLOSE),
		fSVGView(NULL),
		fWidth(64),
		fHeight(64),
		fCountPerRow(13),
		fCountPerColumn(13),
		fSampleSize(2),
		fVerbose(false),
		fOffscreenBitmap(NULL)
{
	fBackground.red = 50;
	fBackground.green = 100;
	fBackground.blue = 150;
	fBackground.alpha = 255;
	
	for (int i = 1; i < argc; i++) {
		BString arg = argv[i];
		
		if (arg == "--help") {
			printf("usage: %s [<folder>] [--width=x] [--height=x] [--countperrow=x] [--countpercolumn=x] [--samplesize=x] [--bg=r,g,b]\n", argv[0]);
			exit(0);
		} else if (arg.ICompare("--width=", 7) == 0) {
			float width = 128;
			if (sscanf(argv[i], "--width=%f", &width) == 1)
				fWidth = width;
		} else if (arg.ICompare("--height=", 8) == 0) {
			float height = 128;
			if (sscanf(argv[i], "--height=%f", &height) == 1)
				fHeight = height;
		} else if (arg.ICompare("--countperrow=", 14) == 0) {
			int countperrow = 13;
			if (sscanf(argv[i], "--countperrow=%d", &countperrow) == 1)
				fCountPerRow = countperrow;
		} else if (arg.ICompare("--countpercolumn=", 17) == 0) {
			int countpercolumn = 13;
			if (sscanf(argv[i], "--countpercolumn=%d", &countpercolumn) == 1)
				fCountPerColumn = countpercolumn;
		} else if (arg.ICompare("--samplesize=", 13) == 0) {
			int samplesize = 2;
			if (sscanf(argv[i], "--samplesize=%d", &samplesize) == 1)
				fSampleSize = samplesize;
		} else if (arg.ICompare("--bg=", 4) == 0) {
			rgb_color bg;
			if (sscanf(argv[i], "--bg=%u,%u,%u,%u", &bg.red, &bg.green, &bg.blue, &bg.alpha) == 4)
				fBackground = bg;
		} else if (arg.ICompare("--verbose", 9) == 0) {
			fVerbose = true;
		} else if (i != 1)
			printf("unknown argument: \"%s\"\n", argv[i]);
	}
	
	SetPulseRate(50);
	rect.OffsetToSelf(0, 0);
	
	fView = new SVGView(rect, "main_view", B_FOLLOW_ALL, this);
	fView->SetViewColor(fBackground);
	
	BEntry dir((argc > 1 ? argv[1] : NULL));
	entry_ref ref;
	
	if (dir.GetRef(&ref) == B_OK)
		RenderDirectory(ref);
	else {
		fOffscreenBitmap = new BBitmap(BRect(0, 0, 0, 0), B_RGBA32);
		((uint32 *)fOffscreenBitmap->Bits())[0] = (0 << 24) + (50 << 16) + (100 << 8) + 150;
	}
	
	AddChild(fView);
}

void
SVGWin::MessageReceived(BMessage *message)
{
	switch (message->what) {
		case B_SIMPLE_DATA:	{
			entry_ref ref;
			if (message->FindRef("refs", &ref) != B_OK)
				return;
			
			RenderDirectory(ref);
		}
		default: BWindow::MessageReceived(message);
	}
}

void
SVGWin::RenderDirectory(entry_ref &ref)
{
	int countx = fCountPerRow;
	int county = fCountPerColumn;
	float stepx = fWidth;
	float stepy = fHeight;
	float totalx = stepx * countx;
	float totaly = stepy * county;
	rgb_color bg = fBackground;
	
	BEntry entry;
	BDirectory dir(&ref);
	dir.Rewind();
	BPath path;
	bool loaded = false;
	BRect bounds(0, 0, stepx, stepy);
	
	delete fOffscreenBitmap;
	fOffscreenBitmap = new BBitmap(BRect(0, 0, totalx, totaly), B_RGBA32, true);
	
	uint32 backcolor = (bg.alpha << 24) + (bg.red << 16) + (bg.green << 8) + bg.blue;
	uint32 *bits = (uint32 *)fOffscreenBitmap->Bits();
	int length = fOffscreenBitmap->BitsLength() / 4;
	fast_memset(length, backcolor, bits);
	
	BStopWatch *allinall = new BStopWatch("allinallwatch");
	fSVGView = new BSVGView(bounds, "SVGView", B_FOLLOW_ALL);
	fSVGView->SetScaleToFit(true);
	fSVGView->SetFitContent(true);
	fSVGView->SetViewColor(fBackground);
	fSVGView->SetSampleSize(fSampleSize);
	
	fOffscreenBitmap->AddChild(fSVGView);
	fOffscreenBitmap->Lock();
	
	bigtime_t time = 0;
	
	for (int j = 0; j < county; j++) {
		for (int i = 0; i < countx; i++) {
			loaded = false;
			while (!loaded && dir.GetNextEntry(&entry) == B_OK) {
				path.SetTo(&entry);
				
				fSVGView->MoveTo(i * stepx, j * stepy);
				fSVGView->Clear();
				
				if (fSVGView->LoadFromFile(path.Path()) != B_OK) {
					printf("could not load: %s\n", path.Path());
					continue;
				} else {
					if (fVerbose)
						printf("loaded: %s\n", path.Path());
				}
				
				loaded = true;
				
				bigtime_t start = system_time();
				fSVGView->Draw(bounds);
				bigtime_t stop = system_time();
				time += stop - start;
			}
		}
	}
	
	fOffscreenBitmap->Unlock();
	fOffscreenBitmap->RemoveChild(fSVGView);
	delete fSVGView;
	delete allinall;
	printf("drawing time: %lld usecs\n", time);
	
	fView->Invalidate();
}
