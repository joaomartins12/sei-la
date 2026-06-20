
#include "stdafx.h"
#include "QuestObj.h"


void cQuestObj::Delete( bool bDeleteMiddle )
{
	cWindow::Delete();

	m_TitleList.Delete();
	m_SpriteBG.Delete();
	SAFE_NIDELETE( m_pStartTarget );
	SAFE_NIDELETE( m_pStartTargetBG );
	SAFE_NIDELETE( m_pEndTarget );
	SAFE_NIDELETE( m_pEndTargetBG );
	m_EtcList.Delete();

	if( bDeleteMiddle == true )
	{
		m_vpParentQuest.Destroy();
		m_vpChildQuest.Destroy();
		SAFE_NIDELETE( m_pParentTail );
		SAFE_NIDELETE( m_pChildTail );

		DeleteMiddle_Tail();
	}
}

void cQuestObj::DeleteMiddle_Tail()
{
	int nCount = m_vpMiddleTail.Size();
	for( int i=0; i<nCount; ++i )
	{
		SAFE_NIDELETE( m_vpMiddleTail[ i ] );
	}
	m_vpMiddleTail.Destroy();
}

CsPoint cQuestObj::GetSpritePos()
{
	return GetPitchPos() + CsPoint( 100, 10 );
}

CsPoint cQuestObj::GetPitchPos()
{
	return CsPoint( m_pFTQuest->GetToolPos().x * PITCH_SIZE_X, m_pFTQuest->GetToolPos().y * PITCH_SIZE_Y );
}

void cQuestObj::_AddEtcString( cText::sTEXTINFO* ti )
{
	cString* pString = NiNew cString;
	pString->AddText( ti );
	m_EtcList.AddTail( pString );
}

void cQuestObj::Init( CsQuest* pFTQuest )
{
	m_eMiddleType = MT_NONE;
	m_pParentTail = NULL;
	m_pChildTail = NULL;
	m_ptNodeIndex = pFTQuest->GetToolPos();

	m_pFTQuest = pFTQuest;

	InitControl();

	_CheckParent();
}

