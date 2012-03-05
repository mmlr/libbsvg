#include <stdio.h>

#include "SVGColors.h"
#include "SVGState.h"

#define ifcomp(x, y)	if (value->Compare(x) == 0) { y; return true; }

paint_t
HandleAttributeValue(const BString *value, target_t target, SVGState *state, SVGColor &color, BString &server, bool first)
{
	if (value->Compare("none") == 0) {
		return B_FILL_NONE;
	
	} else if (value->Compare("inherit") == 0 && state) {
		switch (target) {
			case B_TARGET_FILL: {
				switch (state->fill_type()) {
					case B_FILL_COLOR: color = state->fill_color; break;
					case B_FILL_CURRENT_COLOR: color = state->general_color; break;
					case B_FILL_SERVER: server = state->fill_server.String(); break;
					default: break;
				}
			} break;
			
			case B_TARGET_STROKE: {
				switch (state->stroke_type()) {
					case B_FILL_COLOR: color = state->stroke_color; break;
					case B_FILL_CURRENT_COLOR: color = state->general_color; break;
					case B_FILL_SERVER: server = state->stroke_server.String(); break;
					default: break;
				}
			} break;
			
			case B_TARGET_STOP: {
				color = state->stop_color;
				return B_FILL_COLOR;
			} break;
			
			case B_TARGET_GENERAL: {
				color = state->general_color;
				return B_FILL_COLOR;
			}
		}
		return state->fill_type();
	
	} else if (value->Compare("currentColor") == 0 && state) {
		color = state->general_color;
		return B_FILL_CURRENT_COLOR;
	
	} else if (value->IFindFirst("url(") >= 0) {
		if (ExtractServerName(value, server))
			return B_FILL_SERVER;
	
	} else {
		if (GetColorFromString(value, color))
			return B_FILL_COLOR;
	}
	
	// if we get here everything failed, probably a malformed value
	// use inherited values in that case
	if (!first) {
		// but break out if this is the second try already
		return B_FILL_UNKNOWN;
	} else {
		BString inherit = "inherit";
		return HandleAttributeValue(&inherit, target, state, color, server, false);
	}
}

