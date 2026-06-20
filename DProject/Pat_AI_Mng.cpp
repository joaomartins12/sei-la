
#include "stdafx.h"
#include "Pat_AI_Mng.h"


//===============================================================================================================================
//
//		Base
//
//===============================================================================================================================
CPat_AI_Mng::sAI_Base*
CPat_AI_Mng::sAI_Base::Add( sAI_Base* pNext, int nRate )
{
	s_nNextTotalRate += nRate;

	sNEXT next;
	next.s_pNext = pNext;
	next.s_nRate = nRate;
	s_vpNext.PushBack( next );
	return pNext;
}

void CPat_AI_Mng::sAI_Base::Delete()
{
	int nSize = s_vpNext.Size();
	for( int i=0; i<nSize; ++i )
	{
		sAI_Base* pBase = (sAI_Base*)s_vpNext[ i ].s_pNext;
		switch( pBase->GetModul() )
		{
		case MODUL_Container:
			// 컨테이너는 외부에서 지워준다.
			break;
		case MODUL_Element:
			pBase->Delete();
			NiDelete (sAI_Element*)pBase;
			break;
		default:
			assert_cs( false );
		}
	}
	s_vpNext.Destroy();
}

CPat_AI_Mng::sAI_Base*
CPat_AI_Mng::sAI_Base::FindElement( int nID )
{
	int nSize = s_vpNext.Size();
	for( int i=0; i<nSize; ++i )
	{
		sAI_Base* pBase = (sAI_Base*)s_vpNext[ i ].s_pNext;

		if( pBase->GetModul() != MODUL_Element )
			continue;

		if( pBase->s_nID == nID )
		{
			return pBase;
		}
		else
		{
			sAI_Base* pReturn = pBase->FindElement( nID );
			if( pReturn != NULL )
				return pReturn;
		}
	}
	return NULL;
}

CPat_AI_Mng::sAI_Base*
CPat_AI_Mng::sAI_Base::GetNext()
{
	assert_cs( s_vpNext.Size() > 0 );

	int nRate = rand()%s_nNextTotalRate;
	int nCount = s_vpNext.Size();
	for( int i=0; i<nCount; ++i )
	{
		if( nRate < s_vpNext[ i ].s_nRate )
		{
			sAI_Base* pBase = (sAI_Base*)s_vpNext[ i ].s_pNext;
			if( pBase->GetModul() == MODUL_Element )
				return pBase;
			else
				return pBase->GetNext();
		}
		nRate -= s_vpNext[ i ].s_nRate;
	}

	assert_cs( false );
	if( s_vpNext.Size() )
	{
		sAI_Base* pBase = (sAI_Base*)s_vpNext[ 0 ].s_pNext;
		if( pBase->GetModul() == MODUL_Element )
			return pBase;
		else
			return pBase->GetNext();
	}
	return g_pPatAIMng->GetContainer( g_pPatAIMng->GetContainerStartID() )->GetNext();
}


//===============================================================================================================================
//
//		Element
//
//===============================================================================================================================
CPat_AI_Mng::sAI_Element*
CPat_AI_Mng::sAI_Element::NewTime( int nID, int ani, float fSec )
{
	sAI_Element* pNew = NiNew sAI_Element;
	pNew->s_eCheckType = CHK_TIME;
	pNew->s_nID = nID;
	pNew->s_eAnimation = (ANI::eTYPE)ani;

	assert_cs( fSec > 0.0f );
	pNew->s_Time.s_fTimeSec = fSec;	

	return pNew;
}

CPat_AI_Mng::sAI_Element*
CPat_AI_Mng::sAI_Element::NewAnimation( int nID, int ani, int nLoopCount )
{
	sAI_Element* pNew = NiNew sAI_Element;
	pNew->s_eCheckType = CHK_ANI;
	pNew->s_nID = nID;
	pNew->s_eAnimation = (ANI::eTYPE)ani;

	pNew->s_Animation.s_nLoopCount = nLoopCount;

	return pNew;
}

CPat_AI_Mng::sAI_Element*
CPat_AI_Mng::sAI_Element::NewMove( int nID, int act, int ani )
{
	sAI_Element* pNew = NiNew sAI_Element;
	pNew->s_eCheckType = CHK_MOVE;
	pNew->s_nID = nID;
	pNew->s_Move.s_nAct = act;	
	pNew->s_eAnimation = (ANI::eTYPE)ani;

	return pNew;
}

//CPat_AI_Mng::sAI_Base*
//CPat_AI_Mng::sAI_Element::CheckTime()
//{
//	assert_cs( s_eCheckType == CHK_TIME );
//	if( s_Time.s_TimeSeq.IsEnable() == false )
//		return NULL;
//	
//	return GetNext();
//}
//
//CPat_AI_Mng::sAI_Base*
//CPat_AI_Mng::sAI_Element::CheckAni( bool& bResetAni )
//{
//	bResetAni = false;
//	if( s_eCheckType != CHK_ANI )
//		return NULL;
//
//	if( (--s_nLoopCount) != 0 )
//	{
//		bResetAni = true;
//		return NULL;
//	}
//
//	ANI::eTYPE oldAni = s_eAnimation;
//	sAI_Element* pNext = g_pPatAIMng->GetAI( s_eAct )->GetNext();
//
//	// 다음의 체크타입이 ANI이고 - 애니메이션이 똑같다면 리셋
//	if( pNext->s_eCheckType == CHK_ANI )
//		bResetAni = ( pNext->s_eAnimation == oldAni );
//
//	return pNext;
//}


//===============================================================================================================================
//
//		Container
//
//===============================================================================================================================


//===============================================================================================================================
//
//		AI Manager
//
//===============================================================================================================================
CPat_AI_Mng*	g_pPatAIMng = NULL;

void CPat_AI_Mng::GlobalInit()
{
	assert_cs( g_pPatAIMng == NULL );

	g_pPatAIMng = NiNew CPat_AI_Mng;
	g_pPatAIMng->Init();	
}

void CPat_AI_Mng::GlobalShotDown()
{
	SAFE_NIDELETE( g_pPatAIMng );
}

void CPat_AI_Mng::Delete()
{
	std::map< DWORD, sAI_Container* >::iterator it = m_mapContainer.begin();
	std::map< DWORD, sAI_Container* >::iterator itEnd = m_mapContainer.end();
	for( ; it!=itEnd; ++it )
	{
		it->second->Delete();
		NiDelete it->second;
	}
	m_mapContainer.clear();
}


void CPat_AI_Mng::Init()
{
}

CPat_AI_Mng::sAI_Container* CPat_AI_Mng::CreateContatiner( int nCon )
{
	assert_cs( m_mapContainer.find( nCon ) == m_mapContainer.end() );
	sAI_Container* pCon = NiNew sAI_Container;
	pCon->s_nID = nCon;
	m_mapContainer[ nCon ] = pCon;
	return pCon;
}

CPat_AI_Mng::sAI_Container*	CPat_AI_Mng::GetContainer( int nCon )
{
	assert_cs( m_mapContainer.find( nCon ) != m_mapContainer.end() );	
	return m_mapContainer[ nCon ];
}

