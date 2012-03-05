#include <Bitmap.h>
#include <math.h>
#include <Window.h>
#include <Message.h>

#include "SVGPath.h"
#include "SVGView.h"
#include "SVGColors.h"
#include "SVGDataCreator.h"
#include "SVGPaintServer.h"
#include "TransformIterator.h"
#include "EvenOddIterator.h"
#include <Picture.h>
#include <StopWatch.h>

#define NEXTTOK		strtok(NULL, " ,\t\n")

class BSVGView;

BSVGPath::BSVGPath(BSVGView *parent)
	:	BSVGElement(parent),
		fRenderShape(&fShape),
		fNewSubPath(true),
		fStart(0, 0),
		fLastLocation(0, 0),
		fLastControl(0, 0),
		fView(NULL)
{
}

BSVGPath::BSVGPath(BSVGView *parent, const char *data)
	:	BSVGElement(parent, data),
		fRenderShape(&fShape),
		fNewSubPath(true),
		fStart(0, 0),
		fLastLocation(0, 0),
		fLastControl(0, 0),
		fView(NULL)
{
}

BSVGPath::BSVGPath(BSVGView *parent, BString data)
	:	BSVGElement(parent, data),
		fRenderShape(&fShape),
		fNewSubPath(true),
		fStart(0, 0),
		fLastLocation(0, 0),
		fLastControl(0, 0),
		fView(NULL)
{
}

BSVGPath::~BSVGPath()
{
	delete fView;
}

BSVGPath::BSVGPath(BMessage *data)
	:	BSVGElement(data),
		fRenderShape(&fShape),
		fNewSubPath(true),
		fStart(0, 0),
		fLastLocation(0, 0),
		fLastControl(0, 0),
		fView(NULL)
{
	data->FindString("_headerdata", &fHeaderData);
	data->FindPoint("_start", &fStart);
	data->FindBool("_subpath", &fNewSubPath);
	data->FindBool("_visible", &fVisible);
	
	FINALIZEIF(B_SVG_PATH);
}
	
BArchivable *
BSVGPath::Instantiate(BMessage *data)
{
	if (validate_instantiation(data, "BSVGPath"))
		return new BSVGPath(data);
	return NULL;
}

status_t
BSVGPath::Archive(BMessage *data, bool deep) const
{
	BSVGElement::Archive(data, deep);
	
	data->AddString("_headerdata", fHeaderData);
	data->AddPoint("_start", fStart);
	data->AddBool("_subpath", fNewSubPath);
	data->AddBool("_visible", fVisible);
	
	ADDREPCLASS("BSVGPath");
	ADDREPTYPE(B_SVG_PATH);
	
	return B_OK;
}

void
BSVGPath::HandleAttribute(attribute_s *attr)
{
	if (!attr)
		return;
	
	if (attr->id.Compare("d") == 0) {
		SetHeaderData(attr->value);
	} else {
		BSVGElement::HandleAttribute(attr);
	}
}

