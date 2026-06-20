
#include "stdafx.h"
#include "QuestMng.h"


void cQuestMng::Delete()
{
	std::map< DWORD, cQuestObj* >::iterator it = m_mapQuest.begin();
	std::map< DWORD, cQuestObj* >::iterator itEnd = m_mapQuest.end();
	for( ; it!=itEnd; ++it )
	{
		SAFE_NIDELETE( it->second );
	}
	m_mapQuest.clear();
}

void cQuestMng::Init()
{
	m_eMiddleType = cQuestObj::MT_NONE;

	_ResistQuestMap( nsCsFileTable::g_pQuestMng->GetQuestMap() );

	_CheckQuestMap();

	// 사이즈 전부 재측정
	g_pViewer->_GetNodeMng()->ResetSize();
}

void cQuestMng::CreateMiddleType( cQuestObj::eMIDDLETAIL_TYPE eType )
{
	if( m_eMiddleType == eType )
		return;
	m_eMiddleType = eType;

	// 가운데 꼬리들 전부 생성
	std::map< DWORD, cQuestObj* >::iterator it = m_mapQuest.begin();
	std::map< DWORD, cQuestObj* >::iterator itEnd = m_mapQuest.end();
	for( ; it!=itEnd; ++it )
	{
		it->second->DeleteMiddle_Tail();

		switch( m_eMiddleType )
		{
		case cQuestObj::MT_LINE:
			it->second->CreateMiddle_LineTail();
			break;
		case cQuestObj::MT_JUMP:
			it->second->CreateMiddle_JumpTail();
			break;
		case cQuestObj::MT_LINEJUMP:
			it->second->CreateMiddle_LineJumpTail();
			break;
		default:
			assert_cs( false );
		}
	}
}

