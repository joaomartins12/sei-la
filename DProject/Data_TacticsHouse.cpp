//
#include "stdafx.h"
//#include "Data_TacticsHouse.h"
//
//cData_TacticsHouse::cData_TacticsHouse()
//{
//	m_pTacticsArray = NULL;
//}
//
//void cData_TacticsHouse::Delete()
//{
//	int nCount = nsCsFileTable::g_pBaseMng->GetLimit()->s_nMaxTacticsHouse;
//	for( int i=0; i<nCount; ++i )
//	{
//		m_pTacticsArray[ i ].Delete();
//	}
//	SAFE_DELETE_ARRAY( m_pTacticsArray );
//	SetUseSlotCount( 0 );
//}
//
//void cData_TacticsHouse::Reset()
//{
//	int nCount = nsCsFileTable::g_pBaseMng->GetLimit()->s_nMaxTacticsHouse;
//	for( int i=0; i<nCount; ++i )
//	{
//		m_pTacticsArray[ i ].Delete();
//	}
//	SetUseSlotCount( 0 );
//}
//
//void cData_TacticsHouse::Init()
//{
//	int nCount = nsCsFileTable::g_pBaseMng->GetLimit()->s_nMaxTacticsHouse;
//	assert_cs( m_pTacticsArray == NULL );
//
//	m_pTacticsArray = csnew cData_PostLoad::sDATA[ nCount ];
//
//	for( int i=0; i<nCount; ++i )
//	{
//		m_pTacticsArray[ i ].Delete();
//	}
//	SetUseSlotCount( 0 );
//}
//
//void cData_TacticsHouse::InitNetOff()
//{
//	SetUseSlotCount( nsCsFileTable::g_pBaseMng->GetLimit()->s_nMaxTacticsHouse - 7 );
//
//	
//	/*for( int i=0; i<10; ++i )
//	{
//		m_Tactics[ i ].s_Type.m_nType = 31001;
//
//		_stprintf_s( m_Tactics[ i ].s_szName, Language::pLength::name + 1, _T( "焊包家1" ) );
//
//		m_Tactics[ i ].s_Type.m_nIDX = cType::GetTestIDX( nClass::Digimon );
//		m_Tactics[ i ].s_dwBaseDigimonID = m_Tactics[ i ].s_Type.m_nType;
//
//		m_Tactics[ i ].s_nLevel = 4;	
//		m_Tactics[ i ].s_fScale = 1.0f;		
//		m_Tactics[ i ].s_nMaxEvoUnit = nLimit::EvoUnit - 1;
//
//		for( int e=0; e<nLimit::EvoUnit; ++e )
//		{
//			m_Tactics[ i ].s_EvoUnit[ e ].m_nSkillExp = 0;
//			m_Tactics[ i ].s_EvoUnit[ e ].m_nSkillExpLevel = 0;
//			m_Tactics[ i ].s_EvoUnit[ e ].m_nSkillPoint = 100;
//			m_Tactics[ i ].s_EvoUnit[ e ].m_nSlotState = nEvo::Closed;
//			m_Tactics[ i ].s_EvoUnit[ e ].m_Card.Clear();
//			m_Tactics[ i ].s_EvoUnit[ e ].m_Crest.Clear();
//			for( int s=0; s<nLimit::Skill; ++s )
//				m_Tactics[ i ].s_EvoUnit[ e ].m_nSkillLevel[ s ] = 1;		
//		}
//		m_Tactics[ i ].s_EvoUnit[ 1 ].m_nSlotState = nEvo::Active;
//		m_Tactics[ i ].s_EvoUnit[ 2 ].m_nSlotState = nEvo::Active;
//	}
//
//	for( int i=10; i<15; ++i )
//	{
//		m_Tactics[ i ].s_Type.m_nType = 31002;
//
//		_stprintf_s( m_Tactics[ i ].s_szName, Language::pLength::name + 1, _T( "焊包家2" ) );
//
//		m_Tactics[ i ].s_Type.m_nIDX = cType::GetTestIDX( nClass::Digimon );
//		m_Tactics[ i ].s_dwBaseDigimonID = m_Tactics[ i ].s_Type.m_nType;
//
//		m_Tactics[ i ].s_nLevel = 4;	
//		m_Tactics[ i ].s_fScale = 1.0f;		
//		m_Tactics[ i ].s_nMaxEvoUnit = nLimit::EvoUnit - 1;
//
//		for( int e=0; e<nLimit::EvoUnit; ++e )
//		{
//			m_Tactics[ i ].s_EvoUnit[ e ].m_nSkillExp = 0;
//			m_Tactics[ i ].s_EvoUnit[ e ].m_nSkillExpLevel = 0;
//			m_Tactics[ i ].s_EvoUnit[ e ].m_nSkillPoint = 100;
//			m_Tactics[ i ].s_EvoUnit[ e ].m_nSlotState = nEvo::Closed;
//			m_Tactics[ i ].s_EvoUnit[ e ].m_Card.Clear();
//			m_Tactics[ i ].s_EvoUnit[ e ].m_Crest.Clear();
//			for( int s=0; s<nLimit::Skill; ++s )
//				m_Tactics[ i ].s_EvoUnit[ e ].m_nSkillLevel[ s ] = 1;		
//		}
//		m_Tactics[ i ].s_EvoUnit[ 1 ].m_nSlotState = nEvo::Active;
//		m_Tactics[ i ].s_EvoUnit[ 2 ].m_nSlotState = nEvo::Active;
//	}
//
//	for( int i=15; i<19; ++i )
//	{
//		m_Tactics[ i ].s_Type.m_nType = 31003;
//
//		_stprintf_s( m_Tactics[ i ].s_szName, Language::pLength::name + 1, _T( "焊包家3" ) );
//
//		m_Tactics[ i ].s_Type.m_nIDX = cType::GetTestIDX( nClass::Digimon );
//		m_Tactics[ i ].s_dwBaseDigimonID = m_Tactics[ i ].s_Type.m_nType;
//
//		m_Tactics[ i ].s_nLevel = 4;	
//		m_Tactics[ i ].s_fScale = 1.0f;		
//		m_Tactics[ i ].s_nMaxEvoUnit = nLimit::EvoUnit - 1;
//
//		for( int e=0; e<nLimit::EvoUnit; ++e )
//		{
//			m_Tactics[ i ].s_EvoUnit[ e ].m_nSkillExp = 0;
//			m_Tactics[ i ].s_EvoUnit[ e ].m_nSkillExpLevel = 0;
//			m_Tactics[ i ].s_EvoUnit[ e ].m_nSkillPoint = 100;
//			m_Tactics[ i ].s_EvoUnit[ e ].m_nSlotState = nEvo::Closed;
//			m_Tactics[ i ].s_EvoUnit[ e ].m_Card.Clear();
//			m_Tactics[ i ].s_EvoUnit[ e ].m_Crest.Clear();
//			for( int s=0; s<nLimit::Skill; ++s )
//				m_Tactics[ i ].s_EvoUnit[ e ].m_nSkillLevel[ s ] = 1;		
//		}
//		m_Tactics[ i ].s_EvoUnit[ 1 ].m_nSlotState = nEvo::Active;
//		m_Tactics[ i ].s_EvoUnit[ 2 ].m_nSlotState = nEvo::Active;
//	}*/	
//}
//
//bool cData_TacticsHouse::IsTactics( int nTacticsIndex )
//{
//	return ( m_pTacticsArray[ nTacticsIndex ].s_dwBaseDigimonID != 0 ); 
//}
//cData_PostLoad::sDATA*		cData_TacticsHouse::GetTactics( int nTacticsIndex )
//{
//	assert_cs( nTacticsIndex < m_nUseSlotCount ); 
//	return &m_pTacticsArray[ nTacticsIndex ]; 
//}