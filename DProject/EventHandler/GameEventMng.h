//---------------------------------------------------------------------------
//
// 파일명 : GameEventMng.h
// 작성일 : 2009.6.29
// 작성자 : Th
// 설  명 :
//
//---------------------------------------------------------------------------
#pragma once
#include "EventHandler.h"
#include <map>
#include <list>
#include <vector>

typedef std::list< EventHandlerBase* >								LIST_EVENT_HANDLER;
typedef LIST_EVENT_HANDLER::iterator								LIST_EVENT_HANDLER_IT;

typedef std::map< unsigned int, LIST_EVENT_HANDLER >				MAP_EVENT_HANDLER;
typedef std::map< unsigned int, LIST_EVENT_HANDLER >::iterator		MAP_EVENT_HANDLER_IT;

class GameEventMng : public CSingleton< GameEventMng >
{
public:
	GameEventMng();
	virtual ~GameEventMng();

	template <class T>
	void AddEvent( unsigned int p_eEventCode, T* obj, void (T::*memFn)(void*))	// 이벤트 추가	
	{
		MAP_EVENT_HANDLER_IT map_itr = m_handlers.find( p_eEventCode );
		if( map_itr == m_handlers.end() )
		{
			auto _result = m_handlers.insert( std::make_pair( p_eEventCode, LIST_EVENT_HANDLER() ) );
			if( !_result.second )
				return; // 맵에 이벤트 코드번호를 새로 만들다 실패 함

			map_itr = _result.first;
		}

		if( map_itr != m_handlers.end() )
		{
			EventHandler<T> * p_EventHandler = NiNew EventHandler<T>(obj,memFn);
			if( NULL == p_EventHandler )
				return; // new 실패

			LIST_EVENT_HANDLER_IT vec_itr = map_itr->second.begin();
			while( vec_itr != map_itr->second.end() )
			{
				EventHandler<T>* pHandler = static_cast<EventHandler<T>*>(*vec_itr);
				if( *pHandler == *p_EventHandler )
				{
					SAFE_NIDELETE( p_EventHandler );
					return;// 이미 같은 이벤트 핸들러가 들어있어서 실패 함.
				}
				++vec_itr;
			}

			map_itr->second.push_back( p_EventHandler );
		}
	}
	
	template <class T>
	void DeleteEvent( unsigned int p_eEventCode , T* pCaller)			// 이벤트 제거
	{
		MAP_EVENT_HANDLER_IT map_itr = m_handlers.find( p_eEventCode );
		if( map_itr == m_handlers.end() )
			return;

		LIST_EVENT_HANDLER_IT vec_itr = map_itr->second.begin();
		while( vec_itr != map_itr->second.end() )
		{
			if( *vec_itr )
			{
				EventHandler<T>* pHandler = static_cast<EventHandler<T>*>(*vec_itr);
				if( pHandler->CompareCaller( pCaller ) )
				{
					if( m_nDispatchDepth > 0 )
						m_deferredDelete.push_back( *vec_itr );
					else
						SAFE_NIDELETE( *vec_itr );
					vec_itr = map_itr->second.erase( vec_itr );
					continue;
				}
			}
			++vec_itr;
		}

		if( map_itr->second.empty() )
			m_handlers.erase( map_itr );
	}

	template <class T>
	void DeleteEventAll( T* pCaller )
	{
		MAP_EVENT_HANDLER_IT map_itr = m_handlers.begin();
		while( map_itr != m_handlers.end() )
		{
			LIST_EVENT_HANDLER_IT vec_itr = map_itr->second.begin();
			while( vec_itr != map_itr->second.end() )
			{
				if( *vec_itr )
				{
					EventHandler<T>* pHandler = static_cast<EventHandler<T>*>(*vec_itr);
					if( pHandler->CompareCaller( pCaller ) )
					{
						if( m_nDispatchDepth > 0 )
							m_deferredDelete.push_back( *vec_itr );
						else
							SAFE_NIDELETE( *vec_itr );
						vec_itr = map_itr->second.erase( vec_itr );
						continue;
					}
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
	}

	void OnEvent( unsigned int p_eEventCode , void *p_eAgs = NULL );					// 이벤트 발생
	void DeleteEventCode(unsigned int p_eEventCode );
	
private:
	void FlushDeferredDeletes();
	void RemoveHandlerInstance( EventHandlerBase* pHandler );

 	MAP_EVENT_HANDLER		m_handlers;
	int						m_nDispatchDepth;
	std::vector<EventHandlerBase*> m_deferredDelete;
};

//---------------------------------------------------------------------------
// Singleton
#define GAME_EVENT_ST		GameEventMng::GetSingleton()
#define GAME_EVENT_STPTR	GameEventMng::GetSingletonPtr()
//---------------------------------------------------------------------------
