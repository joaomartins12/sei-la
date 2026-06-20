
#include "stdafx.h"
#include "_DlgTab_Setting.h"


//===========================================================================================================
//
//		선행 조건
//
//===========================================================================================================

void _DlgTab_Setting::__SetPreRequire( cQuestObj* pQuestObj )
{
	if( pQuestObj == NULL )
	{
		// 타입
		m_combo_PreReq.ShowWindow( SW_HIDE );

		// ID
		m_edit_PreReqID.ShowWindow( SW_HIDE );

		// Cnt
		m_edit_PreReqCnt.ShowWindow( SW_HIDE );

		// 테이블 로드 버튼
		m_btn_PreReqTable.ShowWindow( SW_HIDE );

		// 테이블 설명
		m_edit_PreReqDiscript.ShowWindow( SW_HIDE );

		// 리스트
		m_list_PreReqList.ShowWindow( SW_HIDE );

		// 리스트 버튼
		m_btn_PreReqAdd.ShowWindow( SW_HIDE );
		m_btn_PreReqDel.ShowWindow( SW_HIDE );
	}
	else
	{
		CsQuest* pFTQuest = pQuestObj->GetFT();

		// 타입
		m_combo_PreReq.ShowWindow( SW_SHOW );
		m_combo_PreReq.SetCurSel( 0 );

		// 기본값 리셋
		m_PreRequire.Reset();

		// 기존에 저장된 정보 리딩
		__ReadPreRequireList( pFTQuest );

		OnCbnSelchangeComboPrereq();
	}
}

