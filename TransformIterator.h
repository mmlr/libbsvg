#ifndef _TRANSFORM_ITERATOR_H_
#define _TRANSFORM_ITERATOR_H_

#include <Shape.h>
#include <Point.h>
#include "Matrix.h"

class TransformIterator : public BShapeIterator {

public:
						TransformIterator(Matrix2D *transformation, BShape *target);
virtual					~TransformIterator();

virtual	status_t		IterateMoveTo(BPoint *point);
virtual	status_t		IterateLineTo(int32 lineCount, BPoint *linePts);
virtual	status_t		IterateBezierTo(int32 bezierCount, BPoint *bezierPts);
virtual	status_t		IterateClose();

private:
		Matrix2D		*fTransformation;
		BShape			*fTarget;
};

#endif