void
BSVGPath::FinalizeShape()
{
	if (fHeaderData.Length() <= 0)
		return;
	
	BString buffer = fHeaderData.String();
	char *data = buffer.LockBuffer(buffer.Length());
		
	char that;
	char *token = strtok(data, " ,\t\n");
	int want = 0;
	float val[7];
	int count = 0;
	char dummy[2];
	dummy[1] = 0;
	int res;
	
	while (true) {
		if (!token)
			break;
		if (token[0] == 0)
			token = NEXTTOK;
		if (!token)
			break;
		
		that = token[0];
		token++;
		if (token[0] == 0)
			token = NEXTTOK;
		
		switch (that) {
			case 'h':
			case 'H':
			case 'v':
			case 'V': want = 1; break;
			case 'm':
			case 'M':
			case 'l':
			case 'L':
			case 't':
			case 'T': want = 2; break;
			case 's':
			case 'S':
			case 'q':
			case 'Q': want = 4; break;
			case 'c':
			case 'C': want = 6; break;
			case 'a':
			case 'A': want = 7; break;
			case 'z':
			case 'Z': want = 0; break;
			default: token++; continue;
		}
		
		if (!token && want > 0)
			break;
		
		count = want;
		while (count == want) {
			count = 0;
			for (int i = 0; i < want; i++) {
				res = sscanf(token, "%f%c", &val[i], &dummy[0]);
				if (res == 1)
					count++;
				else if (res == 2) {
					count++;
					token = strstr(token + 1, dummy);
					continue;
				} else
					break;
				
				token = NEXTTOK;
				if (!token)
					break;
			}
			
			if (count != want)
				break;
			
			if (fNewSubPath) {
				switch (that) {
					case 'm':	fLastLocation = fStart; fStart += BPoint(val[0], val[1]); break;
					case 'M':	fStart = BPoint(val[0], val[1]); break;
					default:	fShape.MoveTo(fStart); break;
				}
				fNewSubPath = false;
			}
			
			switch (that) {
				case 'm':
				case 'M': MoveTo(BPoint(val[0], val[1]), that == 'm'); break;
				case 'l':
				case 'L': LineTo(BPoint(val[0], val[1]), that == 'l'); break;
				case 'h':
				case 'H': HLineTo(val[0], that == 'h'); break;
				case 'v':
				case 'V': VLineTo(val[0], that == 'v'); break;
				case 'c':
				case 'C': CurveTo(BPoint(val[0], val[1]), BPoint(val[2], val[3]), BPoint(val[4], val[5]), that == 'c'); break;
				case 'a':
				case 'A': EllipticalArcTo(BPoint(val[0], val[1]), val[2], val[3] != 0, val[4] != 0, BPoint(val[5], val[6]), that == 'a'); break;
				case 's':
				case 'S': ShortCurveTo(BPoint(val[0], val[1]), BPoint(val[2], val[3]), that == 's'); break;
				case 'q':
				case 'Q': QuadBezierTo(BPoint(val[0], val[1]), BPoint(val[2], val[3]), that == 'q'); break;
				case 't':
				case 'T': ShortBezierTo(BPoint(val[0], val[1]), that == 't'); break;
				case 'z':
				case 'Z': Close(); break;
				default: break;
			}
			
			if (want == 0 || !token)
				break;
		}
	}
	
	buffer.UnlockBuffer(0);
}

void
BSVGPath::MoveTo(BPoint to, bool relative)
{
	if (relative)
		to += fLastLocation;
	
	fShape.MoveTo(to);
	fLastLocation = to;
	
	fLastWasCurve = false;
	fLastWasQuad = false;
}

void
BSVGPath::LineTo(BPoint to, bool relative)
{
	if (relative)
		to += fLastLocation;
	
	fShape.LineTo(to);
	fLastLocation = to;
	
	fLastWasCurve = false;
	fLastWasQuad = false;
}

void
BSVGPath::HLineTo(float x, bool relative)
{
	if (relative)
		fLastLocation.x += x;
	else
		fLastLocation.x = x;
	
	fShape.LineTo(fLastLocation);
	
	fLastWasCurve = false;
	fLastWasQuad = false;
}

void
BSVGPath::VLineTo(float y, bool relative)
{
	if (relative)
		fLastLocation.y += y;
	else
		fLastLocation.y = y;
	
	fShape.LineTo(fLastLocation);
	
	fLastWasCurve = false;
	fLastWasQuad = false;
}

// note: this is a hack. the app_server seems to have a bug in a very
// special case, where miter_join is active, a curve has a downward slope,
// the ending point is x.00 and the other control-points are in a certain
// pattern. so if a x.00 condition is found a small extra value is added
// to avoid the bug.
#define HACK(pt)	pt.x = (pt.x == (int)pt.x ? pt.x + 0.01 : pt.x);

void
BSVGPath::CurveTo(BPoint control1, BPoint control2, BPoint to, bool relative)
{
	if (relative) {
		control1 += fLastLocation;
		control2 += fLastLocation;
		to += fLastLocation;
	}
		
	BPoint points[3];
	points[0] = control1;
	points[1] = control2;
	points[2] = to;
	
	HACK(points[0]);
	HACK(points[2]);
	
	fShape.BezierTo(points);
	
	fLastLocation = to;
	fLastControl = control2;
	fLastWasCurve = true;
	fLastWasQuad = false;
}