bool
GetColorFromString(const BString *value, SVGColor &result)
{
	switch (value->String()[0]) {
		case 'a':
		case 'A':
			ifcomp("aliceblue", result(240, 248, 255));
			ifcomp("antiquewhite", result(250, 235, 215));
			ifcomp("aqua", result(0, 255, 255));
			ifcomp("aquamarine", result(127, 255, 212));
			ifcomp("azure", result(240, 255, 255));
		case 'b':
		case 'B':
			ifcomp("beige", result(245, 245, 220));
			ifcomp("bisque", result(255, 228, 196));
			ifcomp("black", result(0, 0, 0));
			ifcomp("blanchedalmond", result(255, 235, 205));
			ifcomp("blue", result(0, 0, 255));
			ifcomp("blueviolet", result(138, 43, 226));
			ifcomp("brown", result(165, 42, 42));
			ifcomp("burlywood", result(222, 184, 135));
		case 'c':
		case 'C':
			ifcomp("cadetblue", result(95, 158, 160));
			ifcomp("chartreuse", result(127, 255, 0));
			ifcomp("chocolate", result(210, 105, 30));
			ifcomp("coral", result(255, 127, 80));
			ifcomp("cornflowerblue", result(100, 149, 237));
			ifcomp("cornsilk", result(255, 248, 220));
			ifcomp("crimson", result(220, 20, 60));
			ifcomp("cyan", result(0, 255, 255));
		case 'd':
		case 'D':
			ifcomp("darkblue", result(0, 0, 139));
			ifcomp("darkcyan", result(0, 139, 139));
			ifcomp("darkgoldenrod", result(184, 134, 11));
			ifcomp("darkgray", result(169, 169, 169));
			ifcomp("darkgreen", result(0, 100, 0));
			ifcomp("darkgrey", result(169, 169, 169));
			ifcomp("darkkhaki", result(189, 183, 107));
			ifcomp("darkmagenta", result(139, 0, 139));
			ifcomp("darkolivegreen", result(85, 107, 47));
			ifcomp("darkorange", result(255, 140, 0));
			ifcomp("darkorchid", result(153, 50, 204));
			ifcomp("darkred", result(139, 0, 0));
			ifcomp("darksalmon", result(233, 150, 122));
			ifcomp("darkseagreen", result(143, 188, 143));
			ifcomp("darkslateblue", result(72, 61, 139));
			ifcomp("darkslategray", result(47, 79, 79));
			ifcomp("darkslategrey", result(47, 79, 79));
			ifcomp("darkturquoise", result(0, 206, 209));
			ifcomp("darkviolet", result(148, 0, 211));
			ifcomp("deeppink", result(255, 20, 147));
			ifcomp("deepskyblue", result(0, 191, 255));
			ifcomp("dimgray", result(105, 105, 105));
			ifcomp("dimgrey", result(105, 105, 105));
			ifcomp("dodgerblue", result(30, 144, 255));
		case 'f':
		case 'F':
			ifcomp("firebrick", result(178, 34, 34));
			ifcomp("floralwhite", result(255, 250, 240));
			ifcomp("forestgreen", result(34, 139, 34));
			ifcomp("fuchsia", result(255, 0, 255));
		case 'g':
		case 'G':
			ifcomp("gainsboro", result(220, 220, 220));
			ifcomp("ghostwhite", result(248, 248, 255));
			ifcomp("gold", result(255, 215, 0));
			ifcomp("goldenrod", result(218, 165, 32));
			ifcomp("gray", result(128, 128, 128));
			ifcomp("grey", result(128, 128, 128));
			ifcomp("green", result(0, 128, 0));
			ifcomp("greenyellow", result(173, 255, 47));
		case 'h':
		case 'H':
			ifcomp("honeydew", result(240, 255, 240));
			ifcomp("hotpink", result(255, 105, 180));
		case 'i':
		case 'I':
			ifcomp("indianred", result(205, 92, 92));
			ifcomp("indigo", result(75, 0, 130));
			ifcomp("ivory", result(255, 255, 240));
		case 'k':
		case 'K':
			ifcomp("khaki", result(240, 230, 140));
		case 'l':
		case 'L':
			ifcomp("lavender", result(230, 230, 250));
			ifcomp("lavenderblush", result(255, 240, 245));
			ifcomp("lawngreen", result(124, 252, 0));
			ifcomp("lemonchiffon", result(255, 250, 205));
			ifcomp("lightblue", result(173, 216, 230));
			ifcomp("lightcoral", result(240, 128, 128));
			ifcomp("lightcyan", result(224, 255, 255));
			ifcomp("lightgoldenrodyellow", result(250, 250, 210));
			ifcomp("lightgray", result(211, 211, 211));
			ifcomp("lightgreen", result(144, 238, 144));
			ifcomp("lightgrey", result(211, 211, 211));
			ifcomp("lightpink", result(255, 182, 193));
			ifcomp("lightsalmon", result(255, 160, 122));
			ifcomp("lightseagreen", result(32, 178, 170));
			ifcomp("lightskyblue", result(135, 206, 250));
			ifcomp("lightslategray", result(119, 136, 153));
			ifcomp("lightslategrey", result(119, 136, 153));
			ifcomp("lightsteelblue", result(176, 196, 222));
			ifcomp("lightyellow", result(255, 255, 224));
			ifcomp("lime", result(0, 255, 0));
			ifcomp("limegreen", result(50, 205, 50));
			ifcomp("linen", result(250, 240, 230));
		case 'm':
		case 'M':
			ifcomp("magenta", result(255, 0, 255));
			ifcomp("maroon", result(128, 0, 0));
			ifcomp("mediumaquamarine", result(102, 205, 170));
			ifcomp("mediumblue", result(0, 0, 205));
			ifcomp("mediumorchid", result(186, 85, 211));
			ifcomp("mediumpurple", result(147, 112, 219));
			ifcomp("mediumseagreen", result(60, 179, 113));
			ifcomp("mediumslateblue", result(123, 104, 238));
			ifcomp("mediumspringgreen", result(0, 250, 154));
			ifcomp("mediumturquoise", result(72, 209, 204));
			ifcomp("mediumvioletred", result(199, 21, 133));
			ifcomp("midnightblue", result(25, 25, 112));
			ifcomp("mintcream", result(245, 255, 250));
			ifcomp("mistyrose", result(255, 228, 225));
			ifcomp("moccasin", result(255, 228, 181));
		case 'n':
		case 'N':
			ifcomp("navajowhite", result(255, 222, 173));
			ifcomp("navy", result(0, 0, 128));
		case 'o':
		case 'O':
			ifcomp("oldlace", result(253, 245, 230));
			ifcomp("olive", result(128, 128, 0));
			ifcomp("olivedrab", result(107, 142, 35));
			ifcomp("orange", result(255, 165, 0));
			ifcomp("orangered", result(255, 69, 0));
			ifcomp("orchid", result(218, 112, 214));
		case 'p':
		case 'P':
			ifcomp("palegoldenrod", result(238, 232, 170));
			ifcomp("palegreen", result(152, 251, 152));
			ifcomp("paleturquoise", result(175, 238, 238));
			ifcomp("palevioletred", result(219, 112, 147));
			ifcomp("papayawhip", result(255, 239, 213));
			ifcomp("peachpuff", result(255, 218, 185));
			ifcomp("peru", result(205, 133, 63));
			ifcomp("pink", result(255, 192, 203));
			ifcomp("plum", result(221, 160, 221));
			ifcomp("powderblue", result(176, 224, 230));
			ifcomp("purple", result(128, 0, 128));
		case 'r':
		case 'R':
			ifcomp("red", result(255, 0, 0));
			ifcomp("rosybrown", result(188, 143, 143));
			ifcomp("royalblue", result(65, 105, 225));
		case 's':
		case 'S':
			ifcomp("saddlebrown", result(139, 69, 19));
			ifcomp("salmon", result(250, 128, 114));
			ifcomp("sandybrown", result(244, 164, 96));
			ifcomp("seagreen", result(46, 139, 87));
			ifcomp("seashell", result(255, 245, 238));
			ifcomp("sienna", result(160, 82, 45));
			ifcomp("silver", result(192, 192, 192));
			ifcomp("skyblue", result(135, 206, 235));
			ifcomp("slateblue", result(106, 90, 205));
			ifcomp("slategray", result(112, 128, 144));
			ifcomp("slategrey", result(112, 128, 144));
			ifcomp("snow", result(255, 250, 250));
			ifcomp("springgreen", result(0, 255, 127));
			ifcomp("steelblue", result(70, 130, 180));
		case 't':
		case 'T':
			ifcomp("tan", result(210, 180, 140));
			ifcomp("teal", result(0, 128, 128));
			ifcomp("thistle", result(216, 191, 216));
			ifcomp("tomato", result(255, 99, 71));
			ifcomp("turquoise", result(64, 224, 208));
		case 'v':
		case 'V':
			ifcomp("violet", result(238, 130, 238));
		case 'w':
		case 'W':
			ifcomp("wheat", result(245, 222, 179));
			ifcomp("white", result(255, 255, 255));
			ifcomp("whitesmoke", result(245, 245, 245));
		case 'y':
		case 'Y':
			ifcomp("yellow", result(255, 255, 0));
			ifcomp("yellowgreen", result(154, 205, 50));
	}
	
	if (value->IFindFirst("rgb(") >= 0) {
		BString temp = value->String();
		temp.ReplaceAll("rgb(", "");
		temp.ReplaceAll(",", " ");
		
		float red = 0;
		float green = 0;
		float blue = 0;
		if (sscanf(temp.String(), "%f %f %f", &red, &green, &blue) == 3) {
			result(red, green, blue);
			return true;
		} else if (sscanf(temp.String(), "%f%% %f%% %f%%", &red, &green, &blue) == 3) {
			result(255 * (red / 100), 255 * (green / 100), 255 * (blue / 100));
			return true;
		}
	}
	
	if (value->IFindFirst("#") >= 0) {
		BString temp = value->String();
		temp.ReplaceAll("#", "");
		temp.ReplaceAll(" ", "");

		uint32 color = B_TRANSPARENT_MAGIC_RGBA32;
		if (sscanf(temp.String(), "%x", (unsigned int *)&color) == 1) {
			if (temp.Length() == 3) {
				rgb_color rgbColor;
				rgbColor.red = color >> 8 & 0x0f;
				rgbColor.red += rgbColor.red << 4;
				rgbColor.green = color >> 4 & 0x0f;
				rgbColor.green += rgbColor.green << 4;
				rgbColor.blue = color >> 0 & 0x0f;
				rgbColor.blue += rgbColor.blue << 4;
				result(rgbColor);
			} else
				result((char)(color >> 16), (char)(color >> 8), (char)color);

			return true;
		}
	}	
	
	return false;
}

