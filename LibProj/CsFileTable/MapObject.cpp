
#include "stdafx.h"

#include "MapObject.h"

CsMapObject::CsMapObject():m_nOrderNumber(0)
{

}

CsMapObject::~CsMapObject()
{

}

bool CsMapObject::IsEmpty() const
{
	return m_FactInfoList.empty();
}

void CsMapObject::AddFactorInfo( sFACTOR_INFO& addInfo )
{
	m_FactInfoList.push_back( addInfo );
}

void CsMapObject::SetOrderNumber( int const& nNum )
{
	m_nOrderNumber = nNum;
}

int CsMapObject::GetOrderNumber() const
{
	return m_nOrderNumber;
}
 
int CsMapObject::GetMaxFactorCount() const
{
	return (int)m_FactInfoList.size() * 3;//
}

CsMapObject::FACT_LIST const& CsMapObject::GetFactInfoList() const
{ 
	return m_FactInfoList; 
}

CsMapObject::sFACTOR_INFO const* CsMapObject::GetFactorInfo( BYTE const& byCount ) const
{
	FACT_LIST_CIT it = m_FactInfoList.begin();
	for( int nCount = 0; it != m_FactInfoList.end(); ++it, ++nCount )
	{
		if( nCount == byCount )
			return &(*it);
	}

	return NULL;
}

void CsMapObject::DeleteFacotr( BYTE const& byCount )
{
	FACT_LIST_IT it = m_FactInfoList.begin();
	for( int nCount = 0; it != m_FactInfoList.end(); ++it, ++nCount )
	{
		if( nCount == byCount )
		{
			m_FactInfoList.erase(it);
			break;
		}
	}
}

void CsMapObject::ChangeFactInfo(sFACTOR_INFO& addInfo , int nCount)
{
	FACT_LIST_IT it = m_FactInfoList.begin();
	for( int n = 0; it != m_FactInfoList.end(); ++it, ++n )
	{
		if( nCount == n )
		{
			(*it).s_nOpenType = addInfo.s_nOpenType;
			(*it).s_nFactor = addInfo.s_nFactor;
			(*it).s_nFactorCnt = addInfo.s_nFactorCnt;
			break;
		}
	}
}
