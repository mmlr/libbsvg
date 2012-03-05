#include "SVGElement.h"
#include "SVGPath.h"
#include "SVGShapes.h"
#include "SVGText.h"
#include "SVGContainers.h"
#include "SVGStop.h"

#include "CSSParser.h"
#include "StyleParser.h"
#include "UnitHandler.h"
#include "SVGColors.h"

int AttributeCompareByID(const attribute_s *one, const attribute_s *two)
{
	return one->id.Compare(two->id);
}

int AttributeCompareByValue(const attribute_s *one, const attribute_s *two)
{
	return one->value.Compare(two->value);
}

BSVGElement::BSVGElement()
	:	fHeaderData(""),
		fFooterData(""),
		fID(""),
		fClass(""),
		fLink(""),
		fStatus(B_STATUS_UNRESOLVED),
		fAttributes(20, true),
		fAttributeNames(20, false),
		fAttributeValues(20, false),
		fState(),
		fParent(NULL),
		fParentElement(NULL),
		fElements(10, true),
		fVisible(true),
		fOpen(true)
{
}

BSVGElement::BSVGElement(BSVGView *parent)
	:	fHeaderData(""),
		fFooterData(""),
		fID(""),
		fClass(""),
		fLink(""),
		fStatus(B_STATUS_UNRESOLVED),
		fAttributes(20, true),
		fAttributeNames(20, false),
		fAttributeValues(20, false),
		fState(),
		fParent(parent),
		fParentElement(NULL),
		fElements(10, true),
		fVisible(true),
		fOpen(true)
{
}

BSVGElement::BSVGElement(BSVGView *parent, const BString &data)
	:	fHeaderData(""),
		fFooterData(""),
		fID(""),
		fClass(""),
		fLink(""),
		fStatus(B_STATUS_UNRESOLVED),
		fAttributes(20, true),
		fAttributeNames(20, false),
		fAttributeValues(20, false),
		fState(),
		fParent(parent),
		fParentElement(NULL),
		fElements(10, true),
		fVisible(true),
		fOpen(true)
{
}

BSVGElement::~BSVGElement()
{
	fElements.MakeEmpty();
}

BSVGElement::BSVGElement(BMessage *data)
	:	fStatus(B_STATUS_UNRESOLVED),
		fAttributes(20, true),
		fAttributeNames(20, false),
		fAttributeValues(20, false),
		fParent(NULL),
		fParentElement(NULL),
		fElements(10, true)
{
	BMessage message;
	BArchivable *object;
	
	data->FindString("_id", &fID);
	data->FindString("_class", &fClass);
	data->FindString("_header", &fHeaderData);
	data->FindString("_footer", &fFooterData);
	data->FindString("_link", &fLink);
	data->FindBool("_visible", &fVisible);
	data->FindBool("_open", &fOpen);
	
	if (data->FindMessage("_state", &message) == B_OK) {
		object = instantiate_object(&message);
		SVGState *state = dynamic_cast<SVGState *>(object);
		if (state)
			fState = *state;
	}
	
	//fState.PrintToStream();
	
	int32 type = 0;
	int32 i = 0;
	while (data->FindMessage("_elements", i, &message) == B_OK
			&& data->FindInt32("_elements_type", i++, &type) == B_OK) {
		object = instantiate_object(&message);
		
		switch (type) {
			case B_SVG_ELEMENT: {
				BSVGElement *element = dynamic_cast<BSVGElement *>(object);
				if (element)
					AddElement(element);
			} break;
			case B_SVG_ROOT: {
				BSVGRoot *root = dynamic_cast<BSVGRoot *>(object);
				if (root)
					AddElement(root);
			} break;
			case B_SVG_GROUP: {
				BSVGGroup *group = dynamic_cast<BSVGGroup *>(object);
				if (group)
					AddElement(group);
			} break;
			case B_SVG_DEFS: {
				BSVGDefs *defs = dynamic_cast<BSVGDefs *>(object);
				if (defs)
					AddElement(defs);
			} break;
			case B_SVG_USE: {
				BSVGUse *use = dynamic_cast<BSVGUse *>(object);
				if (use)
					AddElement(use);
			} break;
			case B_SVG_PATH: {
				BSVGPath *path = dynamic_cast<BSVGPath *>(object);
				if (path)
					AddElement(path);
			} break;
			case B_SVG_RECT: {
				BSVGRect *rect = dynamic_cast<BSVGRect *>(object);
				if (rect)
					AddElement(rect);
			} break;
			case B_SVG_ELLIPSE: {
				BSVGEllipse *ellipse = dynamic_cast<BSVGEllipse *>(object);
				if (ellipse)
					AddElement(ellipse);
			} break;
			case B_SVG_CIRCLE: {
				BSVGCircle *circle = dynamic_cast<BSVGCircle *>(object);
				if (circle)
					AddElement(circle);
			} break;
			case B_SVG_LINE: {
				BSVGLine *line = dynamic_cast<BSVGLine *>(object);
				if (line)
					AddElement(line);
			} break;
			case B_SVG_POLYLINE: {
				BSVGPolyline *polyline = dynamic_cast<BSVGPolyline *>(object);
				if (polyline)
					AddElement(polyline);
			} break;
			case B_SVG_POLYGON: {
				BSVGPolygon *polygon = dynamic_cast<BSVGPolygon *>(object);
				if (polygon)
					AddElement(polygon);
			} break;
			case B_SVG_TEXT: {
				BSVGText *text = dynamic_cast<BSVGText *>(object);
				if (text)
					AddElement(text);
			} break;
			case B_SVG_TEXTSPAN: {
				BSVGTextSpan *textspan = dynamic_cast<BSVGTextSpan *>(object);
				if (textspan)
					AddElement(textspan);
			} break;
			case B_SVG_TEXTPATH: {
				BSVGTextPath *textpath = dynamic_cast<BSVGTextPath *>(object);
				if (textpath)
					AddElement(textpath);
			} break;
			case B_SVG_STRING: {
				BSVGString *string = dynamic_cast<BSVGString *>(object);
				if (string)
					AddElement(string);
			} break;
			case B_SVG_GRADIENT: {
				BSVGGradient *gradient = dynamic_cast<BSVGGradient *>(object);
				if (gradient)
					AddElement(gradient);
			} break;
			case B_SVG_LINEARGRAD: {
				BSVGLinearGradient *lineargrad = dynamic_cast<BSVGLinearGradient *>(object);
				if (lineargrad)
					AddElement(lineargrad);
			} break;
			case B_SVG_RADIALGRAD: {
				BSVGRadialGradient *radialgrad = dynamic_cast<BSVGRadialGradient *>(object);
				if (radialgrad)
					AddElement(radialgrad);
			} break;
			case B_SVG_STOP: {
				BSVGStop *stop = dynamic_cast<BSVGStop *>(object);
				if (stop)
					AddElement(stop);
			} break;
			// no default
		}
	}
}

