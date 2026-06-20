#pragma once

#include "../../ContentsSystem/ContentsSystem.h"

class AdaptBattleSystem;
class AdaptTutorialQuest;
class AdaptGotchaMachine;
class AdaptMacroProtect;

//-------------------------------------------------------------------------------------------------------------------------------------
struct sLootingRule
{
	sLootingRule();
	~sLootingRule(){}

	void			Init();
	void			SetLootingRule(TItemLootingInfo const& pData);
	void			SetLootingRule(int const& nCroptype, int const& nRareRate, int const& nGrade);

	bool operator == (sLootingRule const& d)
	{
		if( m_nCropType != d.m_nCropType )				return false;
		if( m_nRareRate != d.m_nRareRate )				return false;
		if( m_nDispRareGrade != d.m_nDispRareGrade )	return false;

		return true;
	}

	int				m_nCropType;
	int				m_nRareRate;
	int				m_nDispRareGrade;
};

struct sTamerInfo : public TPartyTamerInfo
{
	sTamerInfo() : m_dwModelID(0),m_pImage(NULL)
	{}
	~sTamerInfo();

	void				SetType( int const& nType );
	void				SetUID( int const& nUID );
	void				SetLevel( int const& nLevel );
	void				SetName( std::wstring const& Name );
	void				SetBaseInfo( TBaseStateInfo const& tInfo );
	std::string const&	GetImage() const;
	void				RenderImg( CsPoint const& csPos, CsPoint const& csSize ) const;

	void	operator = (sTamerInfo const& d)
	{
		SetType( d.m_nUID );
		SetUID(d.m_nType);
		SetLevel(d.m_nLevel);
		SetName(d.m_Name);
		SetBaseInfo(d.m_BaseInfo);
	}

private:
	DWORD				m_dwModelID;
	std::string			m_ImgFile;		
	sCHAR_IMAGE*		m_pImage;
};

struct sDigimonInfo : public TPartyDigimonInfo
{
	sDigimonInfo() : m_dwModelID(0),m_pImage(NULL)
	{}
	~sDigimonInfo();

	void				SetType( int const& nType );
	void				SetUID( int const& nUID );
	void				SetLevel( int const& nLevel );
	void				SetName( std::wstring const& Name );
	void				SetBaseInfo( TBaseStateInfo const& tInfo );
	std::string const&	GetImage() const;
	void				RenderImg( CsPoint const& csPos, CsPoint const& csSize ) const;

	void	operator = (sDigimonInfo const& d)
	{
		SetType( d.m_nUID );
		SetUID(d.m_nType);
		SetLevel(d.m_nLevel);
		SetName(d.m_Name);
		SetBaseInfo(d.m_BaseInfo);
	}

private:
	DWORD				m_dwModelID;
	std::string			m_ImgFile;		
	sCHAR_IMAGE*		m_pImage;	
};

typedef struct sMemberInfo : NiMemObject
{
public:
	sMemberInfo():m_nMapNo(0),m_nChannelNo(0),m_nSlotNo(0),m_tPos(0,0),m_dPos(0,0),m_bOnline(false),m_bFaraway(false)
	{}
	~sMemberInfo(){}
	void				SetSlotNumber( int const& nSlotNo);
	void				SetMapNumber( int const& nMapID );
	void				SetChannelNumber( int const& nChannelNumber );
	void				SetTamerUID( int const& nTamerUID );
	void				SetDigimonUID( int const& nDigimonUID );
	void				SetTamerPos( nSync::Pos& tPos );
	void				SetDigimonPos( nSync::Pos& dPos );
	void				SetOnOffLine( bool bValue );
	void				SetTamerInfo( sTamerInfo const& sInfo );
	void				SetTamerInfo( TPartyTamerInfo const& sInfo );
	void				SetDigimonInfo( sDigimonInfo const& sInfo );
	void				SetDigimonInfo( TPartyDigimonInfo const& sInfo );	

	void	operator = (sMemberInfo const& d)
	{
		m_nSlotNo = d.m_nSlotNo;
		m_nChannelNo = d.m_nChannelNo;
		m_nMapNo = d.m_nMapNo;
		m_tPos = d.m_tPos;
		m_dPos = d.m_dPos;
		m_bOnline = d.m_bOnline;
		m_TamerInfo = d.m_TamerInfo;
		m_DigimonInfo = d.m_DigimonInfo;
	}

public:
	int					m_nMapNo;
	int					m_nChannelNo;
	int					m_nSlotNo;
	nSync::Pos			m_tPos;
	nSync::Pos			m_dPos;
	bool				m_bOnline;
	sTamerInfo			m_TamerInfo;
	sDigimonInfo		m_DigimonInfo;
	bool				m_bFaraway;
}SMemberInfo;

