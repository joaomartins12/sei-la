
#include "stdafx.h"
#include "_DlgTab_Setting.h"

//===========================================================================================================
//
//		완료 조건
//
//===========================================================================================================


void _DlgTab_Setting::__SetCompRequire( cQuestObj* pQuestObj )
{
	if( pQuestObj == NULL )
	{
		// 타입
		m_combo_CompRequ.ShowWindow( SW_HIDE );

		// ID
		m_edit_CompRequ_ID.ShowWindow( SW_HIDE );

		// Cnt
		m_edit_CompRequ_Cnt.ShowWindow( SW_HIDE );

		// 테이블 로드 버튼
		m_btn_CompRequ_Table.ShowWindow( SW_HIDE );

		// 타겟
		m_edit_CompRequTarget_ID.ShowWindow( SW_HIDE );
		m_edit_CompRequTarget_ID.SetReadOnly( TRUE );

		// 타겟 테이블
		m_btn_CompRequTarget_Table.ShowWindow( SW_HIDE );

		// 테이블 설명
		m_edit_CompRequ_Discript.ShowWindow( SW_HIDE );

		// 리스트
		m_list_CompRequ_List.ShowWindow( SW_HIDE );

		// 리스트 버튼
		m_btn_CompRequ_Add.ShowWindow( SW_HIDE );
		m_btn_CompRequ_Del.ShowWindow( SW_HIDE );
	}
	else
	{
		CsQuest* pFTQuest = pQuestObj->GetFT();

		// 타입
		m_combo_CompRequ.ShowWindow( SW_SHOW );
		m_combo_CompRequ.SetCurSel( 0 );

		// 기본값 리셋
		m_CompRequire.Reset();

		// 기존에 저장된 정보 리딩
		__ReadCompRequireList( pFTQuest );

		OnCbnSelchangeComboCompRequ();
	}
}

