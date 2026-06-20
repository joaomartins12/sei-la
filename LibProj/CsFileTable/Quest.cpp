
#include "stdafx.h"
#include "Quest.h"

CsQuest::CsQuest()
#ifdef 	QUEST_RESET_180712
:m_bResetQuest(false)
#endif
{	
	m_dwUniqID = 0;
	m_bActiveQuest = true;
}

void CsQuest::Delete()
{
	m_dwUniqID = 0;
	m_ApplyRequiteGroup.Delete();
	m_PreRequireGroup.Delete();
	m_CompRequireGroup.Delete();
	m_RequiteGroup.Delete();
}

void CsQuest::Init()
{
	assert_cs( m_dwUniqID == 0 );
	m_dwManageID = 0;
	m_nQuestTargetValue1 = 0;

	InitText();

	m_eQuestType = QT_SUB;
	m_eStartTarget_Type = ST_NPC;
	m_dwStartTarget_ID = 0;

	m_eQuestTargetType = QTG_SELF;
	
	m_nLevel = 1;
	m_ptToolPos = CsPoint( -1, -1 );
	
	m_ApplyRequiteGroup.Init();
	m_PreRequireGroup.Init();
	m_CompRequireGroup.Init();
	m_RequiteGroup.Init();
#ifdef 	QUEST_RESET_180712
	m_bResetQuest = false;
#endif
	memset( m_nEvent, 0, sizeof(int)*EVENT_MAX_COUNT );
}

bool CsQuest::IsQuestType( eQUEST_TYPE const& type ) const
{
	return (m_eQuestType == type) ? true : false;
}

//========================================================================================================
//
//	Text
//
//========================================================================================================

void CsQuest::InitText()
{
	memset( m_szTitleTab, 0, sizeof( TCHAR )*QUEST_TITLE_LEN );
	memset( m_szTitleText, 0, sizeof( TCHAR )*QUEST_TITLE_LEN );
	memset( m_szBodyText, 0, sizeof( TCHAR )*QUEST_BODY_LEN );
	memset( m_szSimpleText, 0, sizeof( TCHAR )*QUEST_SIMPLE_LEN );
	memset( m_szHelperText, 0, sizeof( TCHAR )*QUEST_HELPER_LEN );
	memset( m_szProcess, 0, sizeof( TCHAR )*QUEST_PROCESS_LEN );
	memset( m_szCompleate, 0, sizeof( TCHAR )*QUEST_COMPLEATE_LEN );
	memset( m_szExpert, 0, sizeof( TCHAR )*QUEST_PROCESS_LEN );
}

//========================================================================================================
//
//	StartTarget
//
//========================================================================================================


//========================================================================================================
//
//	ApplyRequite
//
//========================================================================================================
void CsQuest::AddApplyRequite( CsQuestApplyRequite::eTYPE type, UINT nTypeID, int nTypeCount )
{
	m_ApplyRequiteGroup.Add( type, nTypeID, nTypeCount );
}

void CsQuest::DeleteApplyRequite( int nIndex )
{
	m_ApplyRequiteGroup.Delete( nIndex );
}

//========================================================================================================
//
//	PreRequire
//
//========================================================================================================
void CsQuest::AddPreRequire( CsQuestPreRequire::eTYPE type, UINT nTypeID, int nTypeCount )
{
	m_PreRequireGroup.Add( type, nTypeID, nTypeCount );
}

void CsQuest::DeletePreRequire( int nIndex )
{
	m_PreRequireGroup.Delete( nIndex );
}

//========================================================================================================
//
//	CompRequire
//
//========================================================================================================
void CsQuest::AddCompRequire( CsQuestCompRequire::eTYPE type, UINT nTypeID, int nTypeCount, UINT nTargetID, int nSubValue1, int nSubValue2 )
{
	m_CompRequireGroup.Add( type, nTypeID, nTypeCount, nTargetID, nSubValue1, nSubValue2 );
}

void CsQuest::DeleteCompRequire( int nIndex )
{
	m_CompRequireGroup.Delete( nIndex );
}

//========================================================================================================
//
//	Requite
//
//========================================================================================================
void CsQuest::AddRequite( CsQuestRequite::eMETHOD method, CsQuestRequite::eTYPE type, UINT nTypeID, int nTypeCount )
{
	m_RequiteGroup.Add( method, type, nTypeID, nTypeCount );
}

void CsQuest::DeleteRequite( int nIndex )
{
	m_RequiteGroup.Delete( nIndex );
}

