

#include "stdafx.h"
#include "MiniMap.h"

#define IF_MINIMAP_SIZE_X			600
#define IF_MINIMAP_SIZE_Y			400

void cMinimap::Delete()
{
	SAFE_NIDELETE( m_pBG );
	SAFE_NIDELETE( m_pRegion );

	int nCount = m_vpMapBG.Size();
	for( int i=0; i<nCount; ++i )
	{
		SAFE_NIDELETE( m_vpMapBG[ i ] );
	}
	m_vpMapBG.Destroy();

	_DeleteQuestObj();
}

void cMinimap::_DeleteQuestObj()
{
	int nCount = m_vpQuestObj.Size();
	for( int i=0; i<nCount; ++i )
	{
		SAFE_NIDELETE( m_vpQuestObj[ i ] );
	}
	m_vpQuestObj.Destroy();
}

void cMinimap::Init()
{
	m_pBG = NiNew cSprite;
	m_pBG->Init( NULL, CsPoint::ZERO, CsPoint( IF_MINIMAP_SIZE_X, IF_MINIMAP_SIZE_Y ), 1, false );
	//m_pBG->Init( NULL, CsPoint::ZERO, CsPoint( IF_MINIMAP_SIZE_X, IF_MINIMAP_SIZE_Y ), "ToolData\\QuestTool2\\MiniMap.tga", 1, false );

	m_pRegion = NiNew cSprite;
	m_pRegion->Init( NULL, CsPoint::ZERO, CsPoint( 50, 50 ), 1, false, NiColor( 1, 0, 0 ) );
	m_pRegion->SetAlpha( 0.6f );
	//m_pRegion->Init( NULL, CsPoint::ZERO, CsPoint( 50, 50 ), "ToolData\\QuestTool2\\MinimapRegion.tga", 1, false );	
}

