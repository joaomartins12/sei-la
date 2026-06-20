#include "StdAfx.h"
#include "EventSceneData.h"

CEventSceneData::CEventSceneData(void)
{
}

CEventSceneData::~CEventSceneData(void)
{
	Delete();
}

void CEventSceneData::Delete()
{
	int nSize = m_vpEventData.Size();
	for( int i = 0; i < nSize; i++ )
	{
		m_vpEventData[ i ]->Delete();
		SAFE_NIDELETE( m_vpEventData[ i ] );
	}
	m_vpEventData.Destroy();

}

void CEventSceneData::DeleteEvent( int nIdx )
{
	if( m_vpEventData.IsExistElement( nIdx ) )
	{
		m_vpEventData[ nIdx ]->Delete();
		SAFE_NIDELETE( m_vpEventData[ nIdx ] );
		m_vpEventData.DeleteElement( nIdx );
	}
}

void CEventSceneData::Init( UINT nSceneNum, sEvent_Condition* pEventCon )
{
	m_SceneNum = nSceneNum;
	memcpy( &m_sEventCondition, pEventCon, sizeof( sEvent_Condition ) ); 
}

CEventData* CEventSceneData::GetEventData( UINT EventNum )
{
	int nSize = m_vpEventData.Size();
	for( int i = 0; i < nSize; i++ )
	{
		if( m_vpEventData.IsExistElement( i ) )
		{
			if( m_vpEventData[ i ]->GetEventInfo()->s_nEventNum == EventNum )
				return m_vpEventData[ i ];
		}		
	}
	return NULL;
}

CEventData* CEventSceneData::GetEventDatabyIdx( int nIdx )
{
	if( m_vpEventData.IsExistElement( nIdx ) )
		return m_vpEventData[ nIdx ];
	else
		return NULL;
}