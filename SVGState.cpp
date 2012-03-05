#include <Message.h>
#include "SVGDefs.h"
#include "SVGState.h"

SVGState::SVGState(source_t source)
{
	// reset all to default
	Reset(source);
}

SVGState::SVGState(BMessage *data)
{
	Reset();
	
	if (!data)
		return;
	
	data->FindInt32("_fill_type", (int32 *)&fill_type.value);
	data->FindInt32("_fill_color", (int32 *)&fill_color.color);
	data->FindString("_fill_server", &fill_server);
	data->FindFloat("_fill_opacity", &fill_opacity.value);
	data->FindInt32("_fill_rule", (int32 *)&fill_rule.value);
	
	data->FindInt32("_stroke_type", (int32 *)&stroke_type.value);
	data->FindInt32("_stroke_color", (int32 *)&stroke_color.color);
	data->FindString("_stroke_server", &stroke_server);
	data->FindFloat("_stroke_opacity", &stroke_opacity.value);
	
	data->FindFloat("_stroke_width", &stroke_width.value);
	data->FindInt32("_stroke_linecap", (int32 *)&stroke_linecap.value);
	data->FindInt32("_stroke_linejoin", (int32 *)&stroke_linejoin.value);
	data->FindFloat("_stroke_miterlimit", &stroke_miterlimit.value);
	
	data->FindInt32("_stop_color", (int32 *)&stop_color.color);
	data->FindFloat("_stop_opacity", &stop_opacity.value);
	
	data->FindInt32("_general_color", (int32 *)&general_color.color);
	data->FindFloat("_general_opacity", &general_opacity.value);
	data->FindRect("_general_viewport", &general_viewport);
	data->FindRect("_general_viewbox", &general_viewbox);
	data->FindInt32("_general_aspect", (int32 *)&general_aspect.value);
	
	data->FindString("_font_family", &font_family);
	data->FindFloat("_font_size", &font_size.value);
	data->FindFloat("_font_kerning", &font_kerning.value);
	data->FindFloat("_font_letterspacing", &font_letterspacing.value);
	data->FindFloat("_font_wordspacing", &font_wordspacing.value);
	data->FindInt32("_font_decoration", (int32 *)&font_decoration.value);
	data->FindInt32("_text_anchor", (int32 *)&text_anchor.value);
	data->FindFloat("_start_offset", &start_offset.value);
	
	for (int i = 0; i < 3; i++)
		for (int j = 0; j < 3; j++)
			data->FindFloat("_general_transform", i * 3 + j, &general_transform.matrix[i][j]);

	// sources
	data->FindInt32("_fill_type_s", (int32 *)&fill_type.source);
	data->FindInt32("_fill_color_s", (int32 *)&fill_color.source);
	data->FindInt32("_fill_server_s", (int32 *)&fill_server.source);
	data->FindInt32("_fill_opacity_s", (int32 *)&fill_opacity.source);
	data->FindInt32("_fill_rule_s", (int32 *)&fill_rule.source);	
	data->FindInt32("_stroke_type_s", (int32 *)&stroke_type.source);
	data->FindInt32("_stroke_color_s", (int32 *)&stroke_color.source);
	data->FindInt32("_stroke_server_s", (int32 *)&stroke_server.source);
	data->FindInt32("_stroke_opacity_s", (int32 *)&stroke_opacity.source);
	data->FindInt32("_stroke_width_s", (int32 *)&stroke_width.source);
	data->FindInt32("_stroke_linecap_s", (int32 *)&stroke_linecap.source);
	data->FindInt32("_stroke_linejoin_s", (int32 *)&stroke_linejoin.source);
	data->FindInt32("_stroke_miterlimit_s", (int32 *)&stroke_miterlimit.source);
	data->FindInt32("_stop_color_s", (int32 *)&stop_color.source);
	data->FindInt32("_stop_opacity_s", (int32 *)&stop_opacity.source);
	data->FindInt32("_general_color_s", (int32 *)&general_color.source);
	data->FindInt32("_general_opacity_s", (int32 *)&general_opacity.source);
	data->FindInt32("_general_transform_s", (int32 *)&general_transform.source);
	data->FindInt32("_general_viewport_s", (int32 *)&general_viewport.source);
	data->FindInt32("_general_viewbox_s", (int32 *)&general_viewbox.source);
	data->FindInt32("_general_aspect_s", (int32 *)&general_aspect.source);
	data->FindInt32("_font_family_s", (int32 *)&font_family.source);
	data->FindInt32("_font_size_s", (int32 *)&font_size.source);
	data->FindInt32("_font_size_s", (int32 *)&font_size.source);
	data->FindInt32("_font_kerning_s", (int32 *)&font_kerning.source);
	data->FindInt32("_font_letterspacing_s", (int32 *)&font_letterspacing.source);
	data->FindInt32("_font_wordspacing_s", (int32 *)&font_wordspacing.source);
	data->FindInt32("_font_decoration_s", (int32 *)&font_decoration.source);
	data->FindInt32("_text_anchor_s", (int32 *)&text_anchor.source);
	data->FindInt32("_start_offset_s", (int32 *)&start_offset.source);
	
	// types
	data->FindInt32("_fill_opacity_u", (int32 *)&fill_opacity.unit);
	data->FindInt32("_stroke_opacity_u", (int32 *)&stroke_opacity.unit);
	data->FindInt32("_stroke_width_u", (int32 *)&stroke_width.unit);
	data->FindInt32("_stop_opacity_u", (int32 *)&stop_opacity.unit);
	data->FindInt32("_general_opacity_u", (int32 *)&general_opacity.unit);
	data->FindInt32("_font_size_u", (int32 *)&font_size.unit);
	data->FindInt32("_font_kerning_u", (int32 *)&font_kerning.unit);
	data->FindInt32("_font_letterspacing_u", (int32 *)&font_wordspacing.unit);
	data->FindInt32("_font_wordspacing_u", (int32 *)&font_wordspacing.unit);
	data->FindInt32("_start_offset_u", (int32 *)&start_offset.unit);
}