void cQuestObj::InitControl()
{
	cWindow::Init( NULL, CsPoint::ZERO, CsPoint( IF_QUESTNODE_X, IF_QUESTNODE_Y ), 1, false );
	ResetBG();

	cText::sTEXTINFO ti;
	ti.Init( cFont::FS_12, NiColor::BLACK );
	ti.s_bOutLine = false;

	TCHAR*	title = m_pFTQuest->m_szTitleText;
	int size = 180;
	g_pStringAnalysis->Cut_Parcing( &m_TitleList, size, title , &ti );


	// 시작 타겟
	m_pStartTarget = NULL;
	m_pStartTargetBG = NULL;
	switch( m_pFTQuest->GetStartTarget_Type() )
	{
	case CsQuest::ST_NPC:
		{
			// 시작 엔피씨가 존재 하지 않는다면
			if( nsCsFileTable::g_pNpcMng->IsNpc( m_pFTQuest->GetStartTarget_ID() ) == false )
			{				
				CsMessageBox( MB_OK, L"[ %s : %d ] 퀘스트의 시작 엔피씨가 존재하지 않습니다, ID = %d", m_pFTQuest->m_szTitleText, m_pFTQuest->GetUniqID(), m_pFTQuest->GetStartTarget_ID() );
			}
			else
			{
				m_pStartTarget = NiNew cText;
				cText::sTEXTINFO ti;
				ti.Init( cFont::FS_10 );
				ti.s_bOutLine = false;
				ti.SetText( nsCsFileTable::g_pNpcMng->GetNpc( m_pFTQuest->GetStartTarget_ID() )->GetInfo()->s_szName );
				m_pStartTarget->Init( NULL, CsPoint::ZERO, &ti, 1, false );
				m_pStartTargetBG = NiNew cSprite;
				m_pStartTargetBG->Init( NULL, CsPoint::ZERO, CsPoint( m_pStartTarget->GetStringSize().x + 10, 16 ), "ToolData\\QuestTool2\\EtcBG.tga", 1, false );
			}
		}		
		break;
	case CsQuest::ST_DIGIVICE:
		break;
	default:
		assert_cs( false );
	}

	// 완료 타겟
	m_pEndTargetBG = NULL;
	m_pEndTarget = NULL;
	switch( m_pFTQuest->GetQuestTarget() )
	{
	case CsQuest::QTG_COMPLETE:
	case CsQuest::QTG_SELF:			
		break;
	case CsQuest::QTG_NPC:
		{
			if( nsCsFileTable::g_pNpcMng->IsNpc( m_pFTQuest->GetQuestTargetValue1() ) == false )
			{				
				CsMessageBox( MB_OK, L"[ %s : %d ] 퀘스트의 완료 엔피씨가 존재하지 않습니다", m_pFTQuest->m_szTitleText, m_pFTQuest->GetUniqID() );
			}
			else
			{
				m_pEndTarget = NiNew cText;
				cText::sTEXTINFO ti;
				ti.Init( cFont::FS_10 );
				ti.s_bOutLine = false;
				ti.SetText( nsCsFileTable::g_pNpcMng->GetNpc( m_pFTQuest->GetQuestTargetValue1() )->GetInfo()->s_szName );
				m_pEndTarget->Init( NULL, CsPoint::ZERO, &ti, 1, false );
				m_pEndTargetBG = NiNew cSprite;
				m_pEndTargetBG->Init( NULL, CsPoint::ZERO, CsPoint( m_pEndTarget->GetStringSize().x + 10, 16 ), "ToolData\\QuestTool2\\EtcBG.tga", 1, false );
			}
		}
		break;
	default:
		assert_cs( false );
	}

	TCHAR szChar[ 1024 ] = {0};
	// 수행지급	
	CsQuestApplyRequiteGroup* pApplyGroup = m_pFTQuest->GetApplyRequiteGroup();
	int nListCount = pApplyGroup->GetListCount();
	if( nListCount > 0 )
	{
		cText::sTEXTINFO ti;
		ti.Init( cFont::FS_10, NiColor( 150/255.0f, 150/255.0f, 255/255.0f ) );		
		for( int i=0; i<nListCount; ++i )
		{
			CsQuestApplyRequite* pApply = pApplyGroup->Get( i );
			switch( pApply->GetType() )
			{
			case CsQuestApplyRequite::ITEM:
				{
					if( nsCsFileTable::g_pItemMng->IsItem( pApply->GetTypeID() ) == false )
					{
						CsMessageBox( MB_OK, L"[ %s : %d ] 퀘스트의 수행지급 아이템이 존재하지 않습니다", m_pFTQuest->m_szTitleText, m_pFTQuest->GetUniqID() );
					}
					else
					{
						if( pApply->GetTypeCount() > 1 )
						{
							_stprintf_s( szChar, sizeof(szChar), L"%s x %d", nsCsFileTable::g_pItemMng->GetItem( pApply->GetTypeID() )->GetInfo()->s_szName, pApply->GetTypeCount() );
							ti.SetText( szChar );
						}
						else
						{
							ti.SetText( nsCsFileTable::g_pItemMng->GetItem( pApply->GetTypeID() )->GetInfo()->s_szName );
						}
						_AddEtcString( &ti );
					}
				}				
				break;
/*
			case CsQuestApplyRequite::TIME:
				{
					TCHAR sz[ 64 ];
					_stprintf_s( sz, 64, L"TIME %d Seconds", pApply->GetTypeID() );
					ti.SetText( sz );
					_AddEtcString( &ti );
				}				
				break;
*/
			default:
				assert_cs( false );
			}
		}		
	}

	// 수행조건
	CsQuestPreRequireGroup* pPreReqGroup = m_pFTQuest->GetPreRequireGroup();
	nListCount = pPreReqGroup->GetListCount();	
	if( nListCount > 0 )
	{
		cText::sTEXTINFO ti;
		ti.Init( cFont::FS_10, NiColor( 255/255.0f, 150/255.0f, 150/255.0f ) );		
		for( int i=0; i<nListCount; ++i )
		{
			CsQuestPreRequire* pQuestPreReq = pPreReqGroup->Get( i );
			switch( pQuestPreReq->GetType() )
			{
			case CsQuestPreRequire::ITEM:
				{
					if( nsCsFileTable::g_pItemMng->IsItem( pQuestPreReq->GetTypeID() ) == false )
					{
						CsMessageBox( MB_OK, L"[ %s : %d ] 퀘스트의 수행조건 아이템이 존재하지 않습니다", m_pFTQuest->m_szTitleText, m_pFTQuest->GetUniqID() );
						continue;
					}
					else
					{
						if( pQuestPreReq->GetTypeCount() > 1 )
						{
							_stprintf_s( szChar, sizeof(szChar), L"%s x %d", nsCsFileTable::g_pItemMng->GetItem( pQuestPreReq->GetTypeID() )->GetInfo()->s_szName, pQuestPreReq->GetTypeCount() );
							ti.SetText( szChar );
						}
						else
						{
							ti.SetText( nsCsFileTable::g_pItemMng->GetItem( pQuestPreReq->GetTypeID() )->GetInfo()->s_szName );
						}
						_AddEtcString( &ti );
					}			
				}
				break;
			case CsQuestPreRequire::QUEST:
				break;
			case CsQuestPreRequire::T_LEVEL:
				{
					_stprintf_s( szChar, sizeof(szChar), L"TLevel %d", pQuestPreReq->GetTypeID() );
					ti.SetText( szChar );
					_AddEtcString( &ti );
				}
				break;
			case CsQuestPreRequire::D_LEVEL:
				{
					_stprintf_s( szChar, sizeof(szChar), L"DLevel %d", pQuestPreReq->GetTypeID() );
					ti.SetText( szChar );
					_AddEtcString( &ti );
				}
				break;
			case CsQuestPreRequire::D_TYPE:
				{
					if( nsCsFileTable::g_pDigimonMng->GetDigimon( pQuestPreReq->GetTypeID() ) == false )
					{
						CsMessageBox( MB_OK, L"[ %s : %d ] 퀘스트의 디지몬이 존재하지 않습니다", m_pFTQuest->m_szTitleText, m_pFTQuest->GetUniqID() );
						continue;
					}
					else
					{
						_stprintf_s( szChar, sizeof(szChar), L"DType %s", nsCsFileTable::g_pDigimonMng->GetDigimon( pQuestPreReq->GetTypeID() )->GetInfo()->s_szName );
						ti.SetText( szChar );
						_AddEtcString( &ti );
					}
				}
				break;
			case CsQuestPreRequire::REGION:
				{
					if( nsCsMapTable::g_pMapRegionMng->IsRegionGroup( pQuestPreReq->GetTypeID() / 1000 ) == false )
					{
						CsMessageBox( MB_OK, L"[ %s : %d ] 퀘스트의 맵이 존재하지 않습니다", m_pFTQuest->m_szTitleText, m_pFTQuest->GetUniqID() );
						continue;
					}
					else
					{
						if( nsCsMapTable::g_pMapRegionMng->GetRegionGroup( pQuestPreReq->GetTypeID() / 1000 )->GetRegionCount() <= (int)( pQuestPreReq->GetTypeID() % 1000 ) )
						{
							CsMessageBox( MB_OK, L"[ %s : %d ] 퀘스트의 영역이 존재하지 않습니다", m_pFTQuest->m_szTitleText, m_pFTQuest->GetUniqID() );
							continue;
						}

						_stprintf_s( szChar, sizeof(szChar), L"Region 맵:%s 지역:%s",
							nsCsMapTable::g_pMapListMng->GetList( pQuestPreReq->GetTypeID() / 1000 )->GetInfo()->s_szMapDiscript.c_str(),
							nsCsMapTable::g_pMapRegionMng->GetRegionGroup( pQuestPreReq->GetTypeID() / 1000 )->Get( pQuestPreReq->GetTypeID() % 1000 )->GetInfo()->s_szDiscript);
						ti.SetText( szChar );
						_AddEtcString( &ti );
					}
				}
				break;
			case CsQuestPreRequire::D_EVO:
				{
					_stprintf_s( szChar, sizeof(szChar), L"DEVO %d", pQuestPreReq->GetTypeID() );
					ti.SetText( szChar );
					_AddEtcString( &ti );
				}
				break;

			case CsQuestPreRequire::D_EVO_DISABLE:
				{
					_stprintf_s( szChar, sizeof(szChar), L"Closed DEVO %d", pQuestPreReq->GetTypeID() );
					ti.SetText( szChar );
					_AddEtcString( &ti );
				}
				break;

			case CsQuestPreRequire::D_BASE_TABLE_IDX: // 디지몬의 기본체 ID 조건
				{
					_stprintf_s( szChar, sizeof(szChar), L"Digimon Base IDX %d", pQuestPreReq->GetTypeID() );
					ti.SetText( szChar );
					_AddEtcString( &ti );
				}
				break;
			case CsQuestPreRequire::TAMER_TABLE_IDX:		// 테이머 테이블 ID 조건
				{
					_stprintf_s( szChar, sizeof(szChar), L"Tamer Table IDX %d", pQuestPreReq->GetTypeID() );
					ti.SetText( szChar );
					_AddEtcString( &ti );
				}
				break;
			case CsQuestPreRequire::QUEST_ID_CANNOT_COMPLETED:		// 완료된 퀘스트가 있을시 진행 불가
				{
					_stprintf_s( szChar, sizeof(szChar), L"Completed Quest ID %d", pQuestPreReq->GetTypeID() );
					ti.SetText( szChar );
					_AddEtcString( &ti );
				}
				break;
			case CsQuestPreRequire::QUEST_ID_CANNOT_PROGRESS:			// 진행 중인 퀘스트가 있을시 진행 불가
				{
					_stprintf_s( szChar, sizeof(szChar), L"Progress Quest ID %d", pQuestPreReq->GetTypeID() );
					ti.SetText( szChar );
					_AddEtcString( &ti );
				}
				break;
			default:
				assert_cs( false );
			}			
		}
	}

	// 완료 조건
	CsQuestCompRequireGroup* pCompReqGroup = m_pFTQuest->GetCompRequireGroup();
	nListCount = pCompReqGroup->GetListCount();	
	if( nListCount > 0 )
	{
		cText::sTEXTINFO ti;
		ti.Init( cFont::FS_10, NiColor( 120/255.0f, 255/255.0f, 120/255.0f ) );		
		for( int i=0; i<nListCount; ++i )
		{
			CsQuestCompRequire* pQuestCompReq = pCompReqGroup->Get( i );
			switch( pQuestCompReq->GetType() )
			{
			case CsQuestCompRequire::MONSTER:
				{
					if( nsCsFileTable::g_pMonsterMng->IsMonster( pQuestCompReq->GetTypeID() ) == false )
					{
						CsMessageBox( MB_OK, L"[ %s : %d ] 퀘스트의 완료조건 몬스터가 존재하지 않습니다", m_pFTQuest->m_szTitleText, m_pFTQuest->GetUniqID() );
						continue;
					}
					else
					{
						_stprintf_s( szChar, sizeof(szChar), L"%s x %d", nsCsFileTable::g_pMonsterMng->GetMonster( pQuestCompReq->GetTypeID() )->GetInfo()->s_szName, pQuestCompReq->GetTypeCount() );
						ti.SetText( szChar );
						_AddEtcString( &ti );
					}
				}
				break;
			case CsQuestCompRequire::ITEM:
				{
					if( nsCsFileTable::g_pItemMng->IsItem( pQuestCompReq->GetTypeID() ) == false )
					{
						CsMessageBox( MB_OK, L"[ %s : %d ] 퀘스트의 완료조건 아이템이 존재하지 않습니다", m_pFTQuest->m_szTitleText, m_pFTQuest->GetUniqID() );
						continue;
					}
					else
					{
						if( pQuestCompReq->GetTypeCount() > 1 )
						{
							_stprintf_s( szChar, sizeof(szChar), L"%s x %d", nsCsFileTable::g_pItemMng->GetItem( pQuestCompReq->GetTypeID() )->GetInfo()->s_szName, pQuestCompReq->GetTypeCount() );
							ti.SetText( szChar );
						}
						else
						{
							ti.SetText( nsCsFileTable::g_pItemMng->GetItem( pQuestCompReq->GetTypeID() )->GetInfo()->s_szName );
						}
						_AddEtcString( &ti );
					}			
				}
				break;
			case CsQuestCompRequire::CLIENT:
				{
					_stprintf_s( szChar, sizeof(szChar), L"Client %d", pQuestCompReq->GetTypeID() );
					ti.SetText( szChar );
					_AddEtcString( &ti );
				}
				break;
			case CsQuestCompRequire::T_LEVEL:
				{
					_stprintf_s( szChar, sizeof(szChar), L"TLevel %d", pQuestCompReq->GetTypeID() );
					ti.SetText( szChar );
					_AddEtcString( &ti );
				}
				break;
			case CsQuestCompRequire::D_TYPE:
				{
					if( nsCsFileTable::g_pDigimonMng->GetDigimon( pQuestCompReq->GetTypeID() ) == false )
					{
						CsMessageBox( MB_OK, L"[ %s : %d ] 퀘스트의 디지몬이 존재하지 않습니다", m_pFTQuest->m_szTitleText, m_pFTQuest->GetUniqID() );
						continue;
					}
					else
					{
						_stprintf_s( szChar, sizeof(szChar), L"DType %s", nsCsFileTable::g_pDigimonMng->GetDigimon( pQuestCompReq->GetTypeID() )->GetInfo()->s_szName );
						ti.SetText( szChar );
						_AddEtcString( &ti );
					}
				}
				break;
			case CsQuestCompRequire::REGION:
				{
					if( nsCsMapTable::g_pMapRegionMng->IsRegionGroup( pQuestCompReq->GetTypeID() / 1000 ) == false )
					{
						CsMessageBox( MB_OK, L"[ %s : %d ] 퀘스트의 완료조건 탐사맵이 존재하지 않습니다", m_pFTQuest->m_szTitleText, m_pFTQuest->GetUniqID() );
					}
					else if( nsCsMapTable::g_pMapRegionMng->GetRegionGroup( pQuestCompReq->GetTypeID() / 1000 )->GetRegionCount() <= (int)( pQuestCompReq->GetTypeID() % 1000 ) )
					{
						CsMessageBox( MB_OK, L"[ %s : %d ] 퀘스트의 완료조건 탐사맵의 영역이 존재하지 않습니다", m_pFTQuest->m_szTitleText, m_pFTQuest->GetUniqID() );
					}
					else
					{
						CsMapRegion* pRegion = nsCsMapTable::g_pMapRegionMng->GetRegionGroup( pQuestCompReq->GetTypeID() / 1000 )->Get( pQuestCompReq->GetTypeID() % 1000 );					
						// 영역 검사
						_stprintf_s( szChar, sizeof(szChar), L"%s 탐사", pRegion->GetInfo()->s_szDiscript );
						ti.SetText( szChar );
						_AddEtcString( &ti );
					}
				}
				break;
			case CsQuestCompRequire::NPCTALK:
				{
					if( nsCsFileTable::g_pNpcMng->IsNpc( pQuestCompReq->GetTypeID() ) == false )
					{
						CsMessageBox( MB_OK, L"[ %s : %d ] 퀘스트의 완료조건 Npc가 존재하지 않습니다", m_pFTQuest->m_szTitleText, m_pFTQuest->GetUniqID() );
					}
					else
					{
						CsNpc* pNpc = nsCsFileTable::g_pNpcMng->GetNpc( pQuestCompReq->GetTypeID() );						

						_stprintf_s( szChar, sizeof(szChar), L"%s %s 대화", pNpc->GetInfo()->s_szPenName, pNpc->GetInfo()->s_szName );
						ti.SetText( szChar );
						_AddEtcString( &ti );
					}
				}
				break;
			case CsQuestCompRequire::USE_ITEM:
				{
					if( nsCsFileTable::g_pItemMng->IsItem( pQuestCompReq->GetTypeID() ) == false )
					{
						CsMessageBox( MB_OK, L"[ %s : %d ] 퀘스트의 완료조건 아이템이 존재하지 않습니다", m_pFTQuest->m_szTitleText, m_pFTQuest->GetUniqID() );
						continue;
					}
					else
					{
						if( pQuestCompReq->GetTypeCount() > 1 )
						{
							_stprintf_s( szChar, sizeof(szChar), L"%s사용 x %d", nsCsFileTable::g_pItemMng->GetItem( pQuestCompReq->GetTypeID() )->GetInfo()->s_szName, pQuestCompReq->GetTypeCount() );
							ti.SetText( szChar );
						}
						else
						{
							ti.SetText( nsCsFileTable::g_pItemMng->GetItem( pQuestCompReq->GetTypeID() )->GetInfo()->s_szName );
						}
						_AddEtcString( &ti );
					}
				}
				break;
			case CsQuestCompRequire::USE_ITEM_NPC:
				{
					if( nsCsFileTable::g_pItemMng->IsItem( pQuestCompReq->GetTypeID() ) == false )
					{
						CsMessageBox( MB_OK, L"[ %s : %d ] 퀘스트의 완료조건 아이템이 존재하지 않습니다", m_pFTQuest->m_szTitleText, m_pFTQuest->GetUniqID() );
						continue;
					}
					else
					{
						if( nsCsFileTable::g_pNpcMng->IsNpc( pQuestCompReq->GetTargetID() ) == false )
						{
							CsMessageBox( MB_OK, L"[ %s : %d ] 퀘스트의 완료조건 타겟 Npc가 존재하지 않습니다", m_pFTQuest->m_szTitleText, m_pFTQuest->GetUniqID() );
						}
						else
						{
							CsItem* pFTItem = nsCsFileTable::g_pItemMng->GetItem( pQuestCompReq->GetTypeID() );
							CsNpc* pNpc = nsCsFileTable::g_pNpcMng->GetNpc( pQuestCompReq->GetTargetID() );

							_stprintf_s( szChar, sizeof(szChar), L"%s :%s x %d", pFTItem->GetInfo()->s_szName, pNpc->GetInfo()->s_szName, pQuestCompReq->GetTypeCount() );
							ti.SetText( szChar );
							_AddEtcString( &ti );
						}
					}
				}
				break;
			case CsQuestCompRequire::USE_ITEM_MONSTER:
				{
					if( nsCsFileTable::g_pItemMng->IsItem( pQuestCompReq->GetTypeID() ) == false )
					{
						CsMessageBox( MB_OK, L"[ %s : %d ] 퀘스트의 완료조건 아이템이 존재하지 않습니다", m_pFTQuest->m_szTitleText, m_pFTQuest->GetUniqID() );
						continue;
					}
					else
					{
						if( nsCsFileTable::g_pMonsterMng->IsMonster( pQuestCompReq->GetTargetID() ) == false )
						{
							CsMessageBox( MB_OK, L"[ %s : %d ] 퀘스트의 완료조건 타겟 몬스터가 존재하지 않습니다", m_pFTQuest->m_szTitleText, m_pFTQuest->GetUniqID() );
						}
						else
						{
							CsItem* pFTItem = nsCsFileTable::g_pItemMng->GetItem( pQuestCompReq->GetTypeID() );
							CsMonster* pFTMonster = nsCsFileTable::g_pMonsterMng->GetMonster( pQuestCompReq->GetTargetID() );

							_stprintf_s( szChar, sizeof(szChar), L"%s :%s x %d", pFTItem->GetInfo()->s_szName, pFTMonster->GetInfo()->s_szName, pQuestCompReq->GetTypeCount() );
							ti.SetText( szChar );
							_AddEtcString( &ti );
						}
					}
				}
				break;
			case CsQuestCompRequire::USE_ITEM_REGION:
				{
					if( nsCsFileTable::g_pItemMng->IsItem( pQuestCompReq->GetTypeID() ) == false )
					{
						CsMessageBox( MB_OK, L"[ %s : %d ] 퀘스트의 완료조건 아이템이 존재하지 않습니다", m_pFTQuest->m_szTitleText, m_pFTQuest->GetUniqID() );
						continue;
					}
					else
					{
						if( nsCsMapTable::g_pMapRegionMng->IsRegionGroup( pQuestCompReq->GetTargetID() / 1000 ) == false )
						{
							CsMessageBox( MB_OK, L"[ %s : %d ] 퀘스트의 완료조건 탐사맵이 존재하지 않습니다", m_pFTQuest->m_szTitleText, m_pFTQuest->GetUniqID() );
						}
						else if( nsCsMapTable::g_pMapRegionMng->GetRegionGroup( pQuestCompReq->GetTargetID() / 1000 )->GetRegionCount() <= (int)( pQuestCompReq->GetTargetID() % 1000 ) )
						{
							CsMessageBox( MB_OK, L"[ %s : %d ] 퀘스트의 완료조건 탐사맵의 영역이 존재하지 않습니다", m_pFTQuest->m_szTitleText, m_pFTQuest->GetUniqID() );
						}
						else
						{
							CsItem* pFTItem = nsCsFileTable::g_pItemMng->GetItem( pQuestCompReq->GetTypeID() );
							CsMapRegion* pRegion = nsCsMapTable::g_pMapRegionMng->GetRegionGroup( pQuestCompReq->GetTargetID() / 1000 )->Get( pQuestCompReq->GetTargetID() % 1000 );					
							
							_stprintf_s( szChar, sizeof(szChar), L"%s :%s x %d", pFTItem->GetInfo()->s_szName, pRegion->GetInfo()->s_szDiscript, pQuestCompReq->GetTypeCount() );
							ti.SetText( szChar );
							_AddEtcString( &ti );
						}
					}
				}
				break;
			default:
				assert_cs( false );
			}
		}
	}
	else
	{
		cText::sTEXTINFO ti;
		ti.Init( cFont::FS_10, NiColor( 120/255.0f, 255/255.0f, 120/255.0f ) );	
		ti.SetText( L"[ 즉시완료 ]" );
		_AddEtcString( &ti );
	}

	// 보상
	CsQuestRequiteGroup* pRequiteGroup = m_pFTQuest->GetRequiteGroup();
	nListCount = pRequiteGroup->GetListCount();	
	if( nListCount > 0 )
	{
		cString* pSelString = NULL;
		cText::sTEXTINFO ti;
		ti.Init( cFont::FS_10, NiColor( 240/255.0f, 240/255.0f, 240/255.0f ) );		
		for( int i=0; i<nListCount; ++i )
		{
			CsQuestRequite* pRequite = pRequiteGroup->Get( i );
			switch( pRequite->GetMethodID() )
			{
			case CsQuestRequite::RM_GIVE:
				switch( pRequite->GetType() )
				{
				case CsQuestRequite::MONEY:
					{
						_stprintf_s( szChar, sizeof(szChar), L"%d $", pRequite->GetTypeID() );
						ti.SetText( szChar );
						_AddEtcString( &ti );
					}
					break;
				case CsQuestRequite::EXP:
					{
						_stprintf_s( szChar, sizeof(szChar), L"%d Exp", pRequite->GetTypeID() );
						ti.SetText( szChar );
						_AddEtcString( &ti );
					}
					break;
				case CsQuestRequite::ITEM:
					{
						if( nsCsFileTable::g_pItemMng->IsItem( pRequite->GetTypeID() ) == false )
						{
							CsMessageBox( MB_OK, L"[ %s : %d ] 퀘스트의 보상 아이템이 존재하지 않습니다", m_pFTQuest->m_szTitleText, m_pFTQuest->GetUniqID() );
							continue;
						}
						else
						{
							if( pRequite->GetTypeCount() > 1 )
							{
								_stprintf_s( szChar, sizeof(szChar), L"%s x %d", nsCsFileTable::g_pItemMng->GetItem( pRequite->GetTypeID() )->GetInfo()->s_szName, pRequite->GetTypeCount() );
								ti.SetText( szChar );
							}
							else
							{
								ti.SetText( nsCsFileTable::g_pItemMng->GetItem( pRequite->GetTypeID() )->GetInfo()->s_szName );
							}
							_AddEtcString( &ti );
						}			
					}
					break;
				case CsQuestRequite::EVOSLOT:
					{
						_stprintf_s( szChar, sizeof(szChar), L"%d Idx", pRequite->GetTypeID() );
						ti.SetText( szChar );
						_AddEtcString( &ti );
					}
					break;
				default:
					assert_cs( false );
				}
				break;
			case CsQuestRequite::RM_SEL1:
				switch( pRequite->GetType() )
				{				
				case CsQuestRequite::ITEM:
					{
						if( pSelString == NULL )
						{
							pSelString = NiNew cString;
						}
						else
						{
							ti.SetText( L", " );
							pSelString->AddText( &ti );
						}

						if( nsCsFileTable::g_pItemMng->IsItem( pRequite->GetTypeID() ) == false )
						{
							CsMessageBox( MB_OK, L"[ %s : %d ] 퀘스트의 보상 아이템이 존재하지 않습니다", m_pFTQuest->m_szTitleText, m_pFTQuest->GetUniqID() );
							continue;
						}
						else
						{
							if( pRequite->GetTypeCount() > 1 )
							{
								_stprintf_s( szChar, sizeof(szChar), L"%s x %d", nsCsFileTable::g_pItemMng->GetItem( pRequite->GetTypeID() )->GetInfo()->s_szName, pRequite->GetTypeCount() );
								ti.SetText( szChar );
							}
							else
							{
								ti.SetText( nsCsFileTable::g_pItemMng->GetItem( pRequite->GetTypeID() )->GetInfo()->s_szName );
							}
							pSelString->AddText( &ti );
						}
					}
					break;
				case CsQuestRequite::MONEY:
				case CsQuestRequite::EXP:
					{
						CsMessageBox( MB_OK, L"[ %s : %d ] 퀘스트 : 선택 보상은 아이템이여야만 합니다.", m_pFTQuest->m_szTitleText, m_pFTQuest->GetUniqID() );
						continue;
					}
					break;
				default:
					assert_cs( false );
				}
				break;
			}
		}

		if( pSelString != NULL )
		{
			m_EtcList.AddTail( pSelString );
		}
	}
}

