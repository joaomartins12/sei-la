#include "stdafx.h"
#include "_DlgTab_Setting.h"

//===========================================================================================================
//
//		시작 타겟
//
//===========================================================================================================
void _DlgTab_Setting::__SetStartTarget( cQuestObj* pQuestObj )
{
	if( pQuestObj == NULL )
	{
		// 타입
		m_combo_StartTarget.ShowWindow( SW_HIDE );

		// ID
		m_edit_ST_ID.ShowWindow( SW_HIDE );

		// Cnt = 아직 쓰는데 없어서 구현 안함
		m_edit_ST_Cnt.ShowWindow( SW_HIDE );

		// 테이블 로드 버튼
		m_btn_ST_Table.ShowWindow( SW_HIDE );

		// 테이블 설명
		m_edit_ST_Discript.ShowWindow( SW_HIDE );		
	}
	else
	{
		CsQuest* pFTQuest = pQuestObj->GetFT();

		// 타입
		m_combo_StartTarget.ShowWindow( SW_SHOW );
		m_combo_StartTarget.SetCurSel( pFTQuest->GetStartTarget_Type() );		

		OnCbnSelchangeComboStartTarget();
	}
}

// 시작 타겟의 콤보 박스 변경
void _DlgTab_Setting::OnCbnSelchangeComboStartTarget()
{
	bool bChangeType = false;

	CsQuest* pFTQuest =	g_pDiscript->_GetSelQuest()->GetFT();
	assert_cs( pFTQuest != NULL );

	if( pFTQuest->GetStartTarget_Type() != (CsQuest::eST_TYPE)m_combo_StartTarget.GetCurSel() )
	{
		pFTQuest->SetStartTarget_Type( (CsQuest::eST_TYPE)m_combo_StartTarget.GetCurSel() );
		bChangeType = true;

		if( m_combo_StartTarget.GetCurSel() == CsQuest::ST_DIGIVICE )
		{
			pFTQuest->SetStartTarget_ID( 0 );
		}
	}	

	nsCsMfcFunc::Int2Control( &m_edit_ST_ID, pFTQuest->GetStartTarget_ID() );

	switch( m_combo_StartTarget.GetCurSel() )
	{
	case CsQuest::ST_NPC:
		{
			// ID
			m_edit_ST_ID.ShowWindow( SW_SHOW );

			// 테이블 로드 버튼
			m_btn_ST_Table.ShowWindow( SW_SHOW );

			// 테이블 설명
			m_edit_ST_Discript.ShowWindow( SW_SHOW );
		}
		break;
	case CsQuest::ST_DIGIVICE:
		{
			// ID
			m_edit_ST_ID.ShowWindow( SW_HIDE );

			// 테이블 로브 버튼
			m_btn_ST_Table.ShowWindow( SW_HIDE );

			// 테이블 설명
			m_edit_ST_Discript.ShowWindow( SW_HIDE );
		}
		break;
	default:
		assert_cs( false );
	}

	if( bChangeType == true )
	{
		cQuestObj* pObject = g_pDiscript->_GetSelQuest();
		g_pViewer->_GetNodeMng()->ReleaseQuestObj( pObject, false );
		pObject->GetFT()->SetToolPos( CsPoint( -1, -1 ) );
		g_pViewer->_GetNodeMng()->ResistQuestObj( pObject, true );

		pObject->ResetMiddleTail_ParentChild();
		pObject->ResetMiddleTail();
		pObject->ResetControl();		
	}
}


// 테이블 로드 버튼 클릭
void _DlgTab_Setting::OnBnClickedButtonStTable()
{
	switch( m_combo_StartTarget.GetCurSel() )
	{
	case CsQuest::ST_NPC:
		{
			DWORD dwReturn;
			_DlgModal_List dlg( _DlgModal_List::LT_NPC, &dwReturn );
			if( dlg.DoModal() == IDOK )
			{
				nsCsMfcFunc::Int2Control( &m_edit_ST_ID, dwReturn );
			}
		}
		break;
	case CsQuest::ST_DIGIVICE:
		assert_cs( false );
		break;
	default:
		assert_cs( false );
	}
}

// 테이블 텍스트 변경
void _DlgTab_Setting::OnEnChangeEditStId()
{
	CsQuest* pFTQuest =	g_pDiscript->_GetSelQuest()->GetFT();
	assert_cs( pFTQuest != NULL );

	pFTQuest->SetStartTarget_ID( nsCsMfcFunc::Control2Int( &m_edit_ST_ID ) );	

	switch( m_combo_StartTarget.GetCurSel() )
	{
	case CsQuest::ST_NPC:
		{
			// 테이블 설명
			if( nsCsFileTable::g_pNpcMng->IsNpc( pFTQuest->GetStartTarget_ID() ) == false )
			{				
				m_edit_ST_Discript.SetWindowText( L"존재하지 않는 ID 입니다." );
			}
			else
			{
				CsNpc* pFTNpc = nsCsFileTable::g_pNpcMng->GetNpc( pFTQuest->GetStartTarget_ID() );
				DWORD dwMapID = pFTNpc->GetInfo()->s_dwMapID;
				CsMapList* pMapList = nsCsMapTable::g_pMapListMng->GetList( dwMapID );

				TCHAR sz[ 1024 ]={0,};
				_stprintf_s( sz, 1024, L"Map[ %d ] - %s, %s", dwMapID, pMapList->GetInfo()->s_szMapDiscript.c_str(), pFTNpc->GetInfo()->s_szName );
				m_edit_ST_Discript.SetWindowText( sz );
			}			
		}
		break;
	case CsQuest::ST_DIGIVICE:
		break;
	default:
		assert_cs( false );
	}
}