#include "FriendList_Define.h"
//친구 목록
class GS2C_RECV_FRIENDLIST
{
public:
	GS2C_RECV_FRIENDLIST():m_nResult(0)
	{}
	~GS2C_RECV_FRIENDLIST(){}

	int		m_nResult;
	std::list<nsFriendInfo::sFriendInfo> m_FriendList;
};
//차단 목록
class GS2C_RECV_SHUTOUTLIST
{
public:
	GS2C_RECV_SHUTOUTLIST(){}
	~GS2C_RECV_SHUTOUTLIST(){}
	std::list<nsShutout::sShutoutInfo> m_ShoutoutList;
};
//주변 캐릭터 이름 변경
class GS2C_RECV_NAME_CHANGE_NEARBY
{
public:
	GS2C_RECV_NAME_CHANGE_NEARBY(){}
	~GS2C_RECV_NAME_CHANGE_NEARBY(){}

	std::wstring		s_szPreName;
	std::wstring		s_szNextName;
};
//친구 추가
class GS2C_RECV_ADDINFO
{
public:
	GS2C_RECV_ADDINFO(){}
	~GS2C_RECV_ADDINFO(){}

	bool			s_bOnlineState;			//logon/logff 상태
	std::wstring	s_szName;
};

class GS2C_RECV_NOTFOUND
{
public:
	GS2C_RECV_NOTFOUND(){}
	~GS2C_RECV_NOTFOUND(){}

	std::wstring	s_szName;
};
//pbuddy 패킷으로 오는 이름 변경
class GS2C_RECV_NAME_CHANGE_FREIND_SHUTOUT
{
public:
	GS2C_RECV_NAME_CHANGE_FREIND_SHUTOUT(){}
	~GS2C_RECV_NAME_CHANGE_FREIND_SHUTOUT(){}

	bool				s_bIsban;		// false = 친구, true = 차단
	std::wstring		s_szPreName;	// 기존 이름
	std::wstring		s_szNextName;	// 변경된 이름
};