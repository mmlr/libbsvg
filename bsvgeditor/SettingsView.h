#ifndef _SETTINGS_VIEW_H_
#define _SETTINGS_VIEW_H_

#include <View.h>
#include <ColorControl.h>
#include <Slider.h>
#include <TextControl.h>
#include <OptionPopUp.h>
#include <CheckBox.h>
#include "EditorPreView.h"
#include "EditorTabView.h"
#include "SVGGradient.h"

class BSVGPath;
class BSVGGradient;
class BSVGStop;
class EditorTextView;
class EditorPreView;

enum settings_t {
	M_PATH_SETTINGS = 0,
	M_GRADIENT_SETTINGS,
	M_STOP_SETTINGS,
	M_SOURCE_SETTINGS,
	M_PREVIEW_SETTINGS
};

enum setting_t {
	M_SLIDER_SETTING = 0,
	M_POPUP_SETTING,
	M_COLOR_SETTING,
	M_TEXT_SETTING
};

struct Setting {
	// general functions
	void		SetTarget(BView *target) { control->SetTarget(target); inherit->SetTarget(target); };
	void		SetValue(int32 value) { control->SetValue(value); };
	void		SetValue(rgb_color value) { if (type == M_COLOR_SETTING) ((BColorControl *)control)->SetValue(value); };
	void		SetValue(int32 value, source_t source) { SetValue(value); inherit->SetValue(source == SOURCE_SET); };
	void		SetValue(rgb_color value, source_t source) { SetValue(value); inherit->SetValue(source == SOURCE_SET); };
	int32		Value() const { return control->Value(); };
	rgb_color	ValueAsColor() { if (type == M_COLOR_SETTING) return ((BColorControl *)control)->ValueAsColor(); else return SVGColor(Value()).Color(); };
	void		Check(source_t source) { inherit->SetValue(source == SOURCE_SET); };
	bool		Inherit() { return inherit->Value(); };
	source_t	Source() { if (inherit->Value()) return SOURCE_SET; else return SOURCE_UNSET; };
	
	// optionpopup functions
	status_t	SelectOptionFor(int32 value) { if (type == M_POPUP_SETTING) return ((BOptionPopUp *)control)->SelectOptionFor(value); else return B_ERROR; };
	status_t	SelectOptionFor(int32 value, source_t source) { inherit->SetValue(source == SOURCE_SET); return SelectOptionFor(value); };
	int32		SelectedOption(const char **outName) { if (type == M_POPUP_SETTING) return ((BOptionPopUp *)control)->SelectedOption(outName); else return B_ERROR; };
	void		ClearOptions() { if (type == M_POPUP_SETTING) { BOptionPopUp *popup = (BOptionPopUp *)control; while (popup->CountOptions() > 0) popup->RemoveOptionAt(0); } };
	
	// textcontrol functions
	void		SetText(const char *text) { if (type == M_TEXT_SETTING) ((BTextControl *)control)->SetText(text); };
	void		SetText(const char *text, source_t source) { SetText(text); inherit->SetValue(source == SOURCE_SET); };
	const char	*Text() { if (type == M_TEXT_SETTING) return ((BTextControl *)control)->Text(); else return NULL; };
	
	BControl	*control;
	BCheckBox	*inherit;
	BPoint		position;
	setting_t	type;
};

class SettingsView : public BView {
	public:
						SettingsView(BRect frame, const char *name, uint32 resizeMask, uint32 flags);
						
virtual	void			AttachedToWindow();
virtual	void			MessageReceived(BMessage *message);
virtual	void			FrameMoved(BPoint new_position);

virtual	void			SetTo(BSVGElement *element);
virtual	void			SetTo(BSVGGradient *gradient);
virtual	void			SetTo(BSVGStop *stop);
virtual	void			SetTo(EditorTextView *view);
virtual	void			SetTo(EditorPreView *view);

virtual	void			SetType(settings_t type);

virtual	void			UpdateLabel(Setting &setting, float factor = 1);

virtual	void			SetStrokeType(paint_t type, source_t source);
		paint_t			StrokeType();
virtual	void			SetStrokeColor(rgb_color color, source_t source);
		rgb_color		StrokeColor();
virtual	void			SetStrokeGradient(const BString &gradient, source_t source);
		BString			StrokeGradient();
virtual	void			SetStrokeOpacity(float opacity, source_t source);
		float			StrokeOpacity();
virtual	void			SetStrokeWidth(float width, source_t source);
		float			StrokeWidth();
virtual	void			SetStrokeLineCap(cap_mode mode, source_t source);
		cap_mode		StrokeLineCap();
virtual	void			SetStrokeLineJoin(join_mode mode, source_t source);
		join_mode		StrokeLineJoin();
virtual	void			SetStrokeMiterLimit(float limit, source_t source);
		float			StrokeMiterLimit();

virtual	void			SetFillType(paint_t type, source_t source);
		paint_t			FillType();
virtual	void			SetFillColor(rgb_color color, source_t source);
		rgb_color		FillColor();
virtual	void			SetFillGradient(const BString &gradient, source_t source);
		BString			FillGradient();
virtual	void			SetFillOpacity(float opacity, source_t source);
		float			FillOpacity();
virtual	void			SetFillRule(rule_t fillrule, source_t source);
		rule_t			FillRule();

virtual void			SetGradientName(BString name);
		BString			GradientName();
virtual	void			SetGradientType(element_t type);
		element_t		GradientType();
virtual	void			SetGradientUnits(units_t units);
		units_t			GradientUnits();

virtual	void			SetStopOffset(float offset);
		float			StopOffset();
virtual	void			SetStopColor(rgb_color color, source_t source);
		rgb_color		StopColor();
virtual	void			SetStopOpacity(float opacity, source_t source);
		float			StopOpacity();

virtual	void			SetPreViewSampleSize(int32 size);
		int32			PreViewSampleSize();
virtual	void			SetPreViewBackColor(rgb_color color);
		rgb_color		PreViewBackColor();
virtual	void			SetPreViewBackAlpha(uchar alpha);
		uchar			PreViewBackAlpha();

	private:
		EditorTabView	*fPathSettings;
		
		BView			*fPathStroking;
		BView			*fPathFilling;
		
		Setting			fStrokeType;
		Setting			fStrokeColor;
		Setting			fStrokeGradient;
		Setting			fStrokeOpacity;
		Setting			fStrokeWidth;
		Setting			fStrokeLineCap;
		Setting			fStrokeLineJoin;
		Setting			fStrokeMiterLimit;
		
		Setting			fFillType;
		Setting			fFillColor;
		Setting			fFillGradient;
		Setting			fFillOpacity;
		Setting			fFillRule;
		
		Setting			fGradientName;
		Setting			fGradientType;
		Setting			fGradientUnits;
		
		Setting			fStopOffset;
		Setting			fStopColor;
		Setting			fStopOpacity;
		
		Setting			fPreViewBackColor;
		Setting			fPreViewBackAlpha;
		Setting			fPreViewSampleSize;
};

#endif
