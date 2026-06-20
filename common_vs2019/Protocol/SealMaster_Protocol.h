#include "Sealmaster_Define.h"
#include <list>

class GS2C_RECV_SEALMASTER_LEADER
{
public:
	GS2C_RECV_SEALMASTER_LEADER():m_nResult(0), m_nLeader(0)
	{}
	~GS2C_RECV_SEALMASTER_LEADER(){}

	int		m_nResult;
	u2		m_nLeader;	//-1은 리더 없음
};

class GS2C_RECV_SEALMASTER_SEALINFO
{
public:
	GS2C_RECV_SEALMASTER_SEALINFO()
	{}
	~GS2C_RECV_SEALMASTER_SEALINFO(){}

	std::list<RecvCardInfoAll::SealInfo> m_SealInfoList;
	std::list<RecvCardInfoAll::FavoriteInfo> m_FavoriteInfoList;
};
class GS2C_RECV_SEALMASTER_FAVORITE
{
public:
	GS2C_RECV_SEALMASTER_FAVORITE():m_nCardCode(0), m_nBookmark(0)
	{}
	~GS2C_RECV_SEALMASTER_FAVORITE(){}

	u2 m_nCardCode;		// 카드 테이블 Excel의 index(몇번째 줄)
	u1 m_nBookmark;		// 즐찾 여부(true, false)
};

class GS2C_RECV_SEALMASTER_OPENSEAL
{
public:
	GS2C_RECV_SEALMASTER_OPENSEAL():m_nSealItemID(0), m_nSealAmount(0)
	{}
	~GS2C_RECV_SEALMASTER_OPENSEAL(){}

	int m_nSealItemID;			//씰 아이템 ID
	int m_nSealAmount;			//씰 사용량
};

class GS2C_RECV_SEALMASTER_RESEAL
{
public:
	GS2C_RECV_SEALMASTER_RESEAL():m_nSealItemID(0), m_nSealAmount(0)
	{}
	~GS2C_RECV_SEALMASTER_RESEAL(){}

	int m_nSealItemID;			// 실아이템 ID
	int m_nSealAmount;			// 재밀봉 량
};