
#include "stdafx.h"
#include "ModelBase.h"

cModel_Base::cModel_Base()
{
	m_pAM = NULL;
}

void cModel_Base::Delete()
{
	NiDelete m_pAM;
}

void cModel_Base::_Init( DWORD dwModelKey )
{
	ResetAni();
	m_dwModelKey = dwModelKey;
	// 901101 => Client ÀÇ CCharacter ¿¡¼­ ¹ßÃé
	m_pAM->SetTargetAnimation( 901101 );	
}

void cModel_Base::Update( float fElapsedTime, bool bUpdateControl )
{
	if( bUpdateControl == true )
	{
		m_fAniTime += fElapsedTime;
		m_pAM->Update( m_fAniTime );
		m_pAM->GetNIFRoot()->Update( m_fAniTime );
	}
	else
	{
		m_pAM->GetNIFRoot()->Update( m_fAniTime, false );
	}
}