void
BSVGPath::ShortCurveTo(BPoint control, BPoint to, bool relative)
{
	if (relative) {
		control += fLastLocation;
		to += fLastLocation;
	}
	
	BPoint points[3];
	if (fLastWasCurve)
		points[0] = fLastLocation - fLastControl + fLastLocation;
	else
		points[0] = fLastLocation;
	
	points[1] = control;
	points[2] = to;
	
	HACK(points[0]);
	HACK(points[2]);
	
	fShape.BezierTo(points);
	
	fLastLocation = to;
	fLastControl = control;
	fLastWasCurve = true;
	fLastWasQuad = false;
}

void
BSVGPath::QuadBezierTo(BPoint control, BPoint to, bool relative)
{
	if (relative) {
		control += fLastLocation;
		to += fLastLocation;
	}
	
	BPoint points[3];
	points[0] = fLastLocation;
	points[0].x += (control.x - points[0].x) * (2.0 / 3.0);
	points[0].y += (control.y - points[0].y) * (2.0 / 3.0);
	
	points[1] = to;
	points[1].x += (control.x - points[1].x) * (2.0 / 3.0);
	points[1].y += (control.y - points[1].y) * (2.0 / 3.0);
	
	points[2] = to;
	
	HACK(points[0]);
	HACK(points[2]);
	
	fShape.BezierTo(points);
	
	fLastLocation = to;
	fLastControl = control;
	fLastWasCurve = false;
	fLastWasQuad = true;
}

void
BSVGPath::ShortBezierTo(BPoint to, bool relative)
{
	if (relative)
		to += fLastLocation;
	
	BPoint control;
	if (fLastWasQuad)
		control = fLastLocation - fLastControl + fLastLocation;
	else
		control = fLastLocation;
	
	BPoint points[3];
	
	points[0] = fLastLocation;
	points[0].x += (control.x - points[0].x) * (2.0 / 3.0);
	points[0].y += (control.y - points[0].y) * (2.0 / 3.0);
	
	points[1] = to;
	points[1].x += (control.x - points[1].x) * (2.0 / 3.0);
	points[1].y += (control.y - points[1].y) * (2.0 / 3.0);
	
	points[2] = to;
	
	HACK(points[0]);
	HACK(points[2]);
	
	fShape.BezierTo(points);
	
	fLastLocation = to;
	fLastControl = control;
	fLastWasCurve = false;
	fLastWasQuad = true;
}

float
BSVGPath::VectorsToAngle(BPoint u, BPoint v) {
	float products = (u.x * v.x + u.y * v.y);
	float values = sqrt(u.x * u.x + u.y * u.y) * sqrt(v.x * v.x + v.y * v.y);
	float result = DEGREES(acos(CLAMP(products / values, -1, 1)));
	
	if ((result < 0) != (u.x * v.y - u.y * v.x < 0))
		result *= -1;
	
	return result;
}