BArchivable *
SVGState::Instantiate(BMessage *data)
{
	if (validate_instantiation(data, "SVGState"))
		return new SVGState(data);
	return NULL;
}

status_t
SVGState::Archive(BMessage *data, bool deep) const
{
	data->AddInt32("_fill_type", (int32)fill_type.value);
	data->AddInt32("_fill_color", fill_color.Value());
	data->AddString("_fill_server", fill_server);
	data->AddFloat("_fill_opacity", fill_opacity.value);
	data->AddInt32("_fill_rule", (int32)fill_rule.value);
	
	data->AddInt32("_stroke_type", (int32)stroke_type.value);
	data->AddInt32("_stroke_color", stroke_color.Value());
	data->AddString("_stroke_server", stroke_server);
	data->AddFloat("_stroke_opacity", stroke_opacity.value);
	
	data->AddFloat("_stroke_width", stroke_width.value);
	data->AddInt32("_stroke_linecap", (int32)stroke_linecap.value);
	data->AddInt32("_stroke_linejoin", (int32)stroke_linejoin.value);
	data->AddFloat("_stroke_miterlimit", stroke_miterlimit.value);
	
	data->AddInt32("_stop_color", stop_color.Value());
	data->AddFloat("_stop_opacity", stop_opacity.value);
	
	data->AddInt32("_general_color", general_color.Value());
	data->AddFloat("_general_opacity", general_opacity.value);
	data->AddRect("_general_viewport", general_viewport);
	data->AddRect("_general_viewbox", general_viewbox);
	data->AddInt32("_general_aspect", (int32)general_aspect.value);
	
	data->AddString("_font_family", font_family);
	data->AddFloat("_font_size", font_size.value);
	data->AddFloat("_font_kerning", font_kerning.value);
	data->AddFloat("_font_letterspacing", font_letterspacing.value);
	data->AddFloat("_font_wordspacing", font_wordspacing.value);
	data->AddInt32("_font_decoration", (int32)font_decoration.value);
	data->AddInt32("_text_anchor", (int32)text_anchor.value);
	data->AddFloat("_start_offset", start_offset.value);
	
	if (deep) {
		for (int i = 0; i < 3; i++)
			for (int j = 0; j < 3; j++)
				data->AddFloat("_general_transform", general_transform.matrix[i][j]);
		
		// archive sources in deep mode only
		data->AddInt32("_fill_type_s", (int32)fill_type.source);
		data->AddInt32("_fill_color_s", (int32)fill_color.source);
		data->AddInt32("_fill_server_s", (int32)fill_server.source);
		data->AddInt32("_fill_opacity_s", (int32)fill_opacity.source);
		data->AddInt32("_fill_rule_s", (int32)fill_rule.source);	
		data->AddInt32("_stroke_type_s", (int32)stroke_type.source);
		data->AddInt32("_stroke_color_s", (int32)stroke_color.source);
		data->AddInt32("_stroke_server_s", (int32)stroke_server.source);
		data->AddInt32("_stroke_opacity_s", (int32)stroke_opacity.source);
		data->AddInt32("_stroke_width_s", (int32)stroke_width.source);
		data->AddInt32("_stroke_linecap_s", (int32)stroke_linecap.source);
		data->AddInt32("_stroke_linejoin_s", (int32)stroke_linejoin.source);
		data->AddInt32("_stroke_miterlimit_s", (int32)stroke_miterlimit.source);
		data->AddInt32("_stop_color_s", (int32)stop_color.source);
		data->AddInt32("_stop_opacity_s", (int32)stop_opacity.source);
		data->AddInt32("_general_color_s", (int32)general_color.source);
		data->AddInt32("_general_opacity_s", (int32)general_opacity.source);
		data->AddInt32("_general_transform_s", (int32)general_transform.source);
		data->AddInt32("_general_viewport_s", (int32)general_viewport.source);
		data->AddInt32("_general_viewbox_s", (int32)general_viewbox.source);
		data->AddInt32("_general_aspect_s", (int32)general_aspect.source);
		data->AddInt32("_font_family_s", (int32)font_family.source);
		data->AddInt32("_font_size_s", (int32)font_size.source);
		data->AddInt32("_font_size_s", (int32)font_size.source);
		data->AddInt32("_font_kerning_s", (int32)font_kerning.source);
		data->AddInt32("_font_letterspacing_s", (int32)font_letterspacing.source);
		data->AddInt32("_font_wordspacing_s", (int32)font_wordspacing.source);
		data->AddInt32("_font_decoration_s", (int32)font_decoration.source);
		data->AddInt32("_text_anchor_s", (int32)text_anchor.source);
		data->AddInt32("_start_offset_s", (int32)start_offset.source);
		
		// types
		data->AddInt32("_fill_opacity_u", (int32)fill_opacity.unit);
		data->AddInt32("_stroke_opacity_u", (int32)stroke_opacity.unit);
		data->AddInt32("_stroke_width_u", (int32)stroke_width.unit);
		data->AddInt32("_stop_opacity_u", (int32)stop_opacity.unit);
		data->AddInt32("_general_opacity_u", (int32)general_opacity.unit);
		data->AddInt32("_font_size_u", (int32)font_size.unit);
		data->AddInt32("_font_kerning_u", (int32)font_kerning.unit);
		data->AddInt32("_font_letterspacing_u", (int32)font_wordspacing.unit);
		data->AddInt32("_font_wordspacing_u", (int32)font_wordspacing.unit);
		data->AddInt32("_start_offset_u", (int32)start_offset.unit);
	}
	
	ADDREPCLASS("SVGState");
	
	return B_OK;
}

