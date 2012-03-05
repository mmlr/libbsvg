#ifndef _SVG_DEFS_H_
#define _SVG_DEFS_H_

#include <InterfaceDefs.h>
#include <String.h>
#include <Rect.h>
#include <Archivable.h>
#include "ObjectList.h"

// defines
#define EXPORT_LIB_VERSION		"0.5.0"
#define EXPORT_MAX_LINE_WIDTH	85
#define EXPORT_MIN_LINE_WIDTH	20
#define DEFAULT_DPI				90

// text macros
#define INDENT(string, level)		for (int _indent_ = 0; _indent_ < level; _indent_++) string << "\t";

// math macros
#define CLAMP(val, min, max)	(val < min ? min : (val > max ? max : val))

// archive macros
#define ADDREPADDON(x)	if (data->HasString("add_on")) data->ReplaceString("add_on", x); else data->AddString("add_on", x);
#define ADDREPCLASS(x)	if (data->HasString("class")) data->ReplaceString("class", x); else data->AddString("class", x);
#define ADDREPTYPE(x)	if (data->HasInt32("_type")) data->ReplaceInt32("_type", x); else data->AddInt32("_type", x);
#define ARCHIVELIST(w, x, y, z) { for (int i = 0; i < w.CountItems(); i++) x->y(z, *w.ItemAt(i)); }
#define UNARCHIVELIST(w, x, y, z, v) { int i = 0; v temp; while (x->y(z, i++, &temp) == B_OK) w.AddItem(new v(temp)); }
#define FINALIZEIF(x)	int32 type; if (data->FindInt32("_type", &type) == B_OK && type == x) FinalizeShape(); ApplyTransformation();

// prototypes
class BSVGElement;
class BSVGDocument;
class BSVGRoot;
class BSVGGroup;
class BSVGPath;
class BSVGRect;
class BSVGCircle;
class BSVGEllipse;
class BSVGLine;
class BSVGPolyline;
class BSVGPolygon;
class BSVGText;
class BSVGTextSpan;
class BSVGTextPath;
class BSVGGradient;
class BSVGLinearGradient;
class BSVGRadialGradient;
class BSVGStop;
class BSVGPattern;
class BPoint;
class DependentObject;
class SVGState;
class Matrix2D;
struct charpos_s;
struct distance_s;
struct attribute_s;

// typedefs
typedef BObjectList<bool>				BoolList;
typedef BObjectList<float>				FloatList;
typedef BObjectList<BPoint>				PointList;
typedef BObjectList<BString>			StringList;
typedef BObjectList<BSVGElement>		ElementList;
typedef BObjectList<DependentObject>	DependentList;
typedef BObjectList<SVGState>			StateList;

typedef	BObjectList<attribute_s>		AttributeList;
typedef BObjectList<charpos_s>			CharposList;
typedef BObjectList<distance_s>			DistanceList;

// enums
enum element_t {
	B_SVG_ELEMENT		= 0x10000001,	// virtual element
	B_SVG_DOCUMENT		= 0x10000002,	// virtual element
	B_SVG_ROOT			= 0x10000004,	// virtual element
	B_SVG_GROUP			= 0x10000008,
	B_SVG_DEFS			= 0x10000010,
	B_SVG_PATTERN		= 0x10000020,
	B_SVG_USE			= 0x10000040,
	B_SVG_PATH			= 0x20000080,
	B_SVG_RECT			= 0x20000100,
	B_SVG_CIRCLE		= 0x20000200,
	B_SVG_ELLIPSE		= 0x20000400,
	B_SVG_LINE			= 0x20000800,
	B_SVG_POLYLINE		= 0x20001000,
	B_SVG_POLYGON		= 0x20002000,
	B_SVG_TEXT			= 0x40004000,
	B_SVG_TEXTSPAN		= 0x40008000,
	B_SVG_TEXTPATH		= 0x40010000,
	B_SVG_STRING		= 0x40020000,	// virtual element
	B_SVG_GRADIENT		= 0x80040000,	// virtual element
	B_SVG_LINEARGRAD	= 0x80080000,
	B_SVG_RADIALGRAD	= 0x80100000,
	B_SVG_STOP			= 0x80200000,
	B_SVG_PAINTSERVER	= 0x00400000	// virtual element
};

