
#include "stdafx.h"
#include "Employment.h"


THREAD_MEMPOOL_CPP( CEmployment )
#define		PARENT_CLASS		CsC_AvObject
CsCRTTI_CPP( PARENT_CLASS, CEmployment, RTTI_EMPLOYMENT )

CEmployment::CEmployment()
{
	m_pFT_Digimon = NULL;
}

void CEmployment::Delete()
{
	PARENT_CLASS::Delete();
}

void CEmployment::PreDelete()
{
	PARENT_CLASS::PreDelete();

	m_pFT_Digimon = NULL;
}

void CEmployment::Init( uint nUID, uint nFileTableID, NiPoint3 vPos, float fRot, sCREATEINFO* pProp )
{
	m_pFT_Digimon = nsCsFileTable::g_pDigimonMng->GetDigimon( nFileTableID );

	// 초기화
	PARENT_CLASS::Init( nUID, nFileTableID, vPos, fRot, pProp );	
}


void CEmployment::Update( float fDeltaTime, bool bAnimation )
{
	PARENT_CLASS::Update( fDeltaTime, bAnimation );
}

void CEmployment::Render()
{
	PARENT_CLASS::Render();
}

void CEmployment::Render_PostChar()
{
	PARENT_CLASS::Render_PostChar();
}
//====================================================================================
//	파일 테이블
//====================================================================================
CsDigimon* CEmployment::GetDigimonFT()
{ 
	return m_pFT_Digimon;
}
//=============================================================================================================
//
//	CallBack
//
//=============================================================================================================

bool CEmployment::CallBack_EndofSequence( DWORD dwAniID )
{
	return PARENT_CLASS::CallBack_EndofSequence( dwAniID );
}

//====================================================================================
//		Sound
//====================================================================================
void* CEmployment::PlaySound( char* cSoundPath )
{
	return g_pSoundMgr->PlaySystemSound( cSoundPath, GetPos(), this );
}
//====================================================================================================
//	부가 정보
//====================================================================================================
UINT CEmployment::GetClass()
{ 
	return nClass::CommissionShop;
}
UINT CEmployment::GetUniqID()
{ 
	cType type; type.m_nClass = GetClass(); type.m_nIDX = GetIDX(); return type.m_nUID;
}
NiColor CEmployment::GetNameColor()
{ 
	return NiColor( 0, 0, 1 );
}
UINT CEmployment::GetModelID() const
{ 
	return m_pFT_Digimon->GetInfo()->s_dwModelID;
}
ANI::eTYPE CEmployment::GetRandomIdle()
{ 
	return ( rand()%5 == 0 ) ? ANI::IDLE_SHAKE : ANI::IDLE_CHAR;
}
#undef		PARENT_CLASS