BArchivable *
BSVGElement::Instantiate(BMessage *data)
{
	if (validate_instantiation(data, "BSVGElement"))
		return new BSVGElement(data);
	return NULL;
}

status_t
BSVGElement::Archive(BMessage *data, bool deep) const
{
	data->AddString("_header", fHeaderData);
	data->AddString("_footer", fFooterData);
	data->AddString("_id", fID);
	data->AddString("_class", fClass);
	data->AddString("_link", fLink);
	data->AddBool("_visible", fVisible);
	data->AddBool("_open", fOpen);
	
	if (deep) {
		BMessage state;
		fState.Archive(&state, deep);
		data->AddMessage("_state", &state);
		
		for (int i = 0; i < fElements.CountItems(); i++) {
			BMessage message;
			fElements.ItemAt(i)->Archive(&message, deep);
			data->AddMessage("_elements", &message);
			data->AddInt32("_elements_type", fElements.ItemAt(i)->Type());
		}
	}
	
	ADDREPCLASS("BSVGElement");
	return B_OK;
}	

void
BSVGElement::SetHeaderData(const BString &data)
{
	fHeaderData = data;
}

BString
BSVGElement::HeaderData()
{
	return fHeaderData;
}

void
BSVGElement::SetFooterData(const BString &data)
{
	fFooterData = data;
}

BString
BSVGElement::FooterData()
{
	return fFooterData;
}

void
BSVGElement::RecreateData()
{
	AddID(fHeaderData);
	fState.AddToString(&fHeaderData);
	
	for (int i = 0; i < fElements.CountItems(); i++)
		fElements.ItemAt(i)->RecreateData();
	
	if (fElements.CountItems() == 0)
		fHeaderData << " />";
	else
		fHeaderData << ">";
}

void
BSVGElement::CollectData(BString &into, int32 indentlevel)
{
	BString temp = "";
	if (fHeaderData != "") {
		INDENT(into, indentlevel);
		SplitData(&temp, &fHeaderData, indentlevel);
		into << temp << "\n";
	}
	
	for (int i = 0; i < fElements.CountItems(); i++)
		fElements.ItemAt(i)->CollectData(into, indentlevel + 1);
	
	temp = "";
	if (fFooterData != "") {
		INDENT(into, indentlevel);
		SplitData(&temp, &fFooterData, indentlevel);
		into << temp << "\n";
	}
}

void
BSVGElement::CollectBounds(BRect &bounds, bool *first)
{
	for (int i = 0; i < fElements.CountItems(); i++)
		fElements.ItemAt(i)->CollectBounds(bounds, first);
}

#define PROMOTEMEMBER(x, y, z) \
	if (member == &x) { \
		x = *(z *)setto; \
		BSVGElement *element = NULL; \
		for (int i = 0; i < fElements.CountItems(); i++) { \
			element = fElements.ItemAt(i); \
			element->PromoteDownMember((void *)&element->x, setto); \
		} \
		return; \
	}

void
BSVGElement::PromoteDownMember(void *member, void *setto)
{
	PROMOTEMEMBER(fHeaderData, "header", BString);
	PROMOTEMEMBER(fFooterData, "footer", BString);
	PROMOTEMEMBER(fID, "id", BString);
	PROMOTEMEMBER(fClass, "class", BString);
	PROMOTEMEMBER(fLink, "link", BString);
	PROMOTEMEMBER(fState, "state", SVGState);
	PROMOTEMEMBER(fRenderState, "renderstate", SVGState);
	PROMOTEMEMBER(fParent, "parent", BSVGView *);
	PROMOTEMEMBER(fParentElement, "parentelement", BSVGElement *);
	PROMOTEMEMBER(fElements, "elements", ElementList);
	PROMOTEMEMBER(fVisible, "visible", bool);
	PROMOTEMEMBER(fOpen, "open", bool);
	
	printf("can't promote down member: unknown member\n");
}

void
BSVGElement::SetParent(BSVGView *parent)
{
	fParent = parent;
}

BSVGView *
BSVGElement::Parent()
{
	return fParent;
}

void
BSVGElement::SetID(const char *id)
{
	fID = id;
}

void
BSVGElement::SetID(const BString &id)
{
	fID = id;
}

BString
BSVGElement::ID()
{
	return fID;
}

void
BSVGElement::AddID(BString &to)
{
	if (fID != "")
		to << " id=\"" << fID << "\"";
	if (fClass != "")
		to << " class=\"" << fClass << "\"";
}