typedef std::list<SMemberInfo*>					LIST_MEMBER_INFO;
typedef std::list<SMemberInfo*>::iterator		LIST_MEMBER_INFO_IT;
typedef std::list<SMemberInfo*>::const_iterator	LIST_MEMBER_INFO_CIT;

typedef struct sPartyInfo
{
	friend class CPartyContents;

public:
	sPartyInfo();
	~sPartyInfo();

	void				InitData();
	bool				IsParty() const;
	UINT				GetPartyIDX() const;
	bool				IsMember( int const& nSlot ) const;
	bool				IsMaster( int const& nSlot ) const;
	std::wstring		GetMemberName( int const& nSlot ) const;
	size_t				GetMemberCount() const;
	bool				IsPartyMaster() const;
	int					GetPartyMasterSlot() const;
	bool				RemovePartyMemberFromSlotNo( int const& nSlot );
	bool				AddNewMemberInfo( TPartyMember const& pData );
	void				DeleteAllMemberInfo();
	bool				IsMyPartySlot( int const& nSlot );
	void				SetPartyMaster( int const& nSlot );
	int					GetMemberSlotNumFromName(std::wstring const& nName);
	int					GetMyPartySlotNumber() const;
	bool				IsPartyMemberFromUIDX(bool bTamer, int const& nUIDX) const;
	SMemberInfo const*  GetMemberInfoFromUIDX( int const& nUIDX ) const;
	SMemberInfo const*  GetMemberInfoFromSlotNo( int const& nSlot ) const;

	LIST_MEMBER_INFO const& GetList() const;

	void				CheckLeaveMemberInfo( std::list<TPartyMember> const& checkList, std::map<int,std::wstring> & slotList );
	void				CheckNewMemberInfo( std::list<TPartyMember> const& checkList, std::map<int,std::wstring> & slotList );

protected:
	SMemberInfo *		GetMemberInfo( int const& nSlot );
	void				SetDefaultLootingRule();

protected:
	unsigned int			m_unPartyID;			// 파티 ID
	int						m_nMySlotNumber;		// 내 슬롯 번호
	int						m_nMasterSlotNum;		// 파티장 슬롯 번호
	sLootingRule			m_sLootingRule;
	LIST_MEMBER_INFO		m_listMemberInfo;
}SPartyInfo;

