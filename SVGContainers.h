#ifndef _SVG_CONTAINERS_H_
#define _SVG_CONTAINERS_H_

#include "SVGElement.h"
#include "SVGPaintServer.h"

class BSVGDocument : public BSVGElement {

public:
						BSVGDocument();
						BSVGDocument(BSVGView *parent);
						BSVGDocument(BSVGView *parent, const BString &data);

						BSVGDocument(BMessage *data);
static	BArchivable		*Instantiate(BMessage *data);
virtual	status_t		Archive(BMessage *data, bool deep = true) const;

virtual	void			HandleAttribute(attribute_s *attr);
virtual element_t		Type() { return B_SVG_DOCUMENT; };

virtual	void			CollectData(BString &into, int32 indentlevel = 0);
virtual	void			RecreateData();
};

class BSVGRoot : public BSVGElement {

public:
						BSVGRoot();
						BSVGRoot(BSVGView *parent);
						BSVGRoot(BSVGView *parent, const BString &data);

						BSVGRoot(BMessage *data);
static	BArchivable		*Instantiate(BMessage *data);
virtual	status_t		Archive(BMessage *data, bool deep = true) const;

virtual	void			HandleAttribute(attribute_s *attr);
virtual element_t		Type() { return B_SVG_ROOT; };

virtual	void			RecreateData();

virtual	void			PrepareRendering(SVGState *inherit = NULL);

private:
		BPoint			fTranslate;
};

class BSVGGroup : public BSVGElement {

public:
						BSVGGroup();
						BSVGGroup(BSVGView *parent);
						BSVGGroup(BSVGView *parent, const BString &data);

						BSVGGroup(BMessage *data);
static	BArchivable		*Instantiate(BMessage *data);
virtual	status_t		Archive(BMessage *data, bool deep = true) const;

virtual element_t		Type() { return B_SVG_GROUP; };

virtual	void			RecreateData();
};

class BSVGDefs : public BSVGElement {

public:
						BSVGDefs();
						BSVGDefs(BSVGView *parent);
						BSVGDefs(BSVGView *parent, const BString &data);

						BSVGDefs(BMessage *data);
static	BArchivable		*Instantiate(BMessage *data);
virtual	status_t		Archive(BMessage *data, bool deep = true) const;

virtual element_t		Type() { return B_SVG_DEFS; };

virtual	void			CollectBounds(BRect &bounds, bool *first);
virtual	void			RecreateData();

private:
virtual	void			Render(BView *view);
virtual	void			Render(BWindow *window);
virtual	void			Render(BBitmap *bitmap);
virtual	void			RenderCommon();

};

class BSVGPattern : public BSVGPaintServer {

public:
						BSVGPattern();
						BSVGPattern(BSVGView *parent);
						BSVGPattern(BSVGView *parent, const BString &data);

						BSVGPattern(BMessage *data);
static	BArchivable		*Instantiate(BMessage *data);
virtual	status_t		Archive(BMessage *data, bool deep = true) const;

virtual	void			HandleAttribute(attribute_s *attr);
virtual element_t		Type() { return B_SVG_PATTERN; };

virtual	void			CollectBounds(BRect &bounds, bool *first);
virtual	void			RecreateData();

		void			SetContentUnits(units_t units);
		units_t			ContentUnits() const;

virtual	void			PrepareRendering(SVGState *inherit);

virtual	void			ApplyPaint(BView *view);
virtual	void			ApplyPaint(BBitmap *bitmap);

private:
virtual	void			Render(BView *view);
virtual	void			Render(BWindow *window);
virtual	void			Render(BBitmap *bitmap);
virtual	void			RenderCommon();

		units_t			fContentUnits;

		float			fX;
		float			fY;
		float			fWidth;
		float			fHeight;
};

class BSVGUse : public BSVGGroup {

public:
						BSVGUse();
						BSVGUse(BSVGView *parent);
						BSVGUse(BSVGView *parent, const BString &data);
virtual					~BSVGUse();

						BSVGUse(BMessage *data);
static	BArchivable		*Instantiate(BMessage *data);
virtual	status_t		Archive(BMessage *data, bool deep = true) const;

virtual	void			HandleAttribute(attribute_s *attr);
virtual element_t		Type() { return B_SVG_USE; };

virtual	void			RecreateData();
virtual	void			CollectData(BString &into, int32 indentlevel);

virtual	void			PrepareRendering(SVGState *inherit = NULL);
virtual	void			Render(BView *view);
virtual	void			Render(BWindow *window);
virtual	void			Render(BBitmap *bitmap);
virtual	void			RenderCommon();

private:
		BPoint			fTranslate;
};

#endif
