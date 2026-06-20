#pragma once

#include "Chatting_Define.h"
#include <sstream>

struct ST_CHAT_PROTOCOL			// 채팅 관련 외부 관련 이벤트 처리는 전부 처리.
{
public:
	ST_CHAT_PROTOCOL()	{	Clear();	}
	~ST_CHAT_PROTOCOL()	{	Clear();	}
	void	Clear()
	{
		m_Index = 0; m_Type = NS_CHAT::NORMAL;
		m_value = 0; m_wStr.clear();
	}
public:
	int						m_Index;
	NS_CHAT::TYPE			m_Type;		// 타입 종류 : 일반, 파티, 길드, 귓속말, 외치기, 메가폰, 공지, 길드공지~ 각종 이벤트등.
	std::wstring					m_wStr;		// 채팅 내용
	int						m_value;	// 임의의 경우에 사용하기 위한 변수
};