void cQuestObj::ResetControl()
{
	cWindow::Delete();

	m_TitleList.Delete();
	m_SpriteBG.Delete();
	SAFE_NIDELETE( m_pStartTarget );
	SAFE_NIDELETE( m_pStartTargetBG );
	SAFE_NIDELETE( m_pEndTarget );
	SAFE_NIDELETE( m_pEndTargetBG );
	m_EtcList.Delete();

	InitControl();
}

bool cQuestObj::UpdateCursor( CsPoint ptLocalCursor )
{
	CsPoint ptPitchLocal = ptLocalCursor - GetPitchPos();
	if( ( ptPitchLocal.x < 0 )||( ptPitchLocal.x > PITCH_SIZE_X )||
		( ptPitchLocal.y < 0 )||( ptPitchLocal.y > PITCH_SIZE_Y ) )
	{
		return false;
	}

	CsPoint ptSpriteLocal = ptLocalCursor - GetSpritePos();

	sMIDDLE_INFO* mi;
	int nCount = m_vpMiddleTail.Size();
	for( int i=0; i<nCount; ++i )
	{
		mi = m_vpMiddleTail[ i ];
		switch( mi->s_eType )
		{
		case MTE_LINE:
			break;
		case MTE_JUMP_PARENT:
			{
				if( mi->IsClick( ptSpriteLocal ) )
				{					
					g_pViewer->_GetNodeMng()->GetPopup()->SetPopup( cPopup::JUMP, mi->s_pLinkQuestObj, g_pViewer->_GetCursorScreen() + CsPoint( -IF_QUESTNODE_X, 0 ) );
					return true;
				}			
			}
			break;
		case MTE_JUMP_CHILD:
			{
				if( mi->IsClick( ptSpriteLocal ) )
				{					
					g_pViewer->_GetNodeMng()->GetPopup()->SetPopup( cPopup::JUMP, mi->s_pLinkQuestObj, g_pViewer->_GetCursorScreen() + CsPoint( 12, 0 )  );
					return true;
				}			
			}
			break;
		default:
			assert_cs( false );
		}		
	}

	// 영역 안으로 들어 왔으므로
	return true;
}

