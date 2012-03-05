#include "EditorWindow.h"
#include "OffscreenView.h"
#include "SVGStop.h"
#include "SVGContainers.h"

EditorWindow::EditorWindow(BRect frame, const char *title)
	:	BWindow(frame, title, B_TITLED_WINDOW, 0),
		fFilePanel(NULL),
		fTool(NULL)
{
	BRect control(0, 0, frame.Width(), 20);
	fMenu = new BMenuBar(control, "fMenu");
	fMenu->ResizeToPreferred();
	
	BEGINMENUS;
	
	// build file menu
	SETMENU(fFileMenu, "File", fMenu);
	ADDITEM1("Open", M_OPEN_FILE, 'O');
	ADDITEM0("Open Background", M_OPEN_BACKGROUND);
	ADDITEM0("Clear Background", M_CLEAR_BACKGROUND);
	ADDITEM1("Export", M_EXPORT_FILE, 'E');
	ADDITEM1("Quit", B_QUIT_REQUESTED, 'W');
	
	// build path menu
	SETMENU(fElementMenu, "Element", fMenu);
	ADDITEM1("Create new Element", M_CREATE_ELEMENT, 'N');
	ADDITEM1("Delete Element", M_DELETE_ELEMENT, 'D');
	ADDITEM0("Clear Elements", M_CLEAR_ELEMENTS);
	
	// build the tool menu
	SETMENU(fToolMenu, "Tools", fMenu);
	ADDITEM1("Move", M_TOOL_MOVE_TO, 'M');
	ADDITEM1("Line", M_TOOL_LINE_TO, 'L');
	ADDITEM1("Horizontal Line", M_TOOL_H_LINE_TO, 'H');
	ADDITEM1("Vertical Line", M_TOOL_V_LINE_TO, 'V');
	ADDITEM1("Curve", M_TOOL_CURVE_TO, 'C');
	ADDITEM1("Shorthand Curve", M_TOOL_SHORT_CURVE_TO, 'S');
	ADDITEM1("Quadric Bezier", M_TOOL_QUAD_BEZIER_TO, 'Q');
	ADDITEM1("Shorthand Bezier", M_TOOL_SHORT_BEZIER_TO, 'T');
	ADDITEM1("Elliptical Arc", M_TOOL_ARC_TO, 'A');
	ADDITEM1("Close Path", M_TOOL_CLOSE, 'Z');
	
	AddChild(fMenu);
	float menuheight = fMenu->Bounds().Height() + 1;
	
	int left_width = 300;
	int elements_height = 420;
	
	frame.OffsetTo(0, 0);
	control.Set(left_width--, menuheight, frame.Width(), frame.Height());
	fTabView = new EditorTabView(control, "fTabView");
	
	control.OffsetTo(1, 0);
	control.bottom -= fTabView->TabHeight();
	fView = new EditorView(control, "fView", B_FOLLOW_ALL, this);
	fView->SetSampleSize(1);
	fView->SetViewColor(255, 255, 255, 255);
	
	fOffscreenView = new OffscreenView(fView);
	fTabView->AddTab(fOffscreenView);
	fTabView->TabAt(M_DRAW_TAB)->SetLabel("Draw");
	
	fSourceView = new EditorTextView(control, "fSourceView", B_FOLLOW_ALL, this);
	fSourceView->SetText("");
	
	fTabView->AddTab(fSourceView);
	fTabView->TabAt(M_SOURCE_TAB)->SetLabel("Source");
	
	fPreView = new EditorPreView(control, "fPreView", B_FOLLOW_ALL, this);
	fPreView->SetSampleSize(2);
	fPreView->SetScaleToFit(false);
	fPreView->SetFitContent(false);
	fPreView->SetViewColor(255, 255, 255, 255);
	
	fTabView->AddTab(fPreView);
	fTabView->TabAt(M_PREVIEW_TAB)->SetLabel("Preview");
	AddChild(fTabView);
	
	BBox *box = new BBox(BRect(-1, menuheight - 1, left_width + 1, menuheight + elements_height), "box", B_FOLLOW_TOP_BOTTOM);
	BRect boxframe = box->Frame().OffsetToSelf(0, 0);
	boxframe.InsetBy(2, 2);
	boxframe.bottom = elements_height;
	fElements = new ElementView(boxframe, "fElements");
	fElements->SetSelectionMessage(new BMessage(M_ELEMENT_SELECTION));
	box->AddChild(fElements);
	
	AddChild(box);
	
	control.Set(0, elements_height + menuheight, left_width, frame.Height());
	fSettings = new SettingsView(control, "fSettings", B_FOLLOW_BOTTOM, B_WILL_DRAW | B_FRAME_EVENTS);
	fSettings->SetViewColor(ui_color(B_PANEL_BACKGROUND_COLOR));
	AddChild(fSettings);
	
	fTool = new ToolMoveTo(this);
}

