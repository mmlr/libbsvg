#ifndef _DRAW_ITERATOR_H_
#define _DRAW_ITERATOR_H_

#include <Shape.h>
#include <View.h>
#include "EditorView.h"

class DrawIterator : public BShapeIterator {
	public:
					DrawIterator(EditorView &target);
					
		status_t	IterateMoveTo(BPoint *point);
		status_t	IterateLineTo(int32 lineCount, BPoint *linePts);
		status_t	IterateBezierTo(int32 bezierCount, BPoint *bezierPts);
		status_t	IterateClose();
		
	private:
		EditorView	&fTarget;
};

#endif