void
BSVGElement::SetClass(const char *_class)
{
	fClass = _class;
}

void
BSVGElement::SetClass(const BString &_class)
{
	fClass = _class;
}

BString
BSVGElement::Class()
{
	return fClass;
}

void
BSVGElement::SetLink(const char *link)
{
	fLink = link;
}

void
BSVGElement::SetLink(const BString &link)
{
	fLink = link;
}

BString
BSVGElement::Link()
{
	return fLink;
}

void
BSVGElement::SetVisible(bool visible)
{
	fVisible = visible;
}

bool
BSVGElement::Visible()
{
	return fVisible;
}

// attributes
void
BSVGElement::HandleAttribute(attribute_s *attr)
{
	float val = 0;
	UnitHandler::Handle(attr, &val, &fState, fState.general_viewport.Width());
	
	//printf("%s: \"%s\"\n", attr->id.String(), attr->value.String());
	if (attr->id.Compare("id") == 0) {
		SetID(attr->value);
	
	} else if (attr->id.Compare("class") == 0) {
		SetClass(attr->value);
	
	} else if (attr->id.Compare("stroke-width") == 0) {
		fState.stroke_width = val;
	
	} else if (attr->id.Compare("stroke-linecap") == 0) {
		bool error = false;
		cap_mode mode = B_BUTT_CAP;
		
		if (attr->value.Compare("butt") == 0)
			mode = B_BUTT_CAP;
		else if (attr->value.Compare("round") == 0)
			mode = B_ROUND_CAP;
		else if (attr->value.Compare("square") == 0)
			mode = B_SQUARE_CAP;
		else
			error = true;
		
		if (!error)
			fState.stroke_linecap = mode;
	
	} else if (attr->id.Compare("stroke-linejoin") == 0) {
		bool error = false;
		join_mode mode = B_MITER_JOIN;
		
		if (attr->value.Compare("miter") == 0)
			mode = B_MITER_JOIN;
		else if (attr->value.Compare("round") == 0)
			mode = B_ROUND_JOIN;
		else if (attr->value.Compare("bevel") == 0)
			mode = B_BEVEL_JOIN;
		else
			error = true;
		
		if (!error)
			fState.stroke_linejoin = mode;
	
	} else if (attr->id.Compare("stroke-miterlimit") == 0) {
		fState.stroke_miterlimit = val;
	
	} else if (attr->id.Compare("stroke-opacity") == 0) {
		UnitHandler::Handle(attr, &val, &fState, 255);
		fState.stroke_opacity = val;
	
	} else if (attr->id.Compare("stroke") == 0) {
		SVGColor color;
		BString server;
		paint_t type = HandleAttributeValue(&attr->value, B_TARGET_STROKE, (fParentElement ? fParentElement->State() : NULL), color, server);
		
		switch (type) {
			case B_FILL_CURRENT_COLOR:	// handled on rendertime
			case B_FILL_UNKNOWN:
			case B_FILL_NONE:		break;
			case B_FILL_COLOR:		fState.stroke_color = color; break;
			case B_FILL_SERVER:		fState.stroke_server = server; break;
		}
		
		fState.stroke_type = type;
			
	} else if (attr->id.Compare("fill-opacity") == 0) {
		UnitHandler::Handle(attr, &val, &fState, 255);
		fState.fill_opacity = val;
	
	} else if (attr->id.Compare("fill") == 0) {
		SVGColor color;
		BString server;
		paint_t type = HandleAttributeValue(&attr->value, B_TARGET_FILL, (fParentElement ? fParentElement->State() : NULL), color, server);
		
		switch (type) {
			case B_FILL_CURRENT_COLOR:	// handled on rendertime
			case B_FILL_UNKNOWN:
			case B_FILL_NONE:		break;
			case B_FILL_COLOR:		fState.fill_color = color; break;
			case B_FILL_SERVER:		fState.fill_server = server; break;
		}
		
		fState.fill_type = type;
	
	} else if (attr->id.Compare("fill-rule") == 0) {
		bool error = false;
		rule_t rule = B_NONZERO;
		
		if (attr->value.Compare("nonzero") == 0)
			rule = B_NONZERO;
		else if (attr->value.Compare("evenodd") == 0)
			rule = B_EVENODD;
		else
			error = true;
		
		if (!error)
			fState.fill_rule = rule;
	
	} else if (attr->id.Compare("opacity") == 0) {
		UnitHandler::Handle(attr, &val, &fState, 255);
		fState.general_opacity = val;
		fState.fill_opacity = val;
		fState.stroke_opacity = val;
		fState.stop_opacity = val;
	
	} else if (attr->id.Compare("transform") == 0) {
		fState.general_transform = BSVGView::HandleTransformation(attr->value);
	
	} else if (attr->id.Compare("clip-rule") == 0) {
		// todo
	
	} else if (attr->id.Compare("color") == 0) {
		SVGColor color;
		BString server;
		paint_t type = HandleAttributeValue(&attr->value, B_TARGET_GENERAL, (fParentElement ? fParentElement->State() : NULL), color, server);
		
		if (type == B_FILL_COLOR || type == B_FILL_CURRENT_COLOR)
			fState.general_color = color;
	
	} else if (attr->id.Compare("stop-color") == 0) {
		SVGColor color;
		BString server;
		paint_t type = HandleAttributeValue(&attr->value, B_TARGET_STOP, (fParentElement ? fParentElement->State() : NULL), color, server);
		
		if (type == B_FILL_COLOR || type == B_FILL_CURRENT_COLOR)
			fState.stop_color = color.Color();
	
	} else if (attr->id.Compare("stop-opacity") == 0) {
		UnitHandler::Handle(attr, &val, &fState, 255);
		fState.stop_opacity = val;
	
	} else if (attr->id.Compare("font-family") == 0) {
		fState.font_family = attr->value;
	
	} else if (attr->id.Compare("font-size") == 0) {
		fState.font_size = val;
	
	} else if (attr->id.Compare("kerning") == 0) {
		fState.font_kerning = val;
	
	} else if (attr->id.Compare("letter-spacing") == 0) {
		fState.font_letterspacing = val;
	
	} else if (attr->id.Compare("word-spacing") == 0) {
		fState.font_wordspacing = val;
	
	} else if (attr->id.Compare("text-anchor") == 0) {
		bool error = false;
		anchor_t anchor = B_ANCHOR_START;
		
		if (attr->value.Compare("start") == 0)
			anchor = B_ANCHOR_START;
		else if (attr->value.Compare("middle") == 0)
			anchor = B_ANCHOR_MIDDLE;
		else if (attr->value.Compare("end") == 0)
			anchor = B_ANCHOR_END;
		else
			error = true;
		
		if (!error)
			fState.text_anchor = anchor;
	
	} else if (attr->id.Compare("viewBox") == 0) {
		float vals[4];
		int count = sscanf(attr->value.String(), "%f %f %f %f", vals, vals + 1, vals + 2, vals + 3);
		
		if (count == 4)
			fState.general_viewbox = BRect(vals[0], vals[1], vals[2], vals[3]);
	
	} else if (attr->id.Compare("preserveAspectRatio") == 0) {
		const char *string = attr->value.String();
		char buffer[50];
		int32 aspect = ASPECT_NONE;
		
		if (sscanf(string, "%s", buffer) == 1) {
			if (strstr(buffer, "defer") >= string) {
				string += 5;
				sscanf(string, "%s", buffer);
			}
			
			if (strncmp(buffer, "none", 4) != 0) {
				if (strncmp(&buffer[0], "xMin", 4) == 0)
					aspect += ASPECT_XMIN;
				else if (strncmp(&buffer[0], "xMid", 4) == 0)
					aspect += ASPECT_XMID;
				else if (strncmp(&buffer[0], "xMax", 4) == 0)
					aspect += ASPECT_XMAX;
				
				if (strncmp(&buffer[4], "YMin", 4) == 0)
					aspect += ASPECT_YMIN;
				else if (strncmp(&buffer[4], "YMid", 4) == 0)
					aspect += ASPECT_YMID;
				else if (strncmp(&buffer[4], "YMax", 4) == 0)
					aspect += ASPECT_YMAX;
				
				if (sscanf(string, "%*s %s", buffer) > 0) {
					if (strncmp(buffer, "meet", 4) == 0)
						aspect += ASPECT_MEET;
					else if (strncmp(buffer, "slice", 5) == 0)
						aspect += ASPECT_SLICE;
				}
			}
			
			fState.general_aspect = (aspect_t)aspect;
		}
	
	} else if (attr->id.Compare("xlink:href") == 0) {
		// + 1 = strip the #
		SetLink(attr->value.String() + 1);
	}
}

