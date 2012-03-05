#ifndef _SVG_STATE_H_
#define _SVG_STATE_H_

#include "Matrix.h"
#include "SVGDefs.h"

template<class T>
class SourcedClass : public T {

public:
					SourcedClass()
						: T() { source = SOURCE_UNSET; };
					SourcedClass(const T _value, source_t _source = SOURCE_SET)
						: T(_value) { source = _source; };
					
		T 			&operator=(const T &_value)
						{ source = SOURCE_SET; return *((T *)this) = _value; };

		T			&operator+=(const T &_value)
						{ source = SOURCE_SET; return *((T *)this) += _value; };
		T			&operator-=(const T &_value)
						{ source = SOURCE_SET; return *((T *)this) -= _value; };
		T			&operator*=(const T &_value)
						{ source = SOURCE_SET; return *((T *)this) *= _value; };
		T			&operator/=(const T &_value)
						{ source = SOURCE_SET; return *((T *)this) /= _value; };

		source_t	source;
};

template<typename T>
class SourcedValue {

public:
					SourcedValue()
						{ source = SOURCE_UNSET; unit = PX_UNIT; };
					SourcedValue(const T _value, source_t _source = SOURCE_SET, unit_t _unit = PX_UNIT)
						: value(_value), source(_source), unit(_unit) { };
					
		T 			&operator=(const T &_value)
						{ source = SOURCE_SET; return value = _value; };

		T			operator+(const T &_value) const
						{ return value + _value; };
		T			operator-(const T &_value) const
						{ return value - _value; };
		T			operator*(const T &_value) const
						{ return value * _value; };
		T			operator/(const T &_value) const
						{ return value / _value; };

		T			&operator+=(const T &_value)
						{ source = SOURCE_SET; return value += _value; };
		T			&operator-=(const T &_value)
						{ source = SOURCE_SET; return value -= _value; };
		T			&operator*=(const T &_value)
						{ source = SOURCE_SET; return value *= _value; };
		T			&operator/=(const T &_value)
						{ source = SOURCE_SET; return value /= _value; };

		void 		operator()(const T _value, source_t _source, unit_t _unit)
						{ value = _value; source = _source; unit = _unit; };
		T			operator()() const
						{ return value; };

		T			value;
		source_t	source;
		unit_t		unit;
};

class SVGState : public BArchivable {

public:
					SVGState(source_t source = SOURCE_UNSET);

					SVGState(BMessage *data);
static	BArchivable	*Instantiate(BMessage *data);
virtual	status_t	Archive(BMessage *data, bool deep = true) const;

		SVGState	&operator=(const SVGState &other);

		SVGState	&Reset(source_t source = SOURCE_UNSET);
		SVGState	&Inherit(const SVGState &from, bool force = false);
		SVGState	&Merge(const SVGState &with, int32 flags = 0);
		void		PrintToStream();
		void		AddToString(BString *data, source_t source = SOURCE_SET);

		float		ResolveValue(const SourcedValue<float> &value, float percentage_relation = 100);

		void		UnsetField(field_t fieldcode);
		void		SetValue(field_t field, void *to);
		void		GetValue(field_t field, void *result);
		void		SetSource(field_t field, source_t to);
		source_t	GetSource(field_t field);

	SourcedValue<paint_t>		fill_type;
	SourcedClass<SVGColor>		fill_color;
	SourcedClass<BString>		fill_server;
	SourcedValue<float>			fill_opacity;
	SourcedValue<rule_t>		fill_rule;

	SourcedValue<paint_t>		stroke_type;
	SourcedClass<SVGColor>		stroke_color;
	SourcedClass<BString>		stroke_server;
	SourcedValue<float>			stroke_opacity;
	SourcedValue<float>			stroke_width;
	SourcedValue<cap_mode>		stroke_linecap;
	SourcedValue<join_mode>		stroke_linejoin;
	SourcedValue<float>			stroke_miterlimit;

	SourcedClass<SVGColor>		stop_color;
	SourcedValue<float>			stop_opacity;

	SourcedClass<SVGColor>		general_color;
	SourcedValue<float>			general_opacity;
	SourcedClass<Matrix2D>		general_transform;
	SourcedClass<BRect>			general_viewport;
	SourcedClass<BRect>			general_viewbox;
	SourcedValue<aspect_t>		general_aspect;

	SourcedClass<BString>		font_family;
	SourcedValue<float>			font_size;
	SourcedValue<float>			font_kerning;
	SourcedValue<float>			font_letterspacing;
	SourcedValue<float>			font_wordspacing;
	SourcedValue<decoration_t>	font_decoration;
	SourcedValue<anchor_t>		text_anchor;
	SourcedValue<float>			start_offset;
};

#endif