void cQuestObj::RenderTail( CsPoint ptNodePos )
{
	CsPoint pos = ptNodePos + GetSpritePos();

	if( m_pParentTail )
	{
		m_pParentTail->Render( pos + CsPoint( -IF_TAIL_LEN, IF_QUESTNODE_Y - 10 ) );
	}

	if( m_pChildTail )
	{
		m_pChildTail->Render( pos + CsPoint( IF_QUESTNODE_X, IF_QUESTNODE_Y - 10 ) );
	}

	int nCount = m_vpMiddleTail.Size();
	for( int i=0; i<nCount; ++i )
	{
		switch( m_vpMiddleTail[ i ]->s_eType )
		{
		case MTE_LINE:
			m_vpMiddleTail[ i ]->s_Sprite.RenderRotation( pos + CsPoint( IF_QUESTNODE_X + IF_TAIL_LEN, IF_QUESTNODE_Y - 10 ) );
			break;
		case MTE_JUMP_CHILD:
		case MTE_JUMP_PARENT:
			m_vpMiddleTail[ i ]->s_Sprite.Render( pos + m_vpMiddleTail[ i ]->s_ptDelta );
			break;
		default:
			assert_cs( false );
		}
	}
}

void cQuestObj::Render( CsPoint ptNodePos )
{
	CsPoint pos = ptNodePos + GetSpritePos();

	if( g_pDiscript->_GetSelQuest() == this )
		m_SpriteBG.SetAlpha( 0.5f );
	else
		m_SpriteBG.SetAlpha( 1.0f );

	m_SpriteBG.Render( pos );
	m_TitleList.Render( pos + CsPoint( 10, 5 ), 3 );

	if( m_pStartTargetBG )
	{
		assert_cs( m_pStartTarget != NULL );
		CsPoint ptStartTarget = pos + CsPoint( -m_pStartTargetBG->GetSize().x+4, 2 );
		m_pStartTargetBG->Render( ptStartTarget );
		m_pStartTarget->Render( ptStartTarget + CsPoint( 5, 1 ) );
	}

	if( m_pEndTargetBG )
	{
		assert_cs( m_pEndTarget != NULL );
		CsPoint ptEndTarget = pos + CsPoint( IF_QUESTNODE_X - 4, 2 );
		m_pEndTargetBG->Render( ptEndTarget );
		m_pEndTarget->Render( ptEndTarget + CsPoint( 5, 1 ) );
	}

	m_EtcList.Render( pos + CsPoint( 0, IF_QUESTNODE_Y + 3 ), 1 );
}