#define ASIGN(x)		{ x.value = other.x.value; x.source = other.x.source; x.unit = other.x.unit; }
#define ASIGNCLASS(x)	{ x = other.x; x.source = other.x.source; }

SVGState &
SVGState::operator=(const SVGState &other)
{
	ASIGN(fill_type);
	ASIGNCLASS(fill_color);
	ASIGNCLASS(fill_server);
	ASIGN(fill_opacity);
	ASIGN(fill_rule);
	
	ASIGN(stroke_type);
	ASIGNCLASS(stroke_color);
	ASIGNCLASS(stroke_server);
	ASIGN(stroke_opacity);
	ASIGN(stroke_width);
	ASIGN(stroke_linecap);
	ASIGN(stroke_linejoin);
	ASIGN(stroke_miterlimit);
	
	ASIGNCLASS(stop_color);
	ASIGN(stop_opacity);
	
	ASIGNCLASS(general_color);
	ASIGN(general_opacity);
	ASIGNCLASS(general_transform);
	ASIGNCLASS(general_viewport);
	ASIGNCLASS(general_viewbox);
	ASIGN(general_aspect);
	
	ASIGNCLASS(font_family);
	ASIGN(font_size);
	ASIGN(font_kerning);
	ASIGN(font_letterspacing);
	ASIGN(font_wordspacing);
	ASIGN(font_decoration);
	ASIGN(text_anchor);
	ASIGN(start_offset);
	
	//PrintToStream();
	return *this;
}

#define RESET(x, y)		{ x = y; x.source = source; };

SVGState &
SVGState::Reset(source_t source)
{
	RESET(fill_type, B_FILL_COLOR);
	RESET(fill_color, 0);
	RESET(fill_server, "");
	RESET(fill_opacity, 1);
	RESET(fill_rule, B_NONZERO);
	RESET(stroke_type, B_FILL_NONE);
	RESET(stroke_color, 0);
	RESET(stroke_server, "");
	RESET(stroke_opacity, 1);
	RESET(stroke_width, 1);
	RESET(stroke_linecap, B_BUTT_CAP);
	RESET(stroke_linejoin, B_MITER_JOIN);
	RESET(stroke_miterlimit, 4);
	RESET(stop_color, 0);
	RESET(stop_opacity, 1);
	RESET(general_color, 0);
	RESET(general_opacity, 1);
	RESET(general_transform, Matrix2D());
	RESET(general_viewport, BRect(0, 0, 0, 0));
	RESET(general_viewbox, BRect(0, 0, 0, 0));
	RESET(general_aspect, ASPECT_NONE);
	RESET(font_family, "Verdana");
	RESET(font_size, 12);
	RESET(font_kerning, 0);
	RESET(font_letterspacing, 0);
	RESET(font_wordspacing, 0);
	RESET(font_decoration, B_NO_DECORATION);
	RESET(text_anchor, B_ANCHOR_START);
	RESET(start_offset, 0);
	
	return *this;
}

#define PLUSCLASS(x)	if (force || from.x.source != SOURCE_UNSET) { x = from.x; x.source = (from.x.source == SOURCE_UNSET ? SOURCE_UNSET : SOURCE_INHERIT); };
#define PLUS(x)			if (force || from.x.source != SOURCE_UNSET) { x = from.x; x.source = (from.x.source == SOURCE_UNSET ? SOURCE_UNSET : SOURCE_INHERIT); x.unit = from.x.unit; };

SVGState &
SVGState::Inherit(const SVGState &from, bool force)
{
	PLUS(fill_type);
	PLUSCLASS(fill_color);
	PLUSCLASS(fill_server);
	PLUS(fill_opacity);
	PLUS(fill_rule);
	
	PLUS(stroke_type);
	PLUSCLASS(stroke_color);
	PLUSCLASS(stroke_server);
	PLUS(stroke_opacity);
	PLUS(stroke_width);
	PLUS(stroke_linecap);
	PLUS(stroke_linejoin);
	PLUS(stroke_miterlimit);
	
	PLUSCLASS(stop_color);
	PLUS(stop_opacity);
	
	PLUSCLASS(general_color);
	PLUS(general_opacity);
	PLUSCLASS(general_transform);
	PLUSCLASS(general_viewport);
	PLUSCLASS(general_viewbox);
	PLUS(general_aspect);
	
	PLUSCLASS(font_family);
	PLUS(font_size);
	PLUS(font_kerning);
	PLUS(font_letterspacing);
	PLUS(font_wordspacing);
	PLUS(font_decoration);
	PLUS(text_anchor);
	PLUS(start_offset);
	
	return *this;
}

