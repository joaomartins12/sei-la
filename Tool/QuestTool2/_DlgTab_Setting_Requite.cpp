

#include "stdafx.h"
#include "_DlgTab_Setting.h"


//===========================================================================================================
//
//		보상
//
//===========================================================================================================

void _DlgTab_Setting::__SetRequite( cQuestObj* pQuestObj )
{
	if( pQuestObj == NULL )
	{
		// 메서드
		m_combo_Reqt_Method.ShowWindow( SW_HIDE );

		// 타입
		m_combo_Reqt_Type.ShowWindow( SW_HIDE );

		// ID
		m_edit_Reqt_ID.ShowWindow( SW_HIDE );

		// Cnt
		m_edit_Reqt_Cnt.ShowWindow( SW_HIDE );

		// 테이블 로드 버튼
		m_btn_Reqt_Table.ShowWindow( SW_HIDE );

		// 테이블 설명
		m_edit_Reqt_Discript.ShowWindow( SW_HIDE );

		// 리스트
		m_list_Reqt_List.ShowWindow( SW_HIDE );

		// 리스트 버튼
		m_btn_Reqt_Add.ShowWindow( SW_HIDE );
		m_btn_Reqt_Del.ShowWindow( SW_HIDE );
	}
	else
	{
		CsQuest* pFTQuest = pQuestObj->GetFT();

		// 메서드
		m_combo_Reqt_Method.ShowWindow( SW_SHOW );
		m_combo_Reqt_Method.SetCurSel( 0 );

		// 타입
		m_combo_Reqt_Type.ShowWindow( SW_SHOW );
		m_combo_Reqt_Type.SetCurSel( 0 );

		// 기본값 리셋
		m_Requite.Reset();

		// 기존에 저장된 정보 리딩
		__ReadRequiteList( pFTQuest );

		OnCbnSelchangeComboReqtType();
	}
}

void _DlgTab_Setting::__ReadRequiteList( CsQuest* pFTQuest )
{
	int nMax = 0;

	m_list_Reqt_List.ResetContent();
	CsQuestRequiteGroup* pGroup = pFTQuest->GetRequiteGroup();
	CsQuestRequite::LIST* pList = pGroup->GetList();
	CsQuestRequite::LIST_IT it = pList->begin();
	CsQuestRequite::LIST_IT itEnd = pList->end();
	TCHAR sz[ 128 ];
	TCHAR szMethod[ 128 ];
	for( ; it!=itEnd; ++it )
	{
		CsQuestRequite* pElement = (*it);
		switch( pElement->GetMethodID() )
		{
		case CsQuestRequite::RM_GIVE:
			_stprintf_s( szMethod, 128, L"지급" );
			break;
		case CsQuestRequite::RM_SEL1:
			_stprintf_s( szMethod, 128, L"선택1" );
			break;
		default:
			assert_cs( false );
		}

		switch( pElement->GetType() )
		{
		case CsQuestRequite::MONEY:
			{
				_stprintf_s( sz, 128, L"%s : 돈 [ %d ]", szMethod, pElement->GetTypeID() );
			}
			break;
		case CsQuestRequite::EXP:
			{
				_stprintf_s( sz, 128, L"%s : 경험치 [ %d ]", szMethod, pElement->GetTypeID() );
			}
			break;
		case CsQuestRequite::ITEM:
			if( nsCsFileTable::g_pItemMng->IsItem( pElement->GetTypeID() ) )
			{						
				_stprintf_s( sz, 128, L"%s : 아이템 [ %s (%d) ] x %d", szMethod, nsCsFileTable::g_pItemMng->GetItem( pElement->GetTypeID() )->GetInfo()->s_szName,
					pElement->GetTypeID(), pElement->GetTypeCount() );
			}
			else
			{
				_stprintf_s( sz, 128, L"해당 아이템이 존재하지 않습니다" );
			}
			break;
		case  CsQuestRequite::EVOSLOT:
			{
				_stprintf_s( sz, 128, L"%s : D_Base_ID[%d],진화슬롯번호[ %d ]", szMethod, pElement->GetTypeID(), pElement->GetTypeCount() );
			}
			break;
		default:
			assert_cs( false );
		}

		if( nMax < (int)_tcslen( sz ) )
			nMax = (int)_tcslen( sz );

		m_list_Reqt_List.InsertString( m_list_Reqt_List.GetCount(), sz );
	}

	m_list_Reqt_List.SetHorizontalExtent( nMax*9 );
}


