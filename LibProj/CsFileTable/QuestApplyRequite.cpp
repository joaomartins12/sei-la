
#include "stdafx.h"
#include "QuestApplyRequite.h"

void CsQuestApplyRequite::Init(eTYPE type, UINT nTypeID, int nTypeCount )
{
	m_eType = type;
	m_nTypeID = nTypeID;
	m_nTypeCount = nTypeCount;
}

CsQuestApplyRequite* CsQuestApplyRequite::Clone()
{
	CsQuestApplyRequite* pReturn = csnew CsQuestApplyRequite;
	pReturn->Init( m_eType, m_nTypeID, m_nTypeCount );
	return pReturn;
}
