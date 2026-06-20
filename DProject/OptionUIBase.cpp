#include "StdAfx.h"
#include "OptionUIBase.h"

COptionUIBase::COptionUIBase(void)
{
}

COptionUIBase::~COptionUIBase(void)
{
}

void COptionUIBase::IntAttribute(TiXmlElement* pElement, char* name, int* iVal)
{
	if(pElement==NULL || name==NULL || iVal==NULL)
		return;
	if(pElement->QueryIntAttribute(name, iVal) == TIXML_NO_ATTRIBUTE)
		*iVal = 0;
}

void COptionUIBase::BoolAttribute(TiXmlElement* pElement, char* name, bool* bVal, bool	DefaultValue)
{
	if(pElement==NULL || name==NULL || bVal==NULL)
		return;
	if(pElement->QueryBoolAttribute(name, bVal) == TIXML_NO_ATTRIBUTE)
		*bVal = DefaultValue;
}