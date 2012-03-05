#include "GetPointsIterator.h"

GetPointsIterator::GetPointsIterator(BObjectList<BPoint> &target)
	:	fTarget(target)
{
}

status_t
GetPointsIterator::IterateMoveTo(BPoint *point)
{
	fTarget.AddItem(point);
	return B_OK;
}

status_t
GetPointsIterator::IterateLineTo(int32 lineCount, BPoint *linePts)
{
	for (int i = 0; i < lineCount; i++)
		fTarget.AddItem(linePts++);
	
	return B_OK;
}

status_t
GetPointsIterator::IterateBezierTo(int32 bezierCount, BPoint *bezierPts)
{
	for (int i = 0; i < bezierCount * 3; i++)
		fTarget.AddItem(bezierPts++);
	
	return B_OK;
}

status_t
GetPointsIterator::IterateClose()
{
	return B_OK;
}
