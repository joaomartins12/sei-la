
#include "stdafx.h"
#include "Npc.h"


CsNpc::CsNpc()
{
	m_pType = NULL;	
}

void CsNpc::Delete()
{
	_DeleteType();

	for( int i=0; i<nsCsNpcTable::NE_MAX_CNT; ++i )
	{
		if( m_pExtraArray[ i ] == NULL )
			continue;

		DeleteExtra( m_pExtraArray[ i ] );		
	}
}

void CsNpc::Init()
{
	assert_cs( m_pType == NULL );

	m_Info.s_dwNpcID = m_Info.s_dwMapID = m_Info.s_dwModelID = 0;
	m_Info.s_szPenName[ 0 ] = NULL;
	m_Info.s_szName[ 0 ] = NULL;
	m_Info.s_szTalk[ 0 ] = NULL;

	for( int i=0; i<nsCsNpcTable::NE_MAX_CNT; ++i )
		m_pExtraArray[ i ] = NULL;
}

void CsNpc::_DeleteType()
{
	if( m_pType )
	{
		m_pType->Delete();
		switch( m_pType->s_eNpcType )
		{
		case nsCsNpcTable::NT_NONE:		
			delete ( (sNPC_TYPE_NONE*)m_pType );			
			break;
		case nsCsNpcTable::NT_TRADE:
		case nsCsNpcTable::NT_GUILD:
		case nsCsNpcTable::NT_DIGICORE:
		case nsCsNpcTable::NT_EVENT_STORE:
		case nsCsNpcTable::NT_PROPERTY_STORE:
			delete ( (sNPC_TYPE_TRADE*)m_pType );
			break;
		case nsCsNpcTable::NT_DIGITAMA_TRADE:
			delete ( (sNPC_TYPE_DIGITAMA_TRADE*)m_pType );
			break;
		case nsCsNpcTable::NT_PORTAL:
			delete ( (sNPC_TYPE_PORTAL*)m_pType );
			break;
		case nsCsNpcTable::NT_MAKE_TACTICS:
			delete ( (sNPC_TYPE_MAKE_TACTICS*)m_pType );
			break;
		case nsCsNpcTable::NT_ELEMENT_ITEM:
			delete ( (sNPC_TYPE_ELEMENT_ITEM*)m_pType );
			break;
		case nsCsNpcTable::NT_WAREHOUSE:
			delete ( (sNPC_TYPE_WAREHOUSE*)m_pType );
			break;
		case nsCsNpcTable::NT_TACTICSHOUSE:
			delete ( (sNPC_TYPE_TACTICSHOUSE*)m_pType );
			break;
		case nsCsNpcTable::NT_CAPSULE_MACHINE:
			delete ( (sNPC_TYPE_CAPSULE_MACHINE*)m_pType );
			break;
		case nsCsNpcTable::NT_SKILL:
			delete ( (sNPC_TYPE_SKILL*)m_pType );
			break;
		case nsCsNpcTable::NT_DATS_PORTAL:
			delete ( (sNPC_TYPE_DATS_PORTAL*)m_pType );
			break;
		case nsCsNpcTable::NT_MASTERS_MATCHING:
			delete ( (sNPC_TYPE_MASTER_MATCHING*)m_pType );
			break;
		case nsCsNpcTable::NT_GOTCHA_MACHINE:
			delete ( (sNPC_TYPE_GOTCHAMACHINE*)m_pType );
			break;
		case nsCsNpcTable::NT_SPIRIT_EVO:
			delete ( (sNPC_TYPE_SPIRIT_EVO*)m_pType );
			break;
		case nsCsNpcTable::NT_SPECIAL_EVENT:
			delete ( (sNPC_TYPE_SPECIAL_EVENT*)m_pType );
			break;
		case nsCsNpcTable::NT_ITEM_PRODUCTION_NPC:
			delete ( (sNPC_TYPE_ITEM_PRODUCTION*)m_pType );
			break;
		case nsCsNpcTable::NT_BATTLE_REGISTRANT_NPC:
			delete ( (sNPC_TYPE_BATTLEREGISTRANT*)m_pType );
			break;
		case nsCsNpcTable::NT_INFINITEWAR_MANAGER_NPC:
			delete ( (sNPC_TYPE_INFINITEWAR_MANAGER*)m_pType );
			break;
		case nsCsNpcTable::NT_INFINITEWAR_NOTICEBOARD_NPC:
			delete ( (sNPC_TYPE_INFINITEWAR_NOTICEBOARD*)m_pType );
			break;
		case nsCsNpcTable::NT_EXTRA_EVOLUTION_NPC:
			delete ( (sNPC_TYPE_EXTRA_EVOLUTION*)m_pType );
			break;
		default:
			assert_cs( false );
		}
		m_pType = NULL;
	}
}