bool
ExtractServerName(const BString *value, BString &result)
{
	result = value->String();
	int pos = result.FindFirst('#');
	if (pos < 0)
		return false;
	
	result.Remove(0, pos + 1);
	pos = result.FindFirst(')');
	if (pos < 0)
		return false;
	
	result.Remove(pos, result.Length() - pos);
	return true;
}

void
SplitData(BString *data, BString *input, int indentlevel)
{
	int32 length = data->Length() - data->FindLast('\n');
	int32 available = EXPORT_MAX_LINE_WIDTH - length + indentlevel - indentlevel * 4;
	int32 pos = 0;
	int32 next = 0;
	char last = 1;
	
	indentlevel++;
	int32 availperline = MAX(EXPORT_MAX_LINE_WIDTH - indentlevel * 4, EXPORT_MIN_LINE_WIDTH);
	if (available <= 0) {
		*data << "\n";
		INDENT(*data, indentlevel);
		available = availperline;
	}
	
	while (last != 0) {
		pos = input->FindFirst(' ', available);
		if (pos > (next = input->FindFirst('\t', available)) && next >= 0) pos = next;
		if ((next = input->FindFirst('\n', 0)) >= 0) pos = next;
		
		if (pos < 0)
			pos = input->Length();
		
		data->Append(*input, pos);
		*data << "\n";
		
		input->Remove(0, pos);
		last = input->String()[0];
		if (input->Length() > 0) input->Remove(0, 1);
		
		if (last != 0) INDENT(*data, indentlevel);
		available = availperline;
	}
	
	data->RemoveLast("\n");
	data->Append(*input);
}
