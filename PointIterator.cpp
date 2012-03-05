#include "PointIterator.h"
#include "DistanceIterator.h"

#define P2(x)		((x) * (x))
#define P3(x)		((x) * (x) * (x))
#define LENGTH(p)	(sqrt(P2(p.x) + P2(p.y)))
#define THRESHOLD	0.001

PointIterator::PointIterator(const FloatList *offsets, PointList *result)
	:	fCount(0),
		fIndex(0),
		fResult(result),
		fOffsets(offsets),
		fDistance(20),
		fStart(0, 0)
{
}

status_t
PointIterator::IterateMoveTo(BPoint *point)
{
	fStart = *point;
	return B_OK;
}

status_t
PointIterator::IterateLineTo(int32 count, BPoint *pts)
{
	for (int i = 0; i < count; i++) {
		distance_s *dist = fDistance.ItemAt(fCount++);
		float have_offset = dist->offset + dist->length;
		
		while (true) {
			if (fIndex >= fOffsets->CountItems()) break;
			float want_offset = *fOffsets->ItemAt(fIndex);
			
			if (have_offset > want_offset) {
				SVGPoint delta = pts[i] - dist->from;
				float offset = (want_offset - dist->offset) / dist->length;
				delta *= offset;
				delta += dist->from;
				fResult->AddItem(new BPoint(delta.x, delta.y));
				fIndex++;
			} else
				break;
		}
	}
	
	return B_OK;
}

status_t
PointIterator::IterateBezierTo(int32 count, BPoint *pts)
{
	for (int i = 0; i < count; i++) {
		distance_s *dist = fDistance.ItemAt(fCount++);
		float have_offset = dist->offset + dist->length;
		
		while (true) {
			if (fIndex >= fOffsets->CountItems()) break;
			float want_offset = *fOffsets->ItemAt(fIndex);
			
			if (have_offset > want_offset) {
				float offset = (want_offset - dist->offset) / dist->length;
				fResult->AddItem(BezierPoint(offset, &dist->from, &pts[i * 3]));
				fIndex++;
			} else
				break;
		}
	}
	
	return B_OK;
}

BPoint *
PointIterator::BezierPoint(float offset, BPoint *from, BPoint control[3])
{
	float parts[4];
	parts[0] = P3(1 - offset);
	parts[1] = 3 * offset * P2(1 - offset);
	parts[2] = 3 * P2(offset) * (1 - offset);
	parts[3] = P3(offset);
	
	BPoint *result = new BPoint();
	result->x = from->x * parts[0] + control[0].x * parts[1] + control[1].x * parts[2] + control[2].x * parts[3];
	result->y = from->y * parts[0] + control[0].y * parts[1] + control[1].y * parts[2] + control[2].y * parts[3];
	return result;
}

status_t
PointIterator::IterateClose()
{
	IterateLineTo(1, &fStart);
	return B_OK;
}

status_t
PointIterator::Iterate(BShape *shape)
{
	fCount = 0;
	fIndex = 0;
	fStart.Set(0, 0);
	fResult->MakeEmpty();
	
	DistanceIterator iterator(&fDistance);
	iterator.Iterate(shape);
	
	BShapeIterator::Iterate(shape);
	return B_OK;
}
