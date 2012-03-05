#ifndef _ELEMENT_VIEW_H_
#define _ELEMENT_VIEW_H_

#include <OutlineListView.h>

class ElementView : public BOutlineListView {
	public:
						ElementView(BRect frame, const char *name);
						
virtual	void			KeyDown(const char *bytes, int32 numBytes);
};

#endif
