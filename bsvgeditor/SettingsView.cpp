#include "SettingsView.h"
#include "EditorDefs.h"
#include "EditorApp.h"
#include "EditorWindow.h"
#include "SVGStop.h"
#include <ColorControl.h>
#include <Slider.h>
#include <MenuField.h>
#include <CheckBox.h>

float spacing = 2;
float distance = 20;
BRect controlrect;
float heights[3];
float offsets[3];

struct option_s {
	const char *label;
	int32 value;
};

void
remove_all_children(BView *view)
{
	for (int i = view->CountChildren(); i > 0; i--)
		view->RemoveChild(view->ChildAt(0));
}

void
add_setting(BView *parent, Setting &setting, BRect *rect)
{
	setting.position = BPoint(rect->left + distance, rect->top);
	setting.control->MoveTo(setting.position);
	parent->AddChild(setting.control);
	setting.inherit->MoveTo(BPoint(rect->left, rect->top + offsets[setting.type]));
	parent->AddChild(setting.inherit);
	rect->OffsetBy(0, heights[setting.type] + spacing);
}

void
add_setting_after(BView *parent, Setting &setting, Setting &after)
{
	BRect rect = after.inherit->Frame();
	rect.OffsetBy(0, rect.Height());
	add_setting(parent, setting, &rect);
}

void
remove_setting(BView *parent, Setting &setting)
{
	parent->RemoveChild(setting.control);
	parent->RemoveChild(setting.inherit);
}

void
create_common(Setting &setting, int32 message, setting_t type)
{
	setting.control->ResizeToPreferred();
	int32 inherit_msg = message | M_INHERIT_KEY;
	setting.inherit = new BCheckBox(controlrect, "", "", new BMessage(inherit_msg));
	setting.inherit->ResizeToPreferred();
	setting.type = type;
}

void
create_slider_setting(Setting &setting, const char *name, int32 message, int min, int max)
{
	setting.control = new BSlider(controlrect, name, name, new BMessage(message), min, max);
	create_common(setting, message, M_SLIDER_SETTING);
}

void
create_popup_setting(Setting &setting, const char *name, int32 message, option_s options[], int count)
{
	BOptionPopUp *popup = new BOptionPopUp(controlrect, name, name, new BMessage(message));
	setting.control = popup;
	create_common(setting, message, M_POPUP_SETTING);
	setting.control->ResizeTo(controlrect.right - distance, setting.control->Bounds().Height());
	
	for (int i = 0; i < count; i++)
		popup->AddOption(options[i].label, options[i].value);
}

void
create_color_setting(Setting &setting, const char *name, int32 message, color_control_layout layout, float cellsize)
{
	setting.control = new BColorControl(controlrect.LeftTop(), layout, cellsize, name, new BMessage(message));
	create_common(setting, message, M_COLOR_SETTING);
}

void
create_text_setting(Setting &setting, const char *name, int32 message)
{
	setting.control = new BTextControl(controlrect, name, name, "", new BMessage(message));
	create_common(setting, message, M_TEXT_SETTING);
}

