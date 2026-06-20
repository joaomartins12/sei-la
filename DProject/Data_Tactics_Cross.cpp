
#include "stdafx.h"
#include "Data_Tactics_Cross.h"


#ifdef CROSSWARS_SYSTEM
void cData_CrossTactics::InitNetOff()
{
	m_pTacticsArray[ 0 ].s_Type.m_nType = 36001;
	_stprintf_s( m_pTacticsArray[ 0 ].s_szName, Language::pLength::name + 1, _T( "베타" ) );

	m_pTacticsArray[ 1 ].s_Type.m_nType = 36002;
	_stprintf_s( m_pTacticsArray[ 1 ].s_szName, Language::pLength::name + 1, _T( "파닥" ) );

	m_pTacticsArray[ 2 ].s_Type.m_nType = 36000;
	_stprintf_s( m_pTacticsArray[ 2 ].s_szName, Language::pLength::name + 1, _T( "시드" ) );

	m_pTacticsArray[ 3 ].s_Type.m_nType = 36003;
	_stprintf_s( m_pTacticsArray[ 3 ].s_szName, Language::pLength::name + 1, _T( "쿠다" ) );

	for( int i=0; i<4; ++i )
	{
		m_pTacticsArray[ i ].s_nEnable = 1;		
		m_pTacticsArray[ i ].s_Type.m_nIDX = cType::GetTestIDX( nClass::Digimon );
		m_pTacticsArray[ i ].s_dwBaseDigimonID = m_pTacticsArray[ i ].s_Type.m_nType;

		m_pTacticsArray[ i ].s_nLevel = 75;
		m_pTacticsArray[ i ].s_fScale = 1.0f;		
		m_pTacticsArray[ i ].s_nMaxEvoUnit = nLimit::EvoUnit - 1;

		for( int e=0; e<nLimit::EvoUnit; ++e )
		{
			m_pTacticsArray[ i ].s_EvoUnit[ e ].m_nSkillExp = 0;
			m_pTacticsArray[ i ].s_EvoUnit[ e ].m_nSkillExpLevel = 0;
			m_pTacticsArray[ i ].s_EvoUnit[ e ].m_nSkillPoint = 100;
			m_pTacticsArray[ i ].s_EvoUnit[ e ].m_nSlotState = 0;
			for( int s=0; s<nLimit::Skill; ++s )
				m_pTacticsArray[ i ].s_EvoUnit[ e ].m_nSkillLevel[ s ] = 1;		
		}
		m_pTacticsArray[ i ].s_EvoUnit[ 1 ].m_nSlotState = 1;
		m_pTacticsArray[ i ].s_EvoUnit[ 2 ].m_nSlotState = 1;
		m_pTacticsArray[ i ].s_EvoUnit[ 3 ].m_nSlotState = 1;
		m_pTacticsArray[ i ].s_EvoUnit[ 4 ].m_nSlotState = 1;
		m_pTacticsArray[ i ].s_EvoUnit[ 5 ].m_nSlotState = 0;
		m_pTacticsArray[ i ].s_EvoUnit[ 6 ].m_nSlotState = 1;
	}	

}
bool cData_CrossTactics::CheckCodeCrown()
{
	if(0 == m_nCrossID)
		return false;

	SAFE_POINTER_RETVAL( nsCsFileTable::g_pDigimonMng, false );
	CsCrossDigimon* pFTCross = nsCsFileTable::g_pDigimonMng->GetCrossDigimon( m_nCrossID );
	SAFE_POINTER_RETVAL( pFTCross, false );
	CsCrossDigimon::sINFO* pFTCrossInfo = pFTCross->GetInfo();
	SAFE_POINTER_RETVAL( pFTCrossInfo, false );

	SAFE_POINTER_RETVAL( g_pCharMng, false );
	CTamerUser* pTamerUser = g_pCharMng->GetTamerUser();
	SAFE_POINTER_RETVAL( pTamerUser, false );
	CTamerUser::sUSER_STAT* pUserStat = dynamic_cast<CTamerUser::sUSER_STAT*>(pTamerUser->GetBaseStat());
	SAFE_POINTER_RETVAL( pUserStat, false );

	if( pFTCrossInfo->s_nCodeCrownRequire <= pUserStat->GetCR() )
		return true;
	else
	{
		// 코드 크라운 게이지가 부족합니다.
		cPrintMsg::PrintMsg( 30587 );
	}

	return false;
}
void cData_CrossTactics::sCross::ToObject( CsC_AvObject* pAvObject, bool bInit )
{
	cData_PostLoad::sDATA::ToObject(pAvObject, bInit);
}

cData_CrossTactics::cData_CrossTactics() : m_pTacticsArray(NULL), m_bToggleEnable(false), m_nCrossID(0), m_nCrossCnt(0)
{
	m_TempTactics.Delete();
	memset(m_nCrossIdx, -1, sizeof(int)*IF_CROSSLOADERSLOT_COUNT);
}

void cData_CrossTactics::Delete()
{
	int nCount = nsCsFileTable::g_pBaseMng->GetLimit()->s_nMaxCrossTactics;
	for( int i=0; i<nCount; ++i )
	{
		m_pTacticsArray[ i ].Delete();
	}
	SAFE_DELETE_ARRAY( m_pTacticsArray );

	
	m_TempTactics.Delete();

}