void _DlgTab_Setting::__ReadPreRequireList( CsQuest* pFTQuest )
{
	int nMax = 0;

	m_list_PreReqList.ResetContent();
	CsQuestPreRequireGroup* pGroup = pFTQuest->GetPreRequireGroup();
	CsQuestPreRequire::LIST* pList = pGroup->GetList();
	CsQuestPreRequire::LIST_IT it = pList->begin();
	CsQuestPreRequire::LIST_IT itEnd = pList->end();
	TCHAR sz[ 128 ];
	for( ; it!=itEnd; ++it )
	{
		CsQuestPreRequire* pElement = (*it);
		switch( pElement->GetType() )
		{
		case CsQuestPreRequire::QUEST:
			{
				if( nsCsFileTable::g_pQuestMng->IsQuest( pElement->GetTypeID() ) )
				{
					_stprintf_s( sz, 128, L"퀘스트 [ %s (%d) ]", nsCsFileTable::g_pQuestMng->GetQuest( pElement->GetTypeID() )->m_szTitleText,
						pElement->GetTypeID() );
				}
				else
				{
					_stprintf_s( sz, 128, L"해당 퀘스트가 존재하지 않습니다" );
				}
			}				
			break;
		case CsQuestPreRequire::T_LEVEL:
			{
				_stprintf_s( sz, 128, L"테이머레벨 [ %d ]", pElement->GetTypeID() );
			}			
			break;
		case CsQuestPreRequire::ITEM:
			{
				if( nsCsFileTable::g_pItemMng->IsItem( pElement->GetTypeID() ) )
				{						
					_stprintf_s( sz, 128, L"아이템 [ %s (%d) ] x %d", nsCsFileTable::g_pItemMng->GetItem( pElement->GetTypeID() )->GetInfo()->s_szName,
						pElement->GetTypeID(), pElement->GetTypeCount() );
				}
				else
				{
					_stprintf_s( sz, 128, L"해당 아이템이 존재하지 않습니다" );
				}
			}				
			break;
		case CsQuestPreRequire::D_LEVEL:
			{
				_stprintf_s( sz, 128, L"디지몬레벨 [ %d ]", pElement->GetTypeID() );
			}
			break;
		case CsQuestPreRequire::D_TYPE:
			{
				if( nsCsFileTable::g_pDigimonMng->IsDigimon( pElement->GetTypeID() ) )
				{					
					_stprintf_s( sz, 128, L"디지몬타입 [ %s (%d) ]", nsCsFileTable::g_pDigimonMng->GetDigimon( pElement->GetTypeID() )->GetInfo()->s_szName,
						pElement->GetTypeID() );
				}
				else
				{
					_stprintf_s( sz, 128, L"해당 디지몬이 존재하지 않습니다" );
				}
			}
			break;
		case CsQuestPreRequire::REGION:
			{
				if( nsCsMapTable::g_pMapRegionMng->IsRegionGroup( pElement->GetTypeID() / 1000 ) == true )
				{
					if( nsCsMapTable::g_pMapRegionMng->GetRegionGroup( pElement->GetTypeID() / 1000 )->GetRegionCount() > (int)( pElement->GetTypeID() % 1000 ) )
					{						
						_stprintf_s( sz, 128, L"지역[%s:%s]", nsCsMapTable::g_pMapListMng->GetList( pElement->GetTypeID() / 1000 )->GetInfo()->s_szMapDiscript.c_str(),
							nsCsMapTable::g_pMapRegionMng->GetRegionGroup( pElement->GetTypeID() / 1000 )->Get( pElement->GetTypeID() % 1000 )->GetInfo()->s_szDiscript );
					}
					else
					{
						_stprintf_s( sz, 128, L"해당 지역이 존재하지 않습니다" );
					}					
				}
				else
				{
					_stprintf_s( sz, 128, L"해당 맵이 존재하지 않습니다" );
				}
			}
			break;
		case CsQuestPreRequire::D_EVO:
			{
				_stprintf_s( sz, 128, L"진화 슬롯 [ %d ]", pElement->GetTypeID() );
			}
			break;
		case CsQuestPreRequire::D_EVO_DISABLE:
			{
				_stprintf_s( sz, 128, L"오픈 금지 진화 슬롯 [ %d ]", pElement->GetTypeID() );
			}
			break;
		case CsQuestPreRequire::D_BASE_TABLE_IDX:
			{
				_stprintf_s( sz, 128, L"디지몬계열 [ %d ]", pElement->GetTypeID() );
			}
			break;
		case CsQuestPreRequire::TAMER_TABLE_IDX:
			{
				_stprintf_s( sz, 128, L"테이머 테이블ID [ %d ]", pElement->GetTypeID() );
			}
			break;
		case CsQuestPreRequire::QUEST_ID_CANNOT_COMPLETED:
			{
				if( nsCsFileTable::g_pQuestMng->IsQuest( pElement->GetTypeID() ) )
				{
					_stprintf_s( sz, 128, L"특정 퀘스트 완료 시 해당 진행 불가 [ %s (%d) ]", nsCsFileTable::g_pQuestMng->GetQuest( pElement->GetTypeID() )->m_szTitleText,
						pElement->GetTypeID() );
				}
				else
				{
					_stprintf_s( sz, 128, L"해당 퀘스트가 존재하지 않습니다" );
				}
			}
			break;
		case CsQuestPreRequire::QUEST_ID_CANNOT_PROGRESS:
			{
				if( nsCsFileTable::g_pQuestMng->IsQuest( pElement->GetTypeID() ) )
				{
					_stprintf_s( sz, 128, L"특정 퀘스트 진행 시 해당 진행 불가 [ %s (%d) ]", nsCsFileTable::g_pQuestMng->GetQuest( pElement->GetTypeID() )->m_szTitleText,
						pElement->GetTypeID() );
				}
				else
				{
					_stprintf_s( sz, 128, L"해당 퀘스트가 존재하지 않습니다" );
				}
			}
			break;
		default:
			assert_cs( false );
		}

		if( nMax < (int)_tcslen( sz ) )
			nMax = (int)_tcslen( sz );

		m_list_PreReqList.InsertString( m_list_PreReqList.GetCount(), sz );
	}

	m_list_PreReqList.SetHorizontalExtent( nMax*9 );
}