SettingsView::SettingsView(BRect frame, const char *name, uint32 resizeMask, uint32 flags)
	:	BView(frame, name, resizeMask, flags)
{
	float spacing = 2;
	frame.OffsetTo(0, 0);
	controlrect.Set(distance, spacing, frame.Width() - 20 - distance, frame.Height());
	
	// path stroking
	fPathStroking = new BView(frame, "fPathStroking", B_FOLLOW_ALL, B_WILL_DRAW);
	fPathStroking->SetViewColor(ui_color(B_PANEL_BACKGROUND_COLOR));
	
	option_s linecap[] = { { "Round Cap", B_ROUND_CAP }, { "Butt Cap", B_BUTT_CAP }, { "Square Cap", B_SQUARE_CAP } };
	option_s linejoin[] = { { "Round Join", B_ROUND_JOIN }, { "Miter Join", B_MITER_JOIN }, { "Bevel Join", B_BEVEL_JOIN }, { "Butt Join", B_BUTT_JOIN }, { "Square Join", B_SQUARE_JOIN } };
	option_s stroketype[] = { { "None", B_FILL_NONE }, { "Current Color", B_FILL_CURRENT_COLOR }, { "Color", B_FILL_COLOR }, { "Gradient", B_FILL_SERVER }, { "Pattern", B_FILL_SERVER } };
	create_popup_setting(fStrokeLineCap, "Stroke Line Cap", M_STROKE_LINECAP, linecap, 3);
	create_popup_setting(fStrokeLineJoin, "Stroke Line Join", M_STROKE_LINEJOIN, linejoin, 5);
	create_popup_setting(fStrokeType, "Stroke Type", M_STROKE_TYPE, stroketype, 4);
	create_popup_setting(fStrokeGradient, "Stroke Gradient", M_STROKE_GRADIENT, NULL, 0);
	create_slider_setting(fStrokeOpacity, "Stroke Opacity", M_STROKE_OPACITY, 0, 100);	
	create_slider_setting(fStrokeWidth, "Stroke Width", M_STROKE_WIDTH, 0, 100);
	create_slider_setting(fStrokeMiterLimit, "Stroke Miter Limit", M_STROKE_MITERLIMIT, 0, 100);
	create_color_setting(fStrokeColor, "Stroke Color", M_STROKE_COLOR, B_CELLS_32x8, 2);
	
	heights[M_SLIDER_SETTING] = fStrokeOpacity.control->Bounds().Height();
	heights[M_POPUP_SETTING] = fStrokeType.control->Bounds().Height();
	heights[M_COLOR_SETTING] = fStrokeColor.control->Bounds().Height();
	
	float checkboxheight = fStrokeType.inherit->Bounds().Height();
	offsets[M_SLIDER_SETTING] = (heights[M_SLIDER_SETTING] - checkboxheight) / 2;
	offsets[M_POPUP_SETTING] = (heights[M_POPUP_SETTING] - checkboxheight) / 2;
	offsets[M_COLOR_SETTING] = (heights[M_COLOR_SETTING] - checkboxheight) / 2;
	
	// path filling
	fPathFilling = new BView(frame, "fPathFilling", B_FOLLOW_ALL, B_WILL_DRAW);
	fPathFilling->SetViewColor(ui_color(B_PANEL_BACKGROUND_COLOR));
	
	option_s filltype[] = { { "None", B_FILL_NONE }, { "Current Color", B_FILL_CURRENT_COLOR }, { "Color", B_FILL_COLOR }, { "Gradient", B_FILL_SERVER }, { "Pattern", B_FILL_SERVER } };
	option_s fillrule[] = { { "Non-Zero", B_NONZERO }, { "Even-Odd", B_EVENODD } };
	create_popup_setting(fFillType, "Fill Type", M_FILL_TYPE, filltype, 4);
	create_popup_setting(fFillRule, "Fill Rule", M_FILL_RULE, fillrule, 2);
	create_popup_setting(fFillGradient, "Fill Gradient", M_FILL_GRADIENT, NULL, 0);
	create_slider_setting(fFillOpacity, "Fill Opacity", M_FILL_OPACITY, 0, 100);
	create_color_setting(fFillColor, "Fill Color", M_FILL_COLOR, B_CELLS_32x8, 2);
	
	int32 num = 0;
	fPathSettings = new EditorTabView(frame, "fPathSettings");
	fPathSettings->AddTab(fPathStroking);
	fPathSettings->TabAt(num++)->SetLabel("Stroking");
	fPathSettings->AddTab(fPathFilling);
	fPathSettings->TabAt(num++)->SetLabel("Filling");
	
	// gradient settings
	option_s gradienttype[] = { { "Linear Gradient", B_SVG_LINEARGRAD }, { "Radial Gradient", B_SVG_RADIALGRAD } };
	option_s gradientunits[] = { { "Object Bounding Box", B_OBJECT_BOUNDING_BOX }, { "Userspace On Use", B_USERSPACE_ON_USE } };
	create_popup_setting(fGradientType, "Gradient Type", M_GRADIENT_TYPE, gradienttype, 2);
	create_popup_setting(fGradientUnits, "Gradient Units", M_GRADIENT_UNITS, gradientunits, 2);
	create_text_setting(fGradientName, "Gradient Name", M_GRADIENT_NAME);
	
	// stop settings
	create_slider_setting(fStopOffset, "Stop Offset", M_STOP_OFFSET, 0, 100);
	create_slider_setting(fStopOpacity, "Stop Opacity", M_STOP_OPACITY, 0, 100);
	create_color_setting(fStopColor, "Stop Color", M_STOP_COLOR, B_CELLS_32x8, 2);
	
	// source settings
	
	// preview settings
	option_s samplesize[] = { { "No Antialiasing", 1 }, { "2x Antialiasing", 2 }, { "3x Antialiasing", 3 }, { "4x Antialiasing", 4 } };
	create_popup_setting(fPreViewSampleSize, "Sample Size", M_PREVIEW_SAMPLESIZE, samplesize, 4);
	create_slider_setting(fPreViewBackAlpha, "Background Alpha", M_PREVIEW_BACKALPHA, 0, 255);
	create_color_setting(fPreViewBackColor, "Preview Back Color", M_PREVIEW_BACKCOLOR, B_CELLS_32x8, 2);
}

