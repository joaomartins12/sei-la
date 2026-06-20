#pragma once

#include "../../ContentsSystem/ContentsSystem.h"

class AdaptCommunityContesnt;
class AdaptUnionStore;
class AdaptTradeContents;
class AdaptMacroProtect;

class CServerRelocateContents
	: public IUIContentSubject< CServerRelocateContents >
{
public:
	static int const IsContentsIdentity(void);

	enum ePageType
	{
		eNonePage	= -1,		// 이용중이 아니다.
		eTermNCondigitionPage,	// 이용 약관 동의 페이지
		eCautiousPage,			// 주의 사항 페이지
		eRegistPage				// 신청 페이지
	};

	enum eEventType
	{
		eChangePageStep,
		eChangeServerSelected,	// 서버 변경시 이벤트 발생
		eRecvNameCheckPacket,
		eRecvItemUseResult,
		eReCheckTamerName,		// 테이머 이름을 다시 조회 하라는 이벤트
	};

public:
	CServerRelocateContents(void);
	virtual ~CServerRelocateContents(void);

	virtual int const GetContentsIdentity(void) const;

	virtual bool	Initialize(void);
	virtual void	UnInitialize(void);
	virtual bool	IntraConnection(ContentsSystem& System);
	virtual void	Update(float const& fElapsedTime);
	virtual void	NotifyEvent(int const& iNotifiedEvt);
	virtual void	NotifyEventStream(int const& iNotifiedEvt, ContentsStream const& kStream);
	virtual void	MakeMainActorData(void);
	virtual void	ClearMainActorData(void);

public:
	void			CloseServerChangePage();
	int				GetCurrentPageType() const;
	UINT64			GetLimitMaxMoney(int const& nServerIDX) const;
	bool			IsServerRelocateEnable(int const& nServerIDX) const;
	void			SetChangedServerIDX(int const& nServerIdx);
	bool			SendCheckTamerName(std::wstring const& checkName);
	void			SetShareHouseLimitCheck( bool const& bValue );
	bool			GetShareHouseLimitCheck() const;

	void			NextStep();
	void			BackStep();
	bool			IHaveGuild() const;
	bool			IHaveUnionStore() const;
	bool			IsEnableServerRelocateTime() const;
	bool			SendServerChangeRequest();
	bool			IsServerRelocatePageShow() const;	// 서버 변경 페이지 상태인지 체크

	void			Load3DMapData();
	void			Clear3DMapData();
	void			Update3DMapObject( float const& ffAccumTime );
	void			Render3DMapObject();

	void			ReCheckTamerName();

private:
	void			_InitData();
	void			_ClearData();
	void			_OpenUIPage(void* pData);
	bool			_Load3DMap(std::string const& loadFileName);
	void			_SetCameraInfo();
	void			_ResetCameraInfo();

	void			_RecvCheckTamerName_Result(void* pData);
	void			_RecvItemUse_Result(void* pData);
	void			_CheckShowPage(void* pData);

private:
	int				m_nCurrentPageType;
	int				m_nUseItemInvenPos;		// 사용하려는 아이템의 인벤 위치
	int				m_nChangeServerIdx;		// 변경할 서버 인덱스 번호
	std::wstring	m_nChangeTamerName;		// 변경될 서버에서 사용할 테이머 이름
	bool			m_bUsedTamerName;		// 사용 가능한 이름인가?
	bool			m_bCheckedShareHouseLimit;//공유 창고 제한 확인 결과

	CsNodeObj		m_BackgroundMap;		// 3D Map 데이터
	float			m_fBackupDistMin;		// 3D 맵 진입전 카메라 거리 값 저장 변수
	float			m_fBackupDistMax;		// 3D 맵 진입전 카메라 거리 값 저장 변수

	TOwnershipGuard< AdaptCommunityContesnt >	m_AdaptCommunity;
	TOwnershipGuard< AdaptUnionStore >			m_AdaptUnionStore;
	TOwnershipGuard< AdaptTradeContents >		m_AdaptTrade;
	TOwnershipGuard< AdaptMacroProtect >		m_AdaptMacroProtect;	
};
