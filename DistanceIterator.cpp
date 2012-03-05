#include "DistanceIterator.h"

#define P2(x)		((x) * (x))
#define P3(x)		((x) * (x) * (x))
#define LENGTH(p)	(sqrt(P2(p.x) + P2(p.y)))
#define THRESHOLD	0.001

DistanceIterator::DistanceIterator(DistanceList *result)
	:	fLength(0),
		fListResult(result),
		fFloatResult(NULL),
		fLocation(0, 0),
		fStart(0, 0),
		fControl(NULL)
{
}

DistanceIterator::DistanceIterator(float *result)
	:	fLength(0),
		fListResult(NULL),
		fFloatResult(result),
		fLocation(0, 0),
		fStart(0, 0),
		fControl(NULL)
{
}

status_t
DistanceIterator::IterateMoveTo(BPoint *point)
{
	fLocation = *point;
	fStart = fLocation;
	return B_OK;
}

status_t
DistanceIterator::IterateLineTo(int32 count, BPoint *pts)
{
	for (int i = 0; i < count; i++) {
		BPoint delta = fLocation - pts[i];
		
		float length = LENGTH(delta);
		
		if (fListResult)
			fListResult->AddItem(new distance_s(fLocation, fLength, length));
		
		fLength += length;
		
		fLocation = pts[i];
	}
	
	return B_OK;
}

status_t
DistanceIterator::IterateBezierTo(int32 count, BPoint *pts)
{
	for (int i = 0; i < count; i++) {
		BPoint delta = fLocation - pts[i * 3 + 2];
		float len = LENGTH(delta);
		fControl = &pts[i * 3];
		
		float length = 0;
		length += ApproximateRecursive(0.0, 0.5, len / 4);
		length += ApproximateRecursive(0.5, 1.0, len / 4);
		
		if (fListResult)
			fListResult->AddItem(new distance_s(fLocation, fLength, length));
		
		fLength += length;
		
		fLocation = pts[i * 3 + 2];
	}
	
	return B_OK;
}

float
DistanceIterator::ApproximateRecursive(float from, float to, float lastlength)
{
	float parts[4];
	parts[0] = P3(1 - from);
	parts[1] = 3 * from * P2(1 - from);
	parts[2] = 3 * P2(from) * (1 - from);
	parts[3] = P3(from);
	
	BPoint start;
	start.x = fLocation.x * parts[0] + fControl[0].x * parts[1] + fControl[1].x * parts[2] + fControl[2].x * parts[3];
	start.y = fLocation.y * parts[0] + fControl[0].y * parts[1] + fControl[1].y * parts[2] + fControl[2].y * parts[3];
	
	parts[0] = P3(1 - to);
	parts[1] = 3 * to * P2(1 - to);
	parts[2] = 3 * P2(to) * (1 - to);
	parts[3] = P3(to);
	
	BPoint end;
	end.x = fLocation.x * parts[0] + fControl[0].x * parts[1] + fControl[1].x * parts[2] + fControl[2].x * parts[3];
	end.y = fLocation.y * parts[0] + fControl[0].y * parts[1] + fControl[1].y * parts[2] + fControl[2].y * parts[3];
	
	BPoint delta(end.x - start.x, end.y - start.y);
	
	float newlength = 0;
	if (delta.x != 0 || delta.y != 0)
		newlength = LENGTH(delta);
	
	float percentage = 1 - lastlength / newlength;
	if (percentage < 0) percentage *= -1;
	
	if (percentage > 0 && percentage <= THRESHOLD)
		return newlength;
	
	float offset = (to - from) / 2;
	if (offset <= THRESHOLD)
		return newlength;
	
	newlength /= 2;
	return ApproximateRecursive(from, from + offset, newlength) + ApproximateRecursive(from + offset, to, newlength);
}

status_t
DistanceIterator::IterateClose()
{
	IterateLineTo(1, &fStart);
	return B_OK;
}

status_t
DistanceIterator::Iterate(BShape *shape)
{
	fLength = 0;
	fStart.Set(0, 0);
	fLocation = fStart;
	
	if (fListResult)
		fListResult->MakeEmpty();
	
	BShapeIterator::Iterate(shape);
	
	if (fFloatResult)
		*fFloatResult = fLength;
	
	return B_OK;
}