void
SettingsView::AttachedToWindow()
{
	fStrokeOpacity.SetTarget(this);
	fStrokeWidth.SetTarget(this);
	fStrokeLineCap.SetTarget(this);
	fStrokeLineJoin.SetTarget(this);
	fStrokeMiterLimit.SetTarget(this);	
	fStrokeType.SetTarget(this);
	fStrokeColor.SetTarget(this);
	fStrokeGradient.SetTarget(this);
	
	fFillType.SetTarget(this);
	fFillOpacity.SetTarget(this);
	fFillColor.SetTarget(this);
	fFillGradient.SetTarget(this);
	fFillRule.SetTarget(this);
	
	fGradientName.SetTarget(this);
	fGradientType.SetTarget(this);
	fGradientUnits.SetTarget(this);
	
	fStopOffset.SetTarget(this);
	fStopColor.SetTarget(this);
	fStopOpacity.SetTarget(this);
	
	fPreViewSampleSize.SetTarget(this);
	fPreViewBackColor.SetTarget(this);
	fPreViewBackAlpha.SetTarget(this);
}

#define HANDLE_COMMON(type, control, field, func) \
	source_t source = (inherit ? control.Source() : SOURCE_SET); \
	element->SetSource(field, source); \
	if (source == SOURCE_SET) { \
		if (!inherit) \
			element->SetValue(field, &value); \
		else \
			element->GetValue(field, &value); \
	} else \
		element->ResolveField(field, &value); \
	func(value, source);

#define HANDLE_MSG(type, control, field, func) \
{ \
	type value = (type)control.Value(); \
	HANDLE_COMMON(type, control, field, func); \
} break;

#define HANDLE_MSG_F(control, field, func, factor) \
{ \
	float value = (float)control.Value() / factor; \
	HANDLE_COMMON(float, control, field, func); \
} break;

#define HANDLE_MSG_COLOR(control, field, func) \
{ \
	rgb_color value = control.ValueAsColor(); \
	HANDLE_COMMON(rgb_color, control, field, func); \
} break;


#define HANDLE_MSG_TEXT(control, field, func) \
{ \
	const char *buffer = NULL; \
	control.SelectedOption(&buffer); \
	BString value(buffer); \
	HANDLE_COMMON(BString, control, field, func); \
} break;

