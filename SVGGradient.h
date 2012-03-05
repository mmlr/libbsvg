#ifndef _SVG_GRADIENT_H_
#define _SVG_GRADIENT_H_

#include <Message.h>
#include <Rect.h>
#include <Region.h>
#include <String.h>
#include <SupportDefs.h>
#include "SVGDefs.h"
#include "ObjectList.h"
#include "SVGPaintServer.h"

class BBitmap;
class BShape;
class BView;

// BSVGGradient serves as a base class for BLinearGradient and BRadialGradient

class BSVGGradient : public BSVGPaintServer {

public:
						BSVGGradient(BSVGView *parent);
						BSVGGradient(BSVGView *parent, BRect frame, BPoint start, BPoint end);
virtual					~BSVGGradient();

						BSVGGradient(BMessage *data);
static	BArchivable		*Instantiate(BMessage *data);
virtual	status_t		Archive(BMessage *data, bool deep = true) const;

virtual	void			HandleAttribute(attribute_s *attr);
virtual	element_t		Type() { return B_SVG_GRADIENT; };

		void			SetVector(BPoint start, BPoint end);
		void			SetStart(BPoint start);
		void			SetEnd(BPoint end);
		BPoint			Start() const;
		BPoint			End() const;

		void			SetSpreadMethod(spread_t method);
		spread_t		SpreadMethod() const;

		BSVGStop		*StopAt(int index);
		void			AddStop(BSVGStop *stop);
		bool			RemoveStop(BSVGStop *stop);
		BSVGStop		*RemoveStopAt(int index);
		int32			CountStops();
		void			ClearStops();
		void			SortStops();
virtual	bool			IsFullyOpaque() const;

virtual	void			ApplyPaint(BView *view);
virtual	void			ApplyPaint(BBitmap *bitmap);

private:
friend	class BSVGLinearGradient;
friend	class BSVGRadialGradient;

virtual	void			Recalculate();
virtual	void			PrepareRendering(SVGState *inherit);
virtual	void			Render(BView *view);
virtual	void			Render(BWindow *window);
virtual	void			Render(BBitmap *bitmap);
virtual	void			RenderCommon();
virtual	BSVGElement		*ResolveLink();

		BPoint			fStart;			// these two points give us the vector
		BPoint			fEnd;			// the gradient is drawn along
		BPoint			fRenderStart;
		BPoint			fRenderEnd;

		bool			fStartX;
		bool			fStartY;
		bool			fEndX;
		bool			fEndY;
		bool			fHorizontal;

		spread_t		fSpreadMethod;
};


class BSVGLinearGradient : public BSVGGradient
{

public:
						BSVGLinearGradient(BSVGView *parent);
						BSVGLinearGradient(BSVGView *parent, BRect frame, BPoint start, BPoint end);

						BSVGLinearGradient(BMessage *data);
static	BArchivable		*Instantiate(BMessage *data);
virtual	status_t		Archive(BMessage *data, bool deep = true) const;

virtual	void			HandleAttribute(attribute_s *attr);
virtual	element_t		Type() { return B_SVG_LINEARGRAD; };

virtual	void			RecreateData();

virtual	void			ApplyPaint(BView *view);
virtual	void			ApplyPaint(BBitmap *bitmap);

private:
virtual	void			Recalculate();
virtual	BSVGElement		*ResolveLink();

		BPoint			fIntersections[4];
};


class BSVGRadialGradient : public BSVGGradient
{

public:
						BSVGRadialGradient(BSVGView *parent);
						BSVGRadialGradient(BSVGView *parent, BRect frame, BPoint start, BPoint end);

						BSVGRadialGradient(BMessage *data);
static	BArchivable		*Instantiate(BMessage *data);
virtual	status_t		Archive(BMessage *data, bool deep = true) const;

virtual	void			HandleAttribute(attribute_s *attr);
virtual	element_t		Type() { return B_SVG_RADIALGRAD; };

virtual	void			RecreateData();

		void			SetFocal(BPoint focal);
		BPoint			Focal() const;

virtual	void			ApplyPaint(BView *view);
virtual	void			ApplyPaint(BBitmap *bitmap);

private:
virtual	void			Recalculate();
virtual	BSVGElement		*ResolveLink();

		BPoint			fFocal;
		BPoint			fRenderFocal;

		bool			fFocalX;
		bool			fFocalY;
};

#endif	// GRADIENT_H_
