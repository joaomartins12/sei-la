
#include "stdafx.h"
#include "Digimon.h"

CsDigimon::CsDigimon()
{

}

void CsDigimon::Delete()
{

}

#ifdef SDM_TAMER_XGUAGE_20180628
bool CsDigimon::isX_Anti(void)
{
	if(nsCsDigimonTable::ET_X_ROOKIE <= m_Info.s_eEvolutionType && 
		m_Info.s_eEvolutionType <= nsCsDigimonTable::ET_X_JOGRESS)
		return true;

	return false;
}
bool CsDigimon::isX_AntiJogress(void)
{
	if(m_Info.s_eEvolutionType == nsCsDigimonTable::ET_X_JOGRESS)
		return true;

	return false;

}

#endif
void CsDigimon::Init( sINFO* pSrcInfo )
{
	memcpy( &m_Info, pSrcInfo, sizeof( sINFO ) );
}
#ifdef CROSSWARS_SYSTEM
CsCrossDigimon::CsCrossDigimon()
{

}

void CsCrossDigimon::Delete()
{

}

void CsCrossDigimon::Init( sINFO* pSrcInfo )
{
	memcpy( &m_Info, pSrcInfo, sizeof( sINFO ) );
}


void CsCrossScene::Init( sINFO* pSrcInfo )
{
	memcpy( &m_Info, pSrcInfo, sizeof( sINFO ) );
}
#endif

void	CsData_Exchange::Init( CsData_Exchange::sINFO* pSrcInfo )
{
	memcpy( &m_Info, pSrcInfo, sizeof( CsData_Exchange::sINFO ) );
}

CsData_Exchange::sINFO*	CsData_Exchange::GetInfo()
{
	return &m_Info;
}