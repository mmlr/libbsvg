#ifndef _SVG_PAINTSERVER_H_
#define _SVG_PAINTSERVER_H_

#include <Message.h>
#include <Rect.h>
#include "SVGDefs.h"
#include "SVGElement.h"
#include "Matrix.h"

class BBitmap;
class BView;
class BSVGView;

// BSVGGradient serves as a base class for BLinearGradient and BRadialGradient

class BSVGPaintServer : public BSVGElement {

public:
						BSVGPaintServer();
						BSVGPaintServer(BSVGView *parent);
						BSVGPaintServer(BSVGView *parent, const BString &data);
virtual					~BSVGPaintServer();

						BSVGPaintServer(BMessage *data);
static	BArchivable		*Instantiate(BMessage *data);
virtual	status_t		Archive(BMessage *data, bool deep = true) const;

virtual	element_t		Type() { return B_SVG_PAINTSERVER; };
virtual	void			RecreateData();

		void			SetServerUnits(units_t units);
		units_t			ServerUnits() const;

		void			SetBounds(BRect bounds);
		BRect			Bounds();
		void			SetOffset(BPoint offset);
		BPoint			Offset();

		void			SetCoordinateSystem(const Matrix2D &system);
		Matrix2D		CoordinateSystem() const;
		bool			IsFullyOpaque() const;

virtual	void			ApplyPaint(BView *view);
virtual	void			ApplyPaint(BBitmap *bitmap);

protected:
		Matrix2D		fRenderMatrix;	// temporary matrix (transform * system)
		BRect			fBounds;
		BPoint			fOffset;

private:
		Matrix2D		fCoordinateSystem;
		units_t			fUnits;
};

#endif	// _SVG_PAINTSERVER_H_
