#ifndef _CSS_PARSER_H_
#define _CSS_PARSER_H_

#include <String.h>
#include "ObjectList.h"
#include "SVGDefs.h"

class BSVGElement;
class BSVGView;

enum css_selector_t {
	CSS_UNIVERSAL_SELECTOR = 0,
	CSS_TYPE_SELECTOR,
	CSS_DESCENDANT_SELECTOR,
	CSS_CHILD_SELECTOR,
	CSS_ADJACENT_SELECTOR,
	CSS_ATTRIBUTE_SELECTOR,
	CSS_CLASS_SELECTOR,
	CSS_ID_SELECTOR,
	CSS_PSEUDO_CLASS_SELECTOR,
	CSS_PSEUDO_ELEMENT_SELECTOR
};

enum css_pseudo_class_t {
	CSS_PSEUDO_CLASS_FIRST_CHILD = 0,
	CSS_PSEUDO_CLASS_LINK,
	CSS_PSEUDO_CLASS_VISITED,
	CSS_PSEUDO_CLASS_HOVER,
	CSS_PSEUDO_CLASS_ACTIVE,
	CSS_PSEUDO_CLASS_FOCUS,
	CSS_PSEUDO_CLASS_LANG
};

enum matching_t {
	B_UNKNOWN = 0,
	B_MATCHING,
	B_NONE_MATCHING
};

struct css_selector_s {
				css_selector_s()
					:	type(CSS_UNIVERSAL_SELECTOR), value("") { };
				css_selector_s(css_selector_t _selector, const char *_value)
					:	type(_selector), value(_value) { };
				css_selector_s(css_selector_t _selector, const BString &_value)
					:	type(_selector), value(_value) { };

	css_selector_t	type;
	BString			value;
};

struct css_sel_group_s {
				css_sel_group_s()
					:	selectors(2, true) { };

	BObjectList<css_selector_s>	selectors;
};

struct css_rule_s {
				css_rule_s()
					:	groups(2, true), style("") { };

	BObjectList<css_sel_group_s>	groups;
	BString							style;
};

class CSSParser {

public:
						CSSParser();
virtual					~CSSParser();

		void			AddData(const BString &data);
		void			AddData(const char *data);
		void			Parse();
		void			ReadValue(const char *from, int length, BString &to);
		void			ReadBlock(const char *from, BString &to);

		matching_t		IsMatching(BSVGElement *element, css_sel_group_s *group, int index, const char **attr, bool parsed);
		attribute_s		*MatchElement(BSVGElement *element, const char **attr);

		void			PrintToStream();

private:
		BString					fData;
		BObjectList<css_rule_s>	fRules;
};

#endif
