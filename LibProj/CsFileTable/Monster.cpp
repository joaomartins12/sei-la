
#include "stdafx.h"
#include "Monster.h"

void CsMonster::Delete()
{

}

void CsMonster::Init( sINFO* pSrcInfo )
{	
	memcpy( &m_Info, pSrcInfo, sizeof( sINFO ) );
}