void _DlgTab_Setting::OnCbnSelchangeComboPrereq()
{
	m_PreRequire.SetType( (CsQuestPreRequire::eTYPE)m_combo_PreReq.GetCurSel() );

	nsCsMfcFunc::Int2Control( &m_edit_PreReqID, m_PreRequire.GetTypeID() );
	nsCsMfcFunc::Int2Control( &m_edit_PreReqCnt, m_PreRequire.GetTypeCount() );

	switch( m_combo_PreReq.GetCurSel() )
	{
	case CsQuestPreRequire::QUEST:
	case CsQuestPreRequire::QUEST_ID_CANNOT_COMPLETED:		// 완료된 퀘스트가 있을시 진행 불가
	case CsQuestPreRequire::QUEST_ID_CANNOT_PROGRESS:			// 진행 중인 퀘스트가 있을시 진행 불가
		{
			// ID
			m_edit_PreReqID.ShowWindow( SW_SHOW );
			m_edit_PreReqID.SetReadOnly( TRUE );			

			// 테이블 로드 버튼
			m_btn_PreReqTable.ShowWindow( SW_SHOW );

			// 테이블 설명
			m_edit_PreReqDiscript.ShowWindow( SW_SHOW );

			// 카운트
			m_edit_PreReqCnt.ShowWindow( SW_HIDE );		
		}
		break;
	case CsQuestPreRequire::ITEM:
		{
			// ID
			m_edit_PreReqID.ShowWindow( SW_SHOW );
			m_edit_PreReqID.SetReadOnly( FALSE );

			// 테이블 로드 버튼
			m_btn_PreReqTable.ShowWindow( SW_SHOW );

			// 테이블 설명
			m_edit_PreReqDiscript.ShowWindow( SW_SHOW );

			// 카운트
			m_edit_PreReqCnt.ShowWindow( SW_SHOW );
			m_edit_PreReqCnt.SetReadOnly( FALSE );			
		}
		break;
	case CsQuestPreRequire::T_LEVEL:
		{
			// ID
			m_edit_PreReqID.ShowWindow( SW_SHOW );
			m_edit_PreReqID.SetReadOnly( FALSE );

			// 테이블 로드 버튼
			m_btn_PreReqTable.ShowWindow( SW_HIDE );

			// 테이블 설명
			m_edit_PreReqDiscript.ShowWindow( SW_HIDE );

			// 카운트
			m_edit_PreReqCnt.ShowWindow( SW_HIDE );
		}
		break;
	case CsQuestPreRequire::D_LEVEL:
		{
			// ID
			m_edit_PreReqID.ShowWindow( SW_SHOW );
			m_edit_PreReqID.SetReadOnly( FALSE );

			// 테이블 로드 버튼
			m_btn_PreReqTable.ShowWindow( SW_HIDE );

			// 테이블 설명
			m_edit_PreReqDiscript.ShowWindow( SW_HIDE );

			// 카운트
			m_edit_PreReqCnt.ShowWindow( SW_HIDE );
		}
		break;
	case CsQuestPreRequire::D_TYPE:
		{
			// ID
			m_edit_PreReqID.ShowWindow( SW_SHOW );
			m_edit_PreReqID.SetReadOnly( TRUE );

			// 테이블 로드 버튼
			m_btn_PreReqTable.ShowWindow( SW_SHOW );

			// 테이블 설명
			m_edit_PreReqDiscript.ShowWindow( SW_SHOW );

			// 카운트
			m_edit_PreReqCnt.ShowWindow( SW_HIDE );
		}
		break;
	case CsQuestPreRequire::REGION:
		{
			// ID
			m_edit_PreReqID.ShowWindow( SW_SHOW );
			m_edit_PreReqID.SetReadOnly( TRUE );

			// 테이블 로드 버튼
			m_btn_PreReqTable.ShowWindow( SW_SHOW );

			// 테이블 설명
			m_edit_PreReqDiscript.ShowWindow( SW_SHOW );

			// 카운트
			m_edit_PreReqCnt.ShowWindow( SW_HIDE );			
		}
		break;
	case CsQuestPreRequire::D_EVO:
	case CsQuestPreRequire::D_EVO_DISABLE:
	case CsQuestPreRequire::D_BASE_TABLE_IDX: // 디지몬의 기본체 ID 조건
	case CsQuestPreRequire::TAMER_TABLE_IDX:		// 테이머 테이블 ID 조건
		{
			// ID
			m_edit_PreReqID.ShowWindow( SW_SHOW );
			m_edit_PreReqID.SetReadOnly( FALSE );

			// 테이블 로드 버튼
			m_btn_PreReqTable.ShowWindow( SW_HIDE );

			// 테이블 설명
			m_edit_PreReqDiscript.ShowWindow( SW_HIDE );

			// 카운트
			m_edit_PreReqCnt.ShowWindow( SW_HIDE );
		}
		break;
	default:
		assert_cs( false );
	}

	// 리스트
	m_list_PreReqList.ShowWindow( SW_SHOW );

	// 리스트 버튼
	m_btn_PreReqAdd.ShowWindow( SW_SHOW );
	m_btn_PreReqDel.ShowWindow( SW_SHOW );
}