void
BSVGElement::HandleAttributes()
{
	for (int i = 0; i < fAttributes.CountItems(); i++)
		HandleAttribute(fAttributes.ItemAt(i));
}

void
BSVGElement::AddAttribute(attribute_s *attr, bool handle)
{
	if (!attr)
		return;
	
	if (attr->id.Compare("style") == 0) {
		StyleParser parser(attr->value);
		attribute_s *attr;
		while ((attr = parser.GetNext())) {
			AddAttribute(attr, handle);
		}
		delete attr;
		return;
	}
	
	if (!fAttributeNames.BinaryInsertUnique(attr, &AttributeCompareByID)) {
		attribute_s *present = FindAttributeByName(attr->id);
		if (present) {
			present->value = attr->value;
			delete attr;
			attr = present;
			
			fAttributeValues.BinaryInsert(attr, &AttributeCompareByValue);
			fAttributes.RemoveItem(attr, false);
			fAttributes.AddItem(attr);
		}
	} else {
			fAttributeValues.BinaryInsert(attr, &AttributeCompareByValue);
			fAttributes.AddItem(attr);
	}
	
	if (handle)
		HandleAttribute(attr);
}

void
BSVGElement::RemoveAttribute(attribute_s *attr)
{
	fAttributes.RemoveItem(attr);
}

attribute_s *
BSVGElement::RemoveAttributeAt(int32 index)
{
	return fAttributes.RemoveItemAt(index);
}

void
BSVGElement::ClearAttributes()
{
	fAttributes.MakeEmpty();
}

attribute_s *
BSVGElement::AttributeAt(int32 index)
{
	return fAttributes.ItemAt(index);
}

int32
BSVGElement::IndexOf(attribute_s *attr)
{
	return fAttributes.IndexOf(attr);
}

int32
BSVGElement::CountAttributes()
{
	return fAttributes.CountItems();
}

attribute_s *
BSVGElement::FindAttributeByName(const BString &name)
{
	attribute_s attr(name, BString(""));
	return const_cast<attribute_s *>(fAttributeNames.BinarySearch(attr, &AttributeCompareByID));
}

attribute_s *
BSVGElement::FindAttributeByValue(const BString &value)
{
	attribute_s attr(BString(""), value);
	return const_cast<attribute_s *>(fAttributeValues.BinarySearch(attr, &AttributeCompareByValue));
}

bool
BSVGElement::HasAttribute(const BString &name)
{
	return FindAttributeByName(name) != NULL;
}

// elements
void
BSVGElement::OpenElement()
{
	fOpen = true;
}

