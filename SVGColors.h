#ifndef _SVG_COLORS_H_
#define _SVG_COLORS_H_

#include <GraphicsDefs.h>
#include <Message.h>
#include <String.h>
#include "SVGDefs.h"

paint_t		HandleAttributeValue(const BString *value, target_t target, SVGState *current_state, SVGColor &color, BString &server, bool first = true);
bool		GetColorFromString(const BString *value, SVGColor &result);
bool		ExtractServerName(const BString *value, BString &result);
void		SplitData(BString *data, BString *input, int indentlevel);
				// split the new data to chunks of EXPORT_MAX_LINE_WIDTH size

#endif