void cQuestObj::RenderPos( CsPoint ptPos )
{
	m_SpriteBG.Render( ptPos );
	m_TitleList.Render( ptPos + CsPoint( 10, 5 ), 3 );
}

void cQuestObj::ResetDevice()
{
	cWindow::ResetDevice();
	m_SpriteBG.ResetDevice();
	m_TitleList.ResetDevice();
	if( m_pParentTail )
		m_pParentTail->ResetDevice();
	if( m_pChildTail )
		m_pChildTail->ResetDevice();

	int nCount = m_vpMiddleTail.Size();
	for( int i=0; i<nCount; ++i )
	{
		m_vpMiddleTail[ i ]->s_Sprite.ResetDevice();
	}

	if( m_pStartTargetBG )
		m_pStartTargetBG->ResetDevice();
	if( m_pStartTarget )
		m_pStartTarget->ResetDevice();
	if( m_pEndTarget )
		m_pEndTarget->ResetDevice();
	if( m_pEndTargetBG )
		m_pEndTargetBG->ResetDevice();

	m_EtcList.ResetDevice();
}

void cQuestObj::SetAlpha( float fAlpha )
{
	m_SpriteBG.SetAlpha( fAlpha );
	m_TitleList.SetAlpha( fAlpha );
}

void cQuestObj::ResetBG()
{
	m_SpriteBG.Delete();

	if( m_pFTQuest->IsActive() )
	{
		switch( m_pFTQuest->GetQuestType() )
		{
		case CsQuest::QT_SUB:
			m_SpriteBG.Init( this, CsPoint::ZERO, CsPoint( IF_QUESTNODE_X, IF_QUESTNODE_Y ), "ToolData\\QuestTool2\\SubQ.tga", 1, false );
			break;
		case CsQuest::QT_MAIN:
			m_SpriteBG.Init( this, CsPoint::ZERO, CsPoint( IF_QUESTNODE_X, IF_QUESTNODE_Y ), "ToolData\\QuestTool2\\MainQ.tga", 1, false );
			break;
		case CsQuest::QT_REPEAT:
			m_SpriteBG.Init( this, CsPoint::ZERO, CsPoint( IF_QUESTNODE_X, IF_QUESTNODE_Y ), "ToolData\\QuestTool2\\RepeatQ.tga", 1, false );
			break;
		case CsQuest::QT_DAILY:
			m_SpriteBG.Init( this, CsPoint::ZERO, CsPoint( IF_QUESTNODE_X, IF_QUESTNODE_Y ), "ToolData\\QuestTool2\\DailyQ.tga", 1, false );
			break;
		case CsQuest::QT_EVENTREPEAT:
			m_SpriteBG.Init( this, CsPoint::ZERO, CsPoint( IF_QUESTNODE_X, IF_QUESTNODE_Y ), "ToolData\\QuestTool2\\EventRepeatQ.tga", 1, false );
			break;
		case CsQuest::QT_ACHIEVE:
			m_SpriteBG.Init( this, CsPoint::ZERO, CsPoint( IF_QUESTNODE_X, IF_QUESTNODE_Y ), "ToolData\\QuestTool2\\RepeatQ.tga", 1, false );
			break;
		case CsQuest::QT_JOINTPROGRESS:
			m_SpriteBG.Init( this, CsPoint::ZERO, CsPoint( IF_QUESTNODE_X, IF_QUESTNODE_Y ), "ToolData\\QuestTool2\\Jogress.tga", 1, false );
			break;
		case CsQuest::QT_TUTORIAL:
			m_SpriteBG.Init( this, CsPoint::ZERO, CsPoint( IF_QUESTNODE_X, IF_QUESTNODE_Y ), "ToolData\\QuestTool2\\EventRepeatQ.tga", 1, false );
			break;

		case CsQuest::QT_XANTI_JOINTPROGRESS:
			m_SpriteBG.Init( this, CsPoint::ZERO, CsPoint( IF_QUESTNODE_X, IF_QUESTNODE_Y ), "ToolData\\QuestTool2\\XJogress.tga", 1, false );
			break;
			
/*
		case CsQuest::QT_TIME:
			m_SpriteBG.Init( this, CsPoint::ZERO, CsPoint( IF_QUESTNODE_X, IF_QUESTNODE_Y ), "ToolData\\QuestTool2\\EventRepeatQ.tga", 1, false );
			break;
*/
		default:
			assert_cs( false );
		}
	}
	else
	{
		m_SpriteBG.Init( this, CsPoint::ZERO, CsPoint( IF_QUESTNODE_X, IF_QUESTNODE_Y ), "ToolData\\QuestTool2\\InactiveQ.tga", 1, false );
	}
}

