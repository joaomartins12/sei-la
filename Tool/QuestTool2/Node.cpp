
#include "stdafx.h"
#include "Node.h"

void cNode::Delete()
{
	m_listQuest.clear();
}

void cNode::Init( cNode* pPrevNode_X, cNode* pPrevNode_Y, CsPoint ptNodeIndex )
{
	m_pPrevNode_X = pPrevNode_X;
	m_pPrevNode_Y = pPrevNode_Y;
	m_ptNodeIndex = ptNodeIndex;

	m_ptNodePos = CsPoint::ZERO;
	m_ptPitchCount = CsPoint::ZERO;

	_UpdateNodePos();
}

void cNode::Update()
{
	_UpdateNodePos();
}

void cNode::UpdateCursor( CsPoint ptCursor )
{
	CsPoint ptLocal = ptCursor - GetNodePos();
	std::list< cQuestObj* >::iterator it = m_listQuest.begin();
	std::list< cQuestObj* >::iterator itEnd = m_listQuest.end();
	for( ; it!=itEnd; ++it )
	{
		if( (*it)->UpdateCursor( ptLocal ) == true )
			return;
	}
}

void cNode::_UpdateNodePos()
{
	//==============================================================================================================
	// 위치를 구한다
	//==============================================================================================================
	// x 축
	if( m_pPrevNode_X == NULL )
	{
		m_ptNodePos.x = START_POS_X;
	}
	else
	{		
		m_ptNodePos.x = m_pPrevNode_X->GetNodePos().x + m_pPrevNode_X->GetPitchCount().x * PITCH_SIZE_X;
	}

	// y 축
	if( m_pPrevNode_Y == NULL )
	{
		m_ptNodePos.y = START_POS_Y;
	}
	else
	{
		m_ptNodePos.y = m_pPrevNode_Y->GetNodePos().y + m_pPrevNode_Y->GetPitchCount().y * PITCH_SIZE_Y;
	}
}

void cNode::Render( CsPoint ptViewerPos )
{
	CsPoint ptCurNodePos = m_ptNodePos - ptViewerPos;

	std::list< cQuestObj* >::iterator it = m_listQuest.begin();
	std::list< cQuestObj* >::iterator itEnd = m_listQuest.end();
	for( ; it!=itEnd; ++it )
	{
		(*it)->Render( ptCurNodePos );
	}
}

void cNode::RenderTail( CsPoint ptViewerPos )
{
	CsPoint ptCurNodePos = m_ptNodePos - ptViewerPos;

	std::list< cQuestObj* >::iterator it = m_listQuest.begin();
	std::list< cQuestObj* >::iterator itEnd = m_listQuest.end();
	for( ; it!=itEnd; ++it )
	{
		(*it)->RenderTail( ptCurNodePos );
	}
}

void cNode::ResetDevice()
{
	std::list< cQuestObj* >::iterator it = m_listQuest.begin();
	std::list< cQuestObj* >::iterator itEnd = m_listQuest.end();
	for( ; it!=itEnd; ++it )
	{
		(*it)->ResetDevice();
	}
}

void cNode::ResetNodeSize()
{
	CsPoint ptMax( 0, 0 );
	CsPoint ptDest;

	cQuestObj* pQuestObj;
	std::list< cQuestObj* >::iterator it = m_listQuest.begin();
	std::list< cQuestObj* >::iterator itEnd = m_listQuest.end();
	for( ; it!=itEnd; ++it )
	{
		pQuestObj = *it;
		ptDest = pQuestObj->GetFT()->GetToolPos() + CsPoint( 1, 1 );
		if( ptMax.x < ptDest.x )
			ptMax.x = ptDest.x;
		if( ptMax.y < ptDest.y )
			ptMax.y = ptDest.y;
	}
	m_ptPitchCount = ptMax;
}

bool cNode::ResistQuestObj( cQuestObj* pQuestObj )
{
	pQuestObj->SetNodeIndex( m_ptNodeIndex );
	m_listQuest.push_back( pQuestObj );

	// 위치 설정 - 만약 위치 설정이 안되어 있다면
	if( pQuestObj->GetFT()->GetToolPos() == CsPoint( -1, -1 ) )
	{
		// 위치 설정 하자
		_AutoPosition( pQuestObj );		
	}

	// 노드 사이즈 재계산
	ResetNodeSize();

	return true;
}

void cNode::ReleaseQuestObj( cQuestObj* pQuestObj )
{
	std::list< cQuestObj* >::iterator it = m_listQuest.begin();
	std::list< cQuestObj* >::iterator itEnd = m_listQuest.end();
	for( ; it!=itEnd; ++it )
	{
		if( (*it) == pQuestObj )
		{
			m_listQuest.erase( it );
			break;
		}
	}

	// 노드 사이즈 재계산
	ResetNodeSize();
}