void
SettingsView::MessageReceived(BMessage *message)
{
	EditorWindow *window = (EditorWindow *)Window();
	bool inherit = message->what & M_INHERIT_KEY;
	if (inherit)
		message->what -= M_INHERIT_KEY;
	
	switch (message->what) {
		case M_FILL_COLOR:
		case M_FILL_TYPE:
		case M_FILL_GRADIENT:
		case M_FILL_OPACITY:
		case M_FILL_RULE:
		case M_STROKE_COLOR:
		case M_STROKE_OPACITY:
		case M_STROKE_WIDTH:
		case M_STROKE_LINECAP:
		case M_STROKE_LINEJOIN:
		case M_STROKE_MITERLIMIT:
		case M_STROKE_GRADIENT:
		case M_STROKE_TYPE: {
				BSVGElement *element = window->CurrentElement();
				if (!element)
					break;
				
				switch (message->what) {
					case M_STROKE_TYPE: HANDLE_MSG(paint_t, fStrokeType, FIELD_STROKE_TYPE, SetStrokeType);
					case M_STROKE_COLOR: HANDLE_MSG_COLOR(fStrokeColor, FIELD_STROKE_COLOR, SetStrokeColor);
					case M_STROKE_OPACITY: HANDLE_MSG_F(fStrokeOpacity, FIELD_STROKE_OPACITY, SetStrokeOpacity, 100);
					case M_FILL_TYPE: HANDLE_MSG(paint_t, fFillType, FIELD_FILL_TYPE, SetFillType);
					case M_FILL_COLOR: HANDLE_MSG_COLOR(fFillColor, FIELD_FILL_COLOR, SetFillColor);
					case M_FILL_OPACITY: HANDLE_MSG_F(fFillOpacity, FIELD_FILL_OPACITY, SetFillOpacity, 100);
					case M_FILL_RULE: HANDLE_MSG(rule_t, fFillRule, FIELD_FILL_RULE, SetFillRule);
					case M_STROKE_WIDTH: HANDLE_MSG_F(fStrokeWidth, FIELD_STROKE_WIDTH, SetStrokeWidth, 1);
					case M_STROKE_LINECAP: HANDLE_MSG(cap_mode, fStrokeLineCap, FIELD_STROKE_LINECAP, SetStrokeLineCap);
					case M_STROKE_LINEJOIN: HANDLE_MSG(join_mode, fStrokeLineJoin, FIELD_STROKE_LINEJOIN, SetStrokeLineJoin);
					case M_STROKE_MITERLIMIT: HANDLE_MSG_F(fStrokeMiterLimit, FIELD_STROKE_MITERLIMIT, SetStrokeMiterLimit, 1);
					case M_FILL_GRADIENT: HANDLE_MSG_TEXT(fFillGradient, FIELD_FILL_SERVER, SetFillGradient);
					case M_STROKE_GRADIENT: HANDLE_MSG_TEXT(fStrokeGradient, FIELD_STROKE_SERVER, SetStrokeGradient);
				}
				
				window->View()->Invalidate();
			} break;
		
		case M_GRADIENT_NAME:
		case M_GRADIENT_TYPE:
		case M_GRADIENT_UNITS: {
				BSVGGradient *gradient = window->CurrentGradient();
				if (!gradient)
					break;
				
				switch (message->what) {
					case M_GRADIENT_NAME:
						gradient->SetID(fGradientName.Text());
						window->RefreshElements();
						break;
					case M_GRADIENT_UNITS:
						gradient->SetServerUnits((units_t)fGradientUnits.Value());
						break;
					default: break;
				}
				
				window->View()->Invalidate();
			} break;
		case M_STOP_OFFSET:
		case M_STOP_COLOR:
		case M_STOP_OPACITY: {
				BSVGStop *stop = window->CurrentStop();
				if (!stop)
					break;
				
				switch (message->what) {
					case M_STOP_OFFSET:
						stop->SetOffset((float)fStopOffset.Value() / 100);
						window->CurrentGradient()->SortStops();
						window->RefreshElements();
						UpdateLabel(fStopOffset, 0.01);
						break;
					case M_STOP_COLOR:
						stop->SetStopColor(fStopColor.ValueAsColor());
						break;
					case M_STOP_OPACITY:
						stop->SetStopOpacity((float)fStopOpacity.Value() / 100);
						UpdateLabel(fStopOpacity, 0.01);
						break;
				}
				
				window->View()->Invalidate();
			} break;
		case M_PREVIEW_SAMPLESIZE:
		case M_PREVIEW_BACKCOLOR:
		case M_PREVIEW_BACKALPHA: {
				EditorPreView *view = window->PreView();
				rgb_color color = fPreViewBackColor.ValueAsColor();
				color.alpha = (uchar)fPreViewBackAlpha.Value();
				
				switch (message->what) {
					case M_PREVIEW_SAMPLESIZE:
						view->SetSampleSize(fPreViewSampleSize.Value());
						break;
					case M_PREVIEW_BACKCOLOR:
						view->SetViewColor(color);
						break;
					case M_PREVIEW_BACKALPHA:
						view->SetViewColor(color);
						UpdateLabel(fPreViewBackAlpha);
						break;
				}
				
				window->PreView()->Invalidate();
			} break;
		
		default: BView::MessageReceived(message);
	}
}