void cQuestObj::_CheckParent()
{
	CsQuestPreRequireGroup* pPreReqGroup = m_pFTQuest->GetPreRequireGroup();
	CsQuestPreRequire* pPreReq;
	int nReqCount = pPreReqGroup->GetListCount();
	for( int nReq = 0; nReq < nReqCount; ++nReq )
	{
		pPreReq = pPreReqGroup->Get( nReq );
		switch( pPreReq->GetType() )
		{
		// 퀘스트 선행이라면 정보 저장
		case CsQuestPreRequire::QUEST:
			{
				cQuestObj* pParentObj = g_pViewer->_GetQuestMng()->FindNode( nsCsFileTable::g_pQuestMng->GetQuest( pPreReq->GetTypeID() ) );
				// 선행 퀘스트가 존재 한다면
				if( pParentObj != NULL )
				{
					pParentObj->AddChild( this );
					AddParent( pParentObj );					
				}
				else
				{					
					CsMessageBox( MB_OK, L"[ %s : %d ]퀘스트의\n선행퀘스트가 존재 하지 않습니다. ID = %d", m_pFTQuest->m_szTitleText, m_pFTQuest->GetUniqID(), pPreReq->GetTypeID() );
				}
			}			
			break;
		case CsQuestPreRequire::T_LEVEL:
		case CsQuestPreRequire::ITEM:
		case CsQuestPreRequire::D_LEVEL:
		case CsQuestPreRequire::D_TYPE:
		case CsQuestPreRequire::REGION:
		case CsQuestPreRequire::D_EVO:
		case CsQuestPreRequire::D_EVO_DISABLE:
		case CsQuestPreRequire::D_BASE_TABLE_IDX: // 디지몬의 기본체 ID 조건
		case CsQuestPreRequire::TAMER_TABLE_IDX:		// 테이머 테이블 ID 조건
		case CsQuestPreRequire::QUEST_ID_CANNOT_COMPLETED:		// 완료된 퀘스트가 있을시 진행 불가
		case CsQuestPreRequire::QUEST_ID_CANNOT_PROGRESS:			// 진행 중인 퀘스트가 있을시 진행 불가
			break;
		default:
			assert_cs( false );
		}
	}
}

int cQuestObj::FindChild_Index( cQuestObj* pChild )
{
	int nCount = m_vpChildQuest.Size();
	for( int i=0; i<nCount; ++i )
	{
		if( m_vpChildQuest[ i ] == pChild )
			return i;
	}
	assert_cs( false );
	return 0;
}

void cQuestObj::AddParent( cQuestObj* pParent )
{
	m_vpParentQuest.PushBack( pParent );

	if( m_pParentTail == NULL )
	{
		m_pParentTail = NiNew cSprite;
		m_pParentTail->Init( NULL, CsPoint::ZERO, CsPoint( IF_TAIL_LEN, 2 ), 1, false, NiColor( 1, 0, 0 ) );
	}
}

void cQuestObj::DelParent( cQuestObj* pParent )
{
	m_vpParentQuest.DeleteElement_FromData( pParent );
	if( m_vpParentQuest.Size() == 0 )
	{
		SAFE_NIDELETE( m_pParentTail );
	}
}

void cQuestObj::DelAllParent()
{
	m_vpParentQuest.Destroy();
	SAFE_NIDELETE( m_pParentTail );
}