void _DlgTab_Setting::__ReadCompRequireList( CsQuest* pFTQuest )
{
	int nMax = 0;

	m_list_CompRequ_List.ResetContent();
	CsQuestCompRequireGroup* pGroup = pFTQuest->GetCompRequireGroup();
	CsQuestCompRequire::LIST* pList = pGroup->GetList();
	CsQuestCompRequire::LIST_IT it = pList->begin();
	CsQuestCompRequire::LIST_IT itEnd = pList->end();
	TCHAR sz[ 128 ];
	for( ; it!=itEnd; ++it )
	{
		CsQuestCompRequire* pElement = (*it);
		switch( pElement->GetType() )
		{
		case CsQuestCompRequire::MONSTER:
			{
				if( nsCsFileTable::g_pMonsterMng->IsMonster( pElement->GetTypeID() ) )
				{
					_stprintf_s( sz, 128, L"몬스터 [ %s (%d) ] x %d", nsCsFileTable::g_pMonsterMng->GetMonster( pElement->GetTypeID() )->GetInfo()->s_szName,
						pElement->GetTypeID(), pElement->GetTypeCount() );
				}
				else
				{
					_stprintf_s( sz, 128, L"해당 몬스터가 존재하지 않습니다" );
				}
			}				
			break;		
		case CsQuestCompRequire::ITEM:
			{
				if( nsCsFileTable::g_pItemMng->IsItem( pElement->GetTypeID() ) )
				{						
					_stprintf_s( sz, 128, L"아이템 [ %s (%d) ] x %d, 대상 [%d]", nsCsFileTable::g_pItemMng->GetItem( pElement->GetTypeID() )->GetInfo()->s_szName,
						pElement->GetTypeID(), pElement->GetTypeCount(), pElement->GetTargetID() );
				}
				else
				{
					_stprintf_s( sz, 128, L"해당 아이템이 존재하지 않습니다" );
				}
			}				
			break;
		case CsQuestCompRequire::CLIENT:
			{
				switch( pElement->GetTypeID() )
				{
				case clientCompType::eAccept: //퀘스트 수락 완료
					_stprintf_s( sz, 128, L"클라이언트 [ 수락시 완료 (%d) ]", pElement->GetTypeID() );
					break;
				case clientCompType::eMovePosition:	// 특정 위치로 이동
					_stprintf_s( sz, 128, L"클라이언트 [ X,Y좌표이동시완료 (%d) ] %d : %d", pElement->GetTypeID(), pElement->GetSubValue(), pElement->GetSubValue2() );
					break; 
				case clientCompType::eCamerControl:	// 카메라 줌/회전
					{
						switch( pElement->GetSubValue() )
						{
						case 1:	// 카메라 좌우회전
							_stprintf_s( sz, 128, L"클라이언트 [ 카메라 좌/우회전 (%d) ] 줌(%d)", pElement->GetTypeID(), pElement->GetSubValue2() );
							break;
						case 2: // 카메라 줌
							_stprintf_s( sz, 128, L"클라이언트 [ 카메라 줌/아웃 (%d) ] 좌우(%d)", pElement->GetTypeID(), pElement->GetSubValue2() );
							break;
						}
					}					
					break; 
				case clientCompType::eTargetTrace:	// NPC 추적
					{
						switch( pElement->GetSubValue() )
						{
						case 1:			// NPC 추적
							_stprintf_s( sz, 128, L"클라이언트 [ NPC/몹추적(%d) ] NPC추적(%d) NPC_IDX(%d)", pElement->GetTypeID(), pElement->GetSubValue(), pElement->GetTargetID() );
							break;
						case 2:			// 몹 추적
							_stprintf_s( sz, 128, L"클라이언트 [ NPC/몹추적(%d) ] 몹추적(%d) 몹_IDX(%d)", pElement->GetTypeID(), pElement->GetSubValue(), pElement->GetTargetID() );
							break;
						}						
					}					
					break; 
				case clientCompType::eAttack:	// 공격 타입
					{
						switch( pElement->GetSubValue() )
						{
						case 0:
							_stprintf_s( sz, 128, L"클라이언트 [ 공격(%d) ] 모두(%d) 몹_IDX(%d)", pElement->GetTypeID(), pElement->GetSubValue(), pElement->GetTargetID() );
							break;
						case 1:
							_stprintf_s( sz, 128, L"클라이언트 [ 공격(%d) ] 일반(%d) 몹_IDX(%d)", pElement->GetTypeID(), pElement->GetSubValue(), pElement->GetTargetID() );
							break;
						case 2:
							_stprintf_s( sz, 128, L"클라이언트 [ 공격(%d) ] 스킬(%d) 몹_IDX(%d)", pElement->GetTypeID(), pElement->GetSubValue(), pElement->GetTargetID() );
							break;
						}
					}					
					break; 
				case clientCompType::eScanAndRetrieve:	// 스캔
					switch( pElement->GetSubValue() )
					{
					case 1:// 스캔
						_stprintf_s( sz, 128, L"클라이언트 [ 스캔/회수(%d), 스캔(%d) ] ItemIDX(%d)", pElement->GetTypeID(), pElement->GetSubValue(), pElement->GetTargetID() );
						break;
					case 2:// 회수
						_stprintf_s( sz, 128, L"클라이언트 [ 스캔/회수(%d), 회수(%d) ] ItemIDX(%d)", pElement->GetTypeID(), pElement->GetSubValue(), pElement->GetTargetID() );
						break;
					}
					
					break; 
				case clientCompType::eDigimonHatch:	// 부화
					_stprintf_s( sz, 128, L"클라이언트 [ 부화(%d) ] DigimonIDX(%d)", pElement->GetTypeID(), pElement->GetTargetID() );
					break; 
				case clientCompType::eDigimonEnchant:	// 강화
					_stprintf_s( sz, 128, L"클라이언트 [ 디지몬강화(%d) ] DigimonIDX(%d)", pElement->GetTypeID(), pElement->GetTargetID() );
					break; 
				case clientCompType::eDigimonChange:	// 디지몬 교체
					_stprintf_s( sz, 128, L"클라이언트 [ 디지몬교체(%d) ] 디지몬IDX(%d)", pElement->GetTypeID(), pElement->GetTargetID() );
					break; 
				}				
			}
			break;
		case CsQuestCompRequire::T_LEVEL:
			{
				_stprintf_s( sz, 128, L"테이머레벨 [ %d ]", pElement->GetTypeID() );
			}			
			break;
		case CsQuestCompRequire::D_TYPE:
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
		case CsQuestCompRequire::REGION:
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
		case CsQuestCompRequire::NPCTALK:
			{
				if( nsCsFileTable::g_pNpcMng->IsNpc( pElement->GetTypeID() ) )
				{						
					_stprintf_s( sz, 128, L"NPC [ %s (%d) ]", nsCsFileTable::g_pNpcMng->GetNpc( pElement->GetTypeID() )->GetInfo()->s_szName,
						pElement->GetTypeID() );
				}
				else
				{
					_stprintf_s( sz, 128, L"해당 엔피씨가 존재하지 않습니다" );
				}
			}
			break;
		case CsQuestCompRequire::USE_ITEM:
			{
				if( nsCsFileTable::g_pItemMng->IsItem( pElement->GetTypeID() ) )
				{						
					_stprintf_s( sz, 128, L"아이템사용 [ %s (%d) ] x %d", nsCsFileTable::g_pItemMng->GetItem( pElement->GetTypeID() )->GetInfo()->s_szName,
						pElement->GetTypeID(), pElement->GetTypeCount() );
				}
				else
				{
					_stprintf_s( sz, 128, L"해당 아이템이 존재하지 않습니다" );
				}
			}
			break;
		case CsQuestCompRequire::USE_ITEM_NPC:
			{
				if( nsCsFileTable::g_pItemMng->IsItem( pElement->GetTypeID() ) )
				{
					if( nsCsFileTable::g_pNpcMng->IsNpc( pElement->GetTargetID() ) )
					{
						_stprintf_s( sz, 128, L"Npc아이템사용 [ %s - %s ] x %d", nsCsFileTable::g_pItemMng->GetItem( pElement->GetTypeID() )->GetInfo()->s_szName,
							nsCsFileTable::g_pNpcMng->GetNpc( pElement->GetTargetID() )->GetInfo()->s_szName,
							pElement->GetTypeCount() );
					}
					else
					{
						_stprintf_s( sz, 128, L"해당 엔피씨가 존재하지 않습니다" );
					}					
				}
				else
				{
					_stprintf_s( sz, 128, L"해당 아이템이 존재하지 않습니다" );
				}
			}
			break;
		case CsQuestCompRequire::USE_ITEM_MONSTER:
			{
				if( nsCsFileTable::g_pItemMng->IsItem( pElement->GetTypeID() ) )
				{
					if( nsCsFileTable::g_pMonsterMng->IsMonster( pElement->GetTargetID() ) )
					{
						_stprintf_s( sz, 128, L"몬스터아이템사용 [ %s - %s ] x %d", nsCsFileTable::g_pItemMng->GetItem( pElement->GetTypeID() )->GetInfo()->s_szName,
							nsCsFileTable::g_pMonsterMng->GetMonster( pElement->GetTargetID() )->GetInfo()->s_szName,
							pElement->GetTypeCount() );
					}
					else
					{
						_stprintf_s( sz, 128, L"해당 몬스터가 존재하지 않습니다" );
					}
				}
				else
				{
					_stprintf_s( sz, 128, L"해당 아이템이 존재하지 않습니다" );
				}
			}
			break;
		case CsQuestCompRequire::USE_ITEM_REGION:
			{
				if( nsCsFileTable::g_pItemMng->IsItem( pElement->GetTypeID() ) )
				{
					if( nsCsMapTable::g_pMapRegionMng->IsRegionGroup( pElement->GetTargetID() / 1000 ) == true )
					{
						if( nsCsMapTable::g_pMapRegionMng->GetRegionGroup( pElement->GetTargetID() / 1000 )->GetRegionCount() > (int)( pElement->GetTargetID() % 1000 ) )
						{
							_stprintf_s( sz, 128, L"지역아이템사용 [%s - %s:%s] x %d", nsCsFileTable::g_pItemMng->GetItem( pElement->GetTypeID() )->GetInfo()->s_szName,
								nsCsMapTable::g_pMapListMng->GetList( pElement->GetTargetID() / 1000 )->GetInfo()->s_szMapDiscript.c_str(),
								nsCsMapTable::g_pMapRegionMng->GetRegionGroup( pElement->GetTargetID() / 1000 )->Get( pElement->GetTargetID() % 1000 )->GetInfo()->s_szDiscript,
								pElement->GetTypeCount() );
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
				else
				{
					_stprintf_s( sz, 128, L"해당 아이템이 존재하지 않습니다" );
				}
			}
			break;
		default:
			assert_cs( false );
		}

		if( nMax < (int)_tcslen( sz ) )
			nMax = (int)_tcslen( sz );

		m_list_CompRequ_List.InsertString( m_list_CompRequ_List.GetCount(), sz );
	}

	m_list_CompRequ_List.SetHorizontalExtent( nMax*9 );
}

void _DlgTab_Setting::OnCbnSelchangeComboCompRequ()
{
	m_CompRequire.SetType( (CsQuestCompRequire::eTYPE)m_combo_CompRequ.GetCurSel() );

	nsCsMfcFunc::Int2Control( &m_edit_CompRequ_ID, m_CompRequire.GetTypeID() );
	nsCsMfcFunc::Int2Control( &m_edit_CompRequ_Cnt, m_CompRequire.GetTypeCount() );
	nsCsMfcFunc::Int2Control( &m_edit_CompRequTarget_ID, m_CompRequire.GetTargetID() );

	nsCsMfcFunc::Int2Control( &m_edit_Reqt_SubValue1, m_CompRequire.GetSubValue() );
	nsCsMfcFunc::Int2Control( &m_edit_Reqt_SubValue2, m_CompRequire.GetSubValue2() );

	m_edit_Reqt_SubValue1.ShowWindow( SW_HIDE );
	m_edit_Reqt_SubValue2.ShowWindow( SW_HIDE );

	switch( m_combo_CompRequ.GetCurSel() )
	{
	case CsQuestCompRequire::MONSTER:
		{
			// ID
			m_edit_CompRequ_ID.ShowWindow( SW_SHOW );
			m_edit_CompRequ_ID.SetReadOnly( FALSE );			

			// 테이블 로드 버튼
			m_btn_CompRequ_Table.ShowWindow( SW_SHOW );

			// 테이블 설명
			m_edit_CompRequ_Discript.ShowWindow( SW_SHOW );

			// 카운트
			m_edit_CompRequ_Cnt.ShowWindow( SW_SHOW );
			m_edit_CompRequ_Cnt.SetReadOnly( FALSE );

			// 타겟
			m_edit_CompRequTarget_ID.ShowWindow( SW_HIDE );
			m_edit_CompRequTarget_ID.SetReadOnly( FALSE );

			// 타겟 테이블
			m_btn_CompRequTarget_Table.ShowWindow( SW_HIDE );
		}
		break;	
	case CsQuestCompRequire::ITEM:
		{
			// ID
			m_edit_CompRequ_ID.ShowWindow( SW_SHOW );
			m_edit_CompRequ_ID.SetReadOnly( FALSE );

			// 테이블 로드 버튼
			m_btn_CompRequ_Table.ShowWindow( SW_SHOW );

			// 테이블 설명
			m_edit_CompRequ_Discript.ShowWindow( SW_SHOW );

			// 카운트
			m_edit_CompRequ_Cnt.ShowWindow( SW_SHOW );
			m_edit_CompRequ_Cnt.SetReadOnly( FALSE );

			// 타겟
			m_edit_CompRequTarget_ID.ShowWindow( SW_SHOW );
			m_edit_CompRequTarget_ID.SetReadOnly( FALSE );

			// 타겟 테이블
			m_btn_CompRequTarget_Table.ShowWindow( SW_SHOW );
		}
		break;
	case CsQuestCompRequire::CLIENT:
		{
			// ID
			m_edit_CompRequ_ID.ShowWindow( SW_SHOW );
			m_edit_CompRequ_ID.SetReadOnly( TRUE );

			// 테이블 로드 버튼
			m_btn_CompRequ_Table.ShowWindow( SW_SHOW );

			// 테이블 설명
			m_edit_CompRequ_Discript.ShowWindow( SW_HIDE );

			// 카운트
			m_edit_CompRequ_Cnt.ShowWindow( SW_SHOW );

			// 타겟
			m_edit_CompRequTarget_ID.ShowWindow( SW_SHOW );
			m_edit_CompRequTarget_ID.SetReadOnly( FALSE );

			// 타겟 테이블
			m_btn_CompRequTarget_Table.ShowWindow( SW_HIDE );

			m_edit_Reqt_SubValue1.ShowWindow( SW_SHOW );
			m_edit_Reqt_SubValue2.ShowWindow( SW_SHOW );
		}
		break;
	case CsQuestCompRequire::REGION:
		{
			// ID
			m_edit_CompRequ_ID.ShowWindow( SW_SHOW );
			m_edit_CompRequ_ID.SetReadOnly( FALSE );

			// 테이블 로드 버튼
			m_btn_CompRequ_Table.ShowWindow( SW_SHOW );

			// 테이블 설명
			m_edit_CompRequ_Discript.ShowWindow( SW_SHOW );

			// 카운트
			m_edit_CompRequ_Cnt.ShowWindow( SW_HIDE );

			// 타겟
			m_edit_CompRequTarget_ID.ShowWindow( SW_HIDE );
			m_edit_CompRequTarget_ID.SetReadOnly( FALSE );

			// 타겟 테이블
			m_btn_CompRequTarget_Table.ShowWindow( SW_HIDE );
		}
		break;
	case CsQuestCompRequire::T_LEVEL:
		{
			// ID
			m_edit_CompRequ_ID.ShowWindow( SW_SHOW );
			m_edit_CompRequ_ID.SetReadOnly( FALSE );

			// 테이블 로드 버튼
			m_btn_CompRequ_Table.ShowWindow( SW_HIDE );

			// 테이블 설명
			m_edit_CompRequ_Discript.ShowWindow( SW_HIDE );

			// 타겟
			m_edit_CompRequTarget_ID.ShowWindow( SW_HIDE );
			m_edit_CompRequTarget_ID.SetReadOnly( FALSE );

			// 카운트
			m_edit_CompRequ_Cnt.ShowWindow( SW_HIDE );

			// 타겟 테이블
			m_btn_CompRequTarget_Table.ShowWindow( SW_HIDE );
		}
		break;
	case CsQuestCompRequire::D_TYPE:
		{
			// ID
			m_edit_CompRequ_ID.ShowWindow( SW_SHOW );
			m_edit_CompRequ_ID.SetReadOnly( FALSE );

			// 테이블 로드 버튼
			m_btn_CompRequ_Table.ShowWindow( SW_SHOW );

			// 테이블 설명
			m_edit_CompRequ_Discript.ShowWindow( SW_SHOW );

			// 타겟
			m_edit_CompRequTarget_ID.ShowWindow( SW_HIDE );
			m_edit_CompRequTarget_ID.SetReadOnly( FALSE );

			// 카운트
			m_edit_CompRequ_Cnt.ShowWindow( SW_HIDE );

			m_edit_Reqt_SubValue1.ShowWindow( SW_HIDE );
			m_edit_Reqt_SubValue2.ShowWindow( SW_HIDE );
		}
		break;
	case CsQuestCompRequire::NPCTALK:
		{
			// ID
			m_edit_CompRequ_ID.ShowWindow( SW_SHOW );
			m_edit_CompRequ_ID.SetReadOnly( FALSE );

			// 테이블 로드 버튼
			m_btn_CompRequ_Table.ShowWindow( SW_SHOW );

			// 테이블 설명
			m_edit_CompRequ_Discript.ShowWindow( SW_SHOW );

			// 카운트
			m_edit_CompRequ_Cnt.ShowWindow( SW_HIDE );

			// 타겟
			m_edit_CompRequTarget_ID.ShowWindow( SW_HIDE );
			m_edit_CompRequTarget_ID.SetReadOnly( FALSE );

			// 타겟 테이블
			m_btn_CompRequTarget_Table.ShowWindow( SW_HIDE );
		}
		break;
	case CsQuestCompRequire::USE_ITEM:
		{
			// ID
			m_edit_CompRequ_ID.ShowWindow( SW_SHOW );
			m_edit_CompRequ_ID.SetReadOnly( FALSE );

			// 테이블 로드 버튼
			m_btn_CompRequ_Table.ShowWindow( SW_SHOW );

			// 테이블 설명
			m_edit_CompRequ_Discript.ShowWindow( SW_SHOW );

			// 카운트
			m_edit_CompRequ_Cnt.ShowWindow( SW_SHOW );
			m_edit_CompRequ_Cnt.SetReadOnly( FALSE );

			// 타겟
			m_edit_CompRequTarget_ID.ShowWindow( SW_HIDE );
			m_edit_CompRequTarget_ID.SetReadOnly( FALSE );

			// 타겟 테이블
			m_btn_CompRequTarget_Table.ShowWindow( SW_HIDE );
		}
		break;
	case CsQuestCompRequire::USE_ITEM_NPC:
		{
			// ID
			m_edit_CompRequ_ID.ShowWindow( SW_SHOW );
			m_edit_CompRequ_ID.SetReadOnly( FALSE );

			// 테이블 로드 버튼
			m_btn_CompRequ_Table.ShowWindow( SW_SHOW );

			// 테이블 설명
			m_edit_CompRequ_Discript.ShowWindow( SW_SHOW );

			// 카운트
			m_edit_CompRequ_Cnt.ShowWindow( SW_SHOW );
			m_edit_CompRequ_Cnt.SetReadOnly( FALSE );

			// 타겟
			m_edit_CompRequTarget_ID.ShowWindow( SW_SHOW );
			m_edit_CompRequTarget_ID.SetReadOnly( FALSE );
			

			// 타겟 테이블
			m_btn_CompRequTarget_Table.ShowWindow( SW_SHOW );
		}
		break;
	case CsQuestCompRequire::USE_ITEM_MONSTER:
		{
			// ID
			m_edit_CompRequ_ID.ShowWindow( SW_SHOW );
			m_edit_CompRequ_ID.SetReadOnly( FALSE );

			// 테이블 로드 버튼
			m_btn_CompRequ_Table.ShowWindow( SW_SHOW );

			// 테이블 설명
			m_edit_CompRequ_Discript.ShowWindow( SW_SHOW );

			// 카운트
			m_edit_CompRequ_Cnt.ShowWindow( SW_SHOW );
			m_edit_CompRequ_Cnt.SetReadOnly( FALSE );

			// 타겟
			m_edit_CompRequTarget_ID.ShowWindow( SW_SHOW );
			m_edit_CompRequTarget_ID.SetReadOnly( FALSE );

			// 타겟 테이블
			m_btn_CompRequTarget_Table.ShowWindow( SW_SHOW );
		}
		break;
	case CsQuestCompRequire::USE_ITEM_REGION:
		{
			// ID
			m_edit_CompRequ_ID.ShowWindow( SW_SHOW );
			m_edit_CompRequ_ID.SetReadOnly( FALSE );

			// 테이블 로드 버튼
			m_btn_CompRequ_Table.ShowWindow( SW_SHOW );

			// 테이블 설명
			m_edit_CompRequ_Discript.ShowWindow( SW_SHOW );

			// 카운트
			m_edit_CompRequ_Cnt.ShowWindow( SW_SHOW );
			m_edit_CompRequ_Cnt.SetReadOnly( FALSE );

			// 타겟
			m_edit_CompRequTarget_ID.ShowWindow( SW_SHOW );
			m_edit_CompRequTarget_ID.SetReadOnly( FALSE );

			// 타겟 테이블
			m_btn_CompRequTarget_Table.ShowWindow( SW_SHOW );
		}
		break;
	default:
		assert_cs( false );
	}

	// 리스트
	m_list_CompRequ_List.ShowWindow( SW_SHOW );

	// 리스트 버튼
	m_btn_CompRequ_Add.ShowWindow( SW_SHOW );
	m_btn_CompRequ_Del.ShowWindow( SW_SHOW );
}

void _DlgTab_Setting::OnEnChangeEditCompRequId()
{
	m_CompRequire.SetTypeID( nsCsMfcFunc::Control2Int( &m_edit_CompRequ_ID ) );

	switch( m_combo_CompRequ.GetCurSel() )
	{
	case CsQuestCompRequire::MONSTER:
		{
			// 카운트가 0이라면 카운트 강제 1로 적용
			if( nsCsMfcFunc::Control2Int( &m_edit_CompRequ_Cnt ) == 0 )
			{
				nsCsMfcFunc::Int2Control( &m_edit_CompRequ_Cnt, 1 );
			}

			// 테이블 설명
			TCHAR sz[ 128 ];
			if( nsCsFileTable::g_pMonsterMng->IsMonster( m_CompRequire.GetTypeID() ) == false )
			{
				m_edit_CompRequ_Discript.SetWindowText( L"존재하지 않는 ID 입니다." );
			}
			else
			{
				CsMonster* pFT = nsCsFileTable::g_pMonsterMng->GetMonster( m_CompRequire.GetTypeID() );
				_stprintf_s( sz, 128, L"[ %s ]", pFT->GetInfo()->s_szName );
				m_edit_CompRequ_Discript.SetWindowText( sz );
			}
		}
		break;	
	case CsQuestCompRequire::ITEM:
		{
			// 카운트가 0이라면 카운트 강제 1로 적용
			if( nsCsMfcFunc::Control2Int( &m_edit_CompRequ_Cnt ) == 0 )
			{
				nsCsMfcFunc::Int2Control( &m_edit_CompRequ_Cnt, 1 );
			}

			// 테이블 설명
			TCHAR sz[ 128 ];
			if( nsCsFileTable::g_pItemMng->IsItem( m_CompRequire.GetTypeID() ) == false )
			{
				m_edit_CompRequ_Discript.SetWindowText( L"존재하지 않는 ID 입니다." );
			}
			else
			{
				CsItem* pFT = nsCsFileTable::g_pItemMng->GetItem( m_CompRequire.GetTypeID() );
				_stprintf_s( sz, 128, L"[ %s ]", pFT->GetInfo()->s_szName );
				m_edit_CompRequ_Discript.SetWindowText( sz );
			}
		}
		break;	
	case CsQuestCompRequire::REGION:
		{
			// 테이블 설명
			TCHAR sz[ 128 ];
			if( nsCsMapTable::g_pMapRegionMng->IsRegionGroup( m_CompRequire.GetTypeID() / 1000 ) == false )
			{
				m_edit_CompRequ_Discript.SetWindowText( L"존재하지 않는 ID 입니다." );
			}
			else
			{
				if( nsCsMapTable::g_pMapRegionMng->GetRegionGroup( m_CompRequire.GetTypeID() / 1000 )->GetRegionCount() <= (int)( m_CompRequire.GetTypeID() % 1000 ) )
				{
					m_edit_CompRequ_Discript.SetWindowText( L"존재하지 않는 인덱스 입니다." );
				}
				else
				{					
					_stprintf_s( sz, 128, L"[%s:%s]", nsCsMapTable::g_pMapListMng->GetList( m_CompRequire.GetTypeID() / 1000 )->GetInfo()->s_szMapDiscript.c_str(),
						nsCsMapTable::g_pMapRegionMng->GetRegionGroup( m_CompRequire.GetTypeID() / 1000 )->Get( m_CompRequire.GetTypeID() % 1000 )->GetInfo()->s_szDiscript);
					m_edit_CompRequ_Discript.SetWindowText( sz );
				}
			}
		}
		break;
	case CsQuestCompRequire::NPCTALK:
		{
			// 테이블 설명
			TCHAR sz[ 128 ];
			if( nsCsFileTable::g_pNpcMng->IsNpc( m_CompRequire.GetTypeID() ) == false )
			{
				m_edit_CompRequ_Discript.SetWindowText( L"존재하지 않는 ID 입니다." );
			}
			else
			{
				CsNpc* pFT = nsCsFileTable::g_pNpcMng->GetNpc( m_CompRequire.GetTypeID() );
				_stprintf_s( sz, 128, L"[ %s ]", pFT->GetInfo()->s_szName );
				m_edit_CompRequ_Discript.SetWindowText( sz );
			}
		}
		break;
	case CsQuestCompRequire::USE_ITEM:
		{
			// 카운트가 0이라면 카운트 강제 1로 적용
			if( nsCsMfcFunc::Control2Int( &m_edit_CompRequ_Cnt ) == 0 )
			{
				nsCsMfcFunc::Int2Control( &m_edit_CompRequ_Cnt, 1 );
			}

			// 테이블 설명
			TCHAR sz[ 128 ];
			if( nsCsFileTable::g_pItemMng->IsItem( m_CompRequire.GetTypeID() ) == false )
			{
				m_edit_CompRequ_Discript.SetWindowText( L"존재하지 않는 ID 입니다." );
			}
			else
			{
				CsItem* pFT = nsCsFileTable::g_pItemMng->GetItem( m_CompRequire.GetTypeID() );
				_stprintf_s( sz, 128, L"[ %s ]", pFT->GetInfo()->s_szName );
				m_edit_CompRequ_Discript.SetWindowText( sz );
			}
		}
		break;
	case CsQuestCompRequire::USE_ITEM_NPC:
		{
			// 카운트가 0이라면 카운트 강제 1로 적용
			if( nsCsMfcFunc::Control2Int( &m_edit_CompRequ_Cnt ) == 0 )
			{
				nsCsMfcFunc::Int2Control( &m_edit_CompRequ_Cnt, 1 );
			}

			// 테이블 설명
			TCHAR sz[ 128 ];
			if( nsCsFileTable::g_pItemMng->IsItem( m_CompRequire.GetTypeID() ) == false )
			{
				m_edit_CompRequ_Discript.SetWindowText( L"존재하지 않는 Item ID 입니다." );
			}
			else
			{
				// 테이블 설명
				if( nsCsFileTable::g_pNpcMng->IsNpc( m_CompRequire.GetTargetID() ) == false )
				{
					m_edit_CompRequ_Discript.SetWindowText( L"존재하지 않는 Npc ID 입니다." );
				}
				else
				{
					CsItem* pFTItem = nsCsFileTable::g_pItemMng->GetItem( m_CompRequire.GetTypeID() );
					CsNpc* pFTNpc = nsCsFileTable::g_pNpcMng->GetNpc( m_CompRequire.GetTargetID() );
					_stprintf_s( sz, 128, L"[ %s -> %s ]", pFTItem->GetInfo()->s_szName, pFTNpc->GetInfo()->s_szName );
					m_edit_CompRequ_Discript.SetWindowText( sz );
				}
			}
		}
		break;
	case CsQuestCompRequire::USE_ITEM_MONSTER:
		{
			// 카운트가 0이라면 카운트 강제 1로 적용
			if( nsCsMfcFunc::Control2Int( &m_edit_CompRequ_Cnt ) == 0 )
			{
				nsCsMfcFunc::Int2Control( &m_edit_CompRequ_Cnt, 1 );
			}

			// 테이블 설명
			TCHAR sz[ 128 ];
			if( nsCsFileTable::g_pItemMng->IsItem( m_CompRequire.GetTypeID() ) == false )
			{
				m_edit_CompRequ_Discript.SetWindowText( L"존재하지 않는 Item ID 입니다." );
			}
			else
			{
				// 테이블 설명
				if( nsCsFileTable::g_pMonsterMng->IsMonster( m_CompRequire.GetTargetID() ) == false )
				{
					m_edit_CompRequ_Discript.SetWindowText( L"존재하지 않는 Monster ID 입니다." );
				}
				else
				{
					CsItem* pFTItem = nsCsFileTable::g_pItemMng->GetItem( m_CompRequire.GetTypeID() );
					CsMonster* pFTMonster = nsCsFileTable::g_pMonsterMng->GetMonster( m_CompRequire.GetTargetID() );
					_stprintf_s( sz, 128, L"[ %s -> %s ]", pFTItem->GetInfo()->s_szName, pFTMonster->GetInfo()->s_szName );
					m_edit_CompRequ_Discript.SetWindowText( sz );
				}
			}
		}
		break;
	case CsQuestCompRequire::USE_ITEM_REGION:
		{
			// 카운트가 0이라면 카운트 강제 1로 적용
			if( nsCsMfcFunc::Control2Int( &m_edit_CompRequ_Cnt ) == 0 )
			{
				nsCsMfcFunc::Int2Control( &m_edit_CompRequ_Cnt, 1 );
			}

			// 테이블 설명
			TCHAR sz[ 128 ];
			if( nsCsFileTable::g_pItemMng->IsItem( m_CompRequire.GetTypeID() ) == false )
			{
				m_edit_CompRequ_Discript.SetWindowText( L"존재하지 않는 Item ID 입니다." );
			}
			else
			{
				if( nsCsMapTable::g_pMapRegionMng->IsRegionGroup( m_CompRequire.GetTargetID() / 1000 ) == false )
				{
					m_edit_CompRequ_Discript.SetWindowText( L"존재하지 않는 맵 입니다." );
				}
				else
				{
					if( nsCsMapTable::g_pMapRegionMng->GetRegionGroup( m_CompRequire.GetTargetID() / 1000 )->GetRegionCount() <= (int)( m_CompRequire.GetTargetID() % 1000 ) )
					{
						m_edit_CompRequ_Discript.SetWindowText( L"존재하지 않는 영역 인덱스 입니다." );
					}
					else
					{
						CsItem* pFTItem = nsCsFileTable::g_pItemMng->GetItem( m_CompRequire.GetTypeID() );
						_stprintf_s( sz, 128, L"[ %s -> %s:%s ]", pFTItem->GetInfo()->s_szName,
							nsCsMapTable::g_pMapListMng->GetList( m_CompRequire.GetTargetID() / 1000 )->GetInfo()->s_szMapDiscript.c_str(),
							nsCsMapTable::g_pMapRegionMng->GetRegionGroup( m_CompRequire.GetTargetID() / 1000 )->Get( m_CompRequire.GetTargetID() % 1000 )->GetInfo()->s_szDiscript);
						m_edit_CompRequ_Discript.SetWindowText( sz );
					}
				}
			}
		}
		break;
	case CsQuestCompRequire::CLIENT:
// 		{
// 			switch( m_CompRequire.GetTypeID() )
// 			{
// 			case 1:
// 				{
// 					m_edit_CompRequ_Cnt.ShowWindow( SW_HIDE );		// 카운트
// 					m_edit_CompRequTarget_ID.ShowWindow( SW_HIDE );	// 타겟
// 					m_btn_CompRequTarget_Table.ShowWindow( SW_HIDE );// 타겟 테이블
// 				}
// 				break;
// 			case 2:
// 				{
// 					m_edit_CompRequ_Cnt.ShowWindow( SW_SHOW );		// 카운트
// 					m_edit_CompRequTarget_ID.ShowWindow( SW_SHOW );	// 타겟
// 					m_btn_CompRequTarget_Table.ShowWindow( SW_HIDE );// 타겟 테이블
// 				}
// 				break;
// 			}
// 		}
		break;
	case CsQuestCompRequire::T_LEVEL:
		break;
	case CsQuestCompRequire::D_TYPE:
		{
			// 테이블 설명
			TCHAR sz[ 128 ];
			if( nsCsFileTable::g_pDigimonMng->IsDigimon( m_CompRequire.GetTypeID() ) == false )
			{
				m_edit_CompRequ_Discript.SetWindowText( L"존재하지 않는 ID 입니다." );
			}
			else
			{
				CsDigimon* pFT = nsCsFileTable::g_pDigimonMng->GetDigimon( m_CompRequire.GetTypeID() );
				_stprintf_s( sz, 128, L"[ %s ]", pFT->GetInfo()->s_szName );
				m_edit_CompRequ_Discript.SetWindowText( sz );				
			}
		}
		break;
	default:
		assert_cs( false );
	}
}

void _DlgTab_Setting::OnEnChangeEditCompRequCnt()
{
	m_CompRequire.SetTypeCount( nsCsMfcFunc::Control2Int( &m_edit_CompRequ_Cnt ) );	
}

void _DlgTab_Setting::OnEnChangeEditCompRequTargetId()
{
	m_CompRequire.SetTargetID( nsCsMfcFunc::Control2Int( &m_edit_CompRequTarget_ID ) );
	OnEnChangeEditCompRequId();
}

void _DlgTab_Setting::OnEnChangeEditCompRequSubValue()
{
	m_CompRequire.SetSubValue( nsCsMfcFunc::Control2Int( &m_edit_Reqt_SubValue1 ) );
	OnEnChangeEditCompRequId();
}

void _DlgTab_Setting::OnEnChangeEditCompRequSubValue2()
{
	m_CompRequire.SetSubValue2( nsCsMfcFunc::Control2Int( &m_edit_Reqt_SubValue2 ) );
	OnEnChangeEditCompRequId();
}

void _DlgTab_Setting::OnBnClickedButtonCompRequTable()
{
	switch( m_combo_CompRequ.GetCurSel() )
	{
	case CsQuestCompRequire::MONSTER:
		{
			DWORD dwReturn;
			_DlgModal_List dlg( _DlgModal_List::LT_MONSTER, &dwReturn );
			if( dlg.DoModal() == IDOK )
			{
				nsCsMfcFunc::Int2Control( &m_edit_CompRequ_ID, dwReturn );
			}
		}		
		break;
	case CsQuestCompRequire::CLIENT:
		{
			DWORD dwReturn;
			_DlgModal_List dlg( _DlgModal_List::LT_TUTORIAL_CONDITION, &dwReturn );
			if( dlg.DoModal() == IDOK )
			{
				nsCsMfcFunc::Int2Control( &m_edit_CompRequ_ID, dwReturn );
			}
		}
		break;
	case CsQuestCompRequire::ITEM:
	case CsQuestCompRequire::USE_ITEM:
	case CsQuestCompRequire::USE_ITEM_NPC:
	case CsQuestCompRequire::USE_ITEM_MONSTER:
	case CsQuestCompRequire::USE_ITEM_REGION:
		{
			DWORD dwReturn;
			_DlgModal_List dlg( _DlgModal_List::LT_ITEM, &dwReturn );
			if( dlg.DoModal() == IDOK )
			{
				nsCsMfcFunc::Int2Control( &m_edit_CompRequ_ID, dwReturn );
			}
		}
		break;
	case CsQuestCompRequire::REGION:
		{
			DWORD dwReturn;
			_DlgModal_List dlg( _DlgModal_List::LT_REGION, &dwReturn );
			if( dlg.DoModal() == IDOK )
			{
				nsCsMfcFunc::Int2Control( &m_edit_CompRequ_ID, dwReturn );
			}
		}
		break;
	case CsQuestCompRequire::NPCTALK:
		{
			DWORD dwReturn;
			_DlgModal_List dlg( _DlgModal_List::LT_NPC, &dwReturn );
			if( dlg.DoModal() == IDOK )
			{
				nsCsMfcFunc::Int2Control( &m_edit_CompRequ_ID, dwReturn );
			}
		}
		break;
	case CsQuestCompRequire::D_TYPE:
		{
			DWORD dwReturn;
			_DlgModal_List dlg( _DlgModal_List::LT_DIGIMON, &dwReturn );
			if( dlg.DoModal() == IDOK )
			{
				nsCsMfcFunc::Int2Control( &m_edit_CompRequ_ID, dwReturn );
			}
		}
		break;
	default:
		assert_cs( false );
	}
}

void _DlgTab_Setting::OnBnClickedButtonCompRequTargetTable()
{
	switch( m_combo_CompRequ.GetCurSel() )
	{
	case CsQuestCompRequire::ITEM:
		{
			DWORD dwReturn;
			_DlgModal_List dlg( _DlgModal_List::LT_MONSTER, &dwReturn );
			if( dlg.DoModal() == IDOK )
			{
				nsCsMfcFunc::Int2Control( &m_edit_CompRequTarget_ID, dwReturn );
			}
		}		
		break;

	case CsQuestCompRequire::USE_ITEM_MONSTER:
		{
			DWORD dwReturn;
			_DlgModal_List dlg( _DlgModal_List::LT_MONSTER, &dwReturn );
			if( dlg.DoModal() == IDOK )
			{
				nsCsMfcFunc::Int2Control( &m_edit_CompRequTarget_ID, dwReturn );
			}
		}		
		break;
	case CsQuestCompRequire::USE_ITEM_NPC:
		{
			DWORD dwReturn;
			_DlgModal_List dlg( _DlgModal_List::LT_NPC, &dwReturn );
			if( dlg.DoModal() == IDOK )
			{
				nsCsMfcFunc::Int2Control( &m_edit_CompRequTarget_ID, dwReturn );
			}
		}
		break;
	case CsQuestCompRequire::USE_ITEM_REGION:
		{
			DWORD dwReturn;
			_DlgModal_List dlg( _DlgModal_List::LT_REGION, &dwReturn );
			if( dlg.DoModal() == IDOK )
			{
				nsCsMfcFunc::Int2Control( &m_edit_CompRequTarget_ID, dwReturn );
			}
		}
		break;	
	case CsQuestCompRequire::D_TYPE:
		{
			DWORD dwReturn;
			_DlgModal_List dlg( _DlgModal_List::LT_DIGIMON, &dwReturn );
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


void _DlgTab_Setting::OnBnClickedButtonCompRequAdd()
{
	if( m_CompRequire.GetTypeID() == 0 )
	{
		CsMessageBox( MB_OK, L"항목이 올바르게 설정되지 않았습니다." );
		return;
	}

	CsQuest* pFTQuest = g_pDiscript->_GetSelQuest()->GetFT();
	pFTQuest->AddCompRequire( m_CompRequire.GetType(), 
							  m_CompRequire.GetTypeID(), 
							  m_CompRequire.GetTypeCount(), 
							  m_CompRequire.GetTargetID(),
							  m_CompRequire.GetSubValue(),
							  m_CompRequire.GetSubValue2());
	__ReadCompRequireList( pFTQuest );

	g_pDiscript->_GetSelQuest()->ResetControl();
}

void _DlgTab_Setting::OnBnClickedButtonCompRequDel()
{
	if( m_list_CompRequ_List.GetCurSel() < 0 )
	{
		CsMessageBox( MB_OK, L"왼쪽의 리스트에서 항목을 선택해 주셔야만 삭제 가능합니다." );
		return;
	}

	CsQuest* pFTQuest = g_pDiscript->_GetSelQuest()->GetFT();
	pFTQuest->DeleteCompRequire( m_list_CompRequ_List.GetCurSel() );
	__ReadCompRequireList( pFTQuest );

	g_pDiscript->_GetSelQuest()->ResetControl();
}
