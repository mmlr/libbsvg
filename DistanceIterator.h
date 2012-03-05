#ifndef _DISTANCE_ITERATOR_H_
#define _DISTANCE_ITERATOR_H_

#include <Shape.h>
#include <Point.h>
#include "SVGDefs.h"

class DistanceIterator : public BShapeIterator
{
	public:
						DistanceIterator(DistanceList *result);
						DistanceIterator(float *result);
virtual	status_t		IterateMoveTo(BPoint *point);
virtual	status_t		IterateLineTo(int32 count, BPoint *pts);
virtual	status_t		IterateBezierTo(int32 count, BPoint *pts);
virtual	status_t		IterateClose();
		status_t		Iterate(BShape *shape);

	private:
		float			ApproximateRecursive(float from, float to, float lastlength);
		
		float			fLength;
		DistanceList	*fListResult;
		float			*fFloatResult;
		BPoint			fLocation;
		BPoint			fStart;
		BPoint			*fControl;
};

#endif