void cQuestObj::UnlinkParent()
{
	int nParent = GetParentCount();
	for( int i=0; i<nParent; ++i )
	{
		cQuestObj* pParent = GetParent( i );
		pParent->DelChild( this );
		pParent->ResetMiddleTail();
	}
	DelAllParent();	

	CsQuestPreRequireGroup* pGroup = GetFT()->GetPreRequireGroup();
	CsQuestPreRequire::LIST* pList = pGroup->GetList();
	CsQuestPreRequire::LIST_IT it = pList->begin();
	CsQuestPreRequire::LIST_IT itEnd = pList->end();
	while( it!=itEnd )
	{
		if( (*it)->GetType() == CsQuestPreRequire::QUEST )
		{
			SAFE_DELETE( *it );
			it = pList->erase( it );
		}
		else
		{
			++it;
		}
	}
	ResetMiddleTail();
}

void cQuestObj::AddChild( cQuestObj* pChild )
{
	m_vpChildQuest.PushBack( pChild );

	if( m_pChildTail == NULL )
	{
		m_pChildTail = NiNew cSprite;
		m_pChildTail->Init( NULL, CsPoint::ZERO, CsPoint( IF_TAIL_LEN, 2 ), 1, false, NiColor( 1, 0, 0 ) );
	}	
}

void cQuestObj::DelChild( cQuestObj* pChild )
{
	m_vpChildQuest.DeleteElement_FromData( pChild );
	if( m_vpChildQuest.Size() == 0 )
	{
		SAFE_NIDELETE( m_pChildTail );
	}
}

void cQuestObj::DelAllChild()
{
	m_vpChildQuest.Destroy();
	SAFE_NIDELETE( m_pChildTail );
}

void cQuestObj::UnlinkChild()
{
	int nChild = GetChildCount();
	for( int i=0; i<nChild; ++i )
	{
		cQuestObj* pChild = GetChild( i );
		pChild->DelParent( this );
		pChild->ResetMiddleTail();

		CsQuestPreRequireGroup* pGroup = pChild->GetFT()->GetPreRequireGroup();
		CsQuestPreRequire::LIST* pList = pGroup->GetList();
		CsQuestPreRequire::LIST_IT it = pList->begin();
		CsQuestPreRequire::LIST_IT itEnd = pList->end();
		while( it!=itEnd )
		{
			if( ( (*it)->GetType() == CsQuestPreRequire::QUEST )&&( (*it)->GetTypeID() == GetFT()->GetUniqID() ) )
			{
				SAFE_DELETE( *it );
				it = pList->erase( it );
			}
			else
			{
				++it;
			}
		}
	}
	DelAllChild();
	ResetMiddleTail();
}

void cQuestObj::ResetMiddleTail()
{
	if( m_eMiddleType == MT_NONE )
	{
		m_eMiddleType = g_pViewer->_GetQuestMng()->GetQuestMiddleType();
	}
	if( m_eMiddleType == MT_NONE )
	{
		return;
	}
	DeleteMiddle_Tail();
	switch( m_eMiddleType )
	{
	case MT_LINE:
		CreateMiddle_LineTail();
		break;
	case MT_JUMP:
		CreateMiddle_JumpTail();
		break;
	case MT_LINEJUMP:
		CreateMiddle_LineJumpTail();
		break;
	default:
		assert_cs( false );
	}
}

void cQuestObj::ResetMiddleTail_ParentChild()
{
	int nCnt = GetParentCount();
	for( int i=0; i<nCnt; ++i )
	{
		m_vpParentQuest[ i ]->ResetMiddleTail();
	}
	nCnt = GetChildCount();
	for( int i=0; i<nCnt; ++i )
	{
		m_vpChildQuest[ i ]->ResetMiddleTail();
	}
}

void cQuestObj::CreateMiddle_LineTail()
{
	m_eMiddleType = MT_LINE;

	if( m_pChildTail == NULL )
		return;	

	NiColor color;
	int nCount = m_vpChildQuest.Size();
	CsPoint ptStart = GetChildTailPos();
	NiPoint2 vStart = NiPoint2( (float)ptStart.x, (float)ptStart.y );
	for( int i=0; i<nCount; ++i )
	{
		// == 가운데 꼬리생성
		CsPoint ptEnd = m_vpChildQuest[ i ]->GetParentTailPos();
		NiPoint2 vEnd = NiPoint2( (float)ptEnd.x, (float)ptEnd.y );
		NiPoint2 vDir = vEnd - vStart;
		// 사이즈
		int sizex = (int)( vDir.Unitize() + 1 );

		float fRot = NiACos( NiPoint2( 1, 0 ).Dot( vDir ) );
		if( vEnd.y < vStart.y )
			fRot = -fRot;

		if( m_vpChildQuest[ i ]->GetNodeIndex() == GetNodeIndex() )
		{
			color = NiColor( 1, 0, 0 );
		}
		else
		{
			color = NiColor( ( rand()%255*0.6f )/255.0f, ( rand()%100 )/100.0f, ( rand()%200 )/400.0f + 0.5f );
		}

		sMIDDLE_INFO* pMiddle = NiNew sMIDDLE_INFO;
		pMiddle->s_eType = MTE_LINE;
		pMiddle->s_pLinkQuestObj = m_vpChildQuest[ i ];
		pMiddle->s_Sprite.Init( NULL, CsPoint::ZERO, CsPoint( sizex, 2 ), 1, false, color );
		pMiddle->s_Sprite.SetRotation( fRot );
		m_vpMiddleTail.PushBack( pMiddle );
	}
}

void cQuestObj::CreateMiddle_JumpTail()
{
	m_eMiddleType = MT_JUMP;

	if( ( m_pChildTail == NULL )&&( m_pParentTail == NULL ) )
		return;	

	int nCount = m_vpChildQuest.Size();
	for( int i=0; i<nCount; ++i )
	{
		sMIDDLE_INFO* pMiddle = NiNew sMIDDLE_INFO;
		pMiddle->s_eType = MTE_JUMP_CHILD;
		pMiddle->s_pLinkQuestObj = m_vpChildQuest[ i ];
		pMiddle->s_Sprite.Init( NULL, CsPoint::ZERO, CsPoint( 20, 20 ), "ToolData\\QuestTool2\\MT_Jump.tga", 1, false );

		if( i<5 )
			pMiddle->s_ptDelta = CsPoint( IF_QUESTNODE_X + 8, IF_QUESTNODE_Y - 20 + 20*i );
		else
			pMiddle->s_ptDelta = CsPoint( IF_QUESTNODE_X + 28, IF_QUESTNODE_Y - 20 + 20*(i-5) );

		m_vpMiddleTail.PushBack( pMiddle );
	}

	nCount = m_vpParentQuest.Size();
	for( int i=0; i<nCount; ++i )
	{
		sMIDDLE_INFO* pMiddle = NiNew sMIDDLE_INFO;
		pMiddle->s_eType = MTE_JUMP_PARENT;
		pMiddle->s_pLinkQuestObj = m_vpParentQuest[ i ];
		pMiddle->s_Sprite.Init( NULL, CsPoint::ZERO, CsPoint( 20, 20 ), "ToolData\\QuestTool2\\MT_Jump.tga", 1, false );

		if( i<5 )
			pMiddle->s_ptDelta = CsPoint( -28, IF_QUESTNODE_Y - 20 + 20*i );
		else
			pMiddle->s_ptDelta = CsPoint( -48, IF_QUESTNODE_Y - 20 + 20*(i-5) );

		m_vpMiddleTail.PushBack( pMiddle );
	}
}

