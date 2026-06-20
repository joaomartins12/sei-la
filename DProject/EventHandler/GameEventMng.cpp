//---------------------------------------------------------------------------
//
// 파일명 : GameEventMng.cpp
// 작성일 : 2009.6.29
// 작성자 : Th
// 설  명 :
//
//---------------------------------------------------------------------------
#include "stdafx.h"
#include "GameEventMng.h"

//------------------------------------------------------------
GameEventMng::GameEventMng(void)
	: m_nDispatchDepth( 0 )
{

}
//------------------------------------------------------------
GameEventMng::~GameEventMng(void)
{
	MAP_EVENT_HANDLER_IT map_itr = m_handlers.begin();
	while( map_itr != m_handlers.end() )
	{
		LIST_EVENT_HANDLER_IT vec_itr = map_itr->second.begin();
		while( vec_itr != map_itr->second.end() )
		{
			SAFE_NIDELETE( *vec_itr );
			++vec_itr;
		}
		
		map_itr->second.clear();
		map_itr = m_handlers.erase( map_itr );
	}
	m_handlers.clear();
}

//------------------------------------------------------------
// 이벤트를 등록된 이벤트 핸들러들에게 알린다.
//------------------------------------------------------------

void GameEventMng::OnEvent( unsigned int p_eEventCode, void * p_eAgs)
{
	MAP_EVENT_HANDLER_IT map_itr = m_handlers.find( p_eEventCode );
	if( map_itr == m_handlers.end() )
		return;

	std::vector<EventHandlerBase*> dispatchSnapshot;
	dispatchSnapshot.reserve( map_itr->second.size() );
	LIST_EVENT_HANDLER_IT vec_itr = map_itr->second.begin();
	while( vec_itr != map_itr->second.end() )
	{
		if( *vec_itr )
			dispatchSnapshot.push_back( *vec_itr );
		++vec_itr;
	}

	++m_nDispatchDepth;
	for( std::vector<EventHandlerBase*>::iterator it = dispatchSnapshot.begin(); it != dispatchSnapshot.end(); ++it )
	{
		if( *it )
		{
			if( (*it)->exec( p_eAgs ) == false )
				RemoveHandlerInstance( *it );
		}
	}
	--m_nDispatchDepth;

	if( m_nDispatchDepth == 0 )
		FlushDeferredDeletes();
}

//------------------------------------------------------------
// 이벤트 코드로 이벤트 핸들러들을 제거 시킨다.
//------------------------------------------------------------

void GameEventMng::DeleteEventCode(unsigned int uiEventCode )
{
	MAP_EVENT_HANDLER_IT map_itr = m_handlers.find( uiEventCode );
	if( map_itr == m_handlers.end() )
		return;
		
	LIST_EVENT_HANDLER_IT vec_itr = map_itr->second.begin();
	while( vec_itr != map_itr->second.end() )
	{
		if( *vec_itr )
		{
			if( m_nDispatchDepth > 0 )
				m_deferredDelete.push_back( *vec_itr );
			else
				SAFE_NIDELETE(*vec_itr);
			vec_itr = map_itr->second.erase( vec_itr );
			continue;
		}

		++vec_itr;
	}

	m_handlers.erase( map_itr );

	if( m_nDispatchDepth == 0 )
		FlushDeferredDeletes();
}

void GameEventMng::FlushDeferredDeletes()
{
	for( std::vector<EventHandlerBase*>::iterator it = m_deferredDelete.begin(); it != m_deferredDelete.end(); ++it )
	{
		SAFE_NIDELETE( *it );
	}
	m_deferredDelete.clear();
}

void GameEventMng::RemoveHandlerInstance( EventHandlerBase* pHandler )
{
	if( pHandler == NULL )
		return;

	MAP_EVENT_HANDLER_IT map_itr = m_handlers.begin();
	while( map_itr != m_handlers.end() )
	{
		LIST_EVENT_HANDLER_IT vec_itr = map_itr->second.begin();
		while( vec_itr != map_itr->second.end() )
		{
			if( *vec_itr == pHandler )
			{
				if( m_nDispatchDepth > 0 )
					m_deferredDelete.push_back( pHandler );
				else
					SAFE_NIDELETE( pHandler );

				vec_itr = map_itr->second.erase( vec_itr );
				continue;
			}
			++vec_itr;
		}

		if( map_itr->second.empty() )
		{
			map_itr = m_handlers.erase( map_itr );
			continue;
		}
		++map_itr;
	}

	if( m_nDispatchDepth == 0 )
		FlushDeferredDeletes();
}
