#ifndef _EDITOR_DEFS_H_
#define _EDITOR_DEFS_H_

// interface messages
// file
#define M_OPEN_FILE				'open'
#define M_OPEN_BACKGROUND		'opbg'
#define M_CLEAR_BACKGROUND		'clrb'
#define M_EXPORT_FILE			'expr'
#define M_VIEW_SELECTED			'visl'

// elements
#define M_CREATE_ELEMENT		'crel'
#define M_DELETE_ELEMENT		'dlel'
#define M_CLEAR_ELEMENTS		'clre'
#define M_ELEMENT_SELECTION		'elsl'

#define M_OPEN_FILE_REFS		'opfr'
#define M_OPEN_BACKGROUND_REFS	'obgr'
#define M_EXPORT_FILE_REFS		'exfr'

// tools
#define M_TOOL_MOVE_TO			'tmvt'
#define M_TOOL_LINE_TO			'tlnt'
#define M_TOOL_H_LINE_TO		'thlt'
#define M_TOOL_V_LINE_TO		'tvlt'
#define M_TOOL_CURVE_TO			'tcut'
#define M_TOOL_SHORT_CURVE_TO	'tsht'
#define M_TOOL_QUAD_BEZIER_TO	'tqbt'
#define M_TOOL_SHORT_BEZIER_TO	'tsqt'
#define M_TOOL_ARC_TO			'tarc'
#define M_TOOL_CLOSE			'tcls'

#define M_SHAPE_RECT			'srct'
#define M_SHAPE_ELLIPSE			'sell'
#define M_SHAPE_CIRCLE			'scrl'
#define M_SHAPE_LINE			'slin'
#define M_SHAPE_POLYLINE		'spln'
#define M_SHAPE_POLYGON			'spyg'

// settings messages
#define M_STROKE_TYPE			'stty'
#define M_STROKE_COLOR			'strc'
#define M_STROKE_GRADIENT		'sgrd'
#define M_STROKE_OPACITY		'stro'
#define M_STROKE_WIDTH			'strw'
#define M_STROKE_LINECAP		'stlc'
#define M_STROKE_LINEJOIN		'stlj'
#define M_STROKE_MITERLIMIT		'stml'
#define M_FILL_TYPE				'fity'
#define M_FILL_COLOR			'filc'
#define M_FILL_GRADIENT			'fgrd'
#define M_FILL_OPACITY			'filo'
#define M_FILL_RULE				'filr'
#define M_GRADIENT_NAME			'grna'
#define M_GRADIENT_TYPE			'grty'
#define M_GRADIENT_UNITS		'grun'
#define M_STOP_OFFSET			'stof'
#define M_STOP_COLOR			'stoc'
#define M_STOP_OPACITY			'stoo'
#define	M_PREVIEW_SAMPLESIZE	'psam'
#define	M_PREVIEW_BACKCOLOR		'pbak'
#define	M_PREVIEW_BACKALPHA		'pbaa'

#define	M_INHERIT_KEY			0x80000000;

// macros
#define BEGINMENUS										BMenu *cur_menu = NULL; BMenuItem *cur_item = NULL;
#define SETMENU(menu, title, parent)					menu = new BMenu(title); parent->AddItem(menu); cur_menu = menu;
#define ADDITEM0(title, message)						cur_item = new BMenuItem(title, new BMessage(message)); cur_menu->AddItem(cur_item);
#define ADDITEM1(title, message, shortcut)				cur_item = new BMenuItem(title, new BMessage(message), shortcut); cur_menu->AddItem(cur_item);
#define ADDITEM2(title, message, shortcut, modifiers)	cur_item = new BMenuItem(title, new BMessage(message), shortcut, modifiers); cur_menu->AddItem(cur_item);

// enums
enum editor_tab {
	M_DRAW_TAB = 0,
	M_SOURCE_TAB,
	M_PREVIEW_TAB
};

#endif