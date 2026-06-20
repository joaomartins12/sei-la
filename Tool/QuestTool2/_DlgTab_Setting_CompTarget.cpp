

#include "stdafx.h"
#include "_DlgTab_Setting.h"

//===========================================================================================================
//
//		완료 타겟
//
//===========================================================================================================

void _DlgTab_Setting::__SetCompTarget( cQuestObj* pQuestObj )
{
	if( pQuestObj == NULL )
	{
		// 타입
		m_combo_CT.ShowWindow( SW_HIDE );

		// ID
		m_edit_CT_ID.ShowWindow( SW_HIDE );

		// 테이블 로드 버튼
		m_btn_CT_Table.ShowWindow( SW_HIDE );

		// 테이블 설명
		m_edit_CT_Discript.ShowWindow( SW_HIDE );		
	}
	else
	{
		CsQuest* pFTQuest = pQuestObj->GetFT();

		// 타입
		m_combo_CT.ShowWindow( SW_SHOW );
		m_combo_CT.SetCurSel( pFTQuest->GetQuestTarget() );

		OnCbnSelchangeComboCt();
	}
}


void _DlgTab_Setting::OnCbnSelchangeComboCt()
{
	CsQuest* pFTQuest =	g_pDiscript->_GetSelQuest()->GetFT();
	assert_cs( pFTQuest != NULL );
	pFTQuest->SetQuestTarget( (CsQuest::eQUEST_TARGET)m_combo_CT.GetCurSel() );


	// 입력란 변하게 무조건 셋팅
	nsCsMfcFunc::Int2Control( &m_edit_CT_ID, pFTQuest->GetQuestTargetValue1() );
	switch( m_combo_CT.GetCurSel() )
	{
	case CsQuest::QTG_SELF:
		{
			// ID
			m_edit_CT_ID.ShowWindow( SW_HIDE );

			// 테이블 로드 버튼
			m_btn_CT_Table.ShowWindow( SW_HIDE );

			// 테이블 설명
			m_edit_CT_Discript.ShowWindow( SW_HIDE );			
		}
		break;
	case CsQuest::QTG_NPC:
		{
			// ID
			m_edit_CT_ID.ShowWindow( SW_SHOW );

			// 테이블 로브 버튼
			m_btn_CT_Table.ShowWindow( SW_SHOW );

			// 테이블 설명
			m_edit_CT_Discript.ShowWindow( SW_SHOW );
		}
		break;
	case CsQuest::QTG_COMPLETE:
		{
			// ID
			m_edit_CT_ID.ShowWindow( SW_HIDE );

			// 테이블 로드 버튼
			m_btn_CT_Table.ShowWindow( SW_HIDE );

			// 테이블 설명
			m_edit_CT_Discript.ShowWindow( SW_HIDE );
		}
		break;
	default:
		assert_cs( false );
	}
}

void _DlgTab_Setting::OnEnChangeEditCtId()
{
	CsQuest* pFTQuest =	g_pDiscript->_GetSelQuest()->GetFT();
	assert_cs( pFTQuest != NULL );
	pFTQuest->SetQuestTargetValue1( nsCsMfcFunc::Control2Int( &m_edit_CT_ID ) );

	switch( m_combo_CT.GetCurSel() )
	{
	case CsQuest::QTG_NPC:
		{
			// 테이블 설명
			if( nsCsFileTable::g_pNpcMng->IsNpc( pFTQuest->GetQuestTargetValue1() ) == false )
			{				
				m_edit_CT_Discript.SetWindowText( L"존재하지 않는 ID 입니다." );
			}
			else
			{
				CsNpc* pFTNpc = nsCsFileTable::g_pNpcMng->GetNpc( pFTQuest->GetQuestTargetValue1() );
				DWORD dwMapID = pFTNpc->GetInfo()->s_dwMapID;
				CsMapList* pMapList = nsCsMapTable::g_pMapListMng->GetList( dwMapID );

				TCHAR sz[ 1024 ]={0,};
				_stprintf_s( sz, 1024, L"Map[ %d ] - %s, %s", dwMapID, pMapList->GetInfo()->s_szMapDiscript.c_str(), pFTNpc->GetInfo()->s_szName );
				m_edit_CT_Discript.SetWindowText( sz );
			}			
		}
		break;
	case CsQuest::QTG_SELF:
	case CsQuest::QTG_COMPLETE:
		break;
	default:
		assert_cs( false );
	}

	g_pDiscript->_GetSelQuest()->ResetControl();
}

void _DlgTab_Setting::OnBnClickedButtonCtTable()
{
	switch( m_combo_CT.GetCurSel() )
	{
	case CsQuest::QTG_NPC:
		{
			DWORD dwReturn;
			_DlgModal_List dlg( _DlgModal_List::LT_NPC, &dwReturn );
			if( dlg.DoModal() == IDOK )
			{
				nsCsMfcFunc::Int2Control( &m_edit_CT_ID, dwReturn );
			}
		}
		break;
	default:
		assert_cs( false );
	}
}