void _DlgTab_Setting::OnCbnSelchangeComboReqtType()
{
	m_Requite.SetType( (CsQuestRequite::eTYPE)m_combo_Reqt_Type.GetCurSel() );

	nsCsMfcFunc::Int2Control( &m_edit_Reqt_ID, m_Requite.GetTypeID() );
	nsCsMfcFunc::Int2Control( &m_edit_Reqt_Cnt, m_Requite.GetTypeCount() );
	// ID
	m_edit_Reqt_ID.ShowWindow( SW_HIDE );
	m_edit_Reqt_ID.SetReadOnly( TRUE );
	// 테이블 로드 버튼
	m_btn_Reqt_Table.ShowWindow( SW_HIDE );
	// 테이블 설명
	m_edit_Reqt_Discript.ShowWindow( SW_HIDE );
	// 카운트
	m_edit_Reqt_Cnt.ShowWindow( SW_HIDE );
	m_edit_Reqt_Cnt.SetReadOnly( FALSE );

	switch( m_combo_Reqt_Type.GetCurSel() )
	{
	case CsQuestRequite::MONEY:
		{
			// ID
			m_edit_Reqt_ID.ShowWindow( SW_SHOW );
			m_edit_Reqt_ID.SetReadOnly( FALSE );
		}
		break;
	case CsQuestRequite::EXP:
		{
			// ID
			m_edit_Reqt_ID.ShowWindow( SW_SHOW );
			m_edit_Reqt_ID.SetReadOnly( FALSE );
		}
		break;
	case CsQuestRequite::ITEM:
		{
			m_edit_Reqt_ID.SetReadOnly( FALSE );			// ID
			m_edit_Reqt_ID.ShowWindow( SW_SHOW );
			m_btn_Reqt_Table.ShowWindow( SW_SHOW );// 테이블 로드 버튼
			m_edit_Reqt_Discript.ShowWindow( SW_SHOW );// 테이블 설명
			m_edit_Reqt_Cnt.ShowWindow( SW_SHOW );// 카운트
		}
		break;
	case CsQuestRequite::EVOSLOT:
		// ID
		m_edit_Reqt_ID.ShowWindow( SW_SHOW );
		m_edit_Reqt_ID.SetReadOnly( FALSE );
		m_btn_Reqt_Table.ShowWindow( SW_SHOW );// 테이블 로드 버튼
		m_edit_Reqt_Discript.ShowWindow( SW_SHOW );// 테이블 설명
		m_edit_Reqt_Cnt.ShowWindow( SW_SHOW );// 카운트
		break;
	default:
		assert_cs( false );
	}

	// 리스트
	m_list_Reqt_List.ShowWindow( SW_SHOW );

	// 리스트 버튼
	m_btn_Reqt_Add.ShowWindow( SW_SHOW );
	m_btn_Reqt_Del.ShowWindow( SW_SHOW );
}

void _DlgTab_Setting::OnEnChangeEditReqtId()
{
	m_Requite.SetTypeID( nsCsMfcFunc::Control2Int( &m_edit_Reqt_ID ) );

	switch( m_combo_Reqt_Type.GetCurSel() )
	{
	case CsQuestRequite::MONEY:	
	case CsQuestRequite::EXP:
		break;
	case CsQuestRequite::ITEM:
		{
			// 카운트가 0이라면 카운트 강제 1로 적용
			if( nsCsMfcFunc::Control2Int( &m_edit_Reqt_Cnt ) == 0 )
			{
				nsCsMfcFunc::Int2Control( &m_edit_Reqt_Cnt, 1 );
			}

			// 테이블 설명
			TCHAR sz[ 128 ];
			if( nsCsFileTable::g_pItemMng->IsItem( m_Requite.GetTypeID() ) == false )
			{
				m_edit_Reqt_Discript.SetWindowText( L"존재하지 않는 ID 입니다." );
			}
			else
			{
				CsItem* pFTItem = nsCsFileTable::g_pItemMng->GetItem( m_Requite.GetTypeID() );
				_stprintf_s( sz, 128, L"[ %s ]", pFTItem->GetInfo()->s_szName );
				m_edit_Reqt_Discript.SetWindowText( sz );				
			}
		}
		break;
	case CsQuestRequite::EVOSLOT:
		{
			// 카운트가 0이라면 카운트 강제 1로 적용
			if( nsCsMfcFunc::Control2Int( &m_edit_Reqt_Cnt ) == 0 )
			{
				nsCsMfcFunc::Int2Control( &m_edit_Reqt_Cnt, 1 );
			}

			// 테이블 설명
			TCHAR sz[ 128 ];
			if( !nsCsFileTable::g_pEvolMng->IsEvolveInfo( m_Requite.GetTypeID() ) )
			{
				m_edit_Reqt_Discript.SetWindowText( L"디지몬의 진화 정보가 없습니다." );
			}
			else
			{
				CDigimonEvolveInfo* pDEvo = nsCsFileTable::g_pEvolMng->GetEvolveInfo( m_Requite.GetTypeID() );
				if( NULL == pDEvo )
					m_edit_Reqt_Discript.SetWindowText( L"디지몬의 진화 정보가 없습니다." );
				else
				{
					CDigimonEvolveObj* pEvoObj = pDEvo->GetEvolveObjByEvoSlot( m_Requite.GetTypeCount() );
					if( NULL == pEvoObj )
						m_edit_Reqt_Discript.SetWindowText( L"디지몬의 진화 정보가 없습니다." );
					else
					{
						CsDigimon* pEvoDigimon = nsCsFileTable::g_pDigimonMng->GetDigimon( pEvoObj->m_dwID );
						if( pEvoDigimon )
						{
							_stprintf_s( sz, 128, L"[ %s ]", pEvoDigimon->GetInfo()->s_szName );
							m_edit_Reqt_Discript.SetWindowText( sz );								
						}
						else
						{
							m_edit_Reqt_Discript.SetWindowText( L"진화 대상의 디지몬이 존재 하지 않습니다." );
						}
					}				
				}
			}

		}break;
	default:
		assert_cs( false );
	}
}

