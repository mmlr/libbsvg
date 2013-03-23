#ifndef _SVGSHAPES_H_
#define _SVGSHAPES_H_

#include "SVGPath.h"
#include "SVGView.h"

class BSVGRect : public BSVGPath {

public:
						BSVGRect(BSVGView *parent);

						BSVGRect(BMessage *data);
static	BArchivable		*Instantiate(BMessage *data);
virtual	status_t		Archive(BMessage *data, bool deep = true) const;

virtual element_t		Type() { return B_SVG_RECT; };

virtual	void			HandleAttribute(attribute_s *attr);
virtual	void			FinalizeShape();
virtual	void			RecreateData();

		void			SetX(float x);
		void			SetY(float y);

		void			SetWidth(float width);
		void			SetHeight(float height);

		void			SetRX(float rx);
		void			SetRY(float ry);

private:
		float			fX;
		float			fY;
		float			fWidth;
		float			fHeight;
		float			fRX;
		float			fRY;
		bool			fRXSet;
		bool			fRYSet;
};

// **************************************************************************

class BSVGEllipse : public BSVGPath {

public:
						BSVGEllipse(BSVGView *parent);

						BSVGEllipse(BMessage *data);
static	BArchivable		*Instantiate(BMessage *data);
virtual	status_t		Archive(BMessage *data, bool deep = true) const;

virtual element_t		Type() { return B_SVG_ELLIPSE; };

virtual	void			HandleAttribute(attribute_s *attr);
virtual	void			FinalizeShape();
virtual	void			RecreateData();

		void			SetCX(float cx);
		void			SetCY(float cy);

		void			SetRX(float rx);
		void			SetRY(float ry);

private:
friend 	class BSVGCircle;

		BPoint			fCenter;
		float			fRX;
		float			fRY;
		bool			fRXSet;
		bool			fRYSet;
};

// **************************************************************************

class BSVGCircle : public BSVGEllipse {

public:
						BSVGCircle(BSVGView *parent);

						BSVGCircle(BMessage *data);
static	BArchivable		*Instantiate(BMessage *data);
virtual	status_t		Archive(BMessage *data, bool deep = true) const;

virtual element_t		Type() { return B_SVG_CIRCLE; };

virtual	void			HandleAttribute(attribute_s *attr);
virtual	void			RecreateData();

		void			SetR(float r);
};

// **************************************************************************

class BSVGLine : public BSVGPath {

public:
						BSVGLine(BSVGView *parent);

						BSVGLine(BMessage *data);
static	BArchivable		*Instantiate(BMessage *data);
virtual	status_t		Archive(BMessage *data, bool deep = true) const;

virtual element_t		Type() { return B_SVG_LINE; };

virtual	void			HandleAttribute(attribute_s *attr);
virtual	void			FinalizeShape();
virtual void			RecreateData();

		void			SetX1(float x1);
		void			SetY1(float y1);

		void			SetX2(float x2);
		void			SetY2(float y2);

private:
		BPoint			fPoint1;
		BPoint			fPoint2;
};

// **************************************************************************

class BSVGPolyline : public BSVGPath {

public:
						BSVGPolyline(BSVGView *parent);

						BSVGPolyline(BMessage *data);
static	BArchivable		*Instantiate(BMessage *data);
virtual	status_t		Archive(BMessage *data, bool deep = true) const;

virtual element_t		Type() { return B_SVG_POLYLINE; };

virtual	void			HandleAttribute(attribute_s *attr);
virtual	void			FinalizeShape();
virtual	void			RecreateData();
};

// **************************************************************************

class BSVGPolygon : public BSVGPolyline {

public:
						BSVGPolygon(BSVGView *parent);

						BSVGPolygon(BMessage *data);
static	BArchivable		*Instantiate(BMessage *data);
virtual	status_t		Archive(BMessage *data, bool deep = true) const;

virtual element_t		Type() { return B_SVG_POLYGON; };

virtual	void			HandleAttribute(attribute_s *attr);
virtual	void			FinalizeShape();
virtual	void			RecreateData();
};

#endif
