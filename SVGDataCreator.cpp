#include "SVGDataCreator.h"
#include <stdio.h>

BSVGDataCreator::BSVGDataCreator(BString &target)
	:	BShapeIterator(),
		fTarget(target),
		fLast(NULL)
{
}

BSVGDataCreator::~BSVGDataCreator()
{
}

status_t
BSVGDataCreator::IterateMoveTo(BPoint *point)
{
	fTarget << " M" << point->x << " " << point->y;
	fLast = point;
		
	return B_OK;
}

status_t
BSVGDataCreator::IterateLineTo(int32 lineCount, BPoint *linePts)
{
	for (int i = 0; i < lineCount; i++) {
		if (fLast && linePts[i].x == fLast->x)
			fTarget << " V" << linePts[i].y;
		else if (fLast && linePts[i].y == fLast->y)
			fTarget << " H" << linePts[i].x;
		else
			fTarget << " L" << linePts[i].x << " " << linePts[i].y;
		
		fLast = &linePts[i];
	}
	
	return B_OK;
}

status_t
BSVGDataCreator::IterateBezierTo(int32 bezierCount, BPoint *bezierPts)
{
	for (int i = 0; i < bezierCount * 3; i += 3) {
		if (bezierPts[i + 1] == bezierPts[i + 2]) {
			if (i > 0 && bezierPts[i] == bezierPts[i - 1] - bezierPts[i - 3] + bezierPts[i - 1]) {
				// shorthand quadric bezier
				fTarget << " T" << bezierPts[i + 2].x << " " << bezierPts[i + 2].y;
			} else {
				// normal quadric bezier
				fTarget << " Q" << bezierPts[i].x << " " << bezierPts[i].y;
				fTarget << " " << bezierPts[i + 1].x << " " << bezierPts[i + 1].y;
			}
		} else {
			if (i > 0 && bezierPts[i] == bezierPts[i - 1] - bezierPts[i - 2] + bezierPts[i - 1]) {
				// shorthand cubic bezier
				fTarget << " S" << bezierPts[i + 1].x << " " << bezierPts[i + 1].y;
				fTarget << " " << bezierPts[i + 2].x << " " << bezierPts[i + 2].y;
			} else {
				// normal cubic bezier
				fTarget << " C" << bezierPts[i].x << " " << bezierPts[i].y;
				fTarget << " " << bezierPts[i + 1].x << " " << bezierPts[i + 1].y;
				fTarget << " " << bezierPts[i + 2].x << " " << bezierPts[i + 2].y;
			}
		}
	}
	fLast = NULL;
		
	return B_OK;
}

status_t
BSVGDataCreator::IterateClose()
{
	fTarget << " Z";
	fLast = NULL;
	
	return B_OK;
}

// *******************************************************************

BSVGPolyDataCreator::BSVGPolyDataCreator(BString &target)
	:	BSVGDataCreator(target)
{
}

BSVGPolyDataCreator::~BSVGPolyDataCreator()
{
}

status_t
BSVGPolyDataCreator::IterateMoveTo(BPoint *point)
{
	// should only happen once
	fTarget << point->x << "," << point->y;
	
	return B_OK;
}

status_t
BSVGPolyDataCreator::IterateLineTo(int32 lineCount, BPoint *linePts)
{
	for (int i = 0; i < lineCount; i++)
		fTarget << " " << linePts[i].x << "," << linePts[i].y;
	
	return B_OK;
}

status_t
BSVGPolyDataCreator::IterateClose()
{
	// no-op
	return B_OK;
}