void _DlgTab_Setting::OnBnClickedButtonPrereqTable()
{
	switch( m_combo_PreReq.GetCurSel() )
	{
	case CsQuestPreRequire::QUEST:
	case CsQuestPreRequire::QUEST_ID_CANNOT_COMPLETED:
	case CsQuestPreRequire::QUEST_ID_CANNOT_PROGRESS:
		{
			DWORD dwReturn;
			_DlgModal_List dlg( _DlgModal_List::LT_QUEST, &dwReturn );
			if( dlg.DoModal() == IDOK )
			{
				nsCsMfcFunc::Int2Control( &m_edit_PreReqID, dwReturn );
			}
		}		
		break;
	case CsQuestPreRequire::ITEM:
		{
			DWORD dwReturn;
			_DlgModal_List dlg( _DlgModal_List::LT_ITEM, &dwReturn );
			if( dlg.DoModal() == IDOK )
			{
				nsCsMfcFunc::Int2Control( &m_edit_PreReqID, dwReturn );
			}
		}
		break;
	case CsQuestPreRequire::D_TYPE:
		{
			DWORD dwReturn;
			_DlgModal_List dlg( _DlgModal_List::LT_DIGIMON, &dwReturn );
			if( dlg.DoModal() == IDOK )
			{
				nsCsMfcFunc::Int2Control( &m_edit_PreReqID, dwReturn );
			}
		}
		break;
	case CsQuestPreRequire::REGION:
		{
			DWORD dwReturn;
			_DlgModal_List dlg( _DlgModal_List::LT_REGION, &dwReturn );
			if( dlg.DoModal() == IDOK )
			{
				nsCsMfcFunc::Int2Control( &m_edit_PreReqID, dwReturn );
			}
		}
		break;
	default:
		assert_cs( false );
	}
}

void _DlgTab_Setting::OnEnChangeEditPrereqId()
{
	m_PreRequire.SetTypeID( nsCsMfcFunc::Control2Int( &m_edit_PreReqID ) );

	switch( m_combo_PreReq.GetCurSel() )
	{
	case CsQuestPreRequire::QUEST:
	case CsQuestPreRequire::QUEST_ID_CANNOT_COMPLETED:		// 완료된 퀘스트가 있을시 진행 불가
	case CsQuestPreRequire::QUEST_ID_CANNOT_PROGRESS:			// 진행 중인 퀘스트가 있을시 진행 불가
		{
			// 테이블 설명
			TCHAR sz[ 128 ];
			if( nsCsFileTable::g_pQuestMng->IsQuest( m_PreRequire.GetTypeID() ) == false )
			{
				m_edit_PreReqDiscript.SetWindowText( L"존재하지 않는 ID 입니다." );
			}
			else
			{
				CsQuest* pFT = nsCsFileTable::g_pQuestMng->GetQuest( m_PreRequire.GetTypeID() );
				_stprintf_s( sz, 128, L"[ %s ]", pFT->m_szTitleText );
				m_edit_PreReqDiscript.SetWindowText( sz );				
			}
		}
		break;
	case CsQuestPreRequire::ITEM:
		{
			// 카운트가 0이라면 카운트 강제 1로 적용
			if( nsCsMfcFunc::Control2Int( &m_edit_PreReqCnt ) == 0 )
			{
				nsCsMfcFunc::Int2Control( &m_edit_PreReqCnt, 1 );
			}

			// 테이블 설명
			TCHAR sz[ 128 ];
			if( nsCsFileTable::g_pItemMng->IsItem( m_PreRequire.GetTypeID() ) == false )
			{
				m_edit_PreReqDiscript.SetWindowText( L"존재하지 않는 ID 입니다." );
			}
			else
			{
				CsItem* pFTItem = nsCsFileTable::g_pItemMng->GetItem( m_PreRequire.GetTypeID() );
				_stprintf_s( sz, 128, L"[ %s ]", pFTItem->GetInfo()->s_szName );
				m_edit_PreReqDiscript.SetWindowText( sz );				
			}
		}
		break;
	case CsQuestPreRequire::D_TYPE:
		{
			// 테이블 설명
			TCHAR sz[ 128 ];
			if( nsCsFileTable::g_pDigimonMng->IsDigimon( m_PreRequire.GetTypeID() ) == false )
			{
				m_edit_PreReqDiscript.SetWindowText( L"존재하지 않는 ID 입니다." );
			}
			else
			{
				CsDigimon* pFT = nsCsFileTable::g_pDigimonMng->GetDigimon( m_PreRequire.GetTypeID() );
				_stprintf_s( sz, 128, L"[ %s ]", pFT->GetInfo()->s_szName );
				m_edit_PreReqDiscript.SetWindowText( sz );				
			}
		}
		break;
	case CsQuestPreRequire::REGION:
		{
			// 테이블 설명
			TCHAR sz[ 128 ];
			if( nsCsMapTable::g_pMapRegionMng->IsRegionGroup( m_PreRequire.GetTypeID() / 1000 ) == false )
			{
				m_edit_PreReqDiscript.SetWindowText( L"존재하지 않는 ID 입니다." );
			}
			else
			{
				if( nsCsMapTable::g_pMapRegionMng->GetRegionGroup( m_PreRequire.GetTypeID() / 1000 )->GetRegionCount() <= (int)( m_PreRequire.GetTypeID() % 1000 ) )
				{
					m_edit_PreReqDiscript.SetWindowText( L"존재하지 않는 인덱스 입니다." );
				}
				else
				{					
					_stprintf_s( sz, 128, L"[%s:%s]", nsCsMapTable::g_pMapListMng->GetList( m_PreRequire.GetTypeID() / 1000 )->GetInfo()->s_szMapDiscript.c_str(),
						nsCsMapTable::g_pMapRegionMng->GetRegionGroup( m_PreRequire.GetTypeID() / 1000 )->Get( m_PreRequire.GetTypeID() % 1000 )->GetInfo()->s_szDiscript);
					m_edit_PreReqDiscript.SetWindowText( sz );				
				}
			}
		}
		break;
	case CsQuestPreRequire::T_LEVEL:
	case CsQuestPreRequire::D_LEVEL:
	case CsQuestPreRequire::D_EVO:
	case CsQuestPreRequire::D_EVO_DISABLE:
	case CsQuestPreRequire::D_BASE_TABLE_IDX: // 디지몬의 기본체 ID 조건
	case CsQuestPreRequire::TAMER_TABLE_IDX:		// 테이머 테이블 ID 조건
		break;	
	default:
		assert_cs( false );
	}
}