void
BSVGElement::CloseElement()
{
	fOpen = false;
}

bool
BSVGElement::IsOpen()
{
	return fOpen;
}

void
BSVGElement::AddElement(BSVGElement *element, int32 index)
{
	element->SetParentElement(this);
	if ((element->Type() & B_SVG_CONTAINER_KIND) == 0
		&& (element->Type() & B_SVG_TEXT_KIND) == 0)
		element->CloseElement();
	
	if (index >= 0)
		fElements.AddItem(element, index);
	else
		fElements.AddItem(element);
}

int32
BSVGElement::CountElements(bool deep)
{
	if (!deep)
		return fElements.CountItems();
	
	int32 count = fElements.CountItems();
	for (int i = 0; i < fElements.CountItems(); i++)
		count += fElements.ItemAt(i)->CountElements();
	
	// we are not inside another element
	if (!fParentElement)
		count++;
	
	return count;
}

BSVGElement *
BSVGElement::FindElement(const BString &id, uint32 of_type, bool deep)
{	
	//printf("name: %s; search: %s\n", fName.String(), name.String());
	if (fID == id && (of_type == 0 || of_type & Type() & 0x0fffffff))
		return this;
	
	if (!deep) {
		for (int i = 0; i < fElements.CountItems(); i++) {
			BSVGElement *element = fElements.ItemAt(i);
			if (element && (of_type == 0 || of_type & element->Type() & 0x0fffffff))
				return element;
		}
		
		return NULL;
	}
	
	for (int i = 0; i < fElements.CountItems(); i++) {
		BSVGElement *element = fElements.ItemAt(i)->FindElement(id);
		if (element && (of_type == 0 || of_type & element->Type() & 0x0fffffff))
			return element;
	}
	
	return NULL;
}

status_t
BSVGElement::FindElements(const BString &_class, ElementList &elements)
{
	//printf("name: %s; search: %s\n", fName.String(), name.String());
	if (fClass == _class)
		elements.AddItem(this);
	
	for (int i = 0; i < fElements.CountItems(); i++)
		fElements.ItemAt(i)->FindElements(_class, elements);
	
	return B_OK;
}

BSVGElement *
BSVGElement::FindParentElement(uint32 of_type)
{
	BSVGElement *parent = this;
	while (parent) {
		parent = parent->ParentElement();
		
		if (!parent)
			return NULL;
		
		if (of_type == 0 || (of_type & parent->Type() & 0x0fffffff))
			return parent;
	}
	
	printf("e\n");
	return NULL;
}

BSVGElement *
BSVGElement::ElementAt(int index, bool deep)
{
	if (!deep) {
		if (index >= fElements.CountItems())
			return NULL;
		return fElements.ItemAt(index);
	}
	
	BSVGElement *result = NULL;
	ElementAtInternal(index, result);
	return result;
}

int32
BSVGElement::ElementAtInternal(int32 index, BSVGElement *&result)
{
	if (index == -1) {
		result = this;
		return index;
	}
	
	int32 count = fElements.CountItems();
	for (int i = 0; i < count && index >= 0; i++)
		index = fElements.ItemAt(i)->ElementAtInternal(index - 1, result);
	
	return index;
}

int32
BSVGElement::IndexOf(BSVGElement *child, bool deep)
{
	if (!deep)
		return fElements.IndexOf(child);
	
	int32 result = 0;
	if (IndexOfInternal(child, &result))
		return result;
	
	return -1;
}

bool
BSVGElement::IndexOfInternal(BSVGElement *child, int32 *result)
{
	int32 index = fElements.IndexOf(child);
	if (index >= 0) {
		*result += index;
		return true;
	}
	
	for (int i = 0; i < fElements.CountItems(); i++) {
		if (fElements.ItemAt(i)->IndexOfInternal(child, result))
			return true;
	}
	
	result += fElements.CountItems();
	return false;
}

BSVGElement *
BSVGElement::RemoveElement(const BString &name, bool deep)
{
	BSVGElement *element = FindElement(name, 0, deep);
	if (element)
		return RemoveElement(element);
	
	return NULL;
}

BSVGElement *
BSVGElement::RemoveElement(BSVGElement *element, bool deep)
{
	for (int i = 0; i < fElements.CountItems(); i++) {
		if (fElements.ItemAt(i) == element)
			return fElements.RemoveItemAt(i);
	}
		
	if (!deep)
		return NULL;
	
	for (int i = 0; i < fElements.CountItems(); i++) {
		BSVGElement *elem = fElements.ItemAt(i)->RemoveElement(element);
		if (elem)
			return elem;
	}
	
	return NULL;
}

BSVGElement *
BSVGElement::RemoveElementAt(int index, bool deep)
{
	if (!deep) {
		if (index >= fElements.CountItems())
			return NULL;
		return fElements.RemoveItemAt(index);
	}
	
	BSVGElement *element = ElementAt(index, deep);
	if (element) {
		BSVGElement *parent = element->ParentElement();
		if (parent)
			parent->fElements.RemoveItem(element, false);
		return element;
	}
	
	return NULL;
}

BSVGElement *
BSVGElement::RemoveSelf()
{
	if (!fParentElement)
		return this;
	
	return fParentElement->RemoveElement(this, false);
}

BSVGElement *
BSVGElement::LastElement()
{
	if (fElements.CountItems() == 0)
		return this;
	
	return fElements.LastItem()->LastElement();
}

BSVGElement *
BSVGElement::LastContainer()
{
	BSVGElement *element = fElements.LastItem();
	if (element && element->IsOpen())
		return element->LastContainer();
	else {
		if (IsOpen())
			return this;
		
		BSVGElement *element = ParentElement();
		if (element && element->IsOpen())
			return element;
	}
	
	return NULL;
}

