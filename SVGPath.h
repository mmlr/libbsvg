#ifndef _SVG_PATH_H_
#define _SVG_PATH_H_

#include <String.h>
#include <GraphicsDefs.h>
#include "ObjectList.h"
#include <Shape.h>
#include <View.h>
#include "SVGDefs.h"
#include "SVGElement.h"
#include "SVGGradient.h"
#include "Matrix.h"

class BSVGView;

class BSVGPath : public BSVGElement {

public:
						BSVGPath(BSVGView *parent);
						BSVGPath(BSVGView *parent, const char *data);
						BSVGPath(BSVGView *parent, BString data);

virtual					~BSVGPath();

						BSVGPath(BMessage *data);
static	BArchivable		*Instantiate(BMessage *data);
virtual	status_t		Archive(BMessage *data, bool deep = true) const;

virtual element_t		Type() { return B_SVG_PATH; };

virtual	void			HandleAttribute(attribute_s *attr);
virtual	void			FinalizeShape();
		void			ApplyTransformation();

virtual	void			RecreateData();

virtual	void			SetShape(BShape *shape);
		BShape			*Shape();
virtual	BRect			ShapeBounds();
		BShape			*TransformedShape();
virtual	void			CollectBounds(BRect &bounds, bool *first);

		void			MoveTo(BPoint to, bool relative = false);
		void			LineTo(BPoint to, bool relative = false);
		void			HLineTo(float x, bool relative = false);
		void			VLineTo(float y, bool relative = false);
		void			CurveTo(BPoint control1, BPoint control2, BPoint to, bool relative = false);
		void			ShortCurveTo(BPoint control, BPoint to, bool relative = false);
		void			QuadBezierTo(BPoint control, BPoint to, bool relative = false);
		void			ShortBezierTo(BPoint to, bool relative = false);
		void			EllipticalArcTo(BPoint radii, float rotation, bool large, bool sweep, BPoint to, bool relative = false);
static	float 			VectorsToAngle(BPoint u, BPoint v);
		void			RoundArcTo(BPoint center, BPoint radii, float rotation, float theta, float deltatheta, BPoint to = BPoint(0, 0), bool connect = false);
		void			Close();

		bool			NewSubPath();
		BPoint			Start();

		BPoint			LastLocation();
		BPoint			LastControl();
		bool			LastWasCurve();
		bool			LastWasQuadBezier();

virtual	void			PrepareRendering(SVGState *inherit = NULL);

virtual	void			Render(BView *view);
virtual	void			Render(BWindow *window);
virtual	void			Render(BBitmap *bitmap);
							// must accept child views

virtual	void			RenderCommon();

private:

friend	class BSVGRect;
friend	class BSVGEllipse;
friend	class BSVGCircle;
friend	class BSVGLine;
friend	class BSVGPolyline;
friend	class BSVGPolygon;
friend	class BSVGString;

		BShape			fShape;
		BShape			fTransformedShape;
		BShape			*fRenderShape;	// points to fShape or fTranslatedShape

		bool			fNewSubPath;
		BPoint			fStart;
		BPoint			fLastLocation;	// the last pen position is cached
		BPoint			fLastControl;	// the last control point for shorthands
		bool			fLastWasCurve;	// tells us wether we can use fLastControl
		bool			fLastWasQuad;

		void			SetupView(BRect bounds);
		BView			*fView;
};

inline BPoint
BSVGPath::Start()
{
	return fStart;
}

inline bool
BSVGPath::NewSubPath()
{
	return fNewSubPath;
}

inline BPoint
BSVGPath::LastLocation()
{
	return fLastLocation;
}

inline BPoint
BSVGPath::LastControl()
{
	return fLastControl;
}

inline bool
BSVGPath::LastWasCurve()
{
	return fLastWasCurve;
}

inline bool
BSVGPath::LastWasQuadBezier()
{
	return fLastWasQuad;
}

#endif