void _DlgTab_Setting::OnEnChangeEditPrereqCnt()
{
	m_PreRequire.SetTypeCount( nsCsMfcFunc::Control2Int( &m_edit_PreReqCnt ) );	
}



void _DlgTab_Setting::OnBnClickedButtonPrereqAdd()
{
	if( m_PreRequire.GetTypeID() == 0 )
	{
		CsMessageBox( MB_OK, L"항목이 올바르게 설정되지 않았습니다." );
		return;
	}

	CsQuest* pFTQuest = g_pDiscript->_GetSelQuest()->GetFT();
	pFTQuest->AddPreRequire( m_PreRequire.GetType(), m_PreRequire.GetTypeID(), m_PreRequire.GetTypeCount() );
	__ReadPreRequireList( pFTQuest );

	switch( m_PreRequire.GetType() )
	{
	case CsQuestPreRequire::QUEST:
		{
			cQuestObj* pParent = g_pViewer->_GetQuestMng()->FindNode( m_PreRequire.GetTypeID() );
			cQuestObj* pChild = g_pViewer->_GetQuestMng()->FindNode( pFTQuest );

			pChild->AddParent( pParent );
			pParent->AddChild( pChild );
			pChild->ResetMiddleTail();
			pParent->ResetMiddleTail();
		}
		break;
	}

	g_pDiscript->_GetSelQuest()->ResetControl();
}

void _DlgTab_Setting::OnBnClickedButtonPrereqDel()
{
	if( m_list_PreReqList.GetCurSel() < 0 )
	{
		CsMessageBox( MB_OK, L"왼쪽의 리스트에서 항목을 선택해 주셔야만 삭제 가능합니다." );
		return;
	}

	CsQuest* pFTQuest = g_pDiscript->_GetSelQuest()->GetFT();
	CsQuestPreRequire* pSel = pFTQuest->GetPreRequireGroup()->Get( m_list_PreReqList.GetCurSel() );
	
	switch( pSel->GetType() )
	{
	case CsQuestPreRequire::QUEST:
		{
			cQuestObj* pParent = g_pViewer->_GetQuestMng()->FindNode( pSel->GetTypeID() );
			cQuestObj* pChild = g_pViewer->_GetQuestMng()->FindNode( pFTQuest );

			pChild->DelParent( pParent );
			pParent->DelChild( pChild );
			pChild->ResetMiddleTail();
			pParent->ResetMiddleTail();
		}
		break;
	}

	
	pFTQuest->DeletePreRequire( m_list_PreReqList.GetCurSel() );
	__ReadPreRequireList( pFTQuest );

	g_pDiscript->_GetSelQuest()->ResetControl();
}