void _DlgTab_Setting::OnEnChangeEditReqtCnt()
{
	m_Requite.SetTypeCount( nsCsMfcFunc::Control2Int( &m_edit_Reqt_Cnt ) );	
	switch( m_combo_Reqt_Type.GetCurSel() )
	{
	case CsQuestRequite::EVOSLOT:
		{
			// 테이블 설명
			TCHAR sz[ 128 ];
			if( !nsCsFileTable::g_pEvolMng->IsEvolveInfo( m_Requite.GetTypeID() ) )
			{
				m_edit_Reqt_Discript.SetWindowText( L"디지몬의 진화 정보가 없습니다." );
			}
			else
			{
				CDigimonEvolveInfo* pDEvo = nsCsFileTable::g_pEvolMng->GetEvolveInfo( m_Requite.GetTypeID() );
				if( NULL == pDEvo )
					m_edit_Reqt_Discript.SetWindowText( L"디지몬의 진화 정보가 없습니다." );
				else
				{
					CDigimonEvolveObj* pEvoObj = pDEvo->GetEvolveObjByEvoSlot( m_Requite.GetTypeCount() );
					if( NULL == pEvoObj )
						m_edit_Reqt_Discript.SetWindowText( L"디지몬의 진화 정보가 없습니다." );
					else
					{
						CsDigimon* pEvoDigimon = nsCsFileTable::g_pDigimonMng->GetDigimon( pEvoObj->m_dwID );
						if( pEvoDigimon )
						{
							_stprintf_s( sz, 128, L"[ %s ]", pEvoDigimon->GetInfo()->s_szName );
							m_edit_Reqt_Discript.SetWindowText( sz );								
						}
						else
						{
							m_edit_Reqt_Discript.SetWindowText( L"진화 대상의 디지몬이 존재 하지 않습니다." );
						}
					}				
				}
			}
		}
		break;
	}
}

void _DlgTab_Setting::OnBnClickedButtonReqtTable2()
{
	switch( m_combo_Reqt_Type.GetCurSel() )
	{
	case CsQuestRequite::ITEM:
		{
			DWORD dwReturn;
			_DlgModal_List dlg( _DlgModal_List::LT_ITEM, &dwReturn );
			if( dlg.DoModal() == IDOK )
			{
				nsCsMfcFunc::Int2Control( &m_edit_Reqt_ID, dwReturn );
			}
		}
		break;
	case CsQuestRequite::EVOSLOT:
		{
			DWORD dwReturn;
			_DlgModal_List dlg( _DlgModal_List::LT_DIGIMON, &dwReturn );
			if( dlg.DoModal() == IDOK )
			{
				nsCsMfcFunc::Int2Control( &m_edit_Reqt_ID, dwReturn );
			}
		}break;
	default:
		assert_cs( false );
	}
}

void _DlgTab_Setting::OnBnClickedButtonReqtAdd()
{
	if( m_Requite.GetTypeID() == 0 )
	{
		CsMessageBox( MB_OK, L"항목이 올바르게 설정되지 않았습니다." );
		return;
	}

	CsQuest* pFTQuest = g_pDiscript->_GetSelQuest()->GetFT();
	pFTQuest->AddRequite( (CsQuestRequite::eMETHOD)m_combo_Reqt_Method.GetCurSel(), m_Requite.GetType(), m_Requite.GetTypeID(), m_Requite.GetTypeCount() );
	__ReadRequiteList( pFTQuest );

	g_pDiscript->_GetSelQuest()->ResetControl();
}

void _DlgTab_Setting::OnBnClickedButtonReqtDel()
{
	if( m_list_Reqt_List.GetCurSel() < 0 )
	{
		CsMessageBox( MB_OK, L"왼쪽의 리스트에서 항목을 선택해 주셔야만 삭제 가능합니다." );
		return;
	}

	CsQuest* pFTQuest = g_pDiscript->_GetSelQuest()->GetFT();
	pFTQuest->DeleteRequite( m_list_Reqt_List.GetCurSel() );
	__ReadRequiteList( pFTQuest );

	g_pDiscript->_GetSelQuest()->ResetControl();
}
