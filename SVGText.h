#ifndef _SVG_TEXT_H_
#define _SVG_TEXT_H_

#include "SVGPath.h"
#include "SVGDefs.h"

class BSVGString;
class BSVGTextSpan;
class BSVGText;

class BSVGTextSpan : public BSVGPath {
	public:
						BSVGTextSpan(BSVGView *parent);
						BSVGTextSpan(BSVGView *parent, SVGState state);
virtual					~BSVGTextSpan();
						
						BSVGTextSpan(BMessage *data);
static	BArchivable		*Instantiate(BMessage *data);
virtual	status_t		Archive(BMessage *data, bool deep = true) const;
		
virtual	void			GetBounds(BRect *bounds);

virtual	void			PrepareFont();
virtual	void			GetCharpos(CharposList *charpos, FloatList *chunklengths, int32 *index, bool *first);
virtual	void			AddToShape(CharposList *charpos, FloatList *chunklengths, BPoint *absolute, BPoint *relative, float *rotation, progress_t progress);

virtual	void			HandleAttribute(attribute_s *attr);
virtual	void			RecreateData();
		
virtual	BSVGString		*FirstText();
virtual	BSVGString		*LastText();
virtual	BSVGTextSpan	*LastTextContainer();

virtual	void			AddChar(char chr);
virtual	void			AddString(const char *string);
virtual	void			AddString(const BString &string);
virtual	void			AddSpan(BSVGTextSpan *span);

virtual	BString			WholeText();

virtual	void			StripInterSpaces(bool *kept_last);
		void			OverlapOffset(offset_t type);
		void			OverlapOffsets();

		void			SetBase(BSVGText *base);
		BSVGTextSpan	*TextParent();

virtual	BSVGText		*ParentText();
virtual	BSVGTextSpan	*ParentSpan();
virtual	BSVGTextPath	*ParentPath();

		FloatList		*GetOffsetList(offset_t which);

virtual	element_t		Type() { return B_SVG_TEXTSPAN; };

virtual	void			PrepareRendering(SVGState *inherit = NULL);
virtual	void			Render(BView *view);
virtual	void			Render(BWindow *window);
virtual	void			Render(BBitmap *bitmap);

	private:
friend	class BSVGTextPath;

		FloatList		fOffsetX;
		FloatList		fOffsetY;
		FloatList		fOffsetDX;
		FloatList		fOffsetDY;
		FloatList		fOffsetR;
};

class BSVGString : public BSVGTextSpan {

public:
						BSVGString(BSVGView *parent, const BString &text);
						BSVGString(BSVGView *parent, const char *text);

						BSVGString(BMessage *data);
static	BArchivable		*Instantiate(BMessage *data);
virtual	status_t		Archive(BMessage *data, bool deep = true) const;

virtual	void			RecreateData();

		void			SetText(const BString &text);
		void			SetText(const char *text);
		void			ClearText();
		BString			Text();
		int32			Length();
virtual	BString			WholeText();

virtual	void			GetBounds(BRect *bounds);
virtual	void			PrepareFont();
virtual	void			GetCharpos(CharposList *charpos, FloatList *chunklengths, int32 *index, bool *first);
virtual	void			AddToShape(CharposList *charpos, FloatList *chunklengths, BPoint *absolute, BPoint *relative, float *rotation, progress_t progress);

virtual	void			StripInterSpaces(bool *kept_last);

virtual	BSVGString		*FirstText();
virtual	BSVGString		*LastText();
virtual	BSVGTextSpan	*LastTextContainer();

virtual	element_t		Type() { return B_SVG_STRING; };

private:
		BFont			fFont;
		BString			fText;
};

class BSVGTextPath : public BSVGTextSpan {

public:
						BSVGTextPath(BSVGView *parent);
						BSVGTextPath(BSVGView *parent, BString path);

						BSVGTextPath(BMessage *data);
static	BArchivable		*Instantiate(BMessage *data);
virtual	status_t		Archive(BMessage *data, bool deep = true) const;

virtual element_t		Type() { return B_SVG_TEXTPATH; };

virtual	void			FinalizeShape();
virtual	void			RecreateData();

		BSVGPath		*ResolvedPath();
};

class BSVGText : public BSVGTextSpan {

public:
						BSVGText(BSVGView *parent);
						BSVGText(BSVGView *parent, SVGState state);

						BSVGText(BMessage *data);
static	BArchivable		*Instantiate(BMessage *data);
virtual	status_t		Archive(BMessage *data, bool deep = true) const;

virtual element_t		Type() { return B_SVG_TEXT; };

virtual	void			FinalizeShape();
		void			StripWhiteSpaces();
virtual	void			RecreateData();

virtual	void			PrepareRendering(SVGState *inherit = NULL);
virtual	void			Render(BView *view);
virtual	void			Render(BWindow *window);
virtual	void			Render(BBitmap *bitmap);
virtual	void			RenderCommon();

virtual	BRect			ShapeBounds();
virtual	void			CollectBounds(BRect &bounds, bool *first);
};

#endif
