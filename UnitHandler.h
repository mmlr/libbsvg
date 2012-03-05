#ifndef _UNIT_HANDLER_H_
#define _UNIT_HANDLER_H_

#include "SVGView.h"
#include "SVGState.h"

class UnitHandler
{
	public:
					UnitHandler();

static	bool		Handle(attribute_s *attr, float *output, SVGState *state = NULL, float percentage_relation = 100);
static	bool		Handle(attribute_s *attr, SourcedValue<float> *output);
};

#endif
