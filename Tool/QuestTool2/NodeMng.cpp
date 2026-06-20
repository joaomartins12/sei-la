

#include "stdafx.h"
#include "NodeMng.h"

void cNodeMng::sMAP_NAME::Init( int nMapIndex )
{
	cText::sTEXTINFO ti;
	ti.Init( cFont::FS_16 );
	ti.s_bOutLine = false;

	if( nMapIndex < (int)nsCsMapTable::g_pMapListMng->GetListMap()->size() )
	{
		CsMapList* pFTMap = nsCsMapTable::g_pMapListMng->GetListIndex( nMapIndex );
		ti.SetText( pFTMap->GetInfo()->s_szMapDiscript.c_str() );
	}
	else
	{
		int nNewIndex = nMapIndex - (int)nsCsMapTable::g_pMapListMng->GetListMap()->size();
		switch( nNewIndex )
		{
		case EN_DIGIVICE:
			ti.SetText( L"디지바이스" );
			break;
		default:
			assert_cs( false );
		}
	}
	s_Text.Init( NULL, CsPoint::ZERO, &ti, 1, false );
	s_BG.Init( NULL, CsPoint::ZERO, s_Text.GetStringSize() + CsPoint( 10, 4 ), "ToolData\\QuestTool2\\MapNameBG.tga", 1, false );
}

void cNodeMng::sMAP_NAME::Render( CsPoint pos, float fAlpha )
{
	s_BG.SetAlpha( fAlpha );
	s_BG.Render( pos + CsPoint( 1, 1 ) );
	s_Text.SetAlpha( fAlpha );
	s_Text.Render( pos + CsPoint( 6, 3 ) );
}

cNodeMng::cNodeMng()
{
	m_ppNodeArray = NULL;
	m_ptNodeCount = CsPoint::ZERO;

	m_pMapBGArray = NULL;
	m_pMapTextArray = NULL;
	m_pLevelTextArray = NULL;
	m_ppNodeArray = NULL;
	m_pPopup = NULL;
}


void cNodeMng::Delete()
{
	m_LevelBGBar.Delete();
	m_MapBGBar.Delete();
	SAFE_NIDELETE_ARRAY( m_pMapBGArray );
	SAFE_NIDELETE_ARRAY( m_pMapTextArray );
	SAFE_NIDELETE_ARRAY( m_pLevelTextArray );

	for( int y=0; y<m_ptNodeCount.y; ++y )
	{		
		for( int x=0; x<m_ptNodeCount.x; ++x )
		{
			m_ppNodeArray[ y ][ x ].Delete();
		}
		SAFE_DELETE_ARRAY( m_ppNodeArray[ y ] );		
	}
	SAFE_DELETE_ARRAY( m_ppNodeArray );

	SAFE_NIDELETE( m_pPopup );
}

