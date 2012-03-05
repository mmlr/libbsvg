#include "TransformIterator.h"

TransformIterator::TransformIterator(Matrix2D *transformation, BShape *target)
	:	BShapeIterator(),
		fTransformation(transformation),
		fTarget(target)
{
}

TransformIterator::~TransformIterator()
{
}

status_t
TransformIterator::IterateMoveTo(BPoint *point)
{
	fTarget->MoveTo((*fTransformation) * (*point));
	return B_OK;
}

status_t
TransformIterator::IterateLineTo(int32 lineCount, BPoint *linePts)
{
	for (int i = 0; i < lineCount; i++)
		fTarget->LineTo((*fTransformation) * linePts[i]);
	
	return B_OK;
}

status_t
TransformIterator::IterateBezierTo(int32 bezierCount, BPoint *bezierPts)
{
	BPoint pts[3];
	
	for (int i = 0; i < bezierCount * 3;) {
		pts[0] = (*fTransformation) * bezierPts[i++];
		pts[1] = (*fTransformation) * bezierPts[i++];
		pts[2] = (*fTransformation) * bezierPts[i++];
		fTarget->BezierTo(pts);
	}
		
	return B_OK;
}

status_t
TransformIterator::IterateClose()
{
	fTarget->Close();
	return B_OK;
}
