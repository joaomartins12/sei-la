
#include "stdafx.h"
#include "_DlgTab_Setting.h"

//===========================================================================================================
//
//		수행 지급
//
//===========================================================================================================
void _DlgTab_Setting::__SetApplyRequite( cQuestObj* pQuestObj )
{
	if( pQuestObj == NULL )
	{
		// 타입
		m_combo_AP.ShowWindow( SW_HIDE );

		// ID
		m_edit_AP_ID.ShowWindow( SW_HIDE );

		// Cnt
		m_edit_AP_Cnt.ShowWindow( SW_HIDE );

		// 테이블 로드 버튼
		m_btn_AP_Table.ShowWindow( SW_HIDE );

		// 테이블 설명
		m_edit_AP_Discript.ShowWindow( SW_HIDE );

		// 리스트
		m_list_AP_List.ShowWindow( SW_HIDE );

		// 리스트 버튼
		m_btn_AP_Add.ShowWindow( SW_HIDE );
		m_btn_AP_Del.ShowWindow( SW_HIDE );
	}
	else
	{
		CsQuest* pFTQuest = pQuestObj->GetFT();

		// 타입
		m_combo_AP.ShowWindow( SW_SHOW );
		m_combo_AP.SetCurSel( 0 );

		// 기본값 리셋
		m_ApplyRequite.Reset();

		// 기존에 저장된 정보 리딩
		__ReadAPList( pFTQuest );

		OnCbnSelchangeComboAp();
	}
}

void _DlgTab_Setting::__ReadAPList( CsQuest* pFTQuest )
{
	int nMax = 0;

	m_list_AP_List.ResetContent();
	CsQuestApplyRequiteGroup* pApplyGroup = pFTQuest->GetApplyRequiteGroup();
	CsQuestApplyRequite::LIST* pList = pApplyGroup->GetList();
	CsQuestApplyRequite::LIST_IT it = pList->begin();
	CsQuestApplyRequite::LIST_IT itEnd = pList->end();
	TCHAR sz[ 128 ];
	for( ; it!=itEnd; ++it )
	{
		CsQuestApplyRequite* pApply = (*it);
		switch( pApply->GetType() )
		{
		case CsQuestApplyRequite::ITEM:
			{
				if( nsCsFileTable::g_pItemMng->IsItem( pApply->GetTypeID() ) )
				{						
					_stprintf_s( sz, 128, L"아이템 [ %s (%d) ] x %d", nsCsFileTable::g_pItemMng->GetItem( pApply->GetTypeID() )->GetInfo()->s_szName,
						pApply->GetTypeID(), pApply->GetTypeCount() );
				}
				else
				{
					_stprintf_s( sz, 128, L"해당 아이템이 존재하지 않습니다" );
				}
			}				
			break;
/*
		case CsQuestApplyRequite::TIME:
			{
				_stprintf_s( sz, 128, L"제한 시간 [ %d ] 초", pApply->GetTypeID() );
			}				
			break;
*/
		default:
			assert_cs( false );
		}

		if( nMax < (int)_tcslen( sz ) )
			nMax = (int)_tcslen( sz );

		m_list_AP_List.InsertString( m_list_AP_List.GetCount(), sz );
	}

	m_list_AP_List.SetHorizontalExtent( nMax*9 );
}

// 타입 콤보 박스 변경
void _DlgTab_Setting::OnCbnSelchangeComboAp()
{
	m_ApplyRequite.SetType( (CsQuestApplyRequite::eTYPE)m_combo_AP.GetCurSel() );

	nsCsMfcFunc::Int2Control( &m_edit_AP_ID, m_ApplyRequite.GetTypeID() );
	nsCsMfcFunc::Int2Control( &m_edit_AP_Cnt, m_ApplyRequite.GetTypeCount() );

	switch( m_combo_AP.GetCurSel() )
	{
	case CsQuestApplyRequite::ITEM:
		{
			// ID
			m_edit_AP_ID.ShowWindow( SW_SHOW );
//			m_edit_AP_ID.SetReadOnly( TRUE );

			// 테이블 로드 버튼
			m_btn_AP_Table.ShowWindow( SW_SHOW );

			// 테이블 설명
			m_edit_AP_Discript.ShowWindow( SW_SHOW );

			// 카운트
			m_edit_AP_Cnt.ShowWindow( SW_SHOW );			

			// 리스트
			m_list_AP_List.ShowWindow( SW_SHOW );

			// 리스트 버튼
			m_btn_AP_Add.ShowWindow( SW_SHOW );
			m_btn_AP_Del.ShowWindow( SW_SHOW );
		}
		break;
/*
	case CsQuestApplyRequite::TIME:
		{
			// ID
			m_edit_AP_ID.ShowWindow( SW_SHOW );
			m_edit_AP_ID.SetReadOnly( FALSE );

			// 테이블 로드 버튼
			m_btn_AP_Table.ShowWindow( SW_HIDE );

			// 테이블 설명
			m_edit_AP_Discript.ShowWindow( SW_HIDE );

			// 카운트
			m_edit_AP_Cnt.ShowWindow( SW_HIDE );			

			// 리스트
			m_list_AP_List.ShowWindow( SW_SHOW );

			// 리스트 버튼
			m_btn_AP_Add.ShowWindow( SW_SHOW );
			m_btn_AP_Del.ShowWindow( SW_SHOW );
			
		}
		break;
*/
	default:
		assert_cs( false );
	}
}