void cNodeMng::Init()
{
	m_pPopup = NiNew cPopup;

	// 노드 포인터 할당
	m_ptNodeCount.x = MAX_LEVEL;
	m_ptNodeCount.y = (int)nsCsMapTable::g_pMapListMng->GetListMap()->size() + EN_COUNT;		// 1 추가는 디지바이스용

	CsPoint ptNodeIndex = CsPoint::ZERO;
	m_ppNodeArray = csnew cNode*[ m_ptNodeCount.y ];
	for( int y=0; y<m_ptNodeCount.y; ++y )
	{
		m_ppNodeArray[ y ] = csnew cNode[ m_ptNodeCount.x ];
		for( int x=0; x<m_ptNodeCount.x; ++x )
		{
			if( ( x == 0 )&&( y == 0 ) )
			{
				m_ppNodeArray[ y ][ x ].Init( NULL, NULL, ptNodeIndex );
			}
			else if( x == 0 )
			{
				m_ppNodeArray[ y ][ x ].Init( NULL, &m_ppNodeArray[ y - 1 ][ x ], ptNodeIndex );
			}
			else if( y == 0 )
			{
				m_ppNodeArray[ y ][ x ].Init( &m_ppNodeArray[ y ][ x - 1 ], NULL, ptNodeIndex );
			}
			else
			{
				m_ppNodeArray[ y ][ x ].Init( &m_ppNodeArray[ y ][ x - 1 ], &m_ppNodeArray[ y - 1 ][ x ], ptNodeIndex );
			}
			++ptNodeIndex.x;
		}

		ptNodeIndex.x = 0;
		++ptNodeIndex.y;
	}

	cText::sTEXTINFO ti;
	ti.Init( cFont::FS_16 );
	ti.s_bOutLine = false;

	// 레벨 백그라운드 바
	m_LevelBGBar.Init( NULL, CsPoint::ZERO, CsPoint( MAX_SIZE, START_POS_Y ), 1, true, NiColor::BLACK );

	// 레벨 텍스트	
	TCHAR sz[ 32 ];
	m_pLevelTextArray = NiNew cText[ m_ptNodeCount.x ];
	for( int i=0; i<m_ptNodeCount.x; ++i )
	{
		_stprintf_s( sz, 32, L"%d Level", i+1 );
		ti.SetText( sz );
		m_pLevelTextArray[ i ].Init( NULL, CsPoint::ZERO, &ti, 1.0f, false );
	}

	// 맵 백그라운드 바
	m_MapBGBar.Init( NULL, CsPoint::ZERO, CsPoint( START_POS_X, MAX_SIZE ), 1, true, NiColor::BLACK );

	// 맵 영역 백그라운드 바
	m_pMapBGArray = NiNew cSprite[ m_ptNodeCount.y ];
	for( int i=0; i<m_ptNodeCount.y; ++i )
	{
		m_pMapBGArray[ i ].Init( NULL, CsPoint::ZERO, CsPoint( MAX_SIZE, 50 ), 1, false, i%2 == 0 ? NiColor( 0.3f, 0.3f, 0.3f ) : NiColor( 0.4f, 0.4f, 0.4f ) );
	}

	// 맵 텍스트
	m_pMapTextArray = NiNew sMAP_NAME[ m_ptNodeCount.y ];
	for( int i=0; i<m_ptNodeCount.y; ++i )
	{
		m_pMapTextArray[ i ].Init( i );
	}
}

void cNodeMng::Update()
{
	for( int y=0; y<m_ptNodeCount.y; ++y )
	{		
		for( int x=0; x<m_ptNodeCount.x; ++x )
		{
			m_ppNodeArray[ y ][ x ].Update();
		}
	}
}

void cNodeMng::UpdateCursor( CsPoint ptCursor )
{
	FindNode( ptCursor )->UpdateCursor( ptCursor );
}

