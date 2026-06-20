
#include "stdafx.h"
#include "QuestCompRequire.h"

CsQuestCompRequire::CsQuestCompRequire():m_eType(MONSTER),m_nTypeID(0),m_nTypeCount(0),m_nTargetID(0),
m_nSubValue(0),m_nSubValue2(0),m_nCurTypeCount(0)
{

}

void CsQuestCompRequire::Init( eTYPE type, UINT nTypeID, int nTypeCount, UINT nTargetID, int nSubValue, int nSubValue2 )
{
	m_eType = type;
	m_nTypeID = nTypeID;
	m_nTypeCount = nTypeCount;
	m_nTargetID = nTargetID;
	m_nSubValue = nSubValue;
	m_nSubValue2 = nSubValue2;
	m_nCurTypeCount = 0;
}

void CsQuestCompRequire::Reset()
{ 
	m_eType = MONSTER; 
	m_nTypeID = 0; 
	m_nTypeCount = 0; 
	m_nCurTypeCount = 0; 
	m_nTargetID = 0; 
	m_nSubValue = 0;
	m_nSubValue2 = 0;
}

CsQuestCompRequire* CsQuestCompRequire::Clone()
{
	CsQuestCompRequire* pReturn = csnew CsQuestCompRequire;
	pReturn->Init( m_eType, m_nTypeID, m_nTypeCount, m_nTargetID, m_nSubValue, m_nSubValue2 );
	return pReturn;
}
