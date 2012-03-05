#include "CSSParser.h"
#include "SVGView.h"
#include "SVGElement.h"

CSSParser::CSSParser()
	:	fData(""),
		fRules(10, true)
{
}

CSSParser::~CSSParser()
{
	fData.SetTo("");
}

void
CSSParser::AddData(const BString &data)
{
	fData.Append(data);
}

void
CSSParser::AddData(const char *data)
{
	fData.Append(data);
}

#define NEXTTOK		strtok(NULL, " ")

void
CSSParser::Parse()
{
	BString dat = fData;
	
	// whitespacehandling
	
	// strip out comments
	int open_comment = 0;
	int close_comment = 0;
	while ((open_comment = dat.FindFirst("/*")) >= 0) {
		close_comment = dat.FindFirst("*/", open_comment);
		dat.Remove(open_comment, close_comment - open_comment + 2);
	}
	
	dat.ReplaceAll("\n", " ");
	dat.ReplaceAll("\r", " ");
	dat.ReplaceAll("\t", " ");
	
	while (dat.FindFirst("  ") >= 0)
		dat.ReplaceAll("  ", " ");
	
	while (dat.FindFirst("  ") >= 0)
		dat.ReplaceAll("  ", " ");
	
	dat.ReplaceAll(" {", "{");
	dat.ReplaceAll("} ", "}");
	
	dat.ReplaceAll("{ ", "{");
	dat.ReplaceAll(" }", "}");
	
	dat.ReplaceAll(", ", ",");
	dat.ReplaceAll(" ,", ",");
	
	dat.ReplaceAll("> ", ">");
	dat.ReplaceAll(" >", ">");
	
	dat.ReplaceAll("+ ", "+");
	dat.ReplaceAll(" +", "+");
	
	dat.ReplaceAll(". ", ".");
	dat.ReplaceAll(" .", ".");
	
	dat.ReplaceAll("# ", "#");
	dat.ReplaceAll(" #", "#");
	
	int count = 0;
	while (dat.ByteAt(count++) == ' ');
	dat.Remove(0, count - 1);
	
	count = 0;
	while (dat.ByteAt(dat.Length() - 1 - count++) == ' ');
	dat.Truncate(dat.Length() - count + 1);
	
	// whitespacehandling
	
	if (dat.Length() <= 1)
		return;
	
	const char *data = dat.String();
	int pos = 0; // position in current selector
	int extra = 0; // for the "type" type we have no selector sign
	int abspos = 0; // position inside data string
	int length = dat.Length();
	css_selector_t type = CSS_UNIVERSAL_SELECTOR;
	css_selector_s *current_sel = new css_selector_s();
	css_sel_group_s *current_group = new css_sel_group_s();
	css_rule_s *current_rule = new css_rule_s();
	
	while (abspos < length) {
		//printf("pos: %d; data: \"%s\"\n", pos, &data[abspos]);
		if (pos == 0) {
			extra = -1;
			switch (data[abspos]) {
				case '*': type = CSS_UNIVERSAL_SELECTOR; break;
				case ' ': type = CSS_DESCENDANT_SELECTOR; break;
				case '>': type = CSS_CHILD_SELECTOR; break;
				case '+': type = CSS_ADJACENT_SELECTOR; break;
				case '[': type = CSS_ATTRIBUTE_SELECTOR; break;
				case '.': type = CSS_CLASS_SELECTOR; break;
				case '#': type = CSS_ID_SELECTOR; break;
				case ':': type = CSS_PSEUDO_CLASS_SELECTOR; break;
				default: type = CSS_TYPE_SELECTOR; extra = 0; break;
			}
			current_sel->type = type;
			
			// special cases, where there are knwon length values
			switch (type) {
				case CSS_UNIVERSAL_SELECTOR:
				case CSS_CHILD_SELECTOR:
				case CSS_DESCENDANT_SELECTOR:
				case CSS_ADJACENT_SELECTOR: {
					current_group->selectors.AddItem(current_sel);
					current_sel = new css_selector_s();
					pos = -1;
				} break;
				case CSS_ATTRIBUTE_SELECTOR: {
					// skip content, take it as literal
					while (data[abspos] != ']' && data[abspos] != 0) {
						pos++;
						abspos++;
					}
					pos -= 1;
					abspos -= 1;
				} break;
				default: break;
			}
			
			abspos++;
			pos++;
			continue;
		}
		
		const char *where = data + abspos;
		bool new_rule = false, new_group = false, new_sel = false;
		switch (where[0]) {
			case '{': {
				// the block will be put into a style attribute unchanged
				ReadBlock(where, current_rule->style);
				abspos += current_rule->style.Length() + 1;
				
				// this starts a new rule
				fRules.AddItem(current_rule);
				new_rule = true;
			}
			
			case ',': {
				// new selector group, old selector group is done
				current_rule->groups.AddItem(current_group);
				new_group = true;
				abspos += 1;
			}
			
			case '*':
			case '>':
			case '+':
			case '[':
			case '.':
			case '#':
			case ':':
			case ' ': {
				// delimiters, read in the value and add selector
				ReadValue(where, pos + extra, current_sel->value);
				
				// selector specific tasks
				if (current_sel->type == CSS_ATTRIBUTE_SELECTOR) {
					// strip away the extra "]" for attribute selectors
					current_sel->value.Truncate(current_sel->value.Length() - 1);
				}
				
				current_group->selectors.AddItem(current_sel);
				new_sel = true;
				
				pos = -1;
				abspos -= 1;
			}
			
			default: break;
		}
		
		if (new_rule)
			current_rule = new css_rule_s();
		if (new_group)
			current_group = new css_sel_group_s();
		if (new_sel)
			current_sel = new css_selector_s();
		
		abspos++;
		pos++;
	}

#if DEBUG_CSS	
	PrintToStream();
#endif
}

