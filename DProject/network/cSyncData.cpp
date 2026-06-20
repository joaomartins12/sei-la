


#include "StdAfx.h"
#include "cSyncData.h"

//CsMemPool< cSyncDummy >		cSyncUnit::m_mpSyncDummy;
//CsMemPool< cSyncOut >		cSyncUnit::m_mpSyncOut;
//CsMemPool< cSyncDelete >	cSyncUnit::m_mpSyncDelete;
//CsMemPool< cSyncTamer >		cSyncUnit::m_mpSyncTamer;
//CsMemPool< cSyncItem >		cSyncUnit::m_mpSyncItem;
//CsMemPool< cSyncNpc >		cSyncUnit::m_mpSyncNpc;
//CsMemPool< cSyncMonster >	cSyncUnit::m_mpSyncMonster;
//CsMemPool< cSyncDigimon >	cSyncUnit::m_mpSyncDigimon;

void cSyncUnit::DeleteUnit( cSyncUnit* pUnit )
{
	/*switch( pUnit->m_nSyncType )
	{
	case pSync::Out		:	m_mpSyncOut.Push( (cSyncOut*)pUnit );			return;
	case pSync::Delete	:	m_mpSyncDelete.Push( (cSyncDelete*)pUnit );		return;
	case pSync::Dummy	:	m_mpSyncDummy.Push( (cSyncDummy*)pUnit );		return;
	}

	switch( pUnit->m_Type.m_nClass )
	{
	case nClass::Digimon	: m_mpSyncDigimon.Push( (cSyncDigimon*)pUnit );			return;
	case nClass::Tamer		: m_mpSyncTamer.Push( (cSyncTamer*)pUnit );				return;
	case nClass::Item		: m_mpSyncItem.Push( (cSyncItem*)pUnit );				return;
	case nClass::Npc		: m_mpSyncNpc.Push( (cSyncNpc*)pUnit );					return;	
	case nClass::Monster	: m_mpSyncMonster.Push( (cSyncMonster*)pUnit );			return;		
	}*/

	assert_cs( false );
}

cSyncUnit* cSyncUnit::NewUnit( uint nSyncType, uint nClassType )
{
	cSyncUnit* pReturnUnit = NULL;
	assert_cs( false );

	/*switch( nSyncType )
	{
	case pSync::Out		:	pReturnUnit = m_mpSyncOut.Pop();		break;
	case pSync::Delete	:	pReturnUnit = m_mpSyncDelete.Pop();		break;
	case pSync::Dummy	:	pReturnUnit = m_mpSyncDummy.Pop();		break;
	}

	switch( nClassType )
	{
	case nClass::Digimon	: pReturnUnit = m_mpSyncDigimon.Pop();	break;
	case nClass::Tamer		: pReturnUnit = m_mpSyncTamer.Pop();	break;
	case nClass::Item		: pReturnUnit = m_mpSyncItem.Pop();		break;
	case nClass::Npc		: pReturnUnit = m_mpSyncNpc.Pop();		break;
	case nClass::Monster	: pReturnUnit = m_mpSyncMonster.Pop();	break;	
	}

	assert_cs( pReturnUnit != NULL );

	pReturnUnit->m_nSyncType = nSyncType;
	pReturnUnit->m_Type.m_nClass = nClassType;*/
	return pReturnUnit;
}