void cNodeMng::Render( CsPoint ptViewerPos )
{
	CsPoint pos( 0, 0 );
	CsPoint size( MAX_SIZE, 0 );
	// 맵 백 영역 랜더링
	for( int i=0; i<m_ptNodeCount.y; ++i )
	{
		pos.y = m_ppNodeArray[ i ][ 0 ].GetNodePos().y - ptViewerPos.y;
		size.y = m_ppNodeArray[ i ][ 0 ].GetPitchCount().y * PITCH_SIZE_Y;
		m_pMapBGArray[ i ].Render( pos, size );
	}

	// 꼬리들 랜더링
	for( int y=0; y<m_ptNodeCount.y; ++y )
	{		
		for( int x=0; x<m_ptNodeCount.x; ++x )
		{
			m_ppNodeArray[ y ][ x ].RenderTail( ptViewerPos );
		}
	}

	// 노드 랜더링
	for( int y=0; y<m_ptNodeCount.y; ++y )
	{		
		for( int x=0; x<m_ptNodeCount.x; ++x )
		{
			m_ppNodeArray[ y ][ x ].Render( ptViewerPos );
		}
	}

	m_pPopup->Render();

	// 맵이름
	pos.x = 0;
	for( int i=0; i<m_ptNodeCount.y; ++i )
	{
		bool bAlpha = false;
		pos.y = m_ppNodeArray[ i ][ 0 ].GetNodePos().y - ptViewerPos.y;
		if( pos.y < START_POS_Y )
		{
			if( i < m_ptNodeCount.y - 1 )
			{
				int nNextY = m_ppNodeArray[ i + 1 ][ 0 ].GetNodePos().y - ptViewerPos.y;
				if( nNextY > 100 )
				{
					pos.y = START_POS_Y;
					bAlpha = true;
				}
			}
			else
			{
				pos.y = START_POS_Y;
				bAlpha = true;
			}
		}
		m_pMapTextArray[ i ].Render( pos, bAlpha == false ? 1.0f : 0.6f );
	}

	// 레벨 바
	m_LevelBGBar.Render();
	// 레벨
	pos.y = 0;
	for( int i=0; i<m_ptNodeCount.x; ++i )
	{
		bool bAlpha = false;
		pos.x = m_ppNodeArray[ 0 ][ i ].GetNodePos().x - ptViewerPos.x;
		if( pos.x < 0 )
		{
			if( i < m_ptNodeCount.x - 1 )
			{
				int nNextX = m_ppNodeArray[ 0 ][ i + 1 ].GetNodePos().x - ptViewerPos.x;
				if( nNextX > 100 )
				{
					bAlpha = true;
					pos.x = 0;
				}
			}
			else
			{
				bAlpha = true;
				pos.x = 0;
			}
		}
		m_pLevelTextArray[ i ].SetAlpha( bAlpha == false ? 1.0f : 0.6f );
		m_pLevelTextArray[ i ].Render( pos );
	}
}

void cNodeMng::ResetDevice()
{
	m_LevelBGBar.ResetDevice();

	for( int x=0; x<m_ptNodeCount.x; ++x )
	{
		m_pLevelTextArray[ x ].ResetDevice();
	}
	for( int y=0; y<m_ptNodeCount.y; ++y )
	{
		m_pMapBGArray[ y ].ResetDevice();
		m_pMapTextArray[ y ].ResetDevice();
	}

	for( int y=0; y<m_ptNodeCount.y; ++y )
	{		
		for( int x=0; x<m_ptNodeCount.x; ++x )
		{
			m_ppNodeArray[ y ][ x ].ResetDevice();
		}
	}
}

bool cNodeMng::ResistQuestObj( cQuestObj* pQuestObj, bool bResetSize )
{
	// 들어가야할 x축과 y축을 찾는다.
	CsQuest* pFT = pQuestObj->GetFT();
	CsPoint ptNodeIndex;
	ptNodeIndex.x = pFT->GetLevel() - 1;

	if(0 > ptNodeIndex.x)
		CsMessageBox( MB_OK, L"퀘스트 번호%d  레벨값이 %d입니다. ", pFT->GetUniqID(), pFT->GetLevel() );


	// 엔피씨한테 받는경우
	switch( pFT->GetStartTarget_Type() )
	{
	case CsQuest::ST_NPC:
		{
			if( nsCsFileTable::g_pNpcMng->IsNpc( pFT->GetStartTarget_ID() ) == false )
			{
				// 해당 NPC 가 존재 하지 않는다면 리턴
				return false;
			}
			CsNpc::sINFO* pFTNpc = nsCsFileTable::g_pNpcMng->GetNpc( pFT->GetStartTarget_ID() )->GetInfo();
			ptNodeIndex.y = _MapID_To_Index( pFTNpc->s_dwMapID );
		}
		break;
	case CsQuest::ST_DIGIVICE:
		{
			ptNodeIndex.y = (int)nsCsMapTable::g_pMapListMng->GetListMap()->size() + cNodeMng::EN_DIGIVICE;
		}
		break;
	default:
		assert_cs( false );
	}

	assert_cs( ptNodeIndex.x < m_ptNodeCount.x );
	assert_cs( ptNodeIndex.y < m_ptNodeCount.y );

	// 노드를 구하자
	cNode* pNode = GetNode( ptNodeIndex );
	pNode->ResistQuestObj( pQuestObj );

	if( bResetSize == true )
		ResetSize();

	return true;
}

