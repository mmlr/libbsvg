#ifndef _SVG_STOP_H_
#define _SVG_STOP_H_

#include <Archivable.h>
#include <GraphicsDefs.h>
#include <Message.h>
#include "SVGElement.h"

class BSVGStop : public BSVGElement {

public:
					BSVGStop();
					BSVGStop(BSVGStop *stop);
					BSVGStop(rgb_color color, float offset, float opacity = 1.0);
					BSVGStop(uchar r, uchar g, uchar b, float offset, float opacity = 1.0);
						// offset is a value from 0 to 1; it represents
						// a percentage of the gradient's vector

virtual				~BSVGStop();

					BSVGStop(BMessage *data);
static	BArchivable	*Instantiate(BMessage *data);
virtual	status_t	Archive(BMessage *data, bool deep = true) const;

virtual	element_t	Type() { return B_SVG_STOP; };

virtual	void		HandleAttribute(attribute_s *attr);
virtual	void		RecreateData();

		void		SetOffset(float offset);
		float		Offset() const;

		bool		IsOpaque() const;

private:
		float		fOffset;
};

#endif // STOP_H_
