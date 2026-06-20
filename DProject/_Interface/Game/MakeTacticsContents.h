//2017-04-11-nova
#pragma once

class AdaptTutorialQuest;

class MakeTacticsContents: public IUIContentSubject< MakeTacticsContents >
{
public:
	MakeTacticsContents(void);
	virtual ~MakeTacticsContents(void);
	virtual int const GetContentsIdentity(void) const;
	virtual bool Initialize(void)							{ return true; }
	virtual void UnInitialize(void)							{}
	virtual bool IntraConnection(ContentsSystem& System);
	virtual void Update(float const& fElapsedTime)					{}
	virtual void NotifyEvent(int const& iNotifiedEvt)		{}
	virtual void MakeMainActorData(void)					{}	
	virtual void ClearMainActorData(void)					{}
	virtual void MakeWorldData(void)						{}
	virtual void ClearWorldData(void)						{}

	static int const IsContentsIdentity(void);

	bool			IsTutorialPlaying() const;

protected:
	virtual void NotifyEventStream(int const& iNotifiedEvt, ContentsStream const& kStream) {}

protected:
	cItemInfo		m_pDigimonEgg;		
	cItemInfo		m_pBackupDisk;

	int				iSuccessLevel;
	int				iDigimonEggIdx;		//디지몬알 인덱스
	int				iBackupDiskIdx;		//백업디스크 인덱스
	int				iRequireDataCnt;
	int				iDataFlag;			// 중급 데이터를 사용하지 않는 버전은 DATAFLAG 0
	int				iDataFlagMax;		// 데이터 플래그 최종 값.
	int				iEnableGradeMax;	// 디지타마 ID에 따른 알의 최종 부화 가능 단수

	bool			bWaitRecvServer;

public:
	void			Recv_DigimonEgg( void* pData );
	void			Recv_DigimonEggLevel( void* pData );
	void			Recv_DigimonSuccessLevel( void* pData );

	void			Get_DigimonEggCount(void* pData);
	void			Get_DigimonEggIsEnable(void* pData);
	void			Get_DigimonEggType(void* pData);

	void			DataInputFailed();
	void			SetSuccessLevel();

	void			ResetDigimonEgg();
	bool			AutoResistDigiMonEgg();
	bool			InputDataDigimonEgg(int invenIDX, n4 NpcID, bool bVipMemberShip);
	void			RemoveDigimonEgg();	

	cItemInfo*		GetDigimonEgg();
	uint			GetDigimonEggType(); 
	uint			GetDigimonEggCount(); 

	void			ResetBackupDisk();
	bool			AutoResisitBackupDisk();
	bool			InputBackupDisk(int invenIDX, n4 NpcID, bool bVipMemberShip);
	void			RemoveBackupDisk();	

	cItemInfo*		GetBackupDisk();
	uint			GetBackupDiskType(); 
	uint			GetBackupDiskCount(); 

	void			SetWaitRecvServer(bool wait);
	bool			GetWaitRecvServer();

	void			AddDataFlag();
	void			SetDataFlag(int flag);
	int				GetDataFlag();

	void			SetDataFlagMax(int max);
	int				GetDataFlagMax();

	void			SetEnableGradeMax(int grade);

	int				GetDigiMonEggIndex();
	int				GetBackupDiskIndex();

	int				IsDigimonEggResist(int nInvenIndex);
	int				IsBackupDiskResist(int nInvenIndex);

	bool			TryLevelUp1();
	bool			TryLevelUp2();
	bool			Send_Server(int NpcID, bool bVipMemberShip);

	int				Resist_Digitama(int invenIDX, int NpcID, bool bVipMemberShip);
	int				Resist_BackupDisk(int invenIDX, int NpcID, bool bVipMemberShip);
	void			Requite_Tactics(TCHAR* szName, int NpcID, int nPotableIndexp);
	void			DestroyResist_Digitama(int NpcID, bool bVipMemberShip);
	void			Send_MiniGameCheck();
	void			Cancel_BackupDisk(int NpcID, bool bVipMemberShip);
	void			Cancel_Digitama(int NpcID, bool bVipMemberShip);

private:
	TOwnershipGuard< AdaptTutorialQuest >	m_AdaptTutorialQuest;

#ifdef MINIGAME
public:
	enum EventMiniGame
	{
		_Start,			
		_ClickResult,		
		_TimeOut,	
		_OnEndMiniGame,	
		_EndMiniGame,	
	};
	enum EventMakeTactics
	{
		_CloseMiniGame= EventMiniGame::_EndMiniGame,
		_EndMakeTactics,
	};

	void			Send_MiniGame_End(int iValue);
	void			Send_MiniGame_TimeOut(void);
	void			Send_MiniGame_ClickBt(uchar nGameKind, ushort nBarIndex);

	void 			ShowMinigame(void);
	void 			HideMinigame(void);


private:
	void			Recv_DigimonEgg_MiniGame_Start(void* pData);
	void			Recv_DigimonEgg_MiniGame_ClickResult(void* pData);
	void			Recv_DigimonEgg_MiniGame_TimeOut(void* pData);
	void			Recv_DigimonEgg_MiniGame_ErrorMessage(void* pData);
	void			Recv_DigimonEgg_MiniGame_End(void* pData);
	void			Recv_DigimonEgg_Minigame_Info(void* pData);
	void			UpdateMinigame(void);
	void 			SetInvenMoney(void);
	void 			ShowMinigameResultMsg(const int& ResultCnt);

public:	//inlines
	const ushort & GetStartBarTime(void) const;
	const ushort & GetClickResultNextBarIdx(void) const;
	const ushort & GetNextBarChargingTime(void) const;
	const ushort & GetTimeOutNextBarTime(void) const;
	const ushort & GetMinigameSuccessCnt(void)const;
	const uchar & GetSuccessCnt(void)const;
	const bool & GetClickResult(void)const;
	const bool & GetMinigameComp(void)const;

	void SetClickResult(const bool & iValue);
	void SetStartBarChargingTime(const ushort& iValue);
	void SetClickResultNextBarIdx(const ushort &iValue);
	void SetNextBarChargingTime(const ushort &iValue);
	void SetTimeOutNextBarTime(const ushort &iValue);
	void SetSuccessCnt(const uchar &iValue);
	void SetMinigameComp(const bool& bisGame);
	void SetMinigameSuccessCnt(const int& iSuccessCnt);

private:
	ushort iMinigame_StartBarTime;
	ushort iMiniGame_ClickResult_NextBarIdx;
	ushort iMiniGame_NextBarChargingTime;
	ushort iMiniGame_TimeOut_NextBarTime;
	uchar iMiniGame_SuccessCnt;
	bool bMiniGame_ClickResult;

	ushort		m_nSuccessCnt;	//성공 카운트
	bool		m_nIsGame;//true 면 현재 레벨에서 미니게임을 한 것, false면 아직 안한 것
#endif

};

#include "MakeTacticsContects.inl"