void
BSVGPath::EllipticalArcTo(BPoint radii, float rotation, bool large, bool sweep, BPoint to, bool relative)
{
	if (relative)
		to += fLastLocation;
	
	if (fLastLocation == to) {
		fLastWasCurve = false;
		fLastWasQuad = false;
		return;
	}
	
	if (radii.x == 0 || radii.y == 0) {
		LineTo(to);
		return;
	}
	
	if (radii.x < 0) radii.x *= -1;
	if (radii.y < 0) radii.y *= -1;
	
	while (rotation > 360) rotation -= 360;
	while (rotation < 0) rotation += 360;
	
	BPoint midpoint = fLastLocation - to;
	midpoint.x /= 2.0;
	midpoint.y /= 2.0;
	
	Matrix2D matrix;
	BPoint start = midpoint;
	if (rotation != 0) {
		matrix.RotateBy(-rotation);
		start = matrix * start;
	}
	
	float rx2 = radii.x * radii.x;
	float ry2 = radii.y * radii.y;
	float sx2 = start.x * start.x;
	float sy2 = start.y * start.y;
	
	// correct out of range radii
	float lambda = sx2 / rx2 + sy2 / ry2;
	if (lambda > 1) {
		radii.x = sqrt(lambda) * radii.x;
		radii.y = sqrt(lambda) * radii.y;
	}
	
	// if the radii were out of range value is 0 so that the center is (0/0)
	float value = 0;
	if (lambda <= 1) {
		value = rx2 * ry2 - rx2 * sy2 - ry2 * sx2;
		value /= rx2 * sy2 + ry2 * sx2;
		if (value < 0) value *= -1;
		value = sqrt(value);
		if (large == sweep) value *= -1;
	}
	
	BPoint centerprime;
	centerprime.x = value *  (radii.x * start.y / radii.y);
	centerprime.y = value * -(radii.y * start.x / radii.x);
	
	BPoint center = centerprime;
	if (value != 0) {
		matrix.Reset();
		matrix.RotateBy(rotation);
		center = matrix * center;
	}
	
	center.x += (fLastLocation.x + to.x) / 2.0;
	center.y += (fLastLocation.y + to.y) / 2.0;
	
	BPoint vec1(1, 0);
	BPoint vec2;
	vec2.x = (start.x - centerprime.x) / radii.x;
	vec2.y = (start.y - centerprime.y) / radii.y;
	float theta = VectorsToAngle(vec1, vec2);
	
	vec1 = vec2;
	vec2.x = (-start.x - centerprime.x) / radii.x;
	vec2.y = (-start.y - centerprime.y) / radii.y;
	float deltatheta = VectorsToAngle(vec1, vec2);
	while (deltatheta > 360) deltatheta -= 360;
	while (deltatheta < -360) deltatheta += 360;
	
	if (sweep && deltatheta < 0)
		deltatheta += 360;
	else if (!sweep && deltatheta > 0)
		deltatheta -= 360;
	
	RoundArcTo(center, radii, rotation, theta, deltatheta);
	
	fLastLocation = to;
	fLastWasCurve = false;
	fLastWasQuad = false;
}

void
BSVGPath::RoundArcTo(BPoint center, BPoint radii, float rotation, float theta, float deltatheta, BPoint to, bool connect)
{
	float radius = MIN(radii.x, radii.y);
	BPoint realcenter = center;
	center.x = radius;
	center.y = radius;
	
	BPoint scale(1, 1);
	if (radii.x > radii.y)
		scale = BPoint(radii.x / radii.y, 1);
	else if (radii.x < radii.y)
		scale = BPoint(1, radii.y / radii.x);
	
	Matrix2D matrix;
	matrix.TranslateBy(realcenter);
	matrix.RotateBy(rotation);
	matrix.TranslateBy(BPoint(-center.x * scale.x, -center.y * scale.y));
	matrix.ScaleBy(scale);
	
	if (connect)
		fShape.LineTo(matrix * BPoint(center.x + radius * cos(RADIANS(theta)), center.y + radius * sin(RADIANS(theta))));
	
	float abstheta = (deltatheta < 0 ? -deltatheta : deltatheta);
	while (abstheta > 0) {
		BPoint start = center;
		start.x += radius * cos(RADIANS(theta));
		start.y += radius * sin(RADIANS(theta));
		BPoint end = center;
		end.x += radius * cos(RADIANS(theta + CLAMP(deltatheta, -90, 90)));
		end.y += radius * sin(RADIANS(theta + CLAMP(deltatheta, -90, 90)));
		
		float offset = 0.75 - CLAMP(abstheta, 0, 90) / 480;
		BPoint delta;
		BPoint control;
		
		delta = center - start;
		if (delta.y == 0) delta.y += 0.001;
		float b1 = -(delta.x / delta.y);
		
		delta = center - end;
		if (delta.y == 0) delta.y += 0.001;
		float b2 = -(delta.x / delta.y);
		
		float m1 = start.y - b1 * start.x;
		float m2 = end.y - b2 * end.x;
		
		control.x = (m2 - m1) / (b1 - b2);
		control.y = b1 * control.x + m1;
		
		BPoint points[3];
		delta = control - start;
		points[0] = matrix * (start + BPoint(delta.x * offset, delta.y * offset));
		delta = control - end;
		points[1] = matrix * (end + BPoint(delta.x * offset, delta.y * offset));
		points[2] = matrix * end;
		
		fShape.BezierTo(points);
		
		deltatheta -= (deltatheta < 0 ? -90 : 90);
		abstheta -= 90;
		theta += (deltatheta < 0 ? -90 : 90);
	}
	
	if (connect)
		fShape.LineTo(to);
}