#define MERGECLASS(x)	if (with.x.source == SOURCE_SET) { x = with.x; x.source = SOURCE_SET; };
#define MERGE(x)		if (with.x.source == SOURCE_SET) { x = with.x; x.source = SOURCE_SET; x.unit = with.x.unit; };

SVGState &
SVGState::Merge(const SVGState &with, int32 flags)
{
	MERGE(fill_type);
	MERGECLASS(fill_color);
	MERGECLASS(fill_server);
	MERGE(fill_rule);
	
	MERGE(stroke_type);
	MERGECLASS(stroke_color);
	MERGECLASS(stroke_server);
	MERGE(stroke_width);
	MERGE(stroke_linecap);
	MERGE(stroke_linejoin);
	MERGE(stroke_miterlimit);
	
	MERGECLASS(stop_color);
	
	MERGECLASS(general_color);
	MERGECLASS(general_viewport);
	MERGECLASS(general_viewbox);
	MERGE(general_aspect);
	
	MERGECLASS(font_family);
	MERGE(font_size);
	MERGE(font_kerning);
	MERGE(font_letterspacing);
	MERGE(font_wordspacing);
	MERGE(font_decoration);
	MERGE(text_anchor);
	MERGE(start_offset);
	
	if (flags & B_MULTIPLY_BY_SOURCE) {
		// fill opacity
		if (fill_opacity.source == SOURCE_UNSET)
			fill_opacity.value = 1;
		
		if (with.fill_opacity.source != SOURCE_UNSET)
			fill_opacity *= with.fill_opacity.value;
		
		// stroke opacity
		if (stroke_opacity.source == SOURCE_UNSET)
			stroke_opacity.value = 1;
		
		if (with.stroke_opacity.source != SOURCE_UNSET)
			stroke_opacity *= with.stroke_opacity.value;
		
		// general opacity
		if (general_opacity.source == SOURCE_UNSET)
			general_opacity.value = 1;
		
		if (with.general_opacity.source != SOURCE_UNSET)
			general_opacity *= with.general_opacity.value;
		
		// stop opacity
		if (stop_opacity.source == SOURCE_UNSET)
			stop_opacity.value = 1;
		
		if (with.stop_opacity.source != SOURCE_UNSET)
			stop_opacity *= with.stop_opacity.value;
		
		// transform
		if (general_transform.source == SOURCE_UNSET)
			general_transform.Reset();
		
		if (with.general_transform.source != SOURCE_UNSET)
			general_transform *= with.general_transform;
	
	} else if (flags & B_MULTIPLY_BY_DEST) {
		// fill opacity
		float opacity = 1;
		if (with.fill_opacity.source != SOURCE_UNSET)
			opacity = with.fill_opacity.value;
		if (fill_opacity.source != SOURCE_UNSET)
			opacity *= fill_opacity();
		fill_opacity = opacity;
		
		// stroke opacity
		opacity = 1;
		if (with.stroke_opacity.source != SOURCE_UNSET)
			opacity = with.stroke_opacity.value;
		if (stroke_opacity.source != SOURCE_UNSET)
			opacity *= stroke_opacity();
		stroke_opacity = opacity;
		
		// general opacity
		opacity = 1;
		if (with.general_opacity.source != SOURCE_UNSET)
			opacity = with.general_opacity.value;
		if (general_opacity.source != SOURCE_UNSET)
			opacity *= general_opacity();
		general_opacity = opacity;
		
		// stop opacity
		opacity = 1;
		if (with.stop_opacity.source != SOURCE_UNSET)
			opacity = with.stop_opacity.value;
		if (stop_opacity.source != SOURCE_UNSET)
			opacity *= stop_opacity();
		stop_opacity = opacity;
		
		// transformation
		Matrix2D matrix;
		if (with.general_transform.source != SOURCE_UNSET)
			matrix = with.general_transform;
		if (general_transform.source != SOURCE_UNSET)
			matrix *= general_transform;
		general_transform = matrix;
	
	} else {
		MERGE(fill_opacity);
		MERGE(stroke_opacity);
		MERGE(general_opacity);
		MERGE(stop_opacity);
		MERGECLASS(general_transform);
	}
	
	/*if (flags & B_ADD_VIEWPORT
		|| (flags & B_ADD_VIEWPORT_IF_SET
			&& general_viewport.source == SOURCE_SET
			&& general_viewbox.source == SOURCE_SET)) {
		
		BPoint scale(1, 1);
		if (general_aspect() == ASPECT_NONE) {
			scale.x = general_viewport.right / general_viewbox.right;
			scale.y = general_viewport.bottom / general_viewbox.bottom;
		}
		
		general_transform.TranslateBy(general_viewbox.LeftTop());
		general_transform.ScaleBy(scale);
		
		general_viewport.source = SOURCE_UNSET;
		general_viewbox.source = SOURCE_UNSET;
	}*/
	
	return *this;
}

