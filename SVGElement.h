#ifndef _SVG_ELEMENT_H_
#define _SVG_ELEMENT_H_

#include <Archivable.h>
#include "SVGDefs.h"
#include "SVGState.h"

class BBitmap;
class BSVGView;
class BView;
class BWindow;

class BSVGElement : public BArchivable {

public:
						BSVGElement();
						BSVGElement(BSVGView *parent);
						BSVGElement(BSVGView *parent, const BString &data);
virtual					~BSVGElement();

						BSVGElement(BMessage *data);
static	BArchivable		*Instantiate(BMessage *data);
virtual	status_t		Archive(BMessage *data, bool deep = true) const;

virtual element_t		Type() { return B_SVG_ELEMENT; };

virtual	void			SetHeaderData(const BString &data);
		BString			HeaderData();
virtual	void			SetFooterData(const BString &data);
		BString			FooterData();

virtual	void			RecreateData();
virtual	void			CollectData(BString &into, int32 indentlevel = 0);

virtual	void			CollectBounds(BRect &bounds, bool *first);

virtual	void			PromoteDownMember(void *member, void *setto);

virtual	void			SetVisible(bool visible);
		bool			Visible();

virtual	void			SetID(const char *id);
virtual	void			SetID(const BString &id);
		BString			ID();
		void			AddID(BString &to);

virtual	void			SetClass(const char *_class);
virtual	void			SetClass(const BString &_class);
		BString			Class();

virtual	void			SetLink(const char *link);
virtual	void			SetLink(const BString &link);
		BString			Link();

virtual	void			SetParent(BSVGView *parent);
		BSVGView		*Parent();

// attributes
virtual	void			HandleAttribute(attribute_s *attr);
virtual	void			HandleAttributes();
							// called after filling in attributes

		void			AddAttribute(attribute_s *attr, bool handle = false);
		void			RemoveAttribute(attribute_s *attr);
		attribute_s		*RemoveAttributeAt(int32 index);
		void			ClearAttributes();

		attribute_s		*AttributeAt(int32 index);
		int32			IndexOf(attribute_s *attr);

		int32			CountAttributes();
		attribute_s		*FindAttributeByName(const BString &name);
		attribute_s		*FindAttributeByValue(const BString &value);
		bool			HasAttribute(const BString &name);

// elements
		void			OpenElement();
		void			CloseElement();
		bool			IsOpen();

		void			AddElement(BSVGElement *element, int32 index = -1);

		int32			CountElements(bool deep = true);
		BSVGElement		*FindElement(const BString &id, uint32 of_type = 0, bool deep = true);
		status_t		FindElements(const BString &_class, ElementList &elements);
		BSVGElement		*FindParentElement(uint32 of_type = 0);
		BSVGElement		*ElementAt(int index, bool deep = true);
		int32			IndexOf(BSVGElement *child, bool deep = true);

		BSVGElement		*RemoveElement(const BString &id, bool deep = true);
		BSVGElement		*RemoveElement(BSVGElement *element, bool deep = true);
		BSVGElement		*RemoveElementAt(int index, bool deep = true);

		BSVGElement		*RemoveSelf();

		BSVGElement		*LastElement();
		BSVGElement		*LastContainer();
		BSVGElement		*OpenContainer(BSVGElement *container = NULL);
		BSVGElement		*OpenContainer(SVGState *state = NULL);
		void			CloseContainer();

