#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "StyleParser.h"

StyleParser::StyleParser()
	:	fData(""),
		fBuffer(""),
		fPosition(0)
{
}

StyleParser::StyleParser(BString data)
	:	fData(data),
		fBuffer(""),
		fPosition(0)
{
}

StyleParser::StyleParser(const char *data)
	:	fData(data),
		fBuffer(""),
		fPosition(0)
{
}

StyleParser::~StyleParser()
{
}

void
StyleParser::SetData(BString &data)
{
	fData.SetTo(data);
}

void
StyleParser::SetData(const char *data)
{
	fData.SetTo(data);
}

void
StyleParser::AddData(BString &data)
{
	fData.Append(data);
}

void
StyleParser::AddData(const char *data)
{
	fData.Append(data);
}

void
StyleParser::Reset()
{
	fData.SetTo("");
	Rewind();
}

attribute_s *
StyleParser::GetNext()
{
	int length = fData.Length();
	if (length <= 0 || fPosition >= length)
		return NULL;
	
	int pos = fData.FindFirst(':', fPosition);
	if (pos < 0)
		return NULL;
	
	attribute_s *result = new attribute_s;
	
	fData.CopyInto(result->id, fPosition, pos++ - fPosition);
	
	int nextpos = fData.FindFirst(';', pos);
	
	if (nextpos < 0)
		nextpos = length;
	
	fData.CopyInto(result->value, pos, nextpos++ - pos);
	
	fPosition = nextpos;
	
	result->id.ReplaceSet(" \t\n", "");
	result->value.ReplaceSet("\r\t\n", " ");
	
	//printf("next: '%s' '%s'\n", result->id.String(), result->value.String());
	
	return result;
}

void
StyleParser::Rewind()
{
	fPosition = 0;
}

attribute_s *
StyleParser::Find(BString id)
{
	int pos = fData.FindFirst(id);
	if (pos < 0)
		return NULL;
	
	int sep = fData.FindFirst(':', pos);
	if (sep < 0)
		return NULL;
	
	attribute_s *result = new attribute_s;
	
	fData.CopyInto(result->id, pos, sep - pos);
	
	int end = fData.FindFirst(';', sep);
	if (end < 0)
		end = fData.Length();
	
	//printf("l: %d; p: %d; s: %d; e: %d\n", fData.Length(), pos, sep, end);
	fData.CopyInto(result->value, sep + 1, end - sep - 1);
	
	return result;
}

attribute_s *
StyleParser::Find(const char *id)
{
	return Find(BString(id));
}

bool
StyleParser::FindAndAsign(const char *id, const char *format, void *to_value)
{
	attribute_s *style = Find(BString(id));
	return sscanf(style->value.String(), format, to_value) == 1;
}