void
BSVGPath::Close()
{
	fShape.Close();
	
	fLastWasCurve = false;
	fLastWasQuad = false;
	fNewSubPath = true;
}

void
BSVGPath::ApplyTransformation()
{
	Matrix2D transform = fRenderState.general_transform;
	fTransformedShape.Clear();
	TransformIterator transformer(&transform, &fTransformedShape);
	transformer.Iterate(&fShape);
	fRenderShape = &fTransformedShape;
}

void
BSVGPath::RecreateData()
{
	fHeaderData.SetTo("<path");
	AddID(fHeaderData);
	fState.AddToString(&fHeaderData, SOURCE_SET);
	
	fHeaderData << " d=\"";	
	BSVGDataCreator creator(fHeaderData);
	creator.Iterate(&fShape);
	
	fHeaderData << "\" />";
	fFooterData.SetTo("");
}

void
BSVGPath::SetShape(BShape *shape)
{
	fShape.Clear();
	fShape.AddShape(shape);
}

BShape *
BSVGPath::Shape()
{
	return &fShape;
}

BRect
BSVGPath::ShapeBounds()
{
	ApplyTransformation();
	BRect bounds = fRenderShape->Bounds();
	if (fRenderState.stroke_type() != B_FILL_NONE) {
		float width = fRenderState.stroke_width() * fRenderState.general_transform.matrix[0][0] / 2; // scale
		bounds.InsetBy(-width, -width);
	}
	return bounds;
}

BShape *
BSVGPath::TransformedShape()
{
	ApplyTransformation();
	return fRenderShape;
}

void
BSVGPath::CollectBounds(BRect &bounds, bool *first)
{
	if (*first) {
		bounds = ShapeBounds();
		*first = false;
	} else
		bounds = bounds | ShapeBounds();
	
	BSVGElement::CollectBounds(bounds, first);
}

void
BSVGPath::SetupView(BRect bounds)
{
	if (!fView)
		fView = new BView(bounds.OffsetToSelf(0, 0), "path", 0, B_SUBPIXEL_PRECISE);
	else
		fView->ResizeTo(bounds.Width(), bounds.Height());
}

void
BSVGPath::PrepareRendering(SVGState *inherit)
{
	BSVGElement::PrepareRendering(inherit);
}

void
BSVGPath::Render(BView *view)
{
	SetupView(view->Bounds());
	view->AddChild(fView);
	RenderCommon();
	fView->RemoveSelf();
}

void
BSVGPath::Render(BWindow *window)
{
	SetupView(window->Bounds());
	window->AddChild(fView);
	RenderCommon();
	fView->RemoveSelf();
}

void
BSVGPath::Render(BBitmap *bitmap)
{
	SetupView(bitmap->Bounds());
	bitmap->AddChild(fView);
	RenderCommon();
	fView->RemoveSelf();
}