void
CSSParser::ReadValue(const char *from, int length, BString &to)
{
	if (!from)
		return;
	
	to.SetTo(from - length, length);
	//printf("%s\n", to.String());
}

void
CSSParser::ReadBlock(const char *from, BString &to)
{
	if (!from || from[0] != '{')
		return;
	
	int opencount = 0;
	int position = 0;
	
	do {
		switch (from[position]) {
			case '{': opencount++; break;
			case '}': opencount--; break;
			default: break;
		}
		
		position++;
	} while (opencount > 0);
	
	to.SetTo(from + 1, position - 2);
	//printf("%s\n", to.String());
}

#define DEBUG_CSS 0
#if DEBUG_CSS
static BString matchstring;
#endif

matching_t
CSSParser::IsMatching(BSVGElement *element, css_sel_group_s *group, int index, const char **attr, bool parsed)
{
	if (!element || !group)
		return B_NONE_MATCHING;
	
	bool checked = false;
	bool match = true;
	
	// backward applied
	for (int i = index; i >= 0 && match; i--) {
		css_selector_s *selector = group->selectors.ItemAt(i);
		
		switch (selector->type) {
			case CSS_UNIVERSAL_SELECTOR: {
				checked = true;
				// always matchs
#if DEBUG_CSS
				matchstring.Append("universal selector; ");
#endif
			} break;
			
			case CSS_TYPE_SELECTOR: {
				if (selector->value.Compare(element_name(element->Type())) != 0)
					match = false;
				
#if DEBUG_CSS
				if (match) {
					matchstring.Append("type selector: ");
					matchstring.Append(selector->value);
					matchstring.Append(" == ");
					matchstring.Append(element_name(element->Type()));
					matchstring.Append("; ");
				}
#endif
				checked = true;
			} break;
			
			case CSS_DESCENDANT_SELECTOR: {
				// go up in the element tree from now on
#if DEBUG_CSS
				if (match)
					matchstring.Append("descendant selector; ");
#endif
				matching_t descendant_match = B_UNKNOWN;
				BSVGElement *current = element;
				while (current && descendant_match != B_MATCHING) {
					// for this level there was no match, try one higher
					current = current->ParentElement();
					descendant_match = IsMatching(current, group, i - 1, attr, true);
				}
				
				return descendant_match;
			} break;
			
			case CSS_CHILD_SELECTOR: {
				// go one level up and return the result
#if DEBUG_CSS
				if (match)
					matchstring.Append("child selector; ");
#endif
				return IsMatching(element->ParentElement(), group, i - 1, attr, true);
			} break;
			
			case CSS_ADJACENT_SELECTOR: {
				// the element right before us needs to match everything before this selector
				BSVGElement *parent = element->ParentElement();
				BSVGElement *before = NULL;
				if (parent) {
					int32 index = parent->IndexOf(element, false);
					before = parent->ElementAt(index - 1);
				}
				
				if (!before) {
					match = false;
					break;
				}
				
#if DEBUG_CSS
				if (match)
					matchstring.Append("adjacent selector; ");
#endif
				return IsMatching(before, group, i - 1, attr, true);
			} break;
			
			case CSS_ATTRIBUTE_SELECTOR: {
				int exact_pos = selector->value.FindFirst("=");
				int spaced_pos = selector->value.FindFirst("~=");
				int hyphen_pos = selector->value.FindFirst("|=");
				int pos = MAX(spaced_pos, hyphen_pos);
				if (spaced_pos < 0 && hyphen_pos < 0)
					pos = MAX(pos, exact_pos);
				
				bool found = (MAX(pos, exact_pos) >= 0);
				if (!found)
					pos = selector->value.Length();
				BString attribute = "";
				found = false;
				
				// find the attribute we are comparing
				if (!parsed) {
					for (int i = 0; attr[i]; i += 2) {
						if (selector->value.Compare(attr[i], pos) == 0) {
							attribute = attr[i + 1];
							found = true;
							break;
						}
					}
				} else {
					attribute_s *att = element->FindAttributeByName(selector->value);
					if (att) {
						attribute = att->value;
						found = true;
					}
				}
				
				if (!found) {
					match = false;
					checked = true;
					break;
				}
				
				if (exact_pos < 0) {
					// we only checked wether we have it -> done
					checked = true;
#if DEBUG_CSS
					matchstring.Append("attribute selector: \"");
					matchstring.Append(selector->value);
					matchstring.Append("\" exists; ");
#endif
					break;
				}
				
				// determine the type of attribute matching
				found = false;
				if (spaced_pos >= 0 || hyphen_pos >= 0) {
					BString compare; // string that will be looked for
					selector->value.CopyInto(compare, pos + 2, selector->value.Length() - pos - 2);
					const char *delimiter = (spaced_pos >= 0 ? " " : "-");
					BString string = attribute;
					
					// add delemiter on each end of both compare and string
					// then we can just use a findfirst to determine presence
					// without having to tokenize the source string
					string.Insert(delimiter, 0);
					string.Append(delimiter);
					compare.Insert(delimiter, 0);
					compare.Append(delimiter);
					
					pos = string.FindFirst(compare);
					
					// we only match attributes beginning with for hyphens
					if (hyphen_pos >= 0)
						found = (pos == 0);
					else
						found = (pos >= 0);
					
					if (!found)
						match = false;
					
#if DEBUG
					if (match) {
						matchstring.Append("attribute selector: \"");
						matchstring.Append(selector->value);
						matchstring.Append("\" -> \"");
						matchstring.Append(compare);
						matchstring.Append("\" is in \"");
						matchstring.Append(string);
						matchstring.Append("\"; ");
					}
#endif
					
					checked = true;
				} else {
					BString compare;
					selector->value.CopyInto(compare, pos + 1, selector->value.Length() - pos - 1);
					
					// remove extra quotes as we don't get them from expat
					if (compare.FindFirst("\"") == 0 && compare.FindLast("\"") == compare.Length() - 1) {
						compare.RemoveFirst("\"");
						compare.RemoveLast("\"");
					}
					
					if (compare.Compare(attribute) != 0)
						match = false;
#if DEBUG_CSS
					if (match) {
						matchstring.Append("attribute selector: \"");
						matchstring.Append(selector->value);
						matchstring.Append("\" -> \"");
						matchstring.Append(compare);
						matchstring.Append("\" == \"");
						matchstring.Append(attribute);
						matchstring.Append("\"; ");
					}
#endif
					checked = true;
				}
			} break;
			
			case CSS_CLASS_SELECTOR: {
				BString _class = "";
				BString compare = selector->value;
				
				if (!parsed) {
					// "element" may not yet be parsed -> use attributes
					for (int i = 0; attr[i]; i += 2) {
						if (BString(attr[i]).Compare("class") == 0) {
							_class = attr[i + 1];
							break;
						}
					}
				} else {
					// some element that is already filled
					_class = element->Class();
				}
				
				// add delemiter on each end of both compare and string
				// then we can just use a findfirst to determine presence
				// without having to tokenize the source string
				_class.Insert(" ", 0);
				_class.Append(" ");
				compare.Insert(" ", 0);
				compare.Append(" ");
				
				if (_class.Length() == 0 || _class.FindFirst(compare) < 0)
					match = false;
				
#if DEBUG_CSS
				if (match) {
					matchstring.Append("class selector: \"");
					matchstring.Append(compare);
					matchstring.Append("\" is in \"");
					matchstring.Append(_class);
					matchstring.Append("\"; ");
				}
#endif
				checked = true;
			} break;
			
			case CSS_ID_SELECTOR: {
				BString id = "";
				
				if (!parsed) {
					// "element" may not yet be parsed -> use attributes
					for (int i = 0; attr[i]; i += 2) {
						if (BString(attr[i]).Compare("id") == 0) {
							id = attr[i + 1];
							break;
						}
					}
				} else {
					// some element that is already filled
					id = element->ID();
				}
				
				if (id.Length() == 0 || selector->value.Compare(id) != 0)
					match = false;
				
#if DEBUG_CSS
				if (match) {
					matchstring.Append("id selector: ");
					matchstring.Append(selector->value);
					matchstring.Append(" == ");
					matchstring.Append(id);
					matchstring.Append("; ");
				}
#endif
				checked = true;
			} break;
			
			case CSS_PSEUDO_CLASS_SELECTOR: {
				// pseudo classes are:
				const char *pseudo_class_names[] = { "first-child", "link", "visited", "hover", "active", "focus", "lang" };
				css_pseudo_class_t _class = CSS_PSEUDO_CLASS_FIRST_CHILD;
				for (uint32 i = 0; i < sizeof(pseudo_class_names) / sizeof(pseudo_class_names[0]); i++) {
					if (selector->value.Compare(pseudo_class_names[i]) == 0) {
						_class = (css_pseudo_class_t)i;
						break;
					}
				}
				
				switch (_class) {
					case CSS_PSEUDO_CLASS_FIRST_CHILD: {
						if (!element->ParentElement() || element->ParentElement()->ElementAt(0) != element)
							match = false;
						checked = true;
#if DEBUG_CSS
						if (match)
							matchstring.Append("first-child pseudo-class selector; ");
					} break;
					case CSS_PSEUDO_CLASS_LINK: printf("link pseudo-class selector unimplemented\n"); break;
					case CSS_PSEUDO_CLASS_VISITED: printf("visited pseudo-class selector unimplemented\n"); break;
					case CSS_PSEUDO_CLASS_HOVER: printf("hover pseudo-class selector unimplemented\n"); break;
					case CSS_PSEUDO_CLASS_ACTIVE: printf("active pseudo-class selector unimplemented\n"); break;
					case CSS_PSEUDO_CLASS_FOCUS: printf("focus pseudo-class selector unimplemented\n"); break;
					case CSS_PSEUDO_CLASS_LANG: printf("lang pseudo-class selector unimplemented\n"); break;
#else
					} break;
					default: break;
#endif
				}
			} break;
			
			case CSS_PSEUDO_ELEMENT_SELECTOR: {
#if DEBUG_CSS
				printf("pseudo element selector unimplemented\n");
#endif
			} break;
			
			default: break;
		}
	}
	
	if (match && checked)
		return B_MATCHING;
	else if (!match)
		return B_NONE_MATCHING;
	else
		return B_UNKNOWN;
}

