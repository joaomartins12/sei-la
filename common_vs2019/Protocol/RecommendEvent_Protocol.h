#pragma once

#include "RecommendEvent_Define.h"
#include <list>
#include <sstream>
#include "..\pLimit.h"

struct  ST_RECV_RecommendEvent
{
public:
	struct sInfo
	{
	public:
		sInfo() : m_nFriendlyType( 0 )
		{
			memset( m_nFriendlyMark, 0, sizeof(u1) * nLimit::MAX_FRIENDLY_MARK );
			m_szTamerName.clear();
		}

		u1				m_nFriendlyType;
		u1				m_nFriendlyMark[ nLimit::MAX_FRIENDLY_MARK ];
		std::wstring	m_szTamerName;
	};

	ST_RECV_RecommendEvent() : m_nMyRecommendState( 0 ), m_nResult( 0 )
	{	
		m_EventInfoList.clear();
	}
public:
	void		AddTamerInfo(const sInfo& info)
	{
		m_EventInfoList.push_back( info );
	}

public:
	n1						m_nResult;
	u1						m_nMyRecommendState;
	std::list< sInfo >		m_EventInfoList;
};

struct  ST_RECV_RewardItemInfo
{
	ST_RECV_RewardItemInfo() : m_nRecommendType( 0 ), m_nMarkIndex( 0 ), m_nResult( 0 )
	{	
		m_strTamerName.clear();
	}

public:
	u1 m_nRecommendType;					// 친구 추가 타입 1: 내가 추천 2: 친구가 나를 추천
	u2 m_nMarkIndex;						// 테이블 인덱스
	u1 m_nResult;							// 결과
	std::wstring m_strTamerName;			// 친구 이름 ( 나의 보상의 경우 정보 없음 )
};

struct  ST_RECV_PurposeCompleteInfo
{
	ST_RECV_PurposeCompleteInfo() : m_nRecommendType( 0 ), m_nMarkIndex( 0 )
	{	
		m_strTamerName.clear();
	}

public:
	u1 m_nRecommendType;
	u2 m_nMarkIndex;
	std::wstring m_strTamerName;
};

struct  ST_RECV_RecommendResultInfo
{
	ST_RECV_RecommendResultInfo() : m_nRecommendType( 0 ), m_nAnswer( 0 )
	{	
		m_strTamerName.clear();
	}

public:
	u1 m_nRecommendType;
	u2 m_nAnswer;
	std::wstring m_strTamerName;
};