//========================================================================================================
//
//	clone
//
//========================================================================================================
CsQuest* CsQuest::Clone()
{
	CsQuest* pReturn = csnew CsQuest;
	pReturn->Init();

	// UniqID
	pReturn->SetUniqID( m_dwUniqID );

	// Model ID
	pReturn->SetModelID_Recv( m_dwModelID_Recv );
	pReturn->SetModelID_Comp( m_dwModelID_Comp );

	// Level
	pReturn->SetLevel( m_nLevel );

	// ToolPos
	pReturn->SetToolPos( m_ptToolPos );

	// ManageID
	pReturn->SetManageID( m_dwManageID );

	// Active
	pReturn->SetActive( m_bActiveQuest );

	// Immediate
	pReturn->SetImmeiate( m_bImmediate );

#ifdef 	QUEST_RESET_180712
	// Reset Quest
	pReturn->SetResetQuest( m_bResetQuest );
#endif

	// Type
	pReturn->SetQuestType( m_eQuestType );

	// Target
	pReturn->SetQuestTarget( m_eQuestTargetType );
	pReturn->SetQuestTargetValue1( m_nQuestTargetValue1 );

	// Text clone
	_tcscpy_s( pReturn->m_szTitleTab, QUEST_TITLE_LEN, m_szTitleTab );
	_tcscpy_s( pReturn->m_szTitleText, QUEST_TITLE_LEN, m_szTitleText );
	_tcscpy_s( pReturn->m_szBodyText, QUEST_BODY_LEN, m_szBodyText );
	_tcscpy_s( pReturn->m_szSimpleText, QUEST_SIMPLE_LEN, m_szSimpleText );
	_tcscpy_s( pReturn->m_szHelperText, QUEST_HELPER_LEN, m_szHelperText );
	_tcscpy_s( pReturn->m_szProcess, QUEST_PROCESS_LEN, m_szProcess );
	_tcscpy_s( pReturn->m_szCompleate, QUEST_COMPLEATE_LEN, m_szCompleate );
	_tcscpy_s( pReturn->m_szExpert, QUEST_PROCESS_LEN, m_szExpert );

	// StartTarget
	pReturn->SetStartTarget_Type( m_eStartTarget_Type );
	pReturn->SetStartTarget_ID( m_dwStartTarget_ID );

	// ApplyRequite
	pReturn->GetApplyRequiteGroup()->Clone( &m_ApplyRequiteGroup );

	// PreRequire
	pReturn->GetPreRequireGroup()->Clone( &m_PreRequireGroup );

	// CompRequire
	pReturn->GetCompRequireGroup()->Clone( &m_CompRequireGroup );

	// Requite
	pReturn->GetRequiteGroup()->Clone( &m_RequiteGroup );

	// Event
	memcpy( pReturn->m_nEvent, m_nEvent, sizeof( int )*EVENT_MAX_COUNT );

	return pReturn;
}

void CsQuest::CopyData( CsQuest* pSrcData, bool bIncludeTypeID /*= false*/ )
{
	assert_cs( pSrcData != NULL );

	// TypeID
	if( bIncludeTypeID == true )
	{
		SetUniqID( pSrcData->GetUniqID() );		
	}

	// ManageID
	SetManageID( pSrcData->GetManageID() );

	// Active
	SetActive( pSrcData->IsActive() );

	// Immediate
	SetImmeiate( pSrcData->IsImmediate() );

#ifdef QUEST_RESET_180712
	// Reset Quest
	SetResetQuest( pSrcData->GetIsResetQuest() );
#endif
	// Type
	SetQuestType( pSrcData->GetQuestType() );

	// Target
	SetQuestTarget( pSrcData->GetQuestTarget() );
	SetQuestTargetValue1( pSrcData->GetQuestTargetValue1() );

	// Text clone
	_tcscpy_s( m_szTitleTab, QUEST_TITLE_LEN, pSrcData->m_szTitleTab );
	_tcscpy_s( m_szTitleText, QUEST_TITLE_LEN, pSrcData->m_szTitleText );
	_tcscpy_s( m_szBodyText, QUEST_BODY_LEN, pSrcData->m_szBodyText );
	_tcscpy_s( m_szSimpleText, QUEST_SIMPLE_LEN, pSrcData->m_szSimpleText );
	_tcscpy_s( m_szHelperText, QUEST_HELPER_LEN, pSrcData->m_szHelperText );
	_tcscpy_s( m_szProcess, QUEST_PROCESS_LEN, pSrcData->m_szProcess );
	_tcscpy_s( m_szCompleate, QUEST_COMPLEATE_LEN, pSrcData->m_szCompleate );
	_tcscpy_s( m_szExpert, QUEST_PROCESS_LEN, pSrcData->m_szExpert );

	// StartTarget
	m_eStartTarget_Type = pSrcData->GetStartTarget_Type();
	m_dwStartTarget_ID = pSrcData->GetStartTarget_ID();
	
	// ApplyRequite
	m_ApplyRequiteGroup.Delete();
	m_ApplyRequiteGroup.Clone( pSrcData->GetApplyRequiteGroup() );

	// PreRequire
	m_PreRequireGroup.Delete();
	m_PreRequireGroup.Clone( pSrcData->GetPreRequireGroup() );

	// CompRequire
	m_CompRequireGroup.Delete();
	m_CompRequireGroup.Clone( pSrcData->GetCompRequireGroup() );

	// Requite
	m_RequiteGroup.Delete();
	m_RequiteGroup.Clone( pSrcData->GetRequiteGroup() );

	// Event
	memcpy( m_nEvent, pSrcData->m_nEvent, sizeof( int )*EVENT_MAX_COUNT );
}