EditorWindow::~EditorWindow()
{
	((EditorApp *)be_app)->RemoveWindow(this);
}

BSVGElement *
EditorWindow::CurrentElement()
{
	int selection = fElements->FullListCurrentSelection();
	if (selection >= 0)
		return fView->ElementAt(selection - 1, true);
	
	return NULL;
}

BSVGPath *
EditorWindow::CurrentPath()
{
	
	BSVGElement *element = CurrentElement();
	if (!element || element->Type() != B_SVG_PATH)
		return NULL;
	
	return (BSVGPath *)element;
}

BSVGGradient *
EditorWindow::CurrentGradient()
{
	BSVGElement *element = CurrentElement();
	if (!element
		|| (element->Type() != B_SVG_LINEARGRAD && element->Type() != B_SVG_RADIALGRAD))
		return NULL;
	
	return (BSVGGradient *)element;
}

BSVGStop *
EditorWindow::CurrentStop()
{
	BSVGElement *element = CurrentElement();
	if (!element || element->Type() != B_SVG_STOP)
		return NULL;
	
	return (BSVGStop *)element;
}

bool
EditorWindow::MouseDown(BPoint where, uint32 buttons)
{
	return fTool->MouseDown(where, buttons);
}

bool
EditorWindow::KeyDown(const char *bytes, int32 numBytes)
{
	fRelative = false;
	switch (bytes[0]) {
		case 'm': fRelative = true; // fall
		case 'M': fTool = new ToolMoveTo(this); break;
		case 'l': fRelative = true;
		case 'L': fTool = new ToolLineTo(this); break;
		case 'h': fRelative = true;
		case 'H': fTool = new ToolHLineTo(this); break;
		case 'v': fRelative = true;
		case 'V': fTool = new ToolVLineTo(this); break;
		case 'c': fRelative = true;
		case 'C': fTool = new ToolCurveTo(this); break;
		case 's': fRelative = true;
		case 'S': fTool = new ToolShortCurveTo(this); break;
		case 'q': fRelative = true;
		case 'Q': fTool = new ToolQuadBezierTo(this); break;
		case 't': fRelative = true;
		case 'T': fTool = new ToolShortBezierTo(this); break;
		case 'a': fRelative = true;
		case 'A': fTool = new ToolEllipticalArcTo(this); break;
		case 'z': fRelative = true;
		case 'Z': fTool = new ToolClose(this); break;
		
		case '+': {
			fView->SetScale(fView->Scale() * 1.1);
			fView->Invalidate();
			fPreView->Rebuild();
		} break;
		case '-': {
			fView->SetScale(fView->Scale() * 0.9);
			fView->Invalidate();
			fPreView->Rebuild();
		} break;
		case B_DELETE: DeleteElement(); break;
		
		default: return false;
	}
	
	return true;
}

