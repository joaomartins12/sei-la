#pragma once
#include "../../ContentsSystem/ContentsSystem.h"

#define GUILD_MEMO_SIZE		32

class AdaptTutorialQuest;

class cCommunityContent
	: public IUIContentSubject< cCommunityContent >
{
public:
	enum eEventType
	{
		SetNoticeStr,					//공지 설정
		SetGuildInfo,					//SetGuildInfo관련 정보 갱신
		SetUITab,
		SetGuildPersonCnt,
		MakeHistory,
		SetGuildPerson,
		CloseGuildMenuPopup,
		SetGuildSkill,
	};

	enum eRANK
	{
		GR_NONE = 0,
		GR_MASTER,
		GR_2NDMASTER,
		GR_DAT,
		GR_NORMAL,
		GR_NEW,
		GR_END
	};

	struct sGUILD_INFO 
	{
		sGUILD_INFO() :s_nExp(0), s_nGuildLevel(0), s_nPersonCount(0),s_nCurPersonCount(0),s_nMaxPersonCount(0)
			,s_nIncMember(0), s_n2AndMasterNum(0)
#ifdef GUILD_RENEWAL
			,s_nGuildSkillPoint(0)
#endif
		{ }

		int			s_nGuildLevel;
		int			s_nExp;
		int			s_nPersonCount;
		int			s_nCurPersonCount;
		int			s_nMaxPersonCount;
		int			s_nIncMember;
		int			s_n2AndMasterNum;
#ifdef GUILD_RENEWAL
		uint		s_nGuildSkillPoint;	// 길드스킬포인트
#endif

		std::wstring s_szGuildName;
		std::wstring s_szMasterName;
		std::wstring s_szNotice;
	};

	struct sGUILD_PERSON_INFO : public NiMemObject
	{
		sGUILD_PERSON_INFO() : s_bInit(false), s_bOnline(false), s_eRank(GR_NONE),s_nPersonLevel(0)
			,s_nChannel(0), s_nContribution(0), s_Tick(0.0f),s_dwMapID(0),s_dwTamerType(80001)
#ifdef GUILD_RENEWAL
			, s_nUserRank(0), s_nLastPlayTime(0)
#endif
		{ 

		}

		bool		s_bInit;	//게임 접속 알림변수
		float		s_Tick;
		bool		s_bOnline;
		DWORD		s_dwTamerType;
		int			s_nChannel;
		eRANK		s_eRank;
		int			s_nPersonLevel;
		int			s_nContribution;
		DWORD		s_dwMapID;		
		std::wstring s_szName;

#ifdef GUILD_RENEWAL
		USHORT			s_nUserRank;		// 공헌랭크
		uint			s_nLastPlayTime;	// 최종접속시간
		std::wstring	s_szGuildMemo;;	// 길드원 메모
#endif
	};

	struct sHISTORY_INFO : public NiMemObject
	{
		sHISTORY_INFO():s_nType(0), s_nMasterClass(0),s_nMemberClass(0)
		{}

		int		s_nType;		
		int		s_nMasterClass;
		int		s_nMemberClass;
		std::wstring	s_szMemberName;
		std::wstring	s_szMasterName;

		// 시간 정보
		uint	s_nTime;     		
	};
#ifdef GUILD_RENEWAL
	//사용중인 스킬 정보
	struct sGUILDSKILL_INFO
	{
		sGUILDSKILL_INFO():s_nSkillCode(0),s_nEndTime(0),s_nCount(1)
		{  }

		uint	s_nSkillCode;	// 스킬코드( 비교용 )
		uint	s_nEndTime;		// 남은 사용 시간( 사용중이 아닐 때 : 0, 사용중일때 : 종료 시간 ) - Base스킬일 땐 쿨타임 끝나는 시간
		u1		s_nCount;		// 스킬 중첩 수 ( 기본 1, 중첩 시 1씩 증가 )
		bool	_IsCoolTime() const;	// 쿨타임 중인가 ( true : 쿨타임 중 , false : 쿨타임 아님 )
	};
	typedef		std::map< uint, sGUILDSKILL_INFO >					UsingSkillMap;
	typedef		std::map< uint, sGUILDSKILL_INFO >::iterator		UsingSkillMapItr;
	typedef		std::map< uint, sGUILDSKILL_INFO >::const_iterator	UsingSkillMapCItr;
#endif
	struct sNAME : public NiMemObject
	{
		sNAME():s_fLifeTime(0)
		{}

		float		s_fLifeTime;
		std::wstring s_szName;
	};

	typedef std::list< sGUILD_PERSON_INFO >						LIST_GUILD;
	typedef std::list< sGUILD_PERSON_INFO >::iterator			LIST_GUILD_IT;
	typedef std::list< sGUILD_PERSON_INFO >::const_iterator		LIST_GUILD_CIT;

public:

	cCommunityContent(void);
	virtual ~cCommunityContent(void);
	// 상속 기본 선언
	virtual int const GetContentsIdentity(void) const;
	virtual bool Initialize(void);
	virtual void UnInitialize(void);
	virtual bool IntraConnection(ContentsSystem& System);
	virtual void Update(float const& fElapsedTime);
	virtual void NotifyEvent(int const& iNotifiedEvt);
	virtual void NotifyEventStream(int const& iNotifiedEvt, ContentsStream const& kStream);
	virtual void MakeMainActorData(void);
	virtual void ClearMainActorData(void);
	virtual void MakeWorldData(void);
	virtual void ClearWorldData(void);
	static int const IsContentsIdentity(void);

	const sGUILD_INFO*	GetGuildInfo() const;
	const sGUILD_PERSON_INFO& GetMyInfo() const;
	bool					IsHaveMyInfo();
	
	std::wstring*					GetRankName( eRANK eRank );
	std::wstring*					GetRankDesc( eRANK eRank );

	void					SetMyChannel(const int nChannel);

	LIST_GUILD*				GetMap();
	sHISTORY_INFO			GetHistoryInfo( int nIdx );
	int						GetHistoryInfoSize();

	//UI에서 표현되는 순서에 대로 List가 들어 있다. 마우스포커스 정보에서 선택된 UI 엘리멘트 번호로, 정보를 가져온다.
	sGUILD_PERSON_INFO*		GetPersonInfo( int nDataIdx );

	// adapt 사용때문에 퍼블릭
	void					SendGuildInvite(void* pData);				// 길드 초대 보내기
	

#ifdef GUILD_RENEWAL
	int						GetUsingSkillValue( int nApply );	// 적용 중인 스킬의 수치 얻어오기
	void					_PersonSort( int nButton, bool bSort );			// 길드인원 정렬
	sGUILDSKILL_INFO const*	GetUsingSkill( u4 nSkillCode ) const;
	bool					IsHaveUsingSkill( u4 nSkillCode );
#else
	void					PersonSort( int nType, int nButton, bool bSort );
#endif
	// 사용 하려는 스킬보다 상위 스킬이 적용 중인지 확인 ( 0 : 같은그룹 아님, 1: 같은 그룹의 상위 존재, 스킬코드: 같은 그룹이지만 하위 스킬 존재 )
	int						CheckActSkill( int nSkillCode );

	sGUILD_PERSON_INFO*		_GetPersonInfo( std::wstring* szName, int& nIdx );	

	void					SetShowOfflinePerson(bool bValue);
	bool					IsShowOfflinePerson() const;

private:
	void					Delete();
	void					DeleteHistory();

	void					SetRankName( eRANK eRank, std::wstring* szName );
	void					SetRankDesc( eRANK eRank, std::wstring* szDesc );

	void					SetGuildName( std::wstring* szGuildName );
	void					SetNotice( std::wstring* szNotice, bool bChat = true );

	void					SetGuildInfoData();	// 길드 정보 변경
	void					SetGuildPersonInfo( sGUILD_PERSON_INFO* pPerson ); // 정보 갱신때 들어오는 사람들

	void					SetLeavePerson( std::wstring* szTamerName, int nType );	// 길드에서 나간 사람들

	void					LeaveClose();

	bool					IsSendInvate( std::wstring* szTamerName );

	void					SetHistory( sHISTORY_INFO sInfo );
	void					SetLevelUp( std::wstring* szTamerName, int nLevel );

	void					SetJoinPerson( sGUILD_PERSON_INFO* pPerson );		// 신규 길드 가입자일때
	void					RecvInvite( std::wstring* szName );

	void					ReName( std::wstring* szFrom, std::wstring* szTo );
	void					SetClassRefresh( std::wstring* szTamerName, eRANK nClass );
	void					_ChangeGuildMaster( std::wstring* szName );
	void					_Change2ndMaster( std::wstring* szName );
	void					_ChangeDatMember( std::wstring* szName );
	void					_ChangeNormal( std::wstring* szName );
	void					_ChangeNewbi( std::wstring* szName );

	eRANK					GetRank( std::wstring* szName );
	void					SetGuildOnOff( bool bValue, std::wstring* szTamerName, DWORD nMapID, int nChannel, int nLevel );

#ifdef GUILD_RENEWAL
	void					SetUsingSkill( bool bUsing , u4 nSkillCode, u4 nEndTime = 0, u4 nUseTime = 0 );
	void					SetBaseSkill( int nLevel );			// 레벨에 따라 Base길드스킬 -> 적용 스킬 리스트에 넣기
	NiColor					CheckPasiveSkill();					// Base스킬 중 패시브 스킬 능력치 적용 
	bool					IsActSkillExist();					// 액티브스킬 존재 유무( 길드버프 아이콘 표시 확인을 위함 )
#endif

private:
	sGUILD_INFO							m_GuildInfo;		//길드 정보
	//sGUILD_PERSON_INFO*					m_MyInfo;			//자기자신 정보

	LIST_GUILD							m_ListPersonInfo;	//길드원 정보
	std::list< sHISTORY_INFO >			m_HistoryInfoList;	//길드 히스토리 정보(가입/탈퇴/길드렙업/길장변경 등)

	typedef std::map<enum eRANK, std::wstring>					RankStrMap;
	typedef std::map<enum eRANK, std::wstring>::iterator		RankStrMapItr;
	typedef std::map<enum eRANK, std::wstring>::const_iterator	RankStrMapCItr;

	RankStrMap							m_GuildRankNameMap;
	RankStrMap							m_GuildRankDescMap;

	std::list< sNAME >					m_listInviteName;

	bool								m_bNoticeShow;
	bool								m_bShowOfflinePerson;	// 오프라인 인원 표시 여부

#ifdef GUILD_RENEWAL
	UsingSkillMap	m_mapUsingSkill;					// 현재 적용 중인 스킬 (맵)
#endif
private:
	void		GetGuildInfo(void* pData);					// 길드 정보 가져오기
	void		GetMyInfo(void* pData);						// 내 길드 정보 가져오기

	void		CheckIncreaseMemberMax(void* pData);		// 길드원 최대수 증가 아이템 사용전 검사
	
	void		RecvGuildInfo(void* pData);					// 길드 정보 받음(맵이동/접속)
	void		RecvGuildDelete(void* pData);				// 길드 삭제 받음
	void		RecvGuildMemberMAXIncrease(void* pData);	// 최대 길드원수 받음
	void		RecvMapNo(void* pData);						// 맵번호 받음
	void		RecvGuildLeave(void* pData);				// 길드 떠나기(자기자신) 받기
	void		SendGuildLeave(void* pData);				// 길드 떠나기(자기자신) 보내기
	
	void		RecvGuildCreate(void* pData);				// 길드 개설 받기
	void		GetSetPassiveSkill(void* pData);			// 길드 패시브 스킬의 색을 설정하고 가져온다.
	void		CheckGuildDestory(void* pData);				// 길드 삭제전 재질문
	void		SetPersonLevel(void* pData);				// 길드원 레벨 변경
	void		GetPersonInfo(void* pData);					// 길드원 정보 가져오기

	void		GetRankName(void* pData);					// 길드 랭크 네임 가져오기
	void		GetRankDesc(void* pData);					// 길드 랭크 설명 가져오기
	void		GetIsActiveSkill(void* pData);				// 액티브 스킬 소유 여부 가져오기
	
	void		RecvGuildNoticeChange(void* pData);			// 길드 공지 변경
	void		RecvGuildHistory(void* pData);				// 길드 활동 기록 받음

	void		RecvGuildMemberLevelUp(void* pData);		// 길드원 레벨 업 받음
	void		SendGuildDestroy(void* pData);				// 길드 폐쇄 보내기
	void		RecvGuildDestroyBC(void* pData);			// 길드 삭제 주변인 모두 받음(BC 브로드캐스트)
	void		RecvGuildAdd(void* pData);					// 길드 추가
	void		RecvGuildInviteFail(void* pData);			// 길드 초대 실패 받음
	void		RecvGuildReject(void* pData);				// 길드 초대 거절
	void		RecvGuildMemberDelete(void* pData);			// 길드 멤버 삭제
	void		RecvGuildTamerNameChange(void* pData);		// 이름 변경 길드에 적용 받음
	void		SendGuildBan(void* pData);					// 길드 탈퇴 시키기(길드장만 가능)
	void		RecvGuildOn(void* pData);					// 길드 ON 받음
	void		RecvGuildOff(void* pData);					// 길드 OFF 받음
	void		SendGuildChangeClassName(void* pData);		// 길드 등급명 변경 보내기
	void		RecvGuildChangeClassName(void* pData);		// 길드랭크 변경 받음
	void		RecvGuildRankSet(void* pData);				// 길드 등급 임명 받음
	void		SendGuildRankSet(void* pData);				// 길드 등급 임명 보내기
	void		RecvGuildSkillSet(void* pData);				// 길드 스킬 사용여부 받음


	void		RecvGuildSummon(void* pData);				// 길마(부길마) 소환 받음
#ifdef GUILD_RENEWAL
	void		SendGuildMemo(void* pData);					// 길드 메모 보내기
	void		RecvGuildSkillPoint(void* pData);			// 길드 스킬포인트 받음
	void		RecvGuildLevelUp(void* pData);				// 길드 레벨 업 받음
	void		GetUsingSkillMap(void* pData);				// 사용중인 스킬이 저장된 MAP가져 오기
	void		GetUsingSkillValue(void* pData);			// 적용 중인 스킬로 인한 변화값 가져오기
	void		RecvGuildChangeMemo(void* pData);			// 길드원 메모 받음
	void		GetUsingSkill(void* pData);					// 사용중인 스킬 skill code에 따라 가져오기
	void		RecvGuildSkillReset(void* pData);			// 2017-06-29-nova 길드스킬 초기화
#endif

private:
	TOwnershipGuard< AdaptTutorialQuest >	m_AdaptTutorialQuest;
};