void _DlgTab_Setting::OnBnClickedButtonApTable()
{
	switch( m_combo_AP.GetCurSel() )
	{
	case CsQuestApplyRequite::ITEM:
		{
			DWORD dwReturn;
			_DlgModal_List dlg( _DlgModal_List::LT_ITEM, &dwReturn );
			if( dlg.DoModal() == IDOK )
			{
				nsCsMfcFunc::Int2Control( &m_edit_AP_ID, dwReturn );
			}
		}
		break;
	default:
		assert_cs( false );
	}
}

void _DlgTab_Setting::OnEnChangeEditApId()
{
	m_ApplyRequite.SetTypeID( nsCsMfcFunc::Control2Int( &m_edit_AP_ID ) );

	// 카운트가 0이라면 카운트 강제 1로 적용
	if( nsCsMfcFunc::Control2Int( &m_edit_AP_Cnt ) == 0 )
	{
		nsCsMfcFunc::Int2Control( &m_edit_AP_Cnt, 1 );
	}

	switch( m_combo_AP.GetCurSel() )
	{
	case CsQuestApplyRequite::ITEM:
		{
			// 테이블 설명
			TCHAR sz[ 128 ];
			if( nsCsFileTable::g_pItemMng->IsItem( m_ApplyRequite.GetTypeID() ) == false )
			{
				m_edit_AP_Discript.SetWindowText( L"존재하지 않는 ID 입니다." );
			}
			else
			{
				CsItem* pFTItem = nsCsFileTable::g_pItemMng->GetItem( m_ApplyRequite.GetTypeID() );
				_stprintf_s( sz, 128, L"[ %s ]", pFTItem->GetInfo()->s_szName );
				m_edit_AP_Discript.SetWindowText( sz );				
			}
		}
		break;
/*
	case CsQuestApplyRequite::TIME:		
		break;
*/
	default:
		assert_cs( false );
	}
}

void _DlgTab_Setting::OnEnChangeEditApCnt()
{
	m_ApplyRequite.SetTypeCount( nsCsMfcFunc::Control2Int( &m_edit_AP_Cnt ) );	
}

void _DlgTab_Setting::OnBnClickedButtonApAdd()
{
	if( m_ApplyRequite.GetTypeID() == 0 )
	{
		CsMessageBox( MB_OK, L"항목이 올바르게 설정되지 않았습니다." );
		return;
	}

	CsQuest* pFTQuest = g_pDiscript->_GetSelQuest()->GetFT();
	pFTQuest->AddApplyRequite( m_ApplyRequite.GetType(), m_ApplyRequite.GetTypeID(), m_ApplyRequite.GetTypeCount() );
	__ReadAPList( pFTQuest );

	g_pDiscript->_GetSelQuest()->ResetControl();
}

void _DlgTab_Setting::OnBnClickedButtonApDel()
{
	if( m_list_AP_List.GetCurSel() < 0 )
	{
		CsMessageBox( MB_OK, L"왼쪽의 리스트에서 항목을 선택해 주셔야만 삭제 가능합니다." );
		return;
	}
	CsQuest* pFTQuest = g_pDiscript->_GetSelQuest()->GetFT();
	pFTQuest->DeleteApplyRequite( m_list_AP_List.GetCurSel() );
	__ReadAPList( pFTQuest );

	g_pDiscript->_GetSelQuest()->ResetControl();
}