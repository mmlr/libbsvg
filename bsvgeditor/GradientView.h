#ifndef _GRADIENT_VIEW_H_
#define _GRADIENT_VIEW_H_

#include <OutlineListView.h>

class GradientView : public BOutlineListView {
	public:
						GradientView(BRect frame, const char *name);
						
virtual	void			KeyDown(const char *bytes, int32 numBytes);
};

#endif