BSVGElement *
BSVGElement::OpenContainer(BSVGElement *container)
{
	BSVGElement *element = (container ? container : new BSVGGroup(fParent));
	LastContainer()->AddElement(element);
	return element;
}

BSVGElement *
BSVGElement::OpenContainer(SVGState *state)
{
	BSVGElement *element = new BSVGGroup(fParent);
	if (state)
		element->SetState(*state);
	
	LastContainer()->AddElement(element);
	return element;
}

void
BSVGElement::CloseContainer()
{
	BSVGElement *last = LastContainer();
	if (last)
		last->CloseElement();
}

void
BSVGElement::SetParentElement(BSVGElement *parent)
{
	fParentElement = parent;
}

BSVGElement *
BSVGElement::ParentElement()
{
	return fParentElement;
}

void
BSVGElement::DumpTree(int indent)
{
	for (int i = 0; i < indent; i++)
		printf("\t");
	
	const char *type = element_name(Type());
	printf("<%s id=\"%s\" class=\"%s\" status=\"%s\"%s>\n", type, fID.String(), fClass.String(), IsOpen() ? "open" : "closed", fElements.CountItems() == 0 ? " /" : "");
	
	for (int i = 0; i < fElements.CountItems(); i++)
		fElements.ItemAt(i)->DumpTree(indent + 1);
	
	if (fElements.CountItems() > 0) {
		for (int i = 0; i < indent; i++)
			printf("\t");
		
		printf("</%s>\n", type);
	}
}

void
BSVGElement::PrintToStream()
{
	const char *type = element_name(Type());
	printf("BSVGElement: type=\"%s\"; id=\"%s\"; class=\"%s\"; status=\"%s\";\n", type, fID.String(), fClass.String(), IsOpen() ? "open" : "closed");
}

void
BSVGElement::PrepareRendering(SVGState *inherit)
{	
	if (inherit)
		fRenderState.Inherit(*inherit, true);
	else
		fRenderState.Reset();
	
	fRenderState.Merge(fState, B_MULTIPLY_BY_SOURCE);
	
	for (int i = 0; i < fElements.CountItems(); i++)
		fElements.ItemAt(i)->PrepareRendering(&fRenderState);
}

void
BSVGElement::Render(BView *view)
{
	for (int i = 0; i < fElements.CountItems(); i++)
		fElements.ItemAt(i)->Render(view);
}

void
BSVGElement::Render(BWindow *window)
{
	for (int i = 0; i < fElements.CountItems(); i++)
		fElements.ItemAt(i)->Render(window);
}

void
BSVGElement::Render(BBitmap *bitmap)
{
	for (int i = 0; i < fElements.CountItems(); i++)
		fElements.ItemAt(i)->Render(bitmap);
}

void
BSVGElement::RenderCommon()
{
}

void
BSVGElement::SetState(const SVGState &state)
{
	fState = state;
}

SVGState *
BSVGElement::State()
{
	return &fState;
}

void
BSVGElement::SetRenderState(const SVGState &state)
{
	fRenderState = state;
}

SVGState *
BSVGElement::RenderState()
{
	return &fRenderState;
}


status_t
BSVGElement::ResolveField(field_t field, void *result)
{
	source_t source = fState.GetSource(field);
	
	if (source == SOURCE_SET) {
		fState.GetValue(field, result);
		return B_OK;
	}
	
	if (fParentElement)
		return fParentElement->ResolveField(field, result);
	else
		return B_ERROR;
}

// ************************************************************
// boring SVGState API from here

void
BSVGElement::SetValue(field_t field, void *to)
{
	fState.SetValue(field, to);
}

void
BSVGElement::GetValue(field_t field, void *result)
{
	fState.GetValue(field, result);
}

void
BSVGElement::SetSource(field_t field, source_t to)
{
	fState.SetSource(field, to);
}

source_t
BSVGElement::GetSource(field_t field)
{
	return fState.GetSource(field);
}

void
BSVGElement::UnsetField(field_t fieldcode)
{
	fState.UnsetField(fieldcode);
}

void
BSVGElement::SetOpacity(float opacity)
{
	fState.general_opacity = opacity;
}

void
BSVGElement::UnsetOpacity()
{
	fState.general_opacity.source = SOURCE_UNSET;
}

bool
BSVGElement::IsOpacitySet()
{
	return fState.general_opacity.source == SOURCE_SET;
}

void
BSVGElement::SetFillType(paint_t type)
{
	fState.fill_type = type;
}

void
BSVGElement::UnsetFillType()
{
	fState.fill_type.source = SOURCE_UNSET;
}

bool
BSVGElement::IsFillTypeSet()
{
	return fState.fill_type.source == SOURCE_SET;
}

paint_t
BSVGElement::FillType()
{
	return fState.fill_type();
}

void
BSVGElement::SetFillServer(BString id)
{
	fState.fill_server = id;
}

void
BSVGElement::SetFillServer(const char *id)
{
	fState.fill_server = id;
}

void
BSVGElement::UnsetFillServer()
{
	fState.fill_server.source = SOURCE_UNSET;
}

bool
BSVGElement::IsFillServerSet()
{
	return fState.fill_server.source == SOURCE_SET;
}

const char *
BSVGElement::FillServer()
{
	return fState.fill_server.String();
}

void
BSVGElement::SetFillColor(rgb_color color)
{
	fState.fill_color = color;
}

void
BSVGElement::SetFillColor(uchar r, uchar g, uchar b)
{
	fState.fill_color(r, g, b);
	fState.fill_color.source = SOURCE_SET;
}