void
SettingsView::FrameMoved(BPoint new_position)
{
	fStrokeColor.control->MoveTo(fStrokeColor.position);
	fFillColor.control->MoveTo(fFillColor.position);
	fStopColor.control->MoveTo(fStopColor.position);
}

void
SettingsView::SetType(settings_t type)
{
	remove_all_children(this);
	BRect rect(0, spacing, 0, 0);
	
	switch (type) {
		case M_PATH_SETTINGS: {
			AddChild(fPathSettings);
		} break;
		case M_GRADIENT_SETTINGS: {
			add_setting(this, fGradientName, &rect);
			add_setting(this, fGradientType, &rect);
			add_setting(this, fGradientUnits, &rect);
		} break;
		case M_STOP_SETTINGS: {
			add_setting(this, fStopOffset, &rect);
			add_setting(this, fStopColor, &rect);
			add_setting(this, fStopOpacity, &rect);
		} break;
		case M_SOURCE_SETTINGS: {
		} break;
		case M_PREVIEW_SETTINGS: {
			add_setting(this, fPreViewSampleSize, &rect);
			add_setting(this, fPreViewBackColor, &rect);
			add_setting(this, fPreViewBackAlpha, &rect);
		} break;
	}
}

void
SettingsView::SetTo(BSVGElement *element)
{
	if (!element)
		return;
	
	SetType(M_PATH_SETTINGS);
	
	SVGState state = *element->RenderState();
	
	SetStrokeType(state.stroke_type(), state.stroke_type.source);
	SetStrokeColor(state.stroke_color.Color(), state.stroke_color.source);
	SetStrokeGradient(state.stroke_server, state.stroke_server.source);
	SetStrokeOpacity(state.stroke_opacity(), state.stroke_opacity.source);
	SetStrokeWidth(state.stroke_width(), state.stroke_width.source);
	SetStrokeLineCap(state.stroke_linecap(), state.stroke_linecap.source);
	SetStrokeLineJoin(state.stroke_linejoin(), state.stroke_linejoin.source);
	SetStrokeMiterLimit(state.stroke_miterlimit(), state.stroke_miterlimit.source);
	
	SetFillType(state.fill_type(), state.fill_type.source);
	SetFillColor(state.fill_color.Color(), state.fill_color.source);
	SetFillGradient(state.fill_server, state.fill_server.source);
	SetFillOpacity(state.fill_opacity(), state.fill_opacity.source);
	SetFillRule(state.fill_rule(), state.fill_rule.source);
	
	fStrokeGradient.ClearOptions();
	fFillGradient.ClearOptions();
	
	EditorView *view = ((EditorWindow *)Window())->View();
	if (!view)
		return;
	
	/*int32 fill = 0;
	int32 stroke = 0;
	for (int i = 0; i < view->CountGradients(); i++) {
		BSVGGradient *gradient = view->GradientAt(i);
		if (!gradient)
			break;
		
		if (gradient->Name().Compare(element->FillGradient()) == 0)
			fill = i + 1;
		if (gradient->Name().Compare(element->StrokeGradient()) == 0)
			stroke = i + 1;
		
		fFillGradient->AddOptionAt(gradient->Name().String(), i + 1, i + 1);
		fStrokeGradient->AddOptionAt(gradient->Name().String(), i + 1, i + 1);
	}*/
	
	/*fFillGradient.SetValue(fill);
	fFillGradient->MenuField()->Menu()->ItemAt(fill)->SetMarked(true);*/
	/*fStrokeGradient.SetValue(stroke);
	fStrokeGradient->MenuField()->Menu()->ItemAt(stroke)->SetMarked(true);*/
}

void
SettingsView::SetTo(BSVGGradient *gradient)
{
	if (!gradient)
		return;
	
	SetType(M_GRADIENT_SETTINGS);
	
	SetGradientName(gradient->ID());
	SetGradientType(gradient->Type());
	SetGradientUnits(gradient->ServerUnits());
}

void
SettingsView::SetTo(BSVGStop *stop)
{
	if (!stop)
		return;
	
	SetType(M_STOP_SETTINGS);
	
	SVGState state = *stop->State();
	SetStopOffset(stop->Offset());
	SetStopColor(state.stop_color.Color(), state.stop_color.source);
	SetStopOpacity(state.stop_opacity(), state.stop_opacity.source);
}