bool cNode::_AutoPosition( cQuestObj* pQuestObj )
{
	int nParentCount = pQuestObj->GetParentCount();

	// 선행 퀘스트가 존재 하는가
	if( nParentCount != 0 )
	{		
		CsPoint ptNewPos( 0, 0 );

		cQuestObj* pSameNodeParent = NULL;
		// 자동 위치의 x좌표는 선행퀘스트보다 뒤에 존재해야 한다. 다른 맵에 있는 선행퀘스트 포함
		for( int i=0; i<nParentCount; ++i )
		{
			cQuestObj* pParent = pQuestObj->GetParent( i );
			if( ptNewPos.x < pParent->GetFT()->GetToolPos().x + 1 )
				ptNewPos.x = pParent->GetFT()->GetToolPos().x + 1;

			// 같은 노드에 존재 하는지 체크
			if( pSameNodeParent == NULL )
			{
				if( pParent->GetNodeIndex() == pQuestObj->GetNodeIndex() )
				{
					pSameNodeParent = pParent;
				}
			}
		}
		// 같은 영역에 부모가 존재 한다면
		if( pSameNodeParent != NULL )
		{
			// 부모의 바로 뒤에 자리가 있나 확인
			ptNewPos.y = pSameNodeParent->GetFT()->GetToolPos().y;
			while( true )
			{
				if( IsExistPitch( ptNewPos ) == false )
				{
					// 자리가 있다면 부모의 자식들중에 같은 연결선상에 있는게 있는지 체크
					int nChild = pSameNodeParent->GetChildCount();
					bool bFindSameLine = false;
					for( int i=0; i<nChild; ++i )
					{
						if( pSameNodeParent->GetChild( i )->GetFT()->GetToolPos().y == ptNewPos.y )
						{
							bFindSameLine = true;
							break;
						}
					}

					// 같은 연결선상에 없어야 위치 입력 시킨다.
					if( bFindSameLine == false )
					{
						pQuestObj->GetFT()->SetToolPos( ptNewPos );					
						break;
					}					
				}
				++ptNewPos.y;
				// 이 아래있는 피치들은 한칸씩 다 아래로 미루자
				IncreasePitch_Y( ptNewPos.y );
			}
		}
		// 같은 영역에 존재 하지 않는다면 - 가장 밑에다가 달아주자
		else
		{
			_AutoPosigion_AddBottom( pQuestObj, ptNewPos.x );
		}
	}
	// 선행 퀘스트가 존재 하지 않는다면
	else
	{	
		_AutoPosigion_AddBottom( pQuestObj, 0 );
	}

	return true;
}

void cNode::_AutoPosigion_AddBottom( cQuestObj* pQuestObj, int x )
{
	int nLastIndex = -1;
	std::list< cQuestObj* >::iterator it = m_listQuest.begin();
	std::list< cQuestObj* >::iterator itEnd = m_listQuest.end();
	for( ; it!=itEnd; ++it )
	{
		if( nLastIndex < (*it)->GetFT()->GetToolPos().y )
		{
			nLastIndex = (*it)->GetFT()->GetToolPos().y;
		}
	}

	// 가장 밑에다가 달아주자
	pQuestObj->GetFT()->SetToolPos( CsPoint( x, nLastIndex + 1 ) );
}

bool cNode::IsExistPitch( CsPoint ptToolPos )
{
	std::list< cQuestObj* >::iterator it = m_listQuest.begin();
	std::list< cQuestObj* >::iterator itEnd = m_listQuest.end();
	for( ; it!=itEnd; ++it )
	{
		if( (*it)->GetFT()->GetToolPos() == ptToolPos )
			return true;
	}
	return false;
}

bool cNode::IsExistPitch_X( CsPoint ptToolPos )
{
	std::list< cQuestObj* >::iterator it = m_listQuest.begin();
	std::list< cQuestObj* >::iterator itEnd = m_listQuest.end();
	for( ; it!=itEnd; ++it )
	{
		if( (*it)->GetFT()->GetToolPos().x == ptToolPos.x )
			return true;
	}
	return false;
}

bool cNode::IsExistPitch_Y( CsPoint ptToolPos )
{
	std::list< cQuestObj* >::iterator it = m_listQuest.begin();
	std::list< cQuestObj* >::iterator itEnd = m_listQuest.end();
	for( ; it!=itEnd; ++it )
	{
		if( (*it)->GetFT()->GetToolPos().y == ptToolPos.y )
			return true;
	}
	return false;
}




//==================================================================================================================================
//
//		Quest
//
//==================================================================================================================================