void cQuestMng::_ResistQuestMap( CsQuest::MAP* pMap )
{
	SAFE_POINTER_RET( pMap );

	bool bResist = false;
	CsQuest::MAP WaitMap;

	CsQuestPreRequireGroup* pPreReqGroup;
	CsQuestPreRequire* pPreReq;	
	CsQuest::MAP_IT it = pMap->begin();
	CsQuest::MAP_IT itEnd = pMap->end();
	for( ; it!=itEnd; ++it )
	{
		// 선행 요구사항
		bool bWait = false;

		pPreReqGroup = it->second->GetPreRequireGroup();
		int nReqCount = pPreReqGroup->GetListCount();
		for( int nReq = 0; nReq < nReqCount; ++nReq )
		{
			pPreReq = pPreReqGroup->Get( nReq );
			switch( pPreReq->GetType() )
			{
				// 퀘스트 선행이라면 현재 선행 퀘스트가 존재 하는지 살펴 본다
			case CsQuestPreRequire::QUEST:
				{
					// 선행퀘스트가 존재하지 않는다면 기다림 맵에 넣자
					if( m_mapQuest.find( pPreReq->GetTypeID() ) == m_mapQuest.end() )
						bWait = true;
				}
				break;
			case CsQuestPreRequire::T_LEVEL:
			case CsQuestPreRequire::ITEM:
			case CsQuestPreRequire::D_LEVEL:
			case CsQuestPreRequire::D_TYPE:
			case CsQuestPreRequire::REGION:
			case CsQuestPreRequire::D_EVO:
			case CsQuestPreRequire::D_EVO_DISABLE:
			case CsQuestPreRequire::D_BASE_TABLE_IDX:
			case CsQuestPreRequire::TAMER_TABLE_IDX:
			case CsQuestPreRequire::QUEST_ID_CANNOT_COMPLETED:
			case CsQuestPreRequire::QUEST_ID_CANNOT_PROGRESS:
				break;
			default:
				assert_cs( false );
			}
		}

		if( bWait == true )
		{
			WaitMap[ it->second->GetUniqID() ] = it->second;
		}
		else
		{
			cQuestObj* pObj = NiNew cQuestObj;
			pObj->Init( it->second );

			// 레지스트 성공
			if( g_pViewer->_GetNodeMng()->ResistQuestObj( pObj, false ) == true )
			{
				m_mapQuest[ it->second->GetUniqID() ] = pObj;
				bResist = true;
			}
			// 레지스트 실패
			else
			{
				SAFE_NIDELETE( pObj );
			}
		}
	}

	// 더이상 등록 할게 존재 하지 않는다.
	if( bResist == false )
	{
		CsQuest::MAP_IT itWait = WaitMap.begin();
		CsQuest::MAP_IT itWaitEnd = WaitMap.end();
		for( ; itWait!=itWaitEnd; ++itWait )
		{
			// 선행 조건 퀘스트 존재 안함. 선행조건 없애 버리고 등록 시키자			
			CsMessageBox( MB_OK, L"[ %s : %d ] 퀘스트의 선행조건 퀘스트가 존재 하지 않습니다, 선행조건을 제거 하겠습니다", itWait->second->m_szTitleText, itWait->second->GetUniqID() );
			pPreReqGroup = itWait->second->GetPreRequireGroup();
			int nReqCount = pPreReqGroup->GetListCount();
			for( int nReq = 0; nReq < nReqCount; ++nReq )
			{
				pPreReq = pPreReqGroup->Get( nReq );
				switch( pPreReq->GetType() )
				{
					// 퀘스트 선행이라면 현재 선행 퀘스트가 존재 하는지 살펴 본다
				case CsQuestPreRequire::QUEST:
				case CsQuestPreRequire::QUEST_ID_CANNOT_COMPLETED:		// 완료된 퀘스트가 있을시 진행 불가
				case CsQuestPreRequire::QUEST_ID_CANNOT_PROGRESS:			// 진행 중인 퀘스트가 있을시 진행 불가
					{
						// 선행조건 존재 하지 않는다면 없애 버리자
						if( m_mapQuest.find( pPreReq->GetTypeID() ) == m_mapQuest.end() )
						{
							pPreReqGroup->Delete( nReq );
							_ResistQuestMap( &WaitMap );
							return;
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
					break;
				default:
					assert_cs( false );
				}
			}
		}
	}
	// 선행 조건 대기 맵이 존재 한다
	else if( WaitMap.empty() == false )
	{
		_ResistQuestMap( &WaitMap );
	}
}

void cQuestMng::_CheckQuestMap()
{
	CsQuest::MAP* pFTQuestMap = nsCsFileTable::g_pQuestMng->GetQuestMap();

	std::map< DWORD, cQuestObj* >::iterator it = m_mapQuest.begin();
	for( ; it != m_mapQuest.end(); ++it )
	{
		cQuestObj* pQuest = it->second;
		SAFE_POINTER_CON( pQuest );
		
		CsQuest* pFTQuest = pQuest->GetFT();
		SAFE_POINTER_CON( pFTQuest );

		CsQuestPreRequireGroup* pPreReqGroup = pFTQuest->GetPreRequireGroup();
		SAFE_POINTER_CON( pPreReqGroup );

		CsQuestPreRequire::LIST* pPreReqList = pPreReqGroup->GetList();
		CsQuestPreRequire::LIST_IT itReq = pPreReqList->begin();
		for( ; itReq != pPreReqList->end(); )
		{
			if( !(*itReq) )
			{
				++itReq;
				continue;
			}

			switch( (*itReq)->GetType() )
			{
				// 퀘스트 선행이라면 현재 선행 퀘스트가 존재 하는지 살펴 본다
			case CsQuestPreRequire::QUEST:
			case CsQuestPreRequire::QUEST_ID_CANNOT_COMPLETED:		// 완료된 퀘스트가 있을시 진행 불가
			case CsQuestPreRequire::QUEST_ID_CANNOT_PROGRESS:		// 진행 중인 퀘스트가 있을시 진행 불가
				{
					// 선행조건 존재 하지 않는다면 없애 버리자
					CsQuest::MAP_IT itFind = pFTQuestMap->find( (*itReq)->GetTypeID() );
					if( itFind == pFTQuestMap->end() )
					{
						CsMessageBox( MB_OK, L"[ %s : %d ] 퀘스트의 선행조건 퀘스트가 존재하지 않습니다. 선행조건을 제거하겠습니다",
							pFTQuest->m_szTitleText, pFTQuest->GetUniqID() );
						SAFE_DELETE( *itReq );
						itReq = pPreReqList->erase( itReq );
						continue;
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
				break;
			default:
				assert_cs( false );
			}
			++itReq;
		}
	}
}

cQuestObj* cQuestMng::AddQuest( cQuestObj* pObject )
{
	if( g_pViewer->_GetNodeMng()->ResistQuestObj( pObject, true ) == false )
	{
		SAFE_NIDELETE( pObject );
		return NULL;
	}

	m_mapQuest[ pObject->GetFT()->GetUniqID() ] = pObject;
	return pObject;
}

cQuestObj* cQuestMng::AddQuest( CsQuest* pFTQuest )
{
	cQuestObj* pObj = NiNew cQuestObj;
	pObj->Init( pFTQuest );

	return AddQuest( pObj );
}

void cQuestMng::DeleteQuestObj( cQuestObj* pObject )
{
	m_mapQuest.erase( pObject->GetFT()->GetUniqID() );
	SAFE_DELETE( pObject );
}

cQuestObj* cQuestMng::FindNode( CsQuest* pFTQuest )
{
	return FindNode( pFTQuest->GetUniqID() );
}

cQuestObj* cQuestMng::FindNode( DWORD dwQuestID )
{
	if( m_mapQuest.find( dwQuestID ) != m_mapQuest.end() )
		return m_mapQuest[ dwQuestID ];

	return NULL;
}