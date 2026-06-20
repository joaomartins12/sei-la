
#include "stdafx.h"
#include "QuestPreRequire.h"

void CsQuestPreRequire::Init(eTYPE type, UINT nTypeID, int nTypeCount )
{
	m_eType = type;
	m_nTypeID = nTypeID;
	m_nTypeCount = nTypeCount;
}

CsQuestPreRequire* CsQuestPreRequire::Clone()
{
	CsQuestPreRequire* pReturn = csnew CsQuestPreRequire;
	pReturn->Init( m_eType, m_nTypeID, m_nTypeCount );
	return pReturn;
}