void CsNpc::SetType()
{
	_DeleteType();

	switch( m_Info.s_eType )
	{
	case nsCsNpcTable::NT_NONE:
		m_pType = csnew sNPC_TYPE_NONE;
		break;
	case nsCsNpcTable::NT_TRADE:
	case nsCsNpcTable::NT_GUILD:
	case nsCsNpcTable::NT_DIGICORE:
	case nsCsNpcTable::NT_EVENT_STORE:
		m_pType = csnew sNPC_TYPE_TRADE( m_Info.s_eType );
		break;
	case nsCsNpcTable::NT_DIGITAMA_TRADE:
		m_pType = csnew sNPC_TYPE_DIGITAMA_TRADE;
		break;
	case nsCsNpcTable::NT_PORTAL:
		m_pType = csnew sNPC_TYPE_PORTAL;
		break;
	case nsCsNpcTable::NT_MAKE_TACTICS:
		m_pType = csnew sNPC_TYPE_MAKE_TACTICS;
		break;
	case nsCsNpcTable::NT_ELEMENT_ITEM:
		m_pType = csnew sNPC_TYPE_ELEMENT_ITEM;
		break;
	case nsCsNpcTable::NT_WAREHOUSE:
		m_pType = csnew sNPC_TYPE_WAREHOUSE;
		break;
	case nsCsNpcTable::NT_TACTICSHOUSE:
		m_pType = csnew sNPC_TYPE_TACTICSHOUSE;
		break;
	case nsCsNpcTable::NT_CAPSULE_MACHINE:
		m_pType = csnew sNPC_TYPE_CAPSULE_MACHINE;
		break;
	case nsCsNpcTable::NT_SKILL:
		m_pType = csnew sNPC_TYPE_SKILL;
		break;
	case nsCsNpcTable::NT_DATS_PORTAL:
		m_pType = csnew sNPC_TYPE_DATS_PORTAL;
		break;
	case nsCsNpcTable::NT_MASTERS_MATCHING:
		m_pType = csnew sNPC_TYPE_MASTER_MATCHING( m_Info.s_eType );
		break;
	case nsCsNpcTable::NT_PROPERTY_STORE:
		m_pType = csnew sNPC_TYPE_EXCHANGE( m_Info.s_eType );
		break;
	case nsCsNpcTable::NT_GOTCHA_MACHINE:
		m_pType = csnew sNPC_TYPE_GOTCHAMACHINE;
		break;
	case nsCsNpcTable::NT_MYSTERY_MACHINE:
		m_pType = csnew sNPC_TYPE_GOTCHAMACHINE;
		break;
	case nsCsNpcTable::NT_SPIRIT_EVO:
		m_pType = csnew sNPC_TYPE_SPIRIT_EVO;
		break;
	case nsCsNpcTable::NT_SPECIAL_EVENT:
		m_pType = csnew sNPC_TYPE_SPECIAL_EVENT;
		break;
	case nsCsNpcTable::NT_ITEM_PRODUCTION_NPC:
		m_pType = csnew sNPC_TYPE_ITEM_PRODUCTION;
		break;
	case nsCsNpcTable::NT_BATTLE_REGISTRANT_NPC:
		m_pType = csnew sNPC_TYPE_BATTLEREGISTRANT;
		break;
	case nsCsNpcTable::NT_INFINITEWAR_MANAGER_NPC:
		m_pType = csnew sNPC_TYPE_INFINITEWAR_MANAGER;
		break;
	case nsCsNpcTable::NT_INFINITEWAR_NOTICEBOARD_NPC:
		m_pType = csnew sNPC_TYPE_INFINITEWAR_NOTICEBOARD;
		break;
	case nsCsNpcTable::NT_EXTRA_EVOLUTION_NPC:
		m_pType = csnew sNPC_TYPE_EXTRA_EVOLUTION;
		break;
	default:
		assert_cs( false );
	}
}

void CsNpc::SetType( nsCsNpcTable::eNPC_TYPE eType )
{
	if( ( m_pType != NULL )&&( m_Info.s_eType == eType ) )
		return;

	m_Info.s_eType = eType;
	SetType();
}

void CsNpc::SetMoveType( nsCsNpcTable::eNPC_MOVE eType )
{
	if( ( m_pType != NULL )&&( m_Info.s_eMoveType == eType ) )
		return;

	m_Info.s_eMoveType = eType;
}

void CsNpc::SetType( sNPC_TYPE_BASE* pSrcType )
{
	_DeleteType();
	m_pType = pSrcType->Clone();
}

void CsNpc::DeleteExtra( sNPC_EXTRA_BASE* pExtra )
{
	assert_cs( pExtra != NULL );

	int nExtraType = pExtra->s_eNpcExtra;	
	pExtra->Delete();

	switch( nExtraType )
	{
	case nsCsNpcTable::NE_QUEST:
		delete (sNPC_EXTRA_QUEST*)pExtra;
		break;

	default:
		assert_cs( false );
	}

	m_pExtraArray[ nExtraType ] = NULL;
}

void CsNpc::DeleteExtra( nsCsNpcTable::eNPC_EXTRA ne )
{
	if( IsExtra( ne ) == false )
		return;

	DeleteExtra( m_pExtraArray[ ne ] );
}

bool CsNpc::IsExtra( nsCsNpcTable::eNPC_EXTRA ne )
{
	return m_pExtraArray[ ne ] != NULL;
}

sNPC_EXTRA_BASE* CsNpc::AddExtra( nsCsNpcTable::eNPC_EXTRA ne )
{
	if( IsExtra( ne ) )
		return m_pExtraArray[ ne ];

	sNPC_EXTRA_BASE* pReturn = NULL;
	switch( ne )
	{
	case nsCsNpcTable::NE_QUEST:
		pReturn = csnew sNPC_EXTRA_QUEST;
		pReturn->Init();
		break;

	default:
		assert_cs( false );
	}

	m_pExtraArray[ ne ] = pReturn;
	return pReturn;
}

CsNpc* CsNpc::Clone()
{
	CsNpc* pClone =	csnew CsNpc;
	pClone->Init();

	// 데이터 복사
	memcpy( pClone->GetInfo(), &m_Info, sizeof( sINFO ) );

	// 타입 복사
	assert_cs( m_pType != NULL );
	pClone->SetType( m_pType );

	// Extra Data 는 복사 되지 않는다.	
	return pClone;
}
void CsNpc::CopyData( CsNpc* pSrcNpc )
{
	memcpy( &m_Info, pSrcNpc->GetInfo(), sizeof( sINFO ) );
	_DeleteType();
	m_pType = pSrcNpc->GetType()->Clone();
}