void
EditorWindow::MessageReceived(BMessage *message)
{
	BaseTool *oldtool = fTool;
	
	switch (message->what) {
		case B_SIMPLE_DATA: ((EditorApp *)be_app)->MessageReceived(message); break;
		case B_KEY_DOWN: {
			if (fTabView->Selection() != M_DRAW_TAB) {
				BWindow::MessageReceived(message);
				break;
			}
			
			BString string;
			if (message->FindString("bytes", &string) != B_OK)
				break;
			
			KeyDown(string.String(), 1);
		} break;
		
		// menu messages
		case M_OPEN_FILE: OpenFile(); break;
		case M_OPEN_BACKGROUND: OpenBackground(); break;
		case M_CLEAR_BACKGROUND: ClearBackground(); break;
		case M_EXPORT_FILE: ExportFile(); break;
		
		// element messages
		case M_CREATE_ELEMENT: CreateElement(); break;
		case M_DELETE_ELEMENT: DeleteElement(); break;
		case M_CLEAR_ELEMENTS: ClearElements(); break;
		
		// element view messages
		case M_ELEMENT_SELECTION: {
			if (fTabView->Selection() != M_DRAW_TAB)
				break;
			
			fSettings->SetTo(CurrentElement());
			fView->Invalidate();
		} break;
		
		// tool handling
		case M_TOOL_MOVE_TO:		fTool = new ToolMoveTo(this); break;
		case M_TOOL_LINE_TO:		fTool = new ToolLineTo(this); break;
		case M_TOOL_H_LINE_TO:		fTool = new ToolHLineTo(this); break;
		case M_TOOL_V_LINE_TO:		fTool = new ToolVLineTo(this); break;
		case M_TOOL_CURVE_TO:		fTool = new ToolCurveTo(this); break;
		case M_TOOL_SHORT_CURVE_TO:	fTool = new ToolShortCurveTo(this); break;
		case M_TOOL_QUAD_BEZIER_TO:	fTool = new ToolQuadBezierTo(this); break;
		case M_TOOL_SHORT_BEZIER_TO:fTool = new ToolShortBezierTo(this); break;
		case M_TOOL_ARC_TO:			fTool = new ToolEllipticalArcTo(this); break;
		case M_TOOL_CLOSE:			fTool = new ToolClose(this); break;
		
		// send message to the current tool
		default: if (fTool
					&& fTool->MessageReceived(message))	// if not handled by the tool
					BWindow::MessageReceived(message);	// let the system handle it
	}
	
	if (fTool != oldtool)
		delete oldtool;
}

void
EditorWindow::CreateElement(element_t type)
{
	switch (type) {
		case B_SVG_ELEMENT:
		case B_SVG_DOCUMENT:
		case B_SVG_GROUP:
		case B_SVG_DEFS:
		case B_SVG_PATTERN:
		case B_SVG_USE:
		case B_SVG_RECT:
		case B_SVG_CIRCLE:
		case B_SVG_ELLIPSE:
		case B_SVG_LINE:
		case B_SVG_POLYLINE:
		case B_SVG_POLYGON:
		case B_SVG_TEXT:
		case B_SVG_TEXTSPAN:
		case B_SVG_TEXTPATH:
		case B_SVG_STRING:
		case B_SVG_GRADIENT:
		case B_SVG_LINEARGRAD:
		case B_SVG_RADIALGRAD:
		case B_SVG_STOP:
		case B_SVG_PAINTSERVER: break;
		
		case B_SVG_ROOT: fView->AddElement(new BSVGRoot(fView)); break;
		case B_SVG_PATH: fView->AddElement(new BSVGPath(fView)); break;
	}
		
	RefreshElements();
	fElements->Select(fElements->CountItems() - 1);
}

void
EditorWindow::DeleteElement()
{
	int32 selection = fElements->FullListCurrentSelection();
	if (selection == 0 || selection == 1) {
		// we're trying to remove the document or root; just clear instead
		ClearElements();
		CreateElement(B_SVG_ROOT);
		return;
	}
	
	fView->RemoveElementAt(selection - 1);
	RefreshElements();
	fElements->Select(MIN(selection, fElements->CountItems() - 1));
	fView->Invalidate();
}

void
EditorWindow::ClearElements()
{
	fView->ClearElements();
	fView->Invalidate();
	RefreshElements();
}

