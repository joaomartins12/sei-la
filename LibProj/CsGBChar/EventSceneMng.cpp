#include "StdAfx.h"
#include "EventSceneMng.h"

CEventSceneMng*	g_pEventSceneMng = NULL;

CEventSceneMng::CEventSceneMng(void)
{
}

CEventSceneMng::~CEventSceneMng(void)
{
	Delete();
}

void CEventSceneMng::Delete()
{
	m_bCameraScene = false;

	//진행중인 이벤트씬들 삭제	
	SAFE_NIDELETE( m_pScene );	

	//대기중인 이벤트 번호 삭제	
	m_vpSceneNum.Destroy();
}

void CEventSceneMng::ResetScene()
{
	//진행중인 이벤트씬들 삭제	
	m_bCameraScene = false;
	SAFE_NIDELETE( m_pScene );	
}

void CEventSceneMng::Init()
{	
	m_pScene = NULL;
	m_bCameraScene = false;
}

void CEventSceneMng::Update( float fDeltaTime )
{
	if( m_pScene )
	{
		//이벤트 캐릭터 매니져 update
		if( m_pScene->Update( fDeltaTime ) == false )
		{
			if( m_bCameraScene == true )
			{
				g_pEventCall->EventEnd();
			}			

			// 이벤트 본것 기록 저장
			g_pEventCall->EventSave( m_pScene->GetSceneNum() );

			m_bCameraScene = false;			
			SAFE_NIDELETE( m_pScene );			
		}
	}
}

void CEventSceneMng::Render()
{	
	if( m_pScene )
	{
		m_pScene->Render();		
	}
}

bool CEventSceneMng::InsertScene( UINT SceneNum )
{
	if( m_pScene )
	{
		if( m_pScene->GetSceneNum() == SceneNum )
			return false;
	}	

	if( m_vpSceneNum.IsExistData( SceneNum ) )
		return false;	

	m_vpSceneNum.PushBack( SceneNum );
	return true; 
}

void CEventSceneMng::DeleteScene( UINT SceneNum )
{
	m_vpSceneNum.DeleteElement_FromData( SceneNum );
}

void CEventSceneMng::EventCheckCondition( NiPoint2 ptPos, DWORD dwCurMapID )
{	
	/////////////////////////////////////////////////////////////
	// 대기중인 영역 사용 이벤트 조건 확인
	/////////////////////////////////////////////////////////////
	
	int nSize = m_vpSceneNum.Size();
	if( nSize < 1 )
		return; 

	for( int i = nSize - 1; i >= 0; i-- )
	{
		CEventSceneData* pCon = g_pEventDataMng->GetData( m_vpSceneNum[ i ] );		
		assert_cs( pCon != NULL ); 
		
		if( pCon->GetCondition()->s_bArea ) //영역 사용 이벤트
		{		
			if( pCon->GetCondition()->s_dwMapID == dwCurMapID )
			{
				NiPoint2 ptTemp;
				ptTemp = ptPos - pCon->GetCondition()->s_ptPos;

				//영역 안에 캐릭터가 들어오면 조건 허용
				if( ptTemp.Length() < pCon->GetCondition()->s_fRad )
				{		
					assert_cs( pCon->GetEventSize() > 0 );
					assert_csm2( m_pScene == NULL, _T( "중복되는 이벤트조건이 존재함 씬번호 %d, %d" ), m_pScene->GetSceneNum(), pCon->GetSceneNum() );
					m_pScene = NiNew CEventMng;
					m_pScene->Init( pCon->GetSceneNum() );																		
					m_vpSceneNum.DeleteElement( i );

					if( pCon->GetCondition()->s_bUseCamera )
					{ 
						m_bCameraScene = true;
						g_pEventCall->EventStart();
					}
				}				
			}		
		}
		else
		{		
			assert_cs( pCon->GetEventSize() > 0 );
			assert_csm2( m_pScene == NULL, _T( "중복되는 이벤트조건이 존재함 씬번호 %d, %d" ), m_pScene->GetSceneNum(), pCon->GetSceneNum() );
			m_pScene = NiNew CEventMng;
			m_pScene->Init( pCon->GetSceneNum() );											
			m_vpSceneNum.DeleteElement( i );

			if( pCon->GetCondition()->s_bUseCamera )
			{
				m_bCameraScene = true;
				g_pEventCall->EventStart();
			}
		}
	}
	
}

CsC_AvObject* CEventSceneMng::GetEventObject( int nEventNum )
{
	if( m_pScene )
	{
		CEventMng::LIST_EVENT_IT it = m_pScene->GetListEvent()->begin();
		CEventMng::LIST_EVENT_IT itEnd = m_pScene->GetListEvent()->end();
		for( ; it != itEnd; ++it )
		{
			if( (*it)->GetEvent()->GetEventInfo()->s_nEventNum == nEventNum )
				return (*it)->s_pTarget;
		}			
	}	
	assert_csm( false, _T( "실행중인 이벤트에 해당 오브젝트 존재 하지 않음") );
	return NULL;
}

//////////////////////////////////////////////////////////////////////////
//
// 툴에서만 쓰이는 함수들
//
//////////////////////////////////////////////////////////////////////////

int CEventSceneMng::EventProgressCount()
{
	if( m_pScene )
	{
		return (int)m_pScene->GetListEvent()->size();
	}
	return 0;	
}

void CEventSceneMng::DirectScenePlay( UINT SceneNum )
{
	if( m_pScene == NULL )
	{
		CEventSceneData* pCon = g_pEventDataMng->GetData( SceneNum );		
		if( pCon == NULL )
			return;

		m_pScene = NiNew CEventMng;
		m_pScene->Init( pCon->GetSceneNum() );		

		if( pCon->GetCondition()->s_bUseCamera )
		{
			m_bCameraScene = true;
			g_pEventCall->EventStart();
		}
	}
}