void
SettingsView::SetTo(EditorTextView *view)
{
	if (!view)
		return;
	
	SetType(M_SOURCE_SETTINGS);
}

void
SettingsView::SetTo(EditorPreView *view)
{
	if (!view)
		return;
	
	SetType(M_PREVIEW_SETTINGS);
	
	SetPreViewSampleSize(view->SampleSize());
	SetPreViewBackColor(view->ViewColor());
	SetPreViewBackAlpha(view->ViewColor().alpha);
}

void
SettingsView::UpdateLabel(Setting &setting, float factor)
{
	BString newlabel = setting.control->Name();
	if (factor != 1)
		newlabel << ": " << factor * setting.Value();
	else
		newlabel << ": " << setting.Value();
	
	setting.control->SetLabel(newlabel.String());
}

void
SettingsView::SetStrokeType(paint_t type, source_t source)
{
	BRect rect(0, 0, 0, 0);
	fStrokeType.SetValue(type, source);
	remove_all_children(fPathStroking);
	add_setting(fPathStroking, fStrokeType, &rect);
	
	switch (type) {
		case B_FILL_UNKNOWN:
		case B_FILL_NONE:
		case B_FILL_CURRENT_COLOR: break;
		case B_FILL_COLOR: add_setting(fPathStroking, fStrokeColor, &rect); break;
		case B_FILL_SERVER: add_setting(fPathStroking, fStrokeGradient, &rect); break;
	}
	
	if (type > B_FILL_NONE) {
		add_setting(fPathStroking, fStrokeOpacity, &rect);
		add_setting(fPathStroking, fStrokeWidth, &rect);
		add_setting(fPathStroking, fStrokeLineCap, &rect);
		add_setting(fPathStroking, fStrokeLineJoin, &rect);
		if (StrokeLineJoin() == B_MITER_JOIN)
			add_setting(fPathStroking, fStrokeMiterLimit, &rect);
	}
}

paint_t
SettingsView::StrokeType()
{
	return (paint_t)fStrokeType.Value();
}

void
SettingsView::SetStrokeColor(rgb_color color, source_t source)
{
	fStrokeColor.SetValue(color, source);
}

rgb_color
SettingsView::StrokeColor()
{
	return fStrokeColor.ValueAsColor();
}

void
SettingsView::SetStrokeOpacity(float opacity, source_t source)
{
	fStrokeOpacity.SetValue(opacity * 100, source);
	UpdateLabel(fStrokeOpacity, 0.01);
}

float
SettingsView::StrokeOpacity()
{
	return (float)fStrokeOpacity.Value() / 100;
}

void
SettingsView::SetStrokeGradient(const BString &gradient, source_t source)
{
	fStrokeGradient.SetText(gradient.String(), source);
}

BString
SettingsView::StrokeGradient()
{
	return BString(fStrokeGradient.Text());
}

void
SettingsView::SetStrokeWidth(float width, source_t source)
{
	fStrokeWidth.SetValue(width, source);
	UpdateLabel(fStrokeWidth);
}

float
SettingsView::StrokeWidth()
{
	return fStrokeWidth.Value();
}

void
SettingsView::SetStrokeLineCap(cap_mode mode, source_t source)
{
	fStrokeLineCap.SelectOptionFor(mode, source);
}

cap_mode
SettingsView::StrokeLineCap()
{
	return (cap_mode)fStrokeLineCap.Value();
}

void
SettingsView::SetStrokeLineJoin(join_mode mode, source_t source)
{
	fStrokeLineJoin.SelectOptionFor(mode, source);
	remove_setting(fPathStroking, fStrokeMiterLimit);
	
	if (mode == B_MITER_JOIN) {
		add_setting_after(fPathStroking, fStrokeMiterLimit, fStrokeLineJoin);
	}
}

join_mode
SettingsView::StrokeLineJoin()
{
	return (join_mode)fStrokeLineJoin.Value();
}

void
SettingsView::SetStrokeMiterLimit(float limit, source_t source)
{
	fStrokeMiterLimit.SetValue(limit, source);
	UpdateLabel(fStrokeMiterLimit);
}