void
SVGState::AddToString(BString *data, source_t source)
{
	bool nofill = false;
	bool nostroke = false;
	
	if (fill_type.source == source) {
		switch (fill_type()) {
			case B_FILL_COLOR: *data << " fill=\""; fill_color.AppendToString(*data); *data << "\""; break;
			case B_FILL_CURRENT_COLOR: *data << " fill=\"currentColor\""; break;
			case B_FILL_SERVER: *data << " fill=\"url(#" << fill_server << ")\""; break;
			default: *data << " fill=\"none\""; nofill = true; break;
		}
	}
	
	if (!nofill) {
		if (fill_opacity.source == source)
			*data << " fill-opacity=\"" << fill_opacity() << "\"";
		
		if (fill_rule.source == source) {
			switch (fill_rule()) {
				case B_NONZERO: *data << " fill-rule=\"nonzero\""; break;
				case B_EVENODD: *data << " fill-rule=\"evenodd\""; break;
			}
		}
	}
	
	if (stroke_type.source == source) {
		switch (stroke_type()) {
			case B_FILL_COLOR: *data << " stroke=\""; stroke_color.AppendToString(*data); *data << "\""; break;
			case B_FILL_CURRENT_COLOR: *data << " stroke=\"currentColor\""; break;
			case B_FILL_SERVER: *data << " stroke=\"url(#" << stroke_server << ")\""; break;
			default: *data << " stroke=\"none\""; nostroke = true; break;
		}
	}
	
	if (!nostroke) {
		if (stroke_opacity.source == source)
			*data << " stroke-opacity=\"" << stroke_opacity() << "\"";
		
		if (stroke_width.source == source)
			*data << " stroke-width=\"" << stroke_width() << "\"";
		
		if (stroke_linecap.source == source) {
			*data << " stroke-linecap=\"";
			switch (stroke_linecap()) {
				case B_ROUND_CAP: *data << "round"; break;
				case B_BUTT_CAP: *data << "butt"; break;
				case B_SQUARE_CAP: *data << "square"; break;
			}
			*data << "\"";
		}
		
		if (stroke_linejoin.source == source) {
			*data << " stroke-linejoin=\"";
			switch (stroke_linejoin()) {
				case B_ROUND_JOIN: *data << "round"; break;
				case B_MITER_JOIN: *data << "miter"; break;
				case B_BEVEL_JOIN: *data << "bevel"; break;
				case B_BUTT_JOIN: *data << "butt"; break;
				case B_SQUARE_JOIN: *data << "square"; break;
			}
			*data << "\"";
		}
		
		if (stroke_miterlimit.source == source)
			*data << " stroke-miterlimit=\"" << stroke_miterlimit() << "\"";
	}
	
	if (stop_color.source == source) {
		*data << " stop-color=\"";
		stop_color.AppendToString(*data);
		*data << "\"";
	}
	
	if (stop_opacity.source == source)
		*data << " stop-opacity=\"" << stop_opacity() << "\"";
	
	if (general_color.source == source) {
		*data << " color=\"";
		general_color.AppendToString(*data);
		*data << "\"";
	}
	
	if (general_opacity.source == source)
		*data << " opacity=\"" << general_opacity() << "\"";
	
	if (general_transform.source == source) {
		*data << " transform=\"matrix(";
		*data << general_transform.matrix[0][0] << " ";
		*data << general_transform.matrix[1][0] << " ";
		*data << general_transform.matrix[0][1] << " ";
		*data << general_transform.matrix[1][1] << " ";
		*data << general_transform.matrix[0][2] << " ";
		*data << general_transform.matrix[1][2] << ")\"";
	}
	
	if (general_viewbox.source == source) {
		*data << " viewBox=\"";
		*data << general_viewbox.left << " " << general_viewbox.right << " ";
		*data << general_viewbox.Width() << " " << general_viewbox.Height() << "\"";
	}
	
	if (font_family.source == source)
		*data << " font-family=\"" << font_family << "\"";
	
	if (font_size.source == source)
		*data << " font-size=\"" << font_size() << "\"";
	
	if (font_kerning.source == source)
		*data << " kerning=\"" << font_kerning() << "\"";
	
	if (font_letterspacing.source == source)
		*data << " letter-spacing=\"" << font_letterspacing() << "\"";
	
	if (font_wordspacing.source == source)
		*data << " word-spacing=\"" << font_wordspacing() << "\"";
	
	if (text_anchor.source == source) {
		*data << " text-anchor=\"";
		
		switch (text_anchor()) {
			case B_ANCHOR_START: *data << "start"; break;
			case B_ANCHOR_MIDDLE: *data << "middle"; break;
			case B_ANCHOR_END: *data << "end"; break;
		}
		
		*data << "\"";
	}
	
	if (start_offset.source == source)
		*data << " startOffset=\"" << start_offset.value << unit_sign(start_offset.unit) << "\"";
}

