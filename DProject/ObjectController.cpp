

#include "stdafx.h"
#include "ObjectController.h"

void cObjectController::RenderShadow()
{
#ifdef CLIENT_MONSTER
	if( g_pCharMng->m_bRenderChar == false )
		return;
#endif

	if( g_pEventSceneMng->IsCameraScene() == true )
	{
		if( g_pEventSceneMng->GetScene() != NULL )
		{
			CEventMng::LIST_EVENT_IT it = g_pEventSceneMng->GetScene()->GetListEvent()->begin();
			CEventMng::LIST_EVENT_IT itEnd = g_pEventSceneMng->GetScene()->GetListEvent()->end();
			for( ; it != itEnd; ++it )
			{
				_RenderShadowObject( (*it)->s_pTarget );
			}			
		}	
	}
	else
	{

		{
			if( g_pResist->m_Global.s_bFigureTamer == false )
			{
				std::map< DWORD, CTamer* > * pVector = g_pCharMng->GetTamerMap();
				std::map< DWORD, CTamer* >::iterator it = pVector->begin();
				for( ; it != pVector->end(); ++it )
				{
					SAFE_POINTER_CON( it->second );
#ifndef CLIENT_MONSTER
					_RenderShadowObject( it->second );
#else
					if( g_pCharMng->m_bRenderUser )
						_RenderShadowObject( it->second );
					else if( it->second->GetLeafRTTI() == RTTI_TAMER )
						_RenderShadowObject( it->second );
#endif
				}
			}
			else
			{
				_RenderShadowObject( g_pCharMng->GetTamerUser() );
			}
		}	

		{
			if( g_pResist->m_Global.s_bFigureDigimon == false )
			{
				std::map<DWORD, CDigimon*>* pVector = g_pCharMng->GetDigimonMap();
				std::map<DWORD, CDigimon*>::iterator it = pVector->begin();
				for( ; it != pVector->end(); ++it )
				{
					SAFE_POINTER_CON( it->second );
#ifndef CLIENT_MONSTER
					_RenderShadowObject( it->second );
#else
					if( g_pCharMng->m_bRenderUser )
						_RenderShadowObject( it->second );
					else if( it->second->GetLeafRTTI() == RTTI_DIGIMON )
						_RenderShadowObject( it->second );
#endif

				}
			}
			else
			{
				_RenderShadowObject( g_pCharMng->GetDigimonUser( 0 ) );
			}		
		}

		{
			std::map< DWORD, CMonster* >* pVector = g_pCharMng->GetMonsterVector();
			std::map< DWORD, CMonster* >::iterator it = pVector->begin();
			for( ; it != pVector->end(); ++it )
			{
				SAFE_POINTER_CON( it->second );
				if( it->second->GetMonsterState() == CMonster::MONSTER_CREATE )
					continue;
				_RenderShadowObject( it->second );
			}		
		}

		{
			CNpcMng::NpcCont* pVector = g_pNpcMng->GetNpcVector();
			CNpcMng::NpcCont::iterator it = pVector->begin();
			for( ; it != pVector->end(); ++it )
			{
				SAFE_POINTER_CON( it->second );
				if( it->second->IsEnableUse() )
					_RenderShadowObject( it->second );
			}
		}

		{
			if( g_pResist->m_Global.s_bFigureEmployment == FALSE )
			{
				std::map<DWORD, CEmployment*>* pVector = g_pCharMng->GetEmploymentMap();
				std::map<DWORD, CEmployment*>::iterator it = pVector->begin();
				for( ; it != pVector->end(); ++it )
					_RenderShadowObject( it->second );
			}			
		}

		{
			if( g_pResist->m_Global.s_bFigureTamer == FALSE )
			{
				std::map<DWORD,CPat*>* pVector = g_pCharMng->GetPatMap();
				std::map<DWORD,CPat*>::iterator it = pVector->begin();
				for( ; it != pVector->end(); ++it )
					_RenderShadowObject( it->second );
			}			
		}

		{
			std::list< CsC_AvObject* >* pList = g_pCharMng->GetListDeleteChar();
			std::list< CsC_AvObject* >::iterator it = pList->begin();
			std::list< CsC_AvObject* >::iterator itEnd = pList->end();
			for( ; it!=itEnd; ++it )
			{
				_RenderShadowObject( *it );
			}
		}

	} // 이벤트 카메라 씬인지 체크 if문 끝

}

void cObjectController::_RenderShadowObject( CsC_AvObject* pObject )
{
#ifdef GM_CLOCKING
	if( ((CTamer*)pObject)->IsClocking() && ((CTamer*)pObject)->GetLeafRTTI() != RTTI_TAMER_USER )
		return;
	if( ((CDigimon*)pObject)->IsClocking() && ((CDigimon*)pObject)->GetLeafRTTI() != RTTI_DIGIMON_USER )
		return;
#endif
	CsNodeObj* pCsNode = pObject->GetCsNode();
	if( pCsNode == NULL )
		return;

	if( pObject->IsEnableObject() == false )
		return;

	if(GET_SUBCAMERA(CAMERA_01)->UserCulling( pCsNode->m_pNiNode ) == false )	//2015-11-26-nova
		return;

	pCsNode->SetShader( g_pShadow->GetZSCharObjectShader(), g_pShadow->GetZSObjectShader() );
	pCsNode->Render_GeomDataShadow_ExceptPlag( CGeometry::GP_PARTICLE | CGeometry::GP_NO_ZBUFFER_WRITE );
	pCsNode->ActiveShader();
}