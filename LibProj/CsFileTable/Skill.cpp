
#include "stdafx.h"
#include "Skill.h"


void CsSkill::Delete()
{

}

void CsSkill::Init( sINFO* pSrcInfo )
{
	memcpy( &m_Info, pSrcInfo, sizeof( sINFO ) );
}


void CsTamerSkill::TSkill_Init( sTSkill_INFO* pInfo )
{
	memcpy( &m_TSkill_Info, pInfo, sizeof(sTSkill_INFO) );
}

void CsAreaCheck::AreaCheck_Init( sArea_Check* pInfo )
{
	memcpy( &m_Area_Info, pInfo, sizeof(sArea_Check) );
}
