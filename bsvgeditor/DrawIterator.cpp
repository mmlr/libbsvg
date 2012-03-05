#include "DrawIterator.h"

DrawIterator::DrawIterator(EditorView &target)
	:	fTarget(target)
{
}

status_t
DrawIterator::IterateMoveTo(BPoint *point)
{
	fTarget.SetHighColor(255, 0, 0);
	fTarget.StrokeEllipse(*point, 2, 2);
	
	return B_OK;
}

status_t
DrawIterator::IterateLineTo(int32 lineCount, BPoint *linePts)
{
	fTarget.SetHighColor(0, 0, 255);
	for (int i = 0; i < lineCount; i++)
		fTarget.StrokeEllipse(linePts[i], 2, 2);
	
	return B_OK;
}

status_t
DrawIterator::IterateBezierTo(int32 bezierCount, BPoint *bezierPts)
{
	for (int i = 0; i < bezierCount * 3;) {
		fTarget.SetHighColor(0, 255, 0);
		BRect rect1(bezierPts[i], bezierPts[i++]);
		rect1.InsetBy(-2, -2);
		fTarget.StrokeRect(rect1);
		
		BRect rect2(bezierPts[i], bezierPts[i++]);
		rect2.InsetBy(-2, -2);
		fTarget.StrokeRect(rect2);
		
		fTarget.SetHighColor(0, 0, 0);
		fTarget.StrokeEllipse(bezierPts[i++], 2, 2);
	}
	
	return B_OK;
}

status_t
DrawIterator::IterateClose()
{
	return B_OK;
}
