
#pragma once

#include "EventHandler.h"

#include <map>

namespace UI
{	
	class UIEventMng
	{
	public:
		typedef std::map< UINT, EventHandlerBase* >				CONT_EVENTHANDLER;
		typedef std::map< UINT, EventHandlerBase* >::iterator	CONT_EVENTHANDLER_IT;

	public:
		UIEventMng();
		virtual ~UIEventMng();

		template <class T>
		bool AddEvent( unsigned int p_eEventCode, T* obj, void (T::*memFn)(void*,void*))	// 이벤트 추가	
		{
			UIEventHandler<T> * p_EventHandler = NiNew UIEventHandler<T>(obj,memFn);
			if( NULL == p_EventHandler )
				return false; // new 실패

			p_EventHandler->SetSender(this);

			CONT_EVENTHANDLER_IT it = m_handlers.find( p_eEventCode );// 이미 이벤트 코드가 있으면 기존것을 지우고 새로 넣는다.
			if( it != m_handlers.end() )
			{
				SAFE_NIDELETE( it->second );
				m_handlers.erase( it );
			}

			auto _result = m_handlers.insert( std::make_pair( p_eEventCode, p_EventHandler ) );
			return _result.second;
		}

		void OnEvent( unsigned int p_eEventCode , void *p_eAgs = NULL );					// 이벤트 발생

		void DeleteEventCode(unsigned int p_eEventCode );
		void DeleteEventAll();

	private:
		CONT_EVENTHANDLER		m_handlers;
	};
}
