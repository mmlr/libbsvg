#include "UnitHandler.h"

UnitHandler::UnitHandler()
{
}

bool
UnitHandler::Handle(attribute_s *attr, float *output, SVGState *state, float percentage_relation)
{
	float x = 0;
	char buffer[2];
	
	int result = sscanf(attr->value.String(), "%f%c%c", &x, buffer + 0, buffer + 1);
	if (result == 1) {
		*output = x;
		return true;
	
	} else if (result == 2) {
		// we have to work against sscanf here. it understands the e of em and
		// ex as the e in 1.0e2 for example and cuts it off.
		if (buffer[0] == '%')
			x = x / 100 * percentage_relation;
		else if (buffer[0] == 'm')
			x = x * (state ? state->ResolveValue(state->font_size()) : 12);
		else if (buffer[0] == 'x')
			x = x * (state ? state->ResolveValue(state->font_size()) / 2 : 6);
		
		*output = x;
		return true;
	
	} else if (result == 3) {
		switch (buffer[0]) {
			case 'p':
				if (buffer[1] == 'x')
					;// nothing
				else if (buffer[1] == 't')
					x = x / 72 * DEFAULT_DPI;
				else if (buffer[1] == 'c')
					x = x / 72 * DEFAULT_DPI * 12;
				break;
			case 'm':
				if (buffer[1] == 'm')
					x = x / 25.4 * DEFAULT_DPI;
				break;
			case 'c':
				if (buffer[1] == 'm')
					x = x / 2.54 * DEFAULT_DPI;
				break;
			case 'i':
				if (buffer[1] == 'n')
					x = x * DEFAULT_DPI;
				break;
			default: break;
		}
		
		*output = x;
		return true;
	}
	
	return false;
}

bool
UnitHandler::Handle(attribute_s *attr, SourcedValue<float> *output)
{
	float x = 0;
	char buffer[2];
	
	int result = sscanf(attr->value.String(), "%f%c%c", &x, buffer + 0, buffer + 1);
	if (result == 1) {
		output->unit = PX_UNIT;
		output->value = x;
		return true;
	
	} else if (result == 2) {
		// we have to work against sscanf here. it understands the e of em and
		// ex as the e in 1.0e2 for example and cuts it off.
		if (buffer[0] == '%')
			output->unit = PE_UNIT;
		else if (buffer[0] == 'm')
			output->unit = EM_UNIT;
		else if (buffer[0] == 'x')
			output->unit = EX_UNIT;
		
		output->value = x;
		return true;
	
	} else if (result == 3) {
		switch (buffer[0]) {
			case 'p':
				if (buffer[1] == 'x')
					output->unit = PX_UNIT;
				else if (buffer[1] == 't')
					output->unit = PT_UNIT;
				else if (buffer[1] == 'c')
					output->unit = PC_UNIT;
				break;
			case 'm':
				if (buffer[1] == 'm')
					output->unit = MM_UNIT;
				break;
			case 'c':
				if (buffer[1] == 'm')
					output->unit = CM_UNIT;
				break;
			case 'i':
				if (buffer[1] == 'n')
					output->unit = IN_UNIT;
				break;
			default: break;
		}
		
		output->value = x;
		return true;
	}
	
	return false;
}