void cMinimap::Reset()
{
	if( m_vpMapBG.Size() == 0 )
	{
		int nMapBGCount = g_pViewer->_GetNodeMng()->GetNodeCount().y;
		for( int i=0; i<nMapBGCount; ++i )
		{
			cSprite* pMapBG = NiNew cSprite;
			pMapBG->Init( NULL, CsPoint::ZERO, CsPoint( IF_MINIMAP_SIZE_X - 4, 10 ), 1, false, i%2 == 0 ? NiColor( 0.3f, 0.3f, 0.3f ) : NiColor( 0.4f, 0.4f, 0.4f ) );
			m_vpMapBG.PushBack( pMapBG );
		}
	}
	assert_cs( g_pViewer->_GetNodeMng()->GetNodeCount().y == m_vpMapBG.Size() );

	// 필요 계산
	cNodeMng* pNodeMng = g_pViewer->_GetNodeMng();
	CsPoint ptNodeCount = pNodeMng->GetNodeCount();
	cNode* pLastNode = pNodeMng->GetNode( ptNodeCount - CsPoint( 1, 1 ) );
	CsPoint ptTotalSize = pLastNode->GetNodePos() + CsPoint( pLastNode->GetPitchCount().x*PITCH_SIZE_X, pLastNode->GetPitchCount().y*PITCH_SIZE_Y );

	// 맵영역 배경
	cNode* pNode;
	CsPoint ptMapBG( 0, 0 );
	int nCount = m_vpMapBG.Size();
	for( int i=0; i<nCount; ++i )
	{
		pNode = pNodeMng->GetNode( CsPoint( 0, i ) );		
		ptMapBG.y = (int)( pNode->GetNodePos().y / (float)ptTotalSize.y * ( IF_MINIMAP_SIZE_Y - 4 ) + 0.5f );
		int sy = (int)( (pNode->GetPitchCount().y*PITCH_SIZE_Y) / (float)ptTotalSize.y * ( IF_MINIMAP_SIZE_Y - 4 ) + 0.5f );
		m_vpMapBG[ i ]->SetPos( ptMapBG );
		m_vpMapBG[ i ]->SetSize( CsPoint( IF_MINIMAP_SIZE_X - 4, sy ) );
	}

	// 퀘스트 오브젝트
	_DeleteQuestObj();

	CsPoint ptQuestSize = _World_To_Minimap( ptTotalSize, CsPoint( IF_QUESTNODE_X, IF_QUESTNODE_Y ) );
	ptQuestSize.x = CsMax( 2, ptQuestSize.x );
	ptQuestSize.y = CsMax( 2, ptQuestSize.y );
	std::list< cQuestObj* >* pQuestList;
	std::list< cQuestObj* >::iterator itQuest;
	std::list< cQuestObj* >::iterator itQuestEnd;
	for( int y=0; y<ptNodeCount.y; ++y )
	{
		for( int x=0; x<ptNodeCount.x; ++x )
		{
			pNode = pNodeMng->GetNode( CsPoint( x, y ) );
			CsPoint ptNodePos = pNode->GetNodePos();
			pQuestList = pNode->GetQuestList();
			itQuest = pQuestList->begin();
			itQuestEnd = pQuestList->end();
			for( ; itQuest!=itQuestEnd; ++itQuest )
			{
				CsPoint ptToolPos = (*itQuest)->GetFT()->GetToolPos();
				ptToolPos.x *= PITCH_SIZE_X;
				ptToolPos.y *= PITCH_SIZE_Y;
				CsPoint ptQuest = _World_To_Minimap( ptTotalSize, ptNodePos + ptToolPos );

				cSprite* pQuestSprite = NiNew cSprite;

				switch( (*itQuest)->GetFT()->GetQuestType() )
				{
				case CsQuest::QT_SUB:
					pQuestSprite->Init( NULL, ptQuest, ptQuestSize, 1, false, NiColor( 1, 1, 0 ) );
					break;
				case CsQuest::QT_MAIN:
					pQuestSprite->Init( NULL, ptQuest, ptQuestSize, 1, false, NiColor( 1, 0, 1 ) );
					break;
				case CsQuest::QT_REPEAT:
					pQuestSprite->Init( NULL, ptQuest, ptQuestSize, 1, false, NiColor( 0, 1, 0 ) );
					break;
				case CsQuest::QT_DAILY:
					pQuestSprite->Init( NULL, ptQuest, ptQuestSize, 1, false, NiColor( 173/255.0f, 214/255.0f, 250/255.0f ) );
					break;
				case CsQuest::QT_EVENTREPEAT:
					pQuestSprite->Init( NULL, ptQuest, ptQuestSize, 1, false, NiColor( 246/255.0f, 134/255.0f, 31/255.0f ) );
					break;
				case CsQuest::QT_ACHIEVE:
					pQuestSprite->Init( NULL, ptQuest, ptQuestSize, 1, false, NiColor( 0/255.0f, 255/255.0f, 128/255.0f ) );
					break;
				case CsQuest::QT_JOINTPROGRESS:
					pQuestSprite->Init( NULL, ptQuest, ptQuestSize, 1, false, NiColor( 0, 1, 0 ) );
					break;
				case CsQuest::QT_TUTORIAL:
					pQuestSprite->Init( NULL, ptQuest, ptQuestSize, 1, false, NiColor( 200/255.0f, 110/255.0f, 60/255.0f ) );
					break;
					
				case CsQuest::QT_XANTI_JOINTPROGRESS:
					pQuestSprite->Init( NULL, ptQuest, ptQuestSize, 1, false, NiColor( 10/255.0f, 150/255.0f, 255/255.0f ) );
					break;

				default:
					assert_cs( false );
				}				
				m_vpQuestObj.PushBack( pQuestSprite );
			}
		}
	}
}

