
#include "stdafx.h"
#include "TableClass_UIText.h"

BEGIN_NAME_TABLE


CUITextTable::CUITextTable()
{

}
CUITextTable::~CUITextTable()
{

}

void CUITextTable::InitTable(int const& nServiceType)
{
	AddChangeStringEvent( ESTRING_TABLE_UITEXT, this, &CUITextTable::_InsertString );
}

void CUITextTable::ReleaseTable()
{
	m_kItems.clear();
}

bool CUITextTable::_InsertString(ITbImpl const* pData)
{
	ReleaseTable();

	CSTable_UIText const* pStrData = static_cast<CSTable_UIText const*>(pData);
	pStrData->DataSwap( m_kItems );
	return true;
}

std::wstring CUITextTable::GetUIText(char const* pKey) const
{	
	size_t hashKey = DmCS::GetHashCode( pKey );
	ItemCont::const_iterator it = m_kItems.find( hashKey );
	if( it == m_kItems.end() )
		return std::wstring(pKey, pKey + strlen(pKey));
	return it->second;	
}	

END_NAME_TABLE