void cData_CrossTactics::Reset()
{
	int nCount = nsCsFileTable::g_pBaseMng->GetLimit()->s_nMaxCrossTactics;
	for( int i=0; i<nCount; ++i )
	{
		m_pTacticsArray[ i ].Delete();
	}	

	m_nCrossID = m_nCrossCnt = 0;
	memset(m_nCrossIdx, -1, sizeof(int)*IF_CROSSLOADERSLOT_COUNT);
	
	m_TempTactics.Delete();
}


void cData_CrossTactics::Init()
{
	int nCount = nsCsFileTable::g_pBaseMng->GetLimit()->s_nMaxCrossTactics;
	assert_cs( m_pTacticsArray == NULL );

	m_pTacticsArray = csnew sCross[ nCount ];

	for( int i=0; i<nCount; ++i )
	{
		m_pTacticsArray[ i ].Delete();
	}	

	m_nCrossID = m_nCrossCnt = 0;
	memset(m_nCrossIdx, -1, sizeof(int)*IF_CROSSLOADERSLOT_COUNT);
	
	m_TempTactics.Delete();
}



void cData_CrossTactics::AddTactics( sCross* pSrcTactics, int nTacticsIndex )
{
	assert_cs( nTacticsIndex < nsCsFileTable::g_pBaseMng->GetLimit()->s_nMaxCrossTactics );

	sCross* pDestTactics = GetTactics( nTacticsIndex );
	assert_cs( pDestTactics->s_Type.m_nType == 0 );
	memcpy( pDestTactics, pSrcTactics, sizeof( sCross ) );
}

void cData_CrossTactics::DeleteTactics( int nTacticsIndex )
{
	assert_cs( nTacticsIndex < nsCsFileTable::g_pBaseMng->GetLimit()->s_nMaxCrossTactics );		
	m_pTacticsArray[ nTacticsIndex ].Delete();
	// IF에 연결
	g_pGameIF->GetTacticsWindow( nTacticsIndex )->SetTargetObject( 0 );	
}

void cData_CrossTactics::ChangeTactics( CsC_AvObject* pSrc, int nDestTacticsIndex )
{
	assert_cs( nDestTacticsIndex < nsCsFileTable::g_pBaseMng->GetLimit()->s_nMaxCrossTactics );
	assert_cs( pSrc->GetLeafRTTI() == RTTI_DIGIMON_USER );	

	// 이름쪽은 노 신경
	/*cData_PostLoad::sDATA SrcData;
	SrcData.FromObject( pSrc );*/

	cData_PostLoad::sDATA* pCurData = g_pDataMng->GetPostLoad()->GetDigimonData();	// 아구몬 스텟에 아구몬 클래스 정보를 넣는다. 백업.
	pCurData->s_nLevel = pSrc->GetBaseStat()->GetLevel();
	pCurData->s_nExp = pSrc->GetBaseStat()->GetExp();
	pCurData->s_fScale = pSrc->GetOrgGameScale();
	memcpy( pCurData->s_EvoUnit, ( (CDigimonUser*)pSrc )->GetAttributeEvoUnit(), sizeof( cEvoUnit)*nLimit::EvoUnit );

	
	cData_PostLoad::sDATA* pData = GetTactics( nDestTacticsIndex );
	pData->ToObject( pSrc, false ); // pSrc 스텟이 pData값으로 변경된다. // 아구몬에 스텟이 크로스로 변경된다.
	
	*pCurData = *pData;

}

void cData_CrossTactics::ChangeTactics2( CsC_AvObject* pSrc, int nDestTacticsIndex )
{
	assert_cs( nDestTacticsIndex < nsCsFileTable::g_pBaseMng->GetLimit()->s_nMaxCrossTactics );
	assert_cs( pSrc->GetLeafRTTI() == RTTI_DIGIMON_USER );	

	// 이름쪽은 노 신경
	/*cData_PostLoad::sDATA SrcData;
	SrcData.FromObject( pSrc );*/

	cData_PostLoad::sDATA* pCurData = g_pDataMng->GetPostLoad()->GetDigimonData();	// 아구몬 스텟에 아구몬 클래스 정보를 넣는다. 백업.
	pCurData->s_nLevel = pSrc->GetBaseStat()->GetLevel();
	pCurData->s_nExp = pSrc->GetBaseStat()->GetExp();
	pCurData->s_fScale = pSrc->GetOrgGameScale();
	memcpy( pCurData->s_EvoUnit, ( (CDigimonUser*)pSrc )->GetAttributeEvoUnit(), sizeof( cEvoUnit)*nLimit::EvoUnit );

	cData_PostLoad::sDATA* pData = g_pDataMng->GetCrossTactics()->GetTempTactics(); // RecvCrossChangePartner2 할때 서버에서 받은 일반디지몬 정보	
	pData->ToObject( pSrc, false ); // pSrc 스텟이 pData값으로 변경된다. // 아구몬에 스텟이 크로스로 변경된다.
	
	*pCurData = *pData;

}


cData_CrossTactics::sCross*	 cData_CrossTactics::GetTacticsByBaseDigimonID(DWORD dwBaseDigimonID)
{

	int nCount = nsCsFileTable::g_pBaseMng->GetLimit()->s_nMaxCrossTactics;
	assert_cs( m_pTacticsArray != NULL );

	for( int i=0; i<nCount; ++i )
	{
		if(dwBaseDigimonID == m_pTacticsArray[ i ].s_dwBaseDigimonID )
		{
			return &m_pTacticsArray[ i ];
		}
	}
	
	return NULL;
}
#endif