void
SVGState::PrintToStream()
{
	printf("SVGState:\n");
	
	printf("%d: fill_type: ", fill_type.source);
	switch (fill_type()) {
		case B_FILL_UNKNOWN: printf("unknown"); break;
		case B_FILL_NONE: printf("none"); break;
		case B_FILL_COLOR: printf("color"); break;
		case B_FILL_CURRENT_COLOR: printf("current color"); break;
		case B_FILL_SERVER: printf("paintserver"); break;
	}
	printf("\n");
	
	printf("%d: fill_color: ", fill_color.source);
	fill_color.PrintToStream();
	
	printf("%d: fill_server: %s\n", fill_server.source, fill_server.String());
	printf("%d: fill_opacity: %f\n", fill_opacity.source, fill_opacity());
	printf("%d: fill_rule: %s\n", fill_rule.source, fill_rule() == B_NONZERO ? "nonzero" : "evenodd");
	
	printf("%d: stroke_type: ", stroke_type.source);
	switch (stroke_type()) {
		case B_FILL_UNKNOWN: printf("unknown"); break;
		case B_FILL_NONE: printf("none"); break;
		case B_FILL_COLOR: printf("color"); break;
		case B_FILL_CURRENT_COLOR: printf("current color"); break;
		case B_FILL_SERVER: printf("paintserver"); break;
	}
	printf("\n");
	
	printf("%d: stroke_color: ", stroke_color.source);
	stroke_color.PrintToStream();
	
	printf("%d: stroke_server: %s\n", stroke_server.source, stroke_server.String());
	printf("%d: stroke_opacity: %f\n", stroke_opacity.source, stroke_opacity());
	printf("%d: stroke_width: %f\n", stroke_width.source, stroke_width());
	
	printf("%d: stroke_linecap: ", stroke_linecap.source);
	switch (stroke_linecap()) {
		case B_ROUND_CAP: printf("round"); break;
		case B_BUTT_CAP: printf("butt"); break;
		case B_SQUARE_CAP: printf("square"); break;
	}
	printf("\n%d: stroke_linejoin: ", stroke_linejoin.source);
	switch (stroke_linejoin()) {
		case B_ROUND_JOIN: printf("round"); break;
		case B_MITER_JOIN: printf("miter"); break;
		case B_BEVEL_JOIN: printf("bevel"); break;
		case B_BUTT_JOIN: printf("butt"); break;
		case B_SQUARE_JOIN: printf("square"); break;
	}
	printf("\n%d: stroke_miterlimit: %f\n", stroke_miterlimit.source, stroke_miterlimit());
	
	printf("%d: stop_color: ", stop_color.source);
	stop_color.PrintToStream();
	
	printf("%d: stop_opacity: %f\n", stop_opacity.source, stop_opacity());
	
	printf("%d: general_color: ", general_color.source);
	general_color.PrintToStream();
	
	printf("%d: general_opacity: %f\n", general_opacity.source, general_opacity());
	
	printf("%d: general_transform:\n", general_transform.source);
	general_transform.PrintToStream();
	
	printf("%d: general_viewport: ", general_viewport.source);
	general_viewport.PrintToStream();
	
	printf("%d: general_viewbox: ", general_viewbox.source);
	general_viewbox.PrintToStream();
	
	printf("%d: general_aspect: %x = ", general_aspect.source, general_aspect());
	printf("x: %s, ", (general_aspect() & ASPECT_XMIN ? "xMin" : (general_aspect() & ASPECT_XMID ? "xMid" : (general_aspect() & ASPECT_XMAX ? "xMax" : "none"))));
	printf("y: %s, ", (general_aspect() & ASPECT_YMIN ? "yMin" : (general_aspect() & ASPECT_YMID ? "yMid" : (general_aspect() & ASPECT_YMAX ? "yMax" : "none"))));
	printf("type: %s\n", (general_aspect() & ASPECT_MEET ? "meet" : (general_aspect() & ASPECT_SLICE ? "slice" : "none")));
	
	printf("%d: font_family: %s\n", font_family.source, font_family.String());
	printf("%d: font_size: %f\n", font_size.source, font_size());
	printf("%d: font_kerning: %f\n", font_kerning.source, font_kerning());
	printf("%d: font_letterspacing: %f\n", font_letterspacing.source, font_letterspacing());
	printf("%d: font_wordspacing: %f\n", font_wordspacing.source, font_wordspacing());
	
	printf("%d: font_decoration:", font_decoration.source);
	if (font_decoration() & B_NO_DECORATION) printf(" none");
	if (font_decoration() & B_UNDERLINE_DECORATION) printf(" underline");
	if (font_decoration() & B_OVERLINE_DECORATION) printf(" overline");
	if (font_decoration() & B_STRIKEOUT_DECORATION) printf(" strikeout");
	if (font_decoration() & B_BLINK_DECORATION) printf(" blink");
	printf("\n");
	
	printf("%d: text_anchor: ", text_anchor.source);
	switch (text_anchor()) {
		case B_ANCHOR_START: printf("start"); break;
		case B_ANCHOR_MIDDLE: printf("middle"); break;
		case B_ANCHOR_END: printf("end"); break;
	}
	printf("\n");
	
	printf("%d: start_offset: %f\n", start_offset.source, start_offset());
}

float
SVGState::ResolveValue(const SourcedValue<float> &value, float percentage_relation)
{
	switch (value.unit) {
		// absolute units can be computed
		case PT_UNIT: return value.value / 72 * DEFAULT_DPI;
		case PC_UNIT: return value.value / 72 * DEFAULT_DPI * 12;
		case MM_UNIT: return value.value / 25.4 * DEFAULT_DPI;
		case CM_UNIT: return value.value / 2.54 * DEFAULT_DPI;
		case IN_UNIT: return value.value * DEFAULT_DPI;
		
		// relative units need to be set into a relation
		case EM_UNIT: return value.value * ResolveValue(font_size);
		case EX_UNIT: return value.value * ResolveValue(font_size) / 2;
		case PE_UNIT: return value.value / 100 * percentage_relation;
		default: return value.value;
	}
}