//-------------------------------------------------------------------------------------------------------------------------------------
class CPartyContents
	: public IUIContentSubject< CPartyContents >
{
public:
	enum eEvent
	{
		eMEMBER_POSITION_CHANGE,	// 파티원 위치 수정
		eCHANGE_LOOTING_RULE,		// 파티 아이템 루팅 방식 변경
		eCHANGE_PARTY_MASTER,		// 파티장 변경
		eADD_NEW_PARTY_MEMBER,		// 새로운 파티원 추가
		eUPDATE_MEMBER_INFO,		// 파티원 정보 갱신
		eMEMBER_MAP_CHANGE,			// 파티원 맵 이동
		eMEMBER_DIGIMON_CHANGE,		// 파티원 디지몬 변경
		eMEMBER_BANISH,				// 파티 맴버 추방
		eMEMBER_LEAVE,				// 파티원 나감
		eMEMBER_ONOFFLINE_CHANGE,	// 파티원 온/오프라인 변경
		eADD_MEMBER_LOOTING_ITEM_VIEW,
		eADD_ME_LOOTING_ITEM_VIEW,
		eMEMBER_SET_TARGET,
		eMEMBER_RELEASE_TARGET,
		eMEMBER_FARAWAY,
		eMEMBER_REFRESH,
	};

	struct sLootingItems
	{
		sLootingItems():m_nDiceNumber(0),m_nSlotNum(0),m_dwItemIconType(0),m_dwItemIconIdx(0),m_dwItemType(0),m_nItemCount(1)
		{}		
		~sLootingItems(){}

		int				m_nItemCount;
		int				m_dwItemType;
		DWORD			m_dwItemIconType;
		int				m_dwItemIconIdx;
		int				m_nSlotNum;
		int				m_nDiceNumber;
		NiColor			m_ItemColor;
		sLootingRule	m_sLootingrule;	// 아이템을 획득 하였을 때의 루팅 룰
	};

public:
	static int const IsContentsIdentity(void);

public:
	CPartyContents(void);
	virtual ~CPartyContents(void);

	virtual int const GetContentsIdentity(void) const;
	virtual bool	Initialize(void);
	virtual void	UnInitialize(void);
	virtual bool	IntraConnection(ContentsSystem& System);
	virtual void	Update(float const& fElapsedTime);
	virtual void	NotifyEvent(int const& iNotifiedEvt);
	virtual void	NotifyEventStream(int const& iNotifiedEvt, ContentsStream const& kStream);
	virtual void	MakeMainActorData(void);
	virtual void	ClearMainActorData(void);
	virtual void	MakeWorldData(void);
	virtual void	ClearWorldData(void);


public:
	bool			IsJoinParty() const;
	bool			IsMePartyMaster() const;
	int				GetPartyMasterSlot() const;
	bool			IsSendInvite() const;
	bool			IsPartyMaster( int const& nSlot ) const;
	bool			IsMemberFromSlotNo( int const& nSlot ) const;	// 파티원인가?
	std::wstring	GetMemberNameFromSlotNo(int const& nSlot) const;// 파티원의 이름을 얻어온다.
	int				GetMemberSlotNumFromTamerName(std::wstring const& wsTamerName);

	SPartyInfo const*	GetPartyInfo() const;
	SMemberInfo const * GetMemberInfoFromSlotNo(int const& nSlot) const;// 파티원 정보를 얻는다.
	LIST_MEMBER_INFO const& GetMemberList() const;

	void			Send_LootingRuleChange( int nCropType, int nItemClass, int nItemDispGrade );
	void			Send_LootingItemRateChange( int nItemClass );
	bool			IsMeFromName( std::wstring const& wsName );

	bool			GetMySimpleInfo( SMemberInfo & myInfo );
	bool			IsPartyMemberFromTamerUIDX(int const& nUIDX) const;		// 테이머 UIDX로 파티멤버인지 체크
	bool			IsPartyMemberFromDigimonUIDX(int const& nUIDX) const;	// 디지몬 UIDX로 파티멤버인지 체크

	bool			IsBattlePlaying() const;		// 배틀 중인가?

private:
	void			InitData(void);
	void			Recv_Party_MakeMe(void* pData);			// 내가 파티 생성
	void			Recv_PartyInfoInit(void* pData);
	void			Recv_PartyInfo(void* pData);			// 파티 정보 받음.
	void			Recv_PartyInvite_Result(void* pData);	// 파티 초대에 대한 상대방의 결과
	void			Recv_PartyInvite(void* pData);			// 다른 유저로 부터 파티 초대 받음.
	void			Recv_AddNewPartyMember(void* pData);	// 새로운 파티원 추가
	void			Recv_PartyLeave(void * pData);			// 파티 탈퇴 결과
	void			Recv_PartyMemberBanish(void* pData);	// 파티 멤버 강퇴 결과
	void			Recv_ChangeMaster(void* pData);			// 파티장 변경.
	void			Recv_PartyItemLootingInfoChange(void* pData);// 파티 아이템 루팅 정보 변경

	void			Send_PartyInvite_TamerName(void* pData);	// 테이머 이름으로 파티 초대 패킷 보냄
	void			Send_PartyInvite_UniqueID(void* pData);		// 테이머 UIDX로 파티 초대 패킷 보냄
	void			Send_PartyLeave(void * pData);				// 파티 탈퇴 패킷 보냄	
	void			Send_PartyInviteAnswerAccept(void* pData);	// 파티 초대 승락
	void			Send_PartyInviteAnswerReject(void* pData);	// 파티 초대 거절
	void			Send_PartyMemberBanish(void* pData);	// 파티 멤버 강퇴 보냄
	void			Send_ChangeMaster(void* pData);			// 파티장 변경 보냄

	void			NTF_PartyMemberLootItem(void* pData);	// 파티 멤버가 아이템을 획득했다는 패킷
	void			NTF_PartyMebmerMapChange(void* pData);	// 파티 멤버 맵 이동
	void			NTF_PartyMemberInfoChange(void* pData);	// 파티 멤버 정보 변경
	void			NTF_PartyMemberDigimonChange(void* pData);// 파티 멤버 디지몬 변경
	void			NTF_PartyMemberOnOffline(void* pData);		// 파티 멤버 온/오프 라인 정보
	void			NTF_PartyMemberBuffChange(void* pData);// 파티 멤버 버프 정보 변경
	void			NTF_PartyMemberPositionUpdate(void* pData);// 파티원 위치 정보 갱신
	void			NTF_PartyExpUpdate(void* pData);			// 파티 일때 추가 경험치 패킷

	void			GetPartyMemberNameFromIdx(void* pData);		// 
	void			GetLootingInfo(void*pData);
	void			OpenLootingWindow(void * pData);
	void			OpenLootingItemRateWindow(void* pData);
	void			GetPartyMemberInfo( void* pData );

	void			IsPartyMemberFromTamerUIDX(void* pData);
	void			IsPartyMemberFromDigimonUIDX(void* pData);

	void			CheckPartyMemberTargetMark( void* pData );
	void			ReleasePartyMemberTargetMark(void* pData);
	void			GetPartyUIDX(void* pData);
	
private:
	void			_CheckMemberFaraway();
	void			_ClosePartyUI();
	void			_OpenPartyUI();
	void			_SendPartyInvate( std::wstring const& wsName ); // 파티 초대 패킷을 보냄
	void			_SetPartyLooting(TItemLootingInfo const& pData);// 파티 루팅 정보 셋팅
	void			_SetMasterSlotNumber( int const& nSlot );// 파티장 슬롯 번호
	bool			_CheckInviteTamerState();			// 파티 초대를 받을 수 있는 상태 인지 체크하는 함수

	void			_SendLootingRule( sLootingRule const& nRule );

	bool			_IsRequestTamer( std::wstring const& requestTamer );
	void			_RemoveRequestTamer( std::wstring const& requestTamer );
	void			_AddRequestTamerList( std::wstring const& addTamer );

private:
	std::wstring					m_wsInviteTamerName;	// 나를 파치 초대한 테이머 이름
	std::map<std::wstring,float>	m_wsRequestTamersName;	// 파티 초대 보낸 테이머 이름들
	SPartyInfo						m_PartyData;

	int								m_pSelectedMemberSlot;

	TOwnershipGuard< AdaptBattleSystem >	m_AdaptBattleSystem;
	TOwnershipGuard< AdaptTutorialQuest >	m_AdaptTutorialQuest;
	TOwnershipGuard< AdaptGotchaMachine >	m_AdaptGotchaMachine;
	TOwnershipGuard< AdaptMacroProtect >	m_AdaptMacroProtect;
};
