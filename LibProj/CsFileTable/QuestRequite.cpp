
#include "stdafx.h"
#include "QuestRequite.h"

void CsQuestRequite::Init( eMETHOD method, eTYPE type, UINT nTypeID, int nTypeCount )
{
	m_eMethod = method;
	m_eType = type;
	m_nTypeID = nTypeID;
	m_nTypeCount = nTypeCount;
}

CsQuestRequite* CsQuestRequite::Clone()
{
	CsQuestRequite* pReturn = csnew CsQuestRequite;
	pReturn->Init( m_eMethod, m_eType, m_nTypeID, m_nTypeCount );
	return pReturn;
}