void
EditorWindow::AddRecursive(BSVGElement *element, BListItem *under)
{
	if (!element)
		return;
	
	BString string;
	string.SetTo("");
	
	switch (element->Type()) {
		case B_SVG_ELEMENT: string << "Untyped"; break;
		case B_SVG_DOCUMENT: string << "Document"; break;
		case B_SVG_ROOT: string << "Root"; break;
		case B_SVG_GROUP: string << "Group"; break;
		case B_SVG_DEFS: string << "Defs"; break;
		case B_SVG_PATTERN: string << "Pattern"; break;
		case B_SVG_USE: string << "Use"; break;
		case B_SVG_PATH: string << "Path"; break;
		case B_SVG_RECT: string << "Rect"; break;
		case B_SVG_CIRCLE: string << "Circle"; break;
		case B_SVG_ELLIPSE: string << "Ellipse"; break;
		case B_SVG_LINE: string << "Line"; break;
		case B_SVG_POLYLINE: string << "Polyline"; break;
		case B_SVG_POLYGON: string << "Polygon"; break;
		case B_SVG_TEXT: string << "Text"; break;
		case B_SVG_TEXTSPAN: string << "TextSpan"; break;
		case B_SVG_TEXTPATH: string << "TextPath"; break;
		case B_SVG_STRING: string << "String"; break;
		case B_SVG_GRADIENT: string << "Gradient"; break;
		case B_SVG_LINEARGRAD: string << "LinearGradient"; break;
		case B_SVG_RADIALGRAD: string << "RadialGradient"; break;
		case B_SVG_STOP: string << "Stop"; break;
		case B_SVG_PAINTSERVER: string << "PaintServer"; break;
	}
	
	BStringItem *item = new BStringItem(string.String());
	if (!under)
		fElements->AddItem(item);
	else
		fElements->AddUnder(item, under);
	
	int32 count = element->CountElements(false);
	for (int i = 0; i < count; i++) {
		BSVGElement *elem = element->ElementAt(count - i - 1, false);
		
		if (!elem)
			continue;
		
		AddRecursive(elem, item);
	}
}

void
EditorWindow::RefreshElements()
{
	BString string;
	BListItem *item = NULL;
	
	int32 selection = fElements->FullListCurrentSelection();
	
	while ((item = fElements->RemoveItem((long int)0)) != NULL)
		delete item;
	
	BSVGElement *root = fView->ElementAt(-1, true);
	AddRecursive(root, NULL);
	
	fElements->Select(MIN(selection, fElements->CountItems()));
}

/*void
EditorWindow::CreateGradient()
{
	BSVGLinearGradient *gradient = new BSVGLinearGradient(fView);
	gradient->SetVector(BPoint(10, 10), BPoint(50, 10));
	gradient->SetName("New Gradient");
	
	BSVGStop *stop = new BSVGStop();
	stop->SetOffset(0);
	stop->SetStopColor(0, 0, 0);
	gradient->AddStop(stop);
	stop = new BSVGStop();
	stop->SetOffset(1);
	stop->SetStopColor(255, 255, 255);
	gradient->AddStop(stop);
	
	fView->AddElement(gradient);
	
	RefreshGradients();
}

void
EditorWindow::CreateStop()
{
	BSVGGradient *gradient = CurrentGradient();
	if (!gradient)
		return;
	
	BSVGStop *stop = new BSVGStop();
	stop->SetOffset(0.0);
	stop->SetStopColor(128, 128, 128);
	stop->SetStopOpacity(1.0);
	
	gradient->AddStop(stop);
	RefreshGradients();
}

void
EditorWindow::DeleteStop()
{
	BSVGStop *stop = CurrentStop();
	if (!stop)
		return;
	
	BSVGGradient *gradient = CurrentGradient();
	if (!gradient)
		return;
	
	gradient->RemoveStop(stop);
	RefreshGradients();
}

void
EditorWindow::ClearStops()
{
	BSVGGradient *gradient = CurrentGradient();
	if (!gradient)
		return;
	
	gradient->ClearStops();
	RefreshGradients();
}*/

void
EditorWindow::OpenFile()
{
	delete fFilePanel;
	fFilePanel = new BFilePanel(B_OPEN_PANEL, 0, 0, 0, false, new BMessage(M_OPEN_FILE_REFS));
	fFilePanel->Show();
}

void
EditorWindow::OpenBackground()
{
	((EditorApp *)be_app)->SetRequest(this);
	delete fFilePanel;
	fFilePanel = new BFilePanel(B_OPEN_PANEL, 0, 0, 0, false, new BMessage(M_OPEN_BACKGROUND_REFS));
	fFilePanel->Show();
}

void
EditorWindow::ClearBackground()
{
	fView->ClearViewBitmap();
	fView->SetViewColor(255, 255, 255, 255);
	fView->Invalidate();
}

void
EditorWindow::ExportFile()
{
	((EditorApp *)be_app)->SetRequest(this);
	delete fFilePanel;
	fFilePanel = new BFilePanel(B_SAVE_PANEL, 0, 0, 0, false, new BMessage(M_EXPORT_FILE_REFS));
	fFilePanel->Show();
}