enum element_kind_t {
	B_SVG_NO_KIND			= 0x00000000,
	B_SVG_CONTAINER_KIND	= 0x10000000,
	B_SVG_SHAPE_KIND		= 0x20000000,
	B_SVG_TEXT_KIND			= 0x40000000,
	B_SVG_GRADIENT_KIND		= 0x80000000
};

enum resolve_t {
	B_STATUS_UNRESOLVED = 0,
	B_STATUS_RESOLVED,
	B_STATUS_ERROR
};

enum paint_t {
	B_FILL_UNKNOWN = -1,
	B_FILL_NONE = 0,
	B_FILL_COLOR,
	B_FILL_CURRENT_COLOR,
	B_FILL_SERVER
};

enum rule_t {
	B_NONZERO = 0,
	B_EVENODD
};

enum target_t {
	B_TARGET_FILL = 0,
	B_TARGET_STROKE,
	B_TARGET_STOP,
	B_TARGET_GENERAL
};

enum units_t {
	B_OBJECT_BOUNDING_BOX = 0,
	B_USERSPACE_ON_USE
};

enum spread_t {
	B_SPREAD_PAD = 0,
	B_SPREAD_REFLECT,
	B_SPREAD_REPEAT
};

enum decoration_t {
	B_NO_DECORATION = 0x00,
	B_UNDERLINE_DECORATION = 0x01,
	B_OVERLINE_DECORATION = 0x02,
	B_STRIKEOUT_DECORATION = 0x04,
	B_BLINK_DECORATION = 0x08
};

enum state_parts_t {
	FILL_PART = 0x0001,
	STROKE_PART = 0x0002,
	STOP_PART = 0x0004,
	GENERAL_PART = 0x0008,
	FONT_PART = 0x0010,
	ALL_PARTS = 0xffff
};

enum dependent_t {
	DEPENDENT_PARENT = 0,
	DEPENDENT_START,
	DEPENDENT_END
};

enum source_t {
	SOURCE_UNSET = 0,
	SOURCE_INHERIT,
	SOURCE_SET
};

enum unit_t {
	// absolute units
	ABS_UNITS = 0x00000000,
	PX_UNIT	  = 0x00000001,	// 1 pixel = 1 user unit
	PT_UNIT   = 0x00000002,	// 1 point = 1 / 72th of an inch
	PC_UNIT   = 0x00000004,	// 1 pica = 12 points
	CM_UNIT   = 0x00000008,	// centimeters
	MM_UNIT   = 0x00000010,	// millimeters
	IN_UNIT   = 0x00000020,	// 1 inches = 25.4 mm
	// relative units
	REL_UNITS = 0x10000000,
	EM_UNIT   = 0x10000001,	// the current font size
	EX_UNIT   = 0x10000002,	// the current fonts x-height
	PE_UNIT   = 0x10000004  // percentage (%)
};

enum field_t {
	FIELD_FILL_TYPE = 1,
	FIELD_FILL_COLOR,
	FIELD_FILL_SERVER,
	FIELD_FILL_OPACITY,
	FIELD_FILL_RULE,
	FIELD_STROKE_TYPE,
	FIELD_STROKE_COLOR,
	FIELD_STROKE_SERVER,
	FIELD_STROKE_OPACITY,
	FIELD_STROKE_WIDTH,
	FIELD_STROKE_LINECAP,
	FIELD_STROKE_LINEJOIN,
	FIELD_STROKE_MITERLIMIT,
	FIELD_STOP_COLOR,
	FIELD_STOP_OPACITY,
	FIELD_GENERAL_COLOR,
	FIELD_GENERAL_OPACITY,
	FIELD_GENERAL_TRANSFORM,
	FIELD_GENERAL_VIEWPORT,
	FIELD_GENERAL_VIEWBOX,
	FIELD_GENERAL_ASPECT,
	FIELD_FONT_FAMILY,
	FIELD_FONT_SIZE,
	FIELD_FONT_KERNING,
	FIELD_FONT_LETTERSPACING,
	FIELD_FONT_WORDSPACING,
	FIELD_FONT_DECORATION,
	FIELD_TEXT_ANCHOR,
	FIELD_START_OFFSET
};