void
BSVGElement::UnsetFillColor()
{
	fState.fill_color.source = SOURCE_UNSET;
}

bool
BSVGElement::IsFillColorSet()
{
	return fState.fill_color.source == SOURCE_SET;
}

rgb_color
BSVGElement::FillColor()
{
	return fState.fill_color.Color();
}

void
BSVGElement::SetFillOpacity(float opacity)
{
	fState.fill_opacity = opacity;
}

void
BSVGElement::UnsetFillOpacity()
{
	fState.fill_opacity.source = SOURCE_UNSET;
}

bool
BSVGElement::IsFillOpacitySet()
{
	return fState.fill_opacity.source == SOURCE_SET;
}

float
BSVGElement::FillOpacity()
{
	return fState.fill_opacity();
}

void
BSVGElement::SetFillRule(rule_t rule)
{
	fState.fill_rule = rule;
}

void
BSVGElement::UnsetFillRule()
{
	fState.fill_rule.source = SOURCE_UNSET;
}

bool
BSVGElement::IsFillRuleSet()
{
	return fState.fill_rule.source == SOURCE_SET;
}

rule_t
BSVGElement::FillRule()
{
	return fState.fill_rule();
}

void
BSVGElement::SetStrokeType(paint_t type)
{
	fState.stroke_type = type;
}

void
BSVGElement::UnsetStrokeType()
{
	fState.stroke_type.source = SOURCE_UNSET;
}

bool
BSVGElement::IsStrokeTypeSet()
{
	return fState.stroke_type.source == SOURCE_SET;
}

paint_t
BSVGElement::StrokeType()
{
	return fState.stroke_type();
}

void
BSVGElement::SetStrokeServer(BString id)
{
	fState.stroke_server = id;
}

void
BSVGElement::SetStrokeServer(const char *id)
{
	fState.stroke_server = id;
}

void
BSVGElement::UnsetStrokeServer()
{
	fState.stroke_server.source = SOURCE_UNSET;
}

bool
BSVGElement::IsStrokeServerSet()
{
	return fState.stroke_server.source == SOURCE_SET;
}

const char *
BSVGElement::StrokeServer()
{
	return fState.stroke_server.String();
}

void
BSVGElement::SetStrokeColor(rgb_color color)
{
	fState.stroke_color = color;
}

void
BSVGElement::SetStrokeColor(uchar r, uchar g, uchar b)
{
	fState.stroke_color(r, g, b);
	fState.stroke_color.source = SOURCE_SET;
}

void
BSVGElement::UnsetStrokeColor()
{
	fState.stroke_color.source = SOURCE_UNSET;
}

bool
BSVGElement::IsStrokeColorSet()
{
	return fState.stroke_color.source == SOURCE_SET;
}

rgb_color
BSVGElement::StrokeColor()
{
	return fState.stroke_color.Color();
}

void
BSVGElement::SetStrokeOpacity(float opacity)
{
	fState.stroke_opacity = opacity;
}

void
BSVGElement::UnsetStrokeOpacity()
{
	fState.stroke_opacity.source = SOURCE_UNSET;
}

bool
BSVGElement::IsStrokeOpacitySet()
{
	return fState.stroke_opacity.source == SOURCE_SET;
}

float
BSVGElement::StrokeOpacity()
{
	return fState.stroke_opacity();
}

void
BSVGElement::SetStrokeWidth(float width)
{
	fState.stroke_width = width;
}

void
BSVGElement::UnsetStrokeWidth()
{
	fState.stroke_width.source = SOURCE_UNSET;
}

bool
BSVGElement::IsStrokeWidthSet()
{
	return fState.stroke_width.source == SOURCE_SET;
}

float
BSVGElement::StrokeWidth()
{
	return fState.stroke_width();
}

void
BSVGElement::SetStrokeLineCap(cap_mode mode)
{
	fState.stroke_linecap = mode;
}

void
BSVGElement::UnsetStrokeLineCap()
{
	fState.stroke_linecap.source = SOURCE_UNSET;
}

bool
BSVGElement::IsStrokeLineCapSet()
{
	return fState.stroke_linecap.source == SOURCE_SET;
}

cap_mode
BSVGElement::StrokeLineCap()
{
	return fState.stroke_linecap();
}

void
BSVGElement::SetStrokeLineJoin(join_mode mode)
{
	fState.stroke_linejoin = mode;
}

void
BSVGElement::UnsetStrokeLineJoin()
{
	fState.stroke_linejoin.source = SOURCE_UNSET;
}

bool
BSVGElement::IsStrokeLineJoinSet()
{
	return fState.stroke_linejoin.source == SOURCE_SET;
}

join_mode
BSVGElement::StrokeLineJoin()
{
	return fState.stroke_linejoin();
}

void
BSVGElement::SetStrokeMiterLimit(float limit)
{
	fState.stroke_miterlimit = limit;
}

void
BSVGElement::UnsetStrokeMiterLimit()
{
	fState.stroke_miterlimit.source = SOURCE_UNSET;
}

bool
BSVGElement::IsStrokeMiterLimitSet()
{
	return fState.stroke_miterlimit.source == SOURCE_SET;
}

float
BSVGElement::StrokeMiterLimit()
{
	return fState.stroke_miterlimit();
}

void
BSVGElement::SetStopColor(rgb_color color)
{
	fState.stop_color = color;
}

void
BSVGElement::SetStopColor(uchar r, uchar g, uchar b)
{
	fState.stop_color(r, g, b);
	fState.stop_color.source = SOURCE_SET;
}

void
BSVGElement::UnsetStopColor()
{
	fState.stop_color.source = SOURCE_UNSET;
}