void
SVGState::UnsetField(field_t fieldcode)
{
	SetSource(fieldcode, SOURCE_UNSET);
}

void
SVGState::GetValue(field_t field, void *result)
{
	switch (field) {
		case FIELD_FILL_TYPE: *(paint_t *)result = fill_type.value; return;
		case FIELD_FILL_COLOR: *(rgb_color *)result = fill_color.color; return;
		case FIELD_FILL_SERVER: *(BString *)result = fill_server; return;
		case FIELD_FILL_OPACITY: *(float *)result = fill_opacity.value; return;
		case FIELD_FILL_RULE: *(rule_t *)result = fill_rule.value; return;
		
		case FIELD_STROKE_TYPE: *(paint_t *)result = stroke_type.value; return;
		case FIELD_STROKE_COLOR: *(rgb_color *)result = stroke_color.color; return;
		case FIELD_STROKE_SERVER: *(BString *)result = stroke_server; return;
		case FIELD_STROKE_OPACITY: *(float *)result = stroke_opacity.value; return;
		case FIELD_STROKE_WIDTH: *(float *)result = stroke_width.value; return;
		case FIELD_STROKE_LINECAP: *(cap_mode *)result = stroke_linecap.value; return;
		case FIELD_STROKE_LINEJOIN: *(join_mode *)result = stroke_linejoin.value; return;
		case FIELD_STROKE_MITERLIMIT: *(float *)result = stroke_miterlimit.value; return;
		
		case FIELD_STOP_COLOR: *(rgb_color *)result = stop_color.color; return;
		case FIELD_STOP_OPACITY: *(float *)result = stop_opacity.value; return;
		
		case FIELD_GENERAL_COLOR: *(rgb_color *)result = general_color.color; return;
		case FIELD_GENERAL_OPACITY: *(float *)result = general_opacity.value; return;
		case FIELD_GENERAL_TRANSFORM: *(Matrix2D *)result = general_transform; return;
		case FIELD_GENERAL_VIEWPORT: *(BRect *)result = general_viewport; return;
		case FIELD_GENERAL_VIEWBOX: *(BRect *)result = general_viewbox; return;
		case FIELD_GENERAL_ASPECT: *(aspect_t *)result = general_aspect.value; return;
		
		case FIELD_FONT_FAMILY: *(BString *)result = font_family; return;
		case FIELD_FONT_SIZE: *(float *)result = font_size.value; return;
		case FIELD_FONT_KERNING: *(float *)result = font_kerning.value; return;
		case FIELD_FONT_LETTERSPACING: *(float *)result = font_letterspacing.value; return;
		case FIELD_FONT_WORDSPACING: *(float *)result = font_wordspacing.value; return;
		case FIELD_FONT_DECORATION: *(decoration_t *)result = font_decoration.value; return;
		case FIELD_TEXT_ANCHOR: *(anchor_t *)result = text_anchor.value; return;
		case FIELD_START_OFFSET: *(float *)result = start_offset.value; return;
	}
}

source_t
SVGState::GetSource(field_t field)
{
	switch (field) {
		case FIELD_FILL_TYPE: return fill_type.source;
		case FIELD_FILL_COLOR: return fill_color.source;
		case FIELD_FILL_SERVER: return fill_server.source;
		case FIELD_FILL_OPACITY: return fill_opacity.source;
		case FIELD_FILL_RULE: return fill_rule.source;
		
		case FIELD_STROKE_TYPE: return stroke_type.source;
		case FIELD_STROKE_COLOR: return stroke_color.source;
		case FIELD_STROKE_SERVER: return stroke_server.source;
		case FIELD_STROKE_OPACITY: return stroke_opacity.source;
		case FIELD_STROKE_WIDTH: return stroke_width.source;
		case FIELD_STROKE_LINECAP: return stroke_linecap.source;
		case FIELD_STROKE_LINEJOIN: return stroke_linejoin.source;
		case FIELD_STROKE_MITERLIMIT: return stroke_miterlimit.source;
		
		case FIELD_STOP_COLOR: return stop_color.source;
		case FIELD_STOP_OPACITY: return stop_opacity.source;
		
		case FIELD_GENERAL_COLOR: return general_color.source;
		case FIELD_GENERAL_OPACITY: return general_opacity.source;
		case FIELD_GENERAL_TRANSFORM: return general_transform.source;
		case FIELD_GENERAL_VIEWPORT: return general_viewport.source;
		case FIELD_GENERAL_VIEWBOX: return general_viewbox.source;
		case FIELD_GENERAL_ASPECT: return general_aspect.source;
		
		case FIELD_FONT_FAMILY: return font_family.source;
		case FIELD_FONT_SIZE: return font_size.source;
		case FIELD_FONT_KERNING: return font_kerning.source;
		case FIELD_FONT_LETTERSPACING: return font_letterspacing.source;
		case FIELD_FONT_WORDSPACING: return font_wordspacing.source;
		case FIELD_FONT_DECORATION: return font_decoration.source;
		case FIELD_TEXT_ANCHOR: return text_anchor.source;
		case FIELD_START_OFFSET: return start_offset.source;
	}
	
	return SOURCE_UNSET;
}