float
SettingsView::StrokeMiterLimit()
{
	return fStrokeMiterLimit.Value();
}

void
SettingsView::SetFillType(paint_t type, source_t source)
{
	BRect rect(0, 0, 0, 0);
	fFillType.SetValue(type, source);
	remove_all_children(fPathFilling);
	add_setting(fPathFilling, fFillType, &rect);
	
	switch (type) {
		case B_FILL_UNKNOWN:
		case B_FILL_NONE:
		case B_FILL_CURRENT_COLOR: break;
		case B_FILL_COLOR: add_setting(fPathFilling, fFillColor, &rect); break;
		case B_FILL_SERVER: add_setting(fPathFilling, fFillGradient, &rect); break;
	}
	
	if (type > B_FILL_NONE) {
		add_setting(fPathFilling, fFillOpacity, &rect);
		add_setting(fPathFilling, fFillRule, &rect);
	}
}

paint_t
SettingsView::FillType()
{
	return (paint_t)fStrokeType.Value();
}

void
SettingsView::SetFillColor(rgb_color color, source_t source)
{
	fFillColor.SetValue(color, source);
}

rgb_color
SettingsView::FillColor()
{
	return fFillColor.ValueAsColor();
}

void
SettingsView::SetFillGradient(const BString &gradient, source_t source)
{
	fFillGradient.SetText(gradient.String(), source);
}

BString
SettingsView::FillGradient()
{
	return BString(fFillGradient.Text());
}

void
SettingsView::SetFillOpacity(float opacity, source_t source)
{
	fFillOpacity.SetValue(opacity * 100, source);
	UpdateLabel(fFillOpacity, 0.01);
}

float
SettingsView::FillOpacity()
{
	return (float)fFillOpacity.Value() / 100;
}

void
SettingsView::SetFillRule(rule_t fillrule, source_t source)
{
	fFillRule.SelectOptionFor(fillrule, source);
}

rule_t
SettingsView::FillRule()
{
	return (rule_t)fFillRule.Value();
}

void
SettingsView::SetGradientName(BString name)
{
	fGradientName.SetText(name.String());
}

BString
SettingsView::GradientName()
{
	return fGradientName.Text();
}

void
SettingsView::SetGradientType(element_t type)
{
	fGradientType.SelectOptionFor(type);
}

element_t
SettingsView::GradientType()
{
	return (element_t)fGradientType.Value();
}

void
SettingsView::SetGradientUnits(units_t units)
{
	fGradientUnits.SelectOptionFor(units);
}

units_t
SettingsView::GradientUnits()
{
	return (units_t)fGradientUnits.Value();
}

void
SettingsView::SetStopOffset(float offset)
{
	fStopOffset.SetValue(offset * 100);
	UpdateLabel(fStopOffset, 0.01);
}

float
SettingsView::StopOffset()
{
	return (float)fStopOffset.Value() / 100;
}

void
SettingsView::SetStopColor(rgb_color color, source_t source)
{
	fStopColor.SetValue(color, source);
}

rgb_color
SettingsView::StopColor()
{
	return fStopColor.ValueAsColor();
}

void
SettingsView::SetStopOpacity(float opacity, source_t source)
{
	fStopOpacity.SetValue(opacity * 100, source);
	UpdateLabel(fStopOpacity, 0.01);
}

float
SettingsView::StopOpacity()
{
	return (float)fStopOpacity.Value() / 100;
}

void
SettingsView::SetPreViewSampleSize(int32 size)
{
	fPreViewSampleSize.SelectOptionFor(size);
}

int32
SettingsView::PreViewSampleSize()
{
	return fPreViewSampleSize.Value();
}

void
SettingsView::SetPreViewBackColor(rgb_color color)
{
	fPreViewBackColor.SetValue(color);
}

rgb_color
SettingsView::PreViewBackColor()
{
	return fPreViewBackColor.ValueAsColor();
}

void
SettingsView::SetPreViewBackAlpha(uchar alpha)
{
	fPreViewBackAlpha.SetValue(alpha);
	UpdateLabel(fPreViewBackAlpha);
}

uchar
SettingsView::PreViewBackAlpha()
{
	return fPreViewBackAlpha.Value();
}