cQuestObj* cNode::GetQuest_FromPitch( CsPoint ptPitch )
{
	std::list< cQuestObj* >::iterator it = m_listQuest.begin();
	std::list< cQuestObj* >::iterator itEnd = m_listQuest.end();
	for( ; it!=itEnd; ++it )
	{
		if( (*it)->GetFT()->GetToolPos() == ptPitch )
		{
			return (*it);
		}
	}
	return NULL;
}

//==================================================================================================================================
//
//		Pitch
//
//==================================================================================================================================

void cNode::IncreasePitch_X( int nIncreaseIndex )
{
	std::list< cQuestObj* >::iterator it = m_listQuest.begin();
	std::list< cQuestObj* >::iterator itEnd = m_listQuest.end();
	for( ; it!=itEnd; ++it )
	{
		CsPoint ptToolPos = (*it)->GetFT()->GetToolPos();
		if( ptToolPos.x >= nIncreaseIndex )
		{
			++ptToolPos.x;
			(*it)->GetFT()->SetToolPos( ptToolPos );
			(*it)->ResetMiddleTail();
			int nParent = (*it)->GetParentCount();
			for( int i=0; i<nParent; ++i )
			{
				(*it)->GetParent( i )->ResetMiddleTail();
			}
		}
	}
}

void cNode::IncreasePitch_Y( int nIncreaseIndex )
{
	std::list< cQuestObj* >::iterator it = m_listQuest.begin();
	std::list< cQuestObj* >::iterator itEnd = m_listQuest.end();
	for( ; it!=itEnd; ++it )
	{
		CsPoint ptToolPos = (*it)->GetFT()->GetToolPos();
		if( ptToolPos.y >= nIncreaseIndex )
		{
			++ptToolPos.y;
			(*it)->GetFT()->SetToolPos( ptToolPos );
			(*it)->ResetMiddleTail();
			int nParent = (*it)->GetParentCount();
			for( int i=0; i<nParent; ++i )
			{
				(*it)->GetParent( i )->ResetMiddleTail();
			}
		}
	}
}

void cNode::DecreasePitch_X( int nDecreaseIndex )
{
	std::list< cQuestObj* >::iterator it = m_listQuest.begin();
	std::list< cQuestObj* >::iterator itEnd = m_listQuest.end();
	for( ; it!=itEnd; ++it )
	{
		CsPoint ptToolPos = (*it)->GetFT()->GetToolPos();
		if( ptToolPos.x >= nDecreaseIndex )
		{
			--ptToolPos.x;
			(*it)->GetFT()->SetToolPos( ptToolPos );
			(*it)->ResetMiddleTail();
			int nParent = (*it)->GetParentCount();
			for( int i=0; i<nParent; ++i )
			{
				(*it)->GetParent( i )->ResetMiddleTail();
			}
		}
	}
}

void cNode::DecreasePitch_Y( int nDecreaseIndex )
{
	std::list< cQuestObj* >::iterator it = m_listQuest.begin();
	std::list< cQuestObj* >::iterator itEnd = m_listQuest.end();
	for( ; it!=itEnd; ++it )
	{
		CsPoint ptToolPos = (*it)->GetFT()->GetToolPos();
		if( ptToolPos.y >= nDecreaseIndex )
		{
			--ptToolPos.y;
			(*it)->GetFT()->SetToolPos( ptToolPos );
			(*it)->ResetMiddleTail();
			int nParent = (*it)->GetParentCount();
			for( int i=0; i<nParent; ++i )
			{
				(*it)->GetParent( i )->ResetMiddleTail();
			}
		}
	}
}

//==================================================================================================================================
//
//		입력
//
//==================================================================================================================================
void cNode::OnLBtnDown( CsPoint ptWorldClick )
{
	CsPoint ptLocal = ptWorldClick - GetNodePos();

	std::list< cQuestObj* >::iterator it = m_listQuest.begin();
	std::list< cQuestObj* >::iterator itEnd = m_listQuest.end();
	for( ; it!=itEnd; ++it )
	{
		if( (*it)->OnLBtnDown( ptLocal ) == true )
			return;
	}
}

bool cNode::IsInObject( CsPoint ptWorldClick )
{
	CsPoint ptLocal = ptWorldClick - GetNodePos();

	std::list< cQuestObj* >::iterator it = m_listQuest.begin();
	std::list< cQuestObj* >::iterator itEnd = m_listQuest.end();
	for( ; it!=itEnd; ++it )
	{
		if( (*it)->IsInObject( ptLocal ) == true )
			return true;
	}
	return false;
}

CsPoint cNode::CalPitch( CsPoint ptWorldClick )
{
	CsPoint ptLocal = ptWorldClick - GetNodePos();

	CsPoint ptReturn;
	ptReturn.x = ptLocal.x / PITCH_SIZE_X;
	ptReturn.y = ptLocal.y / PITCH_SIZE_Y;
	return ptReturn;
}