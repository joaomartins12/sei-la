#include "StdAfx.h"
#include "EventData.h"

CEventData::CEventData(void)
{	
	m_vEventData.clear();	
}

CEventData::~CEventData(void)
{	
}

void CEventData::Delete()
{
	memset( &m_sEventInfo, 0, sizeof( sEvent_Info ) ); 
	memset( &m_sObjectInfo, 0, sizeof( sEventObject_Info ) );

	std::vector< sBase_Info* >::iterator it = m_vEventData.begin();
	std::vector< sBase_Info* >::iterator itEnd = m_vEventData.end();
	for( ; it != itEnd; it++ )
	{
		switch( (*it)->GetType() )
		{
		case ET_Ani:
			{
				CEventData::sAnimation_Info* pEvent = (CEventData::sAnimation_Info*)(*it);
				SAFE_NIDELETE( pEvent );			
			}			
			break;
		case ET_Move:
			{
				CEventData::sMovePos_Info* pEvent = (CEventData::sMovePos_Info*)(*it);
				SAFE_NIDELETE( pEvent );
			}			
			break;
		case ET_Talk:
			{
				CEventData::sEventTalk_Info* pEvent = (CEventData::sEventTalk_Info*)(*it);
				SAFE_NIDELETE( pEvent );
			}			
			break;
		case ET_Sound:
			{
				CEventData::sEventSound_Info* pEvent = (CEventData::sEventSound_Info*)(*it);
				SAFE_NIDELETE( pEvent );
			}			
			break;
		case ET_Effect:
			{
				CEventData::sEventEffect_Info* pEffect = (CEventData::sEventEffect_Info*)(*it);
				SAFE_NIDELETE( pEffect );
			}			
			break;
		default:
			SAFE_NIDELETE( *it );
			break;
		}		
	}
	m_vEventData.clear();	
}

void CEventData::Init( sEvent_Info* pEventInfo, sEventObject_Info* pObjectInfo )
{
	 memcpy( &m_sEventInfo, pEventInfo, sizeof( sEvent_Info ) ); 
	 memcpy( &m_sObjectInfo, pObjectInfo, sizeof( sEventObject_Info ) );
}

