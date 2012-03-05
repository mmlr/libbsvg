#ifndef _POINT_ITERATOR_H_
#define _POINT_ITERATOR_H_

#include <Shape.h>
#include <Point.h>
#include "SVGDefs.h"

class PointIterator : public BShapeIterator
{
	public:
						PointIterator(const FloatList *offsets, PointList *result);
virtual	status_t		IterateMoveTo(BPoint *point);
virtual	status_t		IterateLineTo(int32 count, BPoint *pts);
virtual	status_t		IterateBezierTo(int32 count, BPoint *pts);
virtual	status_t		IterateClose();
		status_t		Iterate(BShape *shape);

	private:
		BPoint			*BezierPoint(float offset, BPoint *from, BPoint control[3]);
		
		int32			fCount;
		int32			fIndex;
		PointList		*fResult;
const	FloatList		*fOffsets;
		DistanceList	fDistance;
		BPoint			fStart;
};

#endif