enum offset_t {
	B_OFFSET_X = 0,
	B_OFFSET_Y,
	B_OFFSET_DX,
	B_OFFSET_DY,
	B_OFFSET_R
};

enum anchor_t {
	B_ANCHOR_START = 0,
	B_ANCHOR_MIDDLE,
	B_ANCHOR_END
};

enum progress_t {
	B_PROGRESS_LR_TB = 0,
	B_PROGRESS_RL_TB,
	B_PROGRESS_TB_RL,
	B_PROGRESS_TB_LR
};

enum aspect_t {
	ASPECT_NONE = 0x0000,
	ASPECT_XMIN = 0x0001,
	ASPECT_XMID = 0x0002,
	ASPECT_XMAX = 0x0004,
	ASPECT_YMIN = 0x0010,
	ASPECT_YMID = 0x0020,
	ASPECT_YMAX = 0x0040,
	ASPECT_MEET = 0x0100,
	ASPECT_SLICE = 0x0200
};

enum merge_t {
	B_MULTIPLY_BY_SOURCE = 0x0001,
	B_MULTIPLY_BY_DEST = 0x0002,
	B_ADD_VIEWPORT = 0x0010,
	B_ADD_VIEWPORT_IF_SET = 0x0020
};

inline void
fast_memset(int32 length, uint32 value, uint32 *buffer)
{
	for (int32 i = 0; i < length; i++)
		buffer[i] = value;
}

inline const char *
element_name(element_t element)
{
	switch (element) {
		case B_SVG_ELEMENT: return "untyped";
		case B_SVG_DOCUMENT: return "document";
		case B_SVG_ROOT: return "svg";
		case B_SVG_GROUP: return "g";
		case B_SVG_DEFS: return "defs";
		case B_SVG_PATTERN: return "pattern";
		case B_SVG_USE: return "use";
		case B_SVG_PATH: return "path";
		case B_SVG_RECT: return "rect";
		case B_SVG_CIRCLE: return "circle";
		case B_SVG_ELLIPSE: return "ellipse";
		case B_SVG_LINE: return "line";
		case B_SVG_POLYLINE: return "polyline";
		case B_SVG_POLYGON: return "polygon";
		case B_SVG_TEXT: return "text";
		case B_SVG_TEXTSPAN: return "tspan";
		case B_SVG_TEXTPATH: return "textPath";
		case B_SVG_STRING: return "string";
		case B_SVG_GRADIENT: return "untypedGradient";
		case B_SVG_LINEARGRAD: return "linearGradient";
		case B_SVG_RADIALGRAD: return "radialGradient";
		case B_SVG_STOP: return "stop";
		case B_SVG_PAINTSERVER: return "paintserver";
	}
	
	return NULL;
}

inline const char *
unit_sign(unit_t unit)
{
	switch (unit) {
		case ABS_UNITS: return "";
		case PX_UNIT: return "px";
		case PT_UNIT: return "pt";
		case PC_UNIT: return "pc";
		case CM_UNIT: return "cm";
		case MM_UNIT: return "mm";
		case IN_UNIT: return "in";
		case REL_UNITS: return "";
		case EM_UNIT: return "em";
		case EX_UNIT: return "ex";
		case PE_UNIT: return "%";
		default: return "";
	}
}

// structs
struct attribute_s {
			attribute_s()
				:	id(""), value("") { };
			attribute_s(const char *_id, const char *_value)
				:	id(_id), value(_value) { };
			attribute_s(const BString &_id, const BString &_value)
				:	id(_id), value(_value) { };

	BString	id;
	BString	value;
};

struct distance_s {
			distance_s()
				:	from(0, 0), offset(0), length(0) { };
			distance_s(BPoint _from, float _offset, float _length)
				:	from(_from), offset(_offset), length(_length) { };

	BPoint	from;
	float	offset;
	float	length;
};

