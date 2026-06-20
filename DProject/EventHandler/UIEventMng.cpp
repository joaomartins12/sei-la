//---------------------------------------------------------------------------
//
// 파일명 : GameEventMng.cpp
// 작성일 : 2009.6.29
// 작성자 : Th
// 설  명 :
//
//---------------------------------------------------------------------------
#include "stdafx.h"
#include "UIEventMng.h"

namespace UI
{
	//------------------------------------------------------------
	UIEventMng::UIEventMng(void)
	{
		m_handlers.clear();
	}

	//------------------------------------------------------------
	UIEventMng::~UIEventMng(void)
	{
		DeleteEventAll();
	}

	//------------------------------------------------------------
	// 이벤트를 등록된 이벤트 핸들러들에게 알린다.
	//------------------------------------------------------------

	void UIEventMng::OnEvent( unsigned int p_eEventCode, void* p_eAgs)
	{
		CONT_EVENTHANDLER_IT map_itr = m_handlers.find( p_eEventCode );
		if( map_itr == m_handlers.end() )
			return;

		if( map_itr->second )
			map_itr->second->exec(p_eAgs);
	}

	//------------------------------------------------------------
	// 이벤트 코드로 이벤트 핸들러들을 제거 시킨다.
	//------------------------------------------------------------

	void UIEventMng::DeleteEventCode(unsigned int uiEventCode )
	{
		CONT_EVENTHANDLER_IT map_itr = m_handlers.find( uiEventCode );
		if( map_itr == m_handlers.end() )
			return;

		SAFE_NIDELETE( map_itr->second );
		m_handlers.erase( map_itr );
	}

	void UIEventMng::DeleteEventAll()
	{
		CONT_EVENTHANDLER_IT map_itr = m_handlers.begin();
		while( map_itr != m_handlers.end() )
		{
			SAFE_NIDELETE( map_itr->second );
			map_itr = m_handlers.erase( map_itr );
		}
		m_handlers.clear();
	}
}