		void			SetParentElement(BSVGElement *parent);
		BSVGElement		*ParentElement();

// debug
		void			DumpTree(int indent = 0);
		void			PrintToStream();

virtual	void			PrepareRendering(SVGState *inherit = NULL);

virtual	void			Render(BView *view);
virtual	void			Render(BWindow *window);
virtual	void			Render(BBitmap *bitmap);
							// must accept child views

virtual	void			RenderCommon();

// SVGState API
virtual	status_t		ResolveField(field_t field, void *result);
virtual	void			SetValue(field_t field, void *to);
virtual	void			GetValue(field_t field, void *result);
virtual	void			SetSource(field_t field, source_t to);
virtual	source_t		GetSource(field_t field);
virtual	void			UnsetField(field_t field);

virtual	void			SetOpacity(float opacity);
virtual	void			UnsetOpacity();
		bool			IsOpacitySet();
		float			Opacity();

virtual	void			SetFillServer(BString id);
virtual	void			SetFillServer(const char *id);
virtual	void			UnsetFillServer();
		bool			IsFillServerSet();
		const char		*FillServer();

virtual	void			SetFillColor(rgb_color color);
virtual	void			SetFillColor(uchar r, uchar g, uchar b);
virtual	void			UnsetFillColor();
		bool			IsFillColorSet();
		rgb_color		FillColor();

virtual	void			SetFillType(paint_t type);
virtual	void			UnsetFillType();
		bool			IsFillTypeSet();
		paint_t			FillType();

virtual	void			SetFillOpacity(float opacity);
virtual	void			UnsetFillOpacity();
		bool			IsFillOpacitySet();
		float			FillOpacity();

virtual	void			SetFillRule(rule_t rule);
virtual	void			UnsetFillRule();
		bool			IsFillRuleSet();
		rule_t			FillRule();

virtual	void			SetStrokeServer(BString id);
virtual	void			SetStrokeServer(const char *id);
virtual	void			UnsetStrokeServer();
		bool			IsStrokeServerSet();
		const char		*StrokeServer();

virtual	void			SetStrokeColor(rgb_color color);
		void			SetStrokeColor(uchar r, uchar g, uchar b);
virtual	void			UnsetStrokeColor();
		bool			IsStrokeColorSet();
		rgb_color		StrokeColor();

virtual	void			SetStrokeType(paint_t type);
virtual	void			UnsetStrokeType();
		bool			IsStrokeTypeSet();
		paint_t			StrokeType();

virtual	void			SetStrokeWidth(float width);
virtual	void			UnsetStrokeWidth();
		bool			IsStrokeWidthSet();
		float			StrokeWidth();

virtual	void			SetStrokeLineJoin(join_mode mode);
virtual	void			UnsetStrokeLineJoin();
		bool			IsStrokeLineJoinSet();
		join_mode		StrokeLineJoin();

virtual	void			SetStrokeLineCap(cap_mode mode);
virtual	void			UnsetStrokeLineCap();
		bool			IsStrokeLineCapSet();
		cap_mode		StrokeLineCap();

virtual	void			SetStrokeMiterLimit(float limit);
virtual	void			UnsetStrokeMiterLimit();
		bool			IsStrokeMiterLimitSet();
		float			StrokeMiterLimit();

virtual	void			SetStrokeOpacity(float opacity);
virtual	void			UnsetStrokeOpacity();
		bool			IsStrokeOpacitySet();
		float			StrokeOpacity();

virtual	void			SetStopColor(rgb_color color);
virtual	void			SetStopColor(uchar r, uchar g, uchar b);
virtual	void			UnsetStopColor();
		bool			IsStopColorSet();
		rgb_color		StopColor();

virtual	void			SetStopOpacity(float opacity);
virtual	void			UnsetStopOpacity();
		bool			IsStopOpacitySet();
		float			StopOpacity();

virtual	void			SetFontFamily(const char *family);
virtual	void			SetFontFamily(const BString &string);
virtual	void			UnsetFontFamily();
		bool			IsFontFamilySet();
		const char		*FontFamily();

virtual	void			SetFontSize(float size);
virtual	void			UnsetFontSize();
		bool			IsFontSizeSet();
		float			FontSize();

virtual	void			SetFontKerning(float kerning);
virtual	void			UnsetFontKerning();
		bool			IsFontKerningSet();
		float			FontKerning();

virtual	void			SetLetterSpacing(float spacing);
virtual	void			UnsetLetterSpacing();
		bool			IsLetterSpacingSet();
		float			LetterSpacing();

virtual	void			SetWordSpacing(float spacing);
virtual	void			UnsetWordSpacing();
		bool			IsWordSpacingSet();
		float			WordSpacing();

virtual	void			SetFontDecoration(decoration_t decoration);
virtual	void			UnsetFontDecoration();
		bool			IsFontDecorationSet();
		decoration_t	FontDecoration();

virtual	void			SetTextAnchor(anchor_t anchor);
virtual	void			UnsetTextAnchor();
		bool			IsTextAnchorSet();
		anchor_t		TextAnchor();

virtual	void			SetStartOffset(float offset);
virtual	void			UnsetStartOffset();
		bool			IsStartOffsetSet();
		float			StartOffset();

virtual	void			SetTransformation(Matrix2D &transformation);
virtual	void			UnsetTransformation();
		bool			IsTransformationSet();
		Matrix2D		Transformation();

virtual	void			SetCurrentColor(rgb_color color);
		void			SetCurrentColor(uchar r, uchar g, uchar b);
virtual	void			UnsetCurrentColor();
		bool			IsCurrentColorSet();
		rgb_color		CurrentColor();

virtual	void			SetViewPort(BRect viewport);
virtual	void			UnsetViewPort();
		bool			IsViewPortSet();
		BRect			ViewPort();
// end SVGState API

virtual	void			SetState(const SVGState &state);
		SVGState		*State();
virtual	void			SetRenderState(const SVGState &state);
		SVGState		*RenderState();

private:
friend	class BSVGDocument;
friend	class BSVGRoot;
friend	class BSVGGroup;
friend	class BSVGDefs;
friend	class BSVGPattern;
friend	class BSVGUse;
friend	class BSVGPath;
friend	class BSVGRect;
friend	class BSVGEllipse;
friend	class BSVGCircle;
friend	class BSVGLine;
friend	class BSVGPolyline;
friend	class BSVGPolygon;
friend	class BSVGText;
friend	class BSVGTextSpan;
friend	class BSVGTextPath;
friend	class BSVGString;
friend	class BSVGGradient;
friend	class BSVGLinearGradient;
friend	class BSVGRadialGradient;
friend	class BSVGStop;
friend	class BSVGPaintServer;

friend	class BSVGView;

		int32			ElementAtInternal(int32 index, BSVGElement *&result);
		bool			IndexOfInternal(BSVGElement *child, int32 *result);

		BString			fHeaderData;	// element begin
		BString			fFooterData;	// eventual closing element

		BString			fID;
		BString			fClass;
		BString			fLink;
		resolve_t		fStatus;

		AttributeList	fAttributes;
		AttributeList	fAttributeNames;
		AttributeList	fAttributeValues;
		SVGState		fState;
		SVGState		fRenderState;	// temporary state for rendering
		BSVGView		*fParent;		// were we are getting gradients from...
		BSVGElement		*fParentElement;
		ElementList		fElements;
		bool			fVisible;
		bool			fOpen;
};

#endif
