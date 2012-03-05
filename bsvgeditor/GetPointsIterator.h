#ifndef _GET_POINTS_ITERATOR_H_
#define _GET_POINTS_ITERATOR_H_

#include <Shape.h>
#include <View.h>
#include "EditorView.h"

class GetPointsIterator : public BShapeIterator {
	public:
					GetPointsIterator(BObjectList<BPoint> &target);
					
		status_t	IterateMoveTo(BPoint *point);
		status_t	IterateLineTo(int32 lineCount, BPoint *linePts);
		status_t	IterateBezierTo(int32 bezierCount, BPoint *bezierPts);
		status_t	IterateClose();
		
	private:
		BObjectList<BPoint>	&fTarget;
};

#endif