struct charpos_s {
			charpos_s()
				:	absolutex(false), absolutey(false), position(0, 0),
					relativex(false), relativey(false), offset(0, 0),
					charoffset(0, 0), anchor(B_ANCHOR_START), charwidth(0),
					spacing(0), rotation(0) { };

	bool		absolutex;
	bool		absolutey;
	BPoint		position;
	bool		relativex;
	bool		relativey;
	BPoint		offset;

	BPoint		charoffset;
	anchor_t	anchor;
	float		charwidth;
	float		spacing;
	float		rotation;
};

// classes
class SVGPoint : public BPoint {
public:
					SVGPoint()
						: BPoint() { };
					SVGPoint(float x, float y)
						: BPoint(x, y) { };
					SVGPoint(const BPoint &other)
						: BPoint(other) { };

		SVGPoint	&operator=(const BPoint &other)
						{ x = other.x; y = other.y; return *this; };
		SVGPoint	&operator=(const SVGPoint &other)
						{ x = other.x; y = other.y; return *this; };

		SVGPoint	operator+(const float value) const
						{ return SVGPoint(x + value, y + value); };
		SVGPoint	operator-(const float value) const
						{ return SVGPoint(x - value, y - value); };

		SVGPoint	operator*(const float value) const
						{ return SVGPoint(x * value, y * value); };
		SVGPoint	operator*(const SVGPoint &other) const
						{ return SVGPoint(x * other.x, y * other.y); };
		SVGPoint	&operator*=(const float value)
						{ x *= value; y *= value; return *this; };
		SVGPoint	&operator*=(const SVGPoint &other)
						{ x *= other.x; y *= other.y; return *this; };

		SVGPoint	operator/(const float value) const
						{ return SVGPoint(x / value, y / value); };
		SVGPoint	operator/(const SVGPoint &other) const
						{ return SVGPoint(x / other.x, y / other.y); };
		SVGPoint	&operator/=(const float value)
						{ x /= value; y /= value; return *this; };
		SVGPoint	&operator/=(const SVGPoint &other)
						{ x /= other.x; y /= other.y; return *this; };
};


class SVGColor {
public:
					SVGColor()
						{ color.set_to(0, 0, 0, 0); };
					SVGColor(uint32 value)
						{ color = *(rgb_color *)&value; };
					SVGColor(const rgb_color &_color)
						{ color = _color; };
					SVGColor(char r, char g, char b, char a = 255)
						{ color.set_to(r, g, b, a); };

		SVGColor &	operator()(uint32 value)
						{ color = *(rgb_color *)&value; return *this; };
		SVGColor &	operator()(rgb_color _color)
						{ color = color; return *this; };
		SVGColor &	operator()(char r, char g, char b)
						{ color.set_to(r, g, b); return *this; };
		SVGColor &	operator()(char r, char g, char b, char a)
						{ color.set_to(r, g, b, a); return *this; };

		SVGColor &	operator=(const SVGColor &from)
						{ color = from.color; return *this; };
		SVGColor &	operator=(const char allto)
						{ color.red = color.green = color.blue = color.alpha = allto; return *this; };
		SVGColor &	operator=(const rgb_color _color)
						{ color = _color; return *this; };
		SVGColor &	operator=(uint32 to)
						{ color = *(rgb_color *)&to; return *this; };
		char		operator[](int index) const
						{ return ((uint8 *)&color)[index]; };

		bool		operator==(const SVGColor &other) const
						{ return color == other.color; };
		bool		operator!=(const SVGColor &other) const
						{ return color != other.color; };

		rgb_color	Color() const
						{ return color; };
		uint32		Value() const
						{ return *(uint32 *)&color; };

		void		AppendToString(BString &string)
					{
						char buffer[7];
						sprintf(buffer, "#%02x%02x%02x", color.red, color.green,
							color.blue);
						string << buffer;
					}

		void		PrintToStream()
					{
						printf("SVGColor(%d, %d, %d, %d)\n", color.red,
							color.green, color.blue, color.alpha);
					}

		rgb_color	color;
};

#endif // _SVG_DEFS_H_