attribute_s *
CSSParser::MatchElement(BSVGElement *element, const char **attr)
{
	for (int i = fRules.CountItems() - 1; i >= 0; i--) {
		css_rule_s *rule = fRules.ItemAt(i);
		
		for (int j = rule->groups.CountItems() - 1; j >= 0; j--) {
			css_sel_group_s *group = rule->groups.ItemAt(j);
#if DEBUG_CSS
			matchstring.SetTo("");
#endif
			matching_t match = IsMatching(element, group, group->selectors.CountItems() - 1, attr, false);
			if (match == B_MATCHING) {
#if DEBUG_CSS
				printf("matched a \"%s\" element because of: \"%s\" to \"%s\" (rule %d, group %d)\n", element_name(element->Type()), matchstring.String(), rule->style.String(), i, j);
#endif
				return new attribute_s(BString("style"), rule->style);
			}
		}
	}
	
	return NULL;
}

void
CSSParser::PrintToStream()
{
	printf("Input Data: %s\n", fData.String());
	
	for (int i = 0; i < fRules.CountItems(); i++) {
		css_rule_s *rule = fRules.ItemAt(i);
		printf("Rule %d: \"%s\"\n", i, rule->style.String());
		
		for (int j = 0; j < rule->groups.CountItems(); j++) {
			css_sel_group_s *group = rule->groups.ItemAt(j);
			printf("\tGroup %d:\n", j);
			
			for (int k = 0; k < group->selectors.CountItems(); k++) {
				css_selector_s *selector = group->selectors.ItemAt(k);
				
				const char *type = NULL;
				switch (selector->type) {
					case CSS_UNIVERSAL_SELECTOR: type = "universal"; break;
					case CSS_TYPE_SELECTOR: type = "type"; break;
					case CSS_DESCENDANT_SELECTOR: type = "descendant"; break;
					case CSS_CHILD_SELECTOR: type = "child"; break;
					case CSS_ADJACENT_SELECTOR: type = "adjacent"; break;
					case CSS_ATTRIBUTE_SELECTOR: type = "attribute"; break;
					case CSS_CLASS_SELECTOR: type = "class"; break;
					case CSS_ID_SELECTOR: type = "id"; break;
					case CSS_PSEUDO_CLASS_SELECTOR: type = "pseudo class"; break;
					case CSS_PSEUDO_ELEMENT_SELECTOR: type = "pseudo element"; break;
				};
				
				printf("\t\tSelector %d: type: %s; value: \"%s\"\n", k, type, selector->value.String());
			}
		}
	}
}
