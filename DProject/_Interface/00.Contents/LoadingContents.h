#pragma once

#include "../../ContentsSystem/ContentsSystem.h"
class AdaptBattleSystem;
class AdaptTutorialQuest;

class LoadingContents
	: public IUIContentSubject< LoadingContents >
{
public:
	enum eEventType
	{
		eLoadingProgress,
		//eBattlePlayerLoadEnd,
	};

	struct sBattleLoadingInfo
	{
		sBattleLoadingInfo():m_bBattleLoading(false), m_cMyTeamCode(0),m_nBattleType(0),m_nBattleMode(0)
		{}
		~sBattleLoadingInfo(){};

		bool						IsBattleLoading() const;
		void						ResetData();
		void						SetBattlePlayerInfo( BWait::sWait::WAIT_PLAYER_LIST_CON const& info );
		
		bool						m_bBattleLoading;
		char						m_cMyTeamCode;
		unsigned int				m_nBattleType;
		unsigned int				m_nBattleMode;
		BWait::sWait::PLAYER_INFO	m_myBattleTeam;
		BWait::sWait::PLAYER_INFO	m_otherBattleTeam;
	};

public:
	static int const IsContentsIdentity(void);

public:
	LoadingContents(void);
	virtual ~LoadingContents(void);

	virtual int const GetContentsIdentity(void) const;

	virtual bool	Initialize(void);
	virtual void	UnInitialize(void);
	virtual bool	IntraConnection(ContentsSystem& System);
	virtual void	Update(float const& fElapsedTime);
	virtual void	NotifyEvent(int const& iNotifiedEvt);
	virtual void	NotifyEventStream(int const& iNotifiedEvt, ContentsStream const& kStream);
	virtual void	MakeMainActorData(void);
	virtual void	ClearMainActorData(void);
	virtual void	ClearWorldData(void);

	std::string		GetLoadMapImgFile();
	std::string		GetTipImgFile();
	std::wstring	GetLoadingTipString();

	sBattleLoadingInfo const& GetBattleLoadingInfo() const;

private:
	void			Send_Portal_Move(void* pData);

	void			SetLoadingProgress(void* pData);
	void			RecvPlayerDataLoaded(void* pData);
	void			DataLoadStart(void* pData);
	void			LoadingStart(void* pData);

private:
	void			_SetLoadingProgressValue( int const& nValue );
	bool			_DataProcess_Update();
	void			_SetCameraSet();

	void			_DataLoadComplete();
	void			_ResetData();

	void			_SetBattleLoadingInfo();

private:
	TOwnershipGuard< AdaptBattleSystem >	m_AdaptBattleSystem;
	TOwnershipGuard< AdaptTutorialQuest >	m_AdaptTutorialSystem;

	int										m_nLoadingProgress;
	bool									m_bLoadStart;

	sBattleLoadingInfo						m_BattleLoadingInfo;

private:
	void					TableLoadSuccess(void* pData);
	bool					_CheckMapPathHash( DWORD const& nMapIDx, std::string const& sPath );
	std::map<DWORD,size_t>	m_MapNameList;
};