void cMinimap::Render()
{
	if( g_pTop->_IsViewMinimap() == false )
		return;

	// 메인 배경 랜더링
	CsPoint ptMinimap = CsPoint( g_ptScreen.x - IF_MINIMAP_SIZE_X, START_POS_Y );
	m_pBG->Render( ptMinimap );

	// 외곽 자리 패쓰
	ptMinimap += CsPoint( 2, 2 );

	// 맵영역 배경 랜더링
	int nCount = m_vpMapBG.Size();
	for( int i=0; i<nCount; ++i )
	{
		m_vpMapBG[ i ]->Render( ptMinimap );
	}

	// 퀘스트 랜더링
	nCount = m_vpQuestObj.Size();
	for( int i=0; i<nCount; ++i )
	{
		m_vpQuestObj[ i ]->Render( ptMinimap );
	}

	// 필요 계산
	cNodeMng* pNodeMng = g_pViewer->_GetNodeMng();
	cNode* pLastNode = pNodeMng->GetNode( pNodeMng->GetNodeCount() - CsPoint( 1, 1 ) );
	CsPoint ptTotalSize = pLastNode->GetNodePos() + CsPoint( pLastNode->GetPitchCount().x*PITCH_SIZE_X, pLastNode->GetPitchCount().y*PITCH_SIZE_Y );

	// 보이는 영역 랜더링
	CsPoint ptPos = _World_To_Minimap( ptTotalSize, g_pViewer->_GetCurPos() );
	CsPoint ptSize = _World_To_Minimap( ptTotalSize, g_ptScreen );
	m_pRegion->Render( ptMinimap + ptPos, ptSize );
}

void cMinimap::ResetDevice()
{
	m_pBG->ResetDevice();
	m_pRegion->ResetDevice();
	int nCount = m_vpMapBG.Size();
	for( int i=0; i<nCount; ++i )
	{
		m_vpMapBG[ i ]->ResetDevice();
	}
	nCount = m_vpQuestObj.Size();
	for( int i=0; i<nCount; ++i )
	{
		m_vpQuestObj[ i ]->ResetDevice();
	}
}

CsPoint cMinimap::_World_To_Minimap( CsPoint ptTotalSize, CsPoint ptWorld )
{
	CsPoint ptReturn;
	ptReturn.x = (int)( ptWorld.x / (float)ptTotalSize.x * ( IF_MINIMAP_SIZE_X - 4 ) + 0.5f );
	ptReturn.y = (int)( ptWorld.y / (float)ptTotalSize.y * ( IF_MINIMAP_SIZE_Y - 4 ) + 0.5f );
	return ptReturn;
}

CsPoint cMinimap::_Minimap_To_World( CsPoint ptMimimap )
{
	// 필요 계산
	cNodeMng* pNodeMng = g_pViewer->_GetNodeMng();
	cNode* pLastNode = pNodeMng->GetNode( pNodeMng->GetNodeCount() - CsPoint( 1, 1 ) );
	CsPoint ptTotalSize = pLastNode->GetNodePos() + CsPoint( pLastNode->GetPitchCount().x*PITCH_SIZE_X, pLastNode->GetPitchCount().y*PITCH_SIZE_Y );

	CsPoint ptReturn;
	ptReturn.x = (int)( ptMimimap.x / (float)( IF_MINIMAP_SIZE_X - 4 ) * ptTotalSize.x + 0.5f );
	ptReturn.y = (int)( ptMimimap.y / (float)( IF_MINIMAP_SIZE_Y - 4 ) * ptTotalSize.y + 0.5f );
	return ptReturn;
}


void cMinimap::OnLBtnPress( CPoint pt )
{
	if( g_pTop->_IsViewMinimap() == false )
		return;

	CsPoint ptMinimap = CsPoint( g_ptScreen.x - IF_MINIMAP_SIZE_X, START_POS_Y );
	CsPoint ptPos = _Minimap_To_World( CsPoint( pt.x - ptMinimap.x, pt.y - ptMinimap.y ) );
	ptPos.x -= g_ptScreen.x/2;
	ptPos.y -= g_ptScreen.y/2;
	g_pViewer->_SetCurPos( ptPos );
}

bool cMinimap::OnMouseDown( CPoint pt )
{
	if( g_pTop->_IsViewMinimap() == false )
		return false;

	CsPoint ptMinimap = CsPoint( g_ptScreen.x - IF_MINIMAP_SIZE_X, START_POS_Y );
	if( ( pt.x > ptMinimap.x )&&( pt.x < ptMinimap.x + IF_MINIMAP_SIZE_X )&&
		( pt.y > ptMinimap.y )&&( pt.y < ptMinimap.y + IF_MINIMAP_SIZE_Y )	)
	{
		return true;
	}
	return false;
}