bool
BSVGElement::IsStopColorSet()
{
	return fState.stop_color.source == SOURCE_SET;
}

rgb_color
BSVGElement::StopColor()
{
	return fState.stop_color.Color();
}

void
BSVGElement::SetStopOpacity(float opacity)
{
	fState.stop_opacity = opacity;
}

void
BSVGElement::UnsetStopOpacity()
{
	fState.stop_opacity.source = SOURCE_UNSET;
}

bool
BSVGElement::IsStopOpacitySet()
{
	return fState.stop_opacity.source == SOURCE_SET;
}

float
BSVGElement::StopOpacity()
{
	return fState.stop_opacity();
}

void
BSVGElement::SetTransformation(Matrix2D &transformation)
{
	fState.general_transform = transformation;
}

void
BSVGElement::UnsetTransformation()
{
	fState.general_transform.source = SOURCE_UNSET;
}

bool
BSVGElement::IsTransformationSet()
{
	return fState.general_transform.source == SOURCE_SET;
}

Matrix2D
BSVGElement::Transformation()
{
	return fState.general_transform;
}

void
BSVGElement::SetFontFamily(const char *family)
{
	fState.font_family = family;
}

void
BSVGElement::SetFontFamily(const BString &family)
{
	fState.font_family = family;
}

void
BSVGElement::UnsetFontFamily()
{
	fState.font_family.source = SOURCE_UNSET;
}

bool
BSVGElement::IsFontFamilySet()
{
	return fState.font_family.source == SOURCE_SET;
}

const char *
BSVGElement::FontFamily()
{
	return fState.font_family.String();
}

void
BSVGElement::SetFontSize(float size)
{
	fState.font_size = size;
}

void
BSVGElement::UnsetFontSize()
{
	fState.font_size.source = SOURCE_UNSET;
}

bool
BSVGElement::IsFontSizeSet()
{
	return fState.font_size.source == SOURCE_SET;
}

float
BSVGElement::FontSize()
{
	return fState.font_size();
}

void
BSVGElement::SetFontKerning(float kerning)
{
	fState.font_kerning = kerning;
}

void
BSVGElement::UnsetFontKerning()
{
	fState.font_kerning.source = SOURCE_UNSET;
}

bool
BSVGElement::IsFontKerningSet()
{
	return fState.font_kerning.source == SOURCE_SET;
}

float
BSVGElement::FontKerning()
{
	return fState.font_kerning();
}

void
BSVGElement::SetLetterSpacing(float spacing)
{
	fState.font_letterspacing = spacing;
}

void
BSVGElement::UnsetLetterSpacing()
{
	fState.font_letterspacing.source = SOURCE_UNSET;
}

bool
BSVGElement::IsLetterSpacingSet()
{
	return fState.font_letterspacing.source == SOURCE_SET;
}

float
BSVGElement::LetterSpacing()
{
	return fState.font_letterspacing();
}

void
BSVGElement::SetWordSpacing(float spacing)
{
	fState.font_wordspacing = spacing;
}

void
BSVGElement::UnsetWordSpacing()
{
	fState.font_wordspacing.source = SOURCE_UNSET;
}

bool
BSVGElement::IsWordSpacingSet()
{
	return fState.font_wordspacing.source == SOURCE_SET;
}

float
BSVGElement::WordSpacing()
{
	return fState.font_wordspacing();
}

void
BSVGElement::SetFontDecoration(decoration_t decoration)
{
	fState.font_decoration = decoration;
}

void
BSVGElement::UnsetFontDecoration()
{
	fState.font_decoration.source = SOURCE_UNSET;
}

bool
BSVGElement::IsFontDecorationSet()
{
	return fState.font_decoration.source == SOURCE_SET;
}

decoration_t
BSVGElement::FontDecoration()
{
	return fState.font_decoration();
}

void
BSVGElement::SetTextAnchor(anchor_t anchor)
{
	fState.text_anchor = anchor;
}

void
BSVGElement::UnsetTextAnchor()
{
	fState.text_anchor.source = SOURCE_UNSET;
}

bool
BSVGElement::IsTextAnchorSet()
{
	return fState.text_anchor.source == SOURCE_SET;
}

anchor_t
BSVGElement::TextAnchor()
{
	return fState.text_anchor();
}

void
BSVGElement::SetStartOffset(float offset)
{
	fState.start_offset = offset;
}

void
BSVGElement::UnsetStartOffset()
{
	fState.start_offset.source = SOURCE_UNSET;
}

bool
BSVGElement::IsStartOffsetSet()
{
	return fState.start_offset.source == SOURCE_SET;
}

float
BSVGElement::StartOffset()
{
	return fState.start_offset();
}

void
BSVGElement::SetCurrentColor(rgb_color color)
{
	fState.general_color = color;
}

void
BSVGElement::SetCurrentColor(uchar r, uchar g, uchar b)
{
	fState.general_color(r, g, b);
	fState.general_color.source = SOURCE_SET;
}

void
BSVGElement::UnsetCurrentColor()
{
	fState.general_color.source = SOURCE_UNSET;
}

bool
BSVGElement::IsCurrentColorSet()
{
	return fState.general_color.source == SOURCE_SET;
}

rgb_color
BSVGElement::CurrentColor()
{
	return fState.general_color.Color();
}

void
BSVGElement::SetViewPort(BRect viewport)
{
	fState.general_viewport = viewport;
}

void
BSVGElement::UnsetViewPort()
{
	fState.general_viewport.source = SOURCE_UNSET;
}

bool
BSVGElement::IsViewPortSet()
{
	return fState.general_viewport.source == SOURCE_SET;
}

BRect
BSVGElement::ViewPort()
{
	return fState.general_viewport;
}
