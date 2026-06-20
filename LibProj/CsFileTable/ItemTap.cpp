
#include "stdafx.h"
#include "ItemTap.h"

void CsItemTap::Delete()
{

}

void CsItemTap::Init( sINFO* pSrcInfo )
{
	memcpy( &m_Info, pSrcInfo, sizeof( sINFO ) );	
}

// Rank테이블 load관련 추가	lks007
void CsItemRank::Init( sRANK* pSrcRank )
{
	memcpy( &m_Rank, pSrcRank, sizeof(sRANK) );
}
