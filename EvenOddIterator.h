#ifndef _EVENODD_ITERATOR_H_
#define _EVENODD_ITERATOR_H_

#include <Shape.h>
#include <Point.h>
#include <Rect.h>

class BBitmap;
class BView;

class EvenOddIterator : public BShapeIterator
{

public:
						EvenOddIterator(BRect frame);
						~EvenOddIterator();
						
		void			SetOutput(BView *output);
		void			SetOffset(float x, float y);
		BRect			Bounds();	
						
virtual	status_t		IterateMoveTo(BPoint *point);
virtual	status_t		IterateLineTo(int32 count, BPoint *pts);
virtual	status_t		IterateBezierTo(int32 count, BPoint *pts);
virtual	status_t		IterateClose();

		status_t		Iterate(BShape *shape);

private:
		BBitmap			*fBitmap;
		BView			*fView;
		BView			*fOutput;
		BPoint			fLocation;
		BPoint			fStart;
		BPoint			fOffset;
};

#endif