void cNodeMng::ReleaseQuestObj( cQuestObj* pQuestObj, bool bResetSize )
{
	GetNode( pQuestObj->GetNodeIndex() )->ReleaseQuestObj( pQuestObj );

	if( bResetSize == true )
		ResetSize();
}

void cNodeMng::ResetSize_AllNode()
{
	for( int y=0; y<m_ptNodeCount.y; ++y )
	{
		for( int x=0; x<m_ptNodeCount.x; ++x )
		{
			m_ppNodeArray[ y ][ x ].ResetNodeSize();
		}
	}
	ResetSize();
}

void cNodeMng::ResetSize()
{
	for( int x=0; x<m_ptNodeCount.x; ++x )
	{
		int nMax = 0;
		for( int y=0; y<m_ptNodeCount.y; ++y )
		{
			int p = CsMax( 1, m_ppNodeArray[ y ][ x ].GetPitchCount().x );
			if( nMax < p )
				nMax = p;
		}
		for( int y=0; y<m_ptNodeCount.y; ++y )
		{
			m_ppNodeArray[ y ][ x ].SetPitchCount_X( nMax );
		}
	}

	for( int y=0; y<m_ptNodeCount.y; ++y )
	{
		int nMax = 0;
		for( int x=0; x<m_ptNodeCount.x; ++x )
		{
			int p = CsMax( 1, m_ppNodeArray[ y ][ x ].GetPitchCount().y );
			if( nMax < p )
				nMax = p;
		}
		for( int x=0; x<m_ptNodeCount.x; ++x )
		{
			m_ppNodeArray[ y ][ x ].SetPitchCount_Y( nMax );
		}
	}

	Update();

	g_pViewer->_GetMinimap()->Reset();
}

cNode* cNodeMng::FindNode( CsPoint ptWorld )
{
	CsPoint ptNodeIndex = m_ptNodeCount - CsPoint( 1, 1 );
	for( int x=0; x<m_ptNodeCount.x; ++x )
	{
		if( m_ppNodeArray[ 0 ][ x ].GetNodePos().x > ptWorld.x )
		{
			ptNodeIndex.x = CsMax( 0, x - 1 );
			break;
		}
	}

	for( int y=0; y<m_ptNodeCount.y; ++y )
	{
		if( m_ppNodeArray[ y ][ 0 ].GetNodePos().y > ptWorld.y )
		{
			ptNodeIndex.y = CsMax( 0, y - 1 );
			break;
		}
	}
	return GetNode( ptNodeIndex );
}

int cNodeMng::_MapID_To_Index( DWORD dwMapID )
{
	int nIndex = 0;
	CsMapList::MAP* pMap = nsCsMapTable::g_pMapListMng->GetListMap();
	CsMapList::MAP_IT it = pMap->begin();
	CsMapList::MAP_IT itEnd = pMap->end();
	for( ; it!=itEnd; ++it )
	{
		if( it->second->GetInfo()->s_dwMapID == dwMapID )
		{
			return nIndex;
		}
		++nIndex;
	}
	assert_cs( false );
	return 0;
}


void cNodeMng::OnLBtnDown( CsPoint ptWorldClick )
{
	FindNode( ptWorldClick )->OnLBtnDown( ptWorldClick );
}

bool cNodeMng::IsInObject( CsPoint ptWorldClick )
{
	return FindNode( ptWorldClick )->IsInObject( ptWorldClick );
}