void
BSVGPath::RenderCommon()
{
	if (!fView || !fVisible)
		return;
	
	ApplyTransformation();
	//fView->SetHighColor(0, 0, 0, 255);
	//fView->StrokeRect(ShapeBounds());	
	
	BSVGPaintServer *server = NULL;
	if (fParent && fRenderState.fill_type() == B_FILL_SERVER)
		server = (BSVGPaintServer *)fParent->FindElement(fRenderState.fill_server, B_SVG_LINEARGRAD | B_SVG_RADIALGRAD | B_SVG_PATTERN);
	
	if (fRenderState.fill_type() == B_FILL_SERVER && server) {
		
		// prepare the servers' coordinate system
		BRect frame = ShapeBounds();
		BRect frame0 = frame.OffsetToCopy(0, 0);
		if (server->ServerUnits() == B_OBJECT_BOUNDING_BOX) {
			server->SetOffset(BPoint(0, 0));
			server->SetCoordinateSystem(Matrix2D(frame0.right - frame0.left, 0, frame0.left, 0, frame0.bottom - frame0.top, frame0.top));
		} else if (server->ServerUnits() == B_USERSPACE_ON_USE) {
			server->SetOffset(BPoint(-frame.left, -frame.top));
			server->SetCoordinateSystem(fRenderState.general_transform);
		}
		
		// prepare
		BBitmap *bitmap = fParent->NeedTempBitmap(frame0);
		
		// render server
		server->SetBounds(frame0);
		server->ApplyPaint(bitmap);
		
		// erase outside stuff in evenodd fill-rule
		if (fRenderState.fill_rule() == B_EVENODD) {
			BView *tempview = fParent->NeedTempView(frame0);
			EvenOddIterator *iterator = fParent->NeedEvenOdd(frame);
			iterator->SetOffset(-frame.left, -frame.top);
			iterator->SetOutput(tempview);
			iterator->Iterate(fRenderShape);
		}
		
		bool opaque = (fRenderState.fill_opacity() >= 1.0);
		if (!opaque) {
			// apply opacity (paint is at 0, 0)
			uint8 *bits = (uint8 *)bitmap->Bits() + 3;
			float factor = fRenderState.fill_opacity();
			uint32 row = bitmap->BytesPerRow();
			for (uint32 i = 0; i < frame.Height(); i++) {
				for (uint32 j = 0; j < (uint32)frame.Width(); j++)
					bits[i * row + j * 4] = (uint8)(bits[i * row + j * 4] * factor);
			}
		}
		
		if (opaque && server->IsFullyOpaque()) {
			fView->SetDrawingMode(B_OP_COPY);
		} else {
			fView->SetDrawingMode(B_OP_ALPHA);
			fView->SetBlendingMode(B_PIXEL_ALPHA, B_ALPHA_COMPOSITE);
		}
		
		// create clipping picture
		fView->BeginPicture(new BPicture);
		fView->FillShape(fRenderShape);
		BPicture *clippict = fView->EndPicture();
		
		// draw finished paint to the view
		fView->ClipToPicture(clippict);
		fView->DrawBitmap(bitmap, frame0, frame);
		fView->ConstrainClippingRegion(NULL);
		delete clippict;
	
	} else if (fRenderState.fill_type() == B_FILL_COLOR
		|| fRenderState.fill_type() == B_FILL_CURRENT_COLOR) {
		
		SVGColor *color = (fRenderState.fill_type() == B_FILL_COLOR ? &fRenderState.fill_color : &fRenderState.general_color);
		
		if (fRenderState.fill_opacity() != 1.0) {
			fView->SetDrawingMode(B_OP_ALPHA);
			fView->SetBlendingMode(B_CONSTANT_ALPHA, B_ALPHA_COMPOSITE);
			color->color.alpha = (uchar)(255 * fRenderState.fill_opacity());
		} else {
			fView->SetDrawingMode(B_OP_COPY);
			color->color.alpha = 255;
		}
		
		// handle fill-rules properly
		if (fRenderState.fill_rule() == B_EVENODD) {
			BRect frame = ShapeBounds();
			BRect frame0 = frame.OffsetToCopy(0, 0);			
			BBitmap *bitmap = fParent->NeedTempBitmap(frame0);
			BView *tempview = fParent->NeedTempView(frame0);
			EvenOddIterator *iterator = fParent->NeedEvenOdd(frame);
			
			if (tempview->LockLooper()) {
				tempview->SetDrawingMode(B_OP_COPY);
				tempview->SetHighColor(color->color);
				tempview->FillRect(frame0, B_SOLID_HIGH);
				tempview->UnlockLooper();
			}
			
			iterator->SetOffset(-frame.left, -frame.top);
			iterator->SetOutput(tempview);
			iterator->Iterate(fRenderShape);
			
			// if drawing mode is copy we need no alpha (see above)
			if (fView->DrawingMode() == B_OP_COPY)
				fView->SetDrawingMode(B_OP_OVER);
			
			fView->SetHighColor(color->color);
			fView->DrawBitmap(bitmap, frame0, frame);
		} else if (fRenderState.fill_rule() == B_NONZERO) {
			fView->SetHighColor(color->color);
			fView->FillShape(fRenderShape);
		}
	}
	
	//*** stroking
	if (fRenderState.stroke_type() != B_FILL_NONE && fRenderState.stroke_type() != B_FILL_UNKNOWN) {	
		fView->SetPenSize(fRenderState.stroke_width() * fRenderState.general_transform.matrix[0][0]);
		fView->SetLineMode(fRenderState.stroke_linecap(), fRenderState.stroke_linejoin(), fRenderState.stroke_miterlimit());
	}
	
	if (fParent && fRenderState.stroke_type() == B_FILL_SERVER)
		server = (BSVGPaintServer *)fParent->FindElement(fRenderState.stroke_server, B_SVG_LINEARGRAD | B_SVG_RADIALGRAD | B_SVG_PATTERN);
	
	if (fRenderState.stroke_type() == B_FILL_SERVER && server) {
		
		// prepare the servers' coordinate system
		BRect frame = ShapeBounds();
		BRect frame0 = frame.OffsetToCopy(0, 0);
		if (server->ServerUnits() == B_OBJECT_BOUNDING_BOX) {
			server->SetOffset(BPoint(0, 0));
			server->SetCoordinateSystem(Matrix2D(frame0.right - frame0.left, 0, frame0.left, 0, frame0.bottom - frame0.top, frame0.top));
		} else if (server->ServerUnits() == B_USERSPACE_ON_USE) {
			server->SetOffset(BPoint(-frame.left, -frame.top));
			server->SetCoordinateSystem(fRenderState.general_transform);
		}
		
		// prepare
		BBitmap *bitmap = fParent->NeedTempBitmap(frame0);
		
		// render server
		server->SetBounds(frame0);
		server->ApplyPaint(bitmap);
		
		bool opaque = (fRenderState.stroke_opacity() >= 1.0);
		if (!opaque) {
			// apply opacity (paint is at 0, 0)
			uint8 *bits = (uint8 *)bitmap->Bits() + 3;
			float factor = fRenderState.stroke_opacity();
			uint32 row = bitmap->BytesPerRow();
			for (uint32 i = 0; i < frame.Height(); i++) {
				for (uint32 j = 0; j < (uint32)frame.Width(); j++)
					bits[i * row + j * 4] = (uint8)(bits[i * row + j * 4] * factor);
			}
		}
		
		if (opaque && server->IsFullyOpaque()) {
			fView->SetDrawingMode(B_OP_COPY);
		} else {
			fView->SetDrawingMode(B_OP_ALPHA);
			fView->SetBlendingMode(B_PIXEL_ALPHA, B_ALPHA_COMPOSITE);
		}
		
		// create clipping picture
		fView->BeginPicture(new BPicture);
		fView->StrokeShape(fRenderShape);
		BPicture *clippict = fView->EndPicture();
		
		// draw finished gradient to the view
		fView->ClipToPicture(clippict);
		fView->DrawBitmap(bitmap, frame0, frame);
		delete clippict;
	
	} else if (fRenderState.stroke_type() == B_FILL_COLOR
		|| fRenderState.stroke_type() == B_FILL_CURRENT_COLOR) {
		
		SVGColor *color = (fRenderState.stroke_type() == B_FILL_COLOR ? &fRenderState.stroke_color : &fRenderState.general_color);
		
		if (fRenderState.stroke_opacity() != 1.0) {
			fView->SetDrawingMode(B_OP_ALPHA);
			fView->SetBlendingMode(B_CONSTANT_ALPHA, B_ALPHA_COMPOSITE);
			color->color.alpha = (uchar)(255 * fRenderState.stroke_opacity());
		} else {
			fView->SetDrawingMode(B_OP_COPY);
			color->color.alpha = 255;
		}
		
		fView->SetHighColor(color->color);
		fView->StrokeShape(fRenderShape);
	}
}