void
SVGState::SetValue(field_t field, void *to)
{
	switch (field) {
		case FIELD_FILL_TYPE: fill_type.value = *(paint_t *)to; break;
		case FIELD_FILL_COLOR: fill_color = *(rgb_color *)to; break;
		case FIELD_FILL_SERVER: fill_server = *(BString *)to; break;
		case FIELD_FILL_OPACITY: fill_opacity.value = *(float *)to; break;
		case FIELD_FILL_RULE: fill_rule.value = *(rule_t *)to; break;
		
		case FIELD_STROKE_TYPE: stroke_type.value = *(paint_t *)to; break;
		case FIELD_STROKE_COLOR: stroke_color = *(rgb_color *)to; break;
		case FIELD_STROKE_SERVER: stroke_server = *(BString *)to; break;
		case FIELD_STROKE_OPACITY: stroke_opacity.value = *(float *)to; break;
		case FIELD_STROKE_WIDTH: stroke_width.value = *(float *)to; break;
		case FIELD_STROKE_LINECAP: stroke_linecap.value = *(cap_mode *)to; break;
		case FIELD_STROKE_LINEJOIN: stroke_linejoin.value = *(join_mode *)to; break;
		case FIELD_STROKE_MITERLIMIT: stroke_miterlimit.value = *(float *)to; break;
		
		case FIELD_STOP_COLOR: stop_color = *(rgb_color *)to; break;
		case FIELD_STOP_OPACITY: stop_opacity.value = *(float *)to; break;
		
		case FIELD_GENERAL_COLOR: general_color = *(rgb_color *)to; break;
		case FIELD_GENERAL_OPACITY: general_opacity.value = *(float *)to; break;
		case FIELD_GENERAL_TRANSFORM: general_transform = *(Matrix2D *)to; break;
		case FIELD_GENERAL_VIEWPORT: general_viewport = *(BRect *)to; break;
		case FIELD_GENERAL_VIEWBOX: general_viewbox = *(BRect *)to; break;
		case FIELD_GENERAL_ASPECT: general_aspect.value = *(aspect_t *)to; break;
		
		case FIELD_FONT_FAMILY: font_family = *(BString *)to; break;
		case FIELD_FONT_SIZE: font_size.value = *(float *)to; break;
		case FIELD_FONT_KERNING: font_kerning.value = *(float *)to; break;
		case FIELD_FONT_LETTERSPACING: font_letterspacing.value = *(float *)to; break;
		case FIELD_FONT_WORDSPACING: font_wordspacing.value = *(float *)to; break;
		case FIELD_FONT_DECORATION: font_decoration.value = *(decoration_t *)to; break;
		case FIELD_TEXT_ANCHOR: text_anchor.value = *(anchor_t *)to; break;
		case FIELD_START_OFFSET: start_offset.value = *(float *)to; break;
	}
}

void
SVGState::SetSource(field_t fieldcode, source_t to)
{
	switch (fieldcode) {
		case FIELD_FILL_TYPE: fill_type.source = to; break;
		case FIELD_FILL_COLOR: fill_color.source = to; break;
		case FIELD_FILL_SERVER: fill_server.source = to; break;
		case FIELD_FILL_OPACITY: fill_opacity.source = to; break;
		case FIELD_FILL_RULE: fill_rule.source = to; break;
		
		case FIELD_STROKE_TYPE: stroke_type.source = to; break;
		case FIELD_STROKE_COLOR: stroke_color.source = to; break;
		case FIELD_STROKE_SERVER: stroke_server.source = to; break;
		case FIELD_STROKE_OPACITY: stroke_opacity.source = to; break;
		case FIELD_STROKE_WIDTH: stroke_width.source = to; break;
		case FIELD_STROKE_LINECAP: stroke_linecap.source = to; break;
		case FIELD_STROKE_LINEJOIN: stroke_linejoin.source = to; break;
		case FIELD_STROKE_MITERLIMIT: stroke_miterlimit.source = to; break;
		
		case FIELD_STOP_COLOR: stop_color.source = to; break;
		case FIELD_STOP_OPACITY: stop_opacity.source = to; break;
		
		case FIELD_GENERAL_COLOR: general_color.source = to; break;
		case FIELD_GENERAL_OPACITY: general_opacity.source = to; break;
		case FIELD_GENERAL_TRANSFORM: general_transform.source = to; break;
		case FIELD_GENERAL_VIEWPORT: general_viewport.source = to; break;
		case FIELD_GENERAL_VIEWBOX: general_viewbox.source = to; break;
		case FIELD_GENERAL_ASPECT: general_aspect.source = to; break;
		
		case FIELD_FONT_FAMILY: font_family.source = to; break;
		case FIELD_FONT_SIZE: font_size.source = to; break;
		case FIELD_FONT_KERNING: font_kerning.source = to; break;
		case FIELD_FONT_LETTERSPACING: font_letterspacing.source = to; break;
		case FIELD_FONT_WORDSPACING: font_wordspacing.source = to; break;
		case FIELD_FONT_DECORATION: font_decoration.source = to; break;
		case FIELD_TEXT_ANCHOR: text_anchor.source = to; break;
		case FIELD_START_OFFSET: start_offset.source = to; break;
	}
}