void cQuestObj::CreateMiddle_LineJumpTail()
{
	m_eMiddleType = MT_LINEJUMP;

	if( ( m_pChildTail == NULL )&&( m_pParentTail == NULL ) )
		return;	

	int nCount = m_vpChildQuest.Size();
	CsPoint ptStart = GetChildTailPos();
	NiPoint2 vStart = NiPoint2( (float)ptStart.x, (float)ptStart.y );
	for( int i=0; i<nCount; ++i )
	{
		if( m_vpChildQuest[ i ]->GetNodeIndex() == GetNodeIndex() )
		{
			// == 가운데 꼬리생성
			CsPoint ptEnd = m_vpChildQuest[ i ]->GetParentTailPos();
			NiPoint2 vEnd = NiPoint2( (float)ptEnd.x, (float)ptEnd.y );
			NiPoint2 vDir = vEnd - vStart;
			// 사이즈
			int sizex = (int)( vDir.Unitize() + 1 );

			float fRot = NiACos( NiPoint2( 1, 0 ).Dot( vDir ) );
			if( vEnd.y < vStart.y )
				fRot = -fRot;


			sMIDDLE_INFO* pMiddle = NiNew sMIDDLE_INFO;
			pMiddle->s_eType = MTE_LINE;
			pMiddle->s_pLinkQuestObj = m_vpChildQuest[ i ];
			pMiddle->s_Sprite.Init( NULL, CsPoint::ZERO, CsPoint( sizex, 2 ), 1, false, NiColor( 1, 0, 0 ) );
			pMiddle->s_Sprite.SetRotation( fRot );			
			m_vpMiddleTail.PushBack( pMiddle );
		}
	}

	int nIndex = m_vpMiddleTail.Size() == 0 ? 0 : 1;
	for( int i=0; i<nCount; ++i )
	{
		if( m_vpChildQuest[ i ]->GetNodeIndex() != GetNodeIndex() )
		{
			sMIDDLE_INFO* pMiddle = NiNew sMIDDLE_INFO;
			pMiddle->s_eType = MTE_JUMP_CHILD;
			pMiddle->s_pLinkQuestObj = m_vpChildQuest[ i ];
			pMiddle->s_Sprite.Init( NULL, CsPoint::ZERO, CsPoint( 20, 20 ), "ToolData\\QuestTool2\\MT_Jump.tga", 1, false );

			if( nIndex<5 )
				pMiddle->s_ptDelta = CsPoint( IF_QUESTNODE_X + 8, IF_QUESTNODE_Y - 20 + 20*nIndex );
			else
				pMiddle->s_ptDelta = CsPoint( IF_QUESTNODE_X + 28, IF_QUESTNODE_Y - 20 + 20*(nIndex-5) );

			m_vpMiddleTail.PushBack( pMiddle );

			++nIndex;
		}
	}

	nIndex = 0;
	nCount = m_vpParentQuest.Size();
	for( int i=0; i<nCount; ++i )
	{
		if( m_vpParentQuest[ i ]->GetNodeIndex() == GetNodeIndex() )
		{
			nIndex = 1;
			break;
		}
	}

	for( int i=0; i<nCount; ++i )
	{
		if( m_vpParentQuest[ i ]->GetNodeIndex() != GetNodeIndex() )
		{
			sMIDDLE_INFO* pMiddle = NiNew sMIDDLE_INFO;
			pMiddle->s_eType = MTE_JUMP_PARENT;
			pMiddle->s_pLinkQuestObj = m_vpParentQuest[ i ];
			pMiddle->s_Sprite.Init( NULL, CsPoint::ZERO, CsPoint( 20, 20 ), "ToolData\\QuestTool2\\MT_Jump.tga", 1, false );
			if( nIndex<5 )
				pMiddle->s_ptDelta = CsPoint( -28, IF_QUESTNODE_Y - 20 + 20*nIndex );
			else
				pMiddle->s_ptDelta = CsPoint( -48, IF_QUESTNODE_Y - 20 + 20*(nIndex-5) );

			m_vpMiddleTail.PushBack( pMiddle );

			++nIndex;
		}
	}
}

CsPoint cQuestObj::GetChildTailPos()
{
	assert_cs( m_ptNodeIndex.x >= 0 );
	assert_cs( m_ptNodeIndex.y >= 0 );

	CsPoint pos = g_pViewer->_GetNodeMng()->GetNode( m_ptNodeIndex )->GetNodePos() + GetSpritePos();	
	return pos + CsPoint( IF_QUESTNODE_X + IF_TAIL_LEN, IF_QUESTNODE_Y - 10 );
}

CsPoint cQuestObj::GetParentTailPos()
{
	assert_cs( m_ptNodeIndex.x >= 0 );
	assert_cs( m_ptNodeIndex.y >= 0 );

	CsPoint pos = g_pViewer->_GetNodeMng()->GetNode( m_ptNodeIndex )->GetNodePos() + GetSpritePos();	
	return pos + CsPoint( -IF_TAIL_LEN, IF_QUESTNODE_Y - 10 );
}

//==================================================================================================================================
//
//		입력
//
//==================================================================================================================================
bool cQuestObj::OnLBtnDown( CsPoint ptLocalClick )
{
	if( IsInObject( ptLocalClick) == false )
		return false;

	CsPoint ptSpriteLocal = ptLocalClick - GetSpritePos();

	sMIDDLE_INFO* mi;
	int nCount = m_vpMiddleTail.Size();
	for( int i=0; i<nCount; ++i )
	{
		mi = m_vpMiddleTail[ i ];
		switch( mi->s_eType )
		{
		case MTE_LINE:
			break;
		case MTE_JUMP_PARENT:
		case MTE_JUMP_CHILD:
			{
				if( mi->IsClick( ptSpriteLocal ) )
				{
					CsPoint pt = g_pViewer->_GetNodeMng()->GetNode( mi->s_pLinkQuestObj->GetNodeIndex() )->GetNodePos();
					pt += mi->s_pLinkQuestObj->GetSpritePos();
					pt.x -= ( g_ptScreen.x/2 - IF_QUESTNODE_X/2 );
					pt.y -= ( g_ptScreen.y/2 - IF_QUESTNODE_Y/2 );
					g_pViewer->_SetCurPos( pt );
					return true;
				}				
			}
			break;
		default:
			assert_cs( false );
		}		
	}

	// 영역 안쪽이 클릭
	g_pDiscript->_SetSelQuest( this );	
	return true;
}

bool cQuestObj::IsInObject( CsPoint ptLocalClick )
{
	CsPoint ptPitchLocal = ptLocalClick - GetPitchPos();
	if( ( ptPitchLocal.x < 0 )||( ptPitchLocal.x > PITCH_SIZE_X )||
		( ptPitchLocal.y < 0 )||( ptPitchLocal.y > PITCH_SIZE_Y ) )
	{
		return false;
	}
	return true;
}