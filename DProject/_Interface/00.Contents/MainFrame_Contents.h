#pragma once

#include "../../ContentsSystem/ContentsSystem.h"

class AdaptTutorialQuest;

//-------------------------------------------------------------------------------------------------------------------------------------
class CMainFrameContents
	: public IUIContentSubject< CMainFrameContents >
{
public:
	static int const IsContentsIdentity(void);

	enum eEvent
	{
		eUpdate_Current_XaiGuage,
		eUpdate_Max_XaiGuage,
		eChange_QuickSlot_Expand,
		eChange_Near_Npc,
#ifdef UI_INVENTORY_RENEWAL
		eNotifyNew,
#endif

		eSetTab,
		eUpdateStatus,
#ifdef CROSSWARS_SYSTEM
		eOpenCrossLoader,
		eChangeCInvenSize,
		eChangeCrossTactics,
#endif
#ifdef USE_BARCODE_REDER
		eUseScanner,
#endif

		eUpdateStat,
		eUpdateSkill,
		eChangeDigimon,

		eSetTarget,
		eSetTargetMonster,
		eReleaseTarget,
		eUpdateTarget,
	};

public:
	CMainFrameContents(void);
	virtual ~CMainFrameContents(void);

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
	std::string		GetTamerImgFile();
	std::string		GetDigimonImgFile();

	bool			SendXGToCrystal();
	bool			SendCrystalToXG();

	unsigned short  GetMaxCrystal() const;
	unsigned int	GetMaxXGuage() const;

	unsigned short  GetCurrentCrystal() const;
	unsigned int	GetCurrentXGuage() const;
	unsigned int	GetConsumChargeValue();

private:
	void			_ChangeEquip(void* pData);
	void			_XGuageUpdate(void* pData);
	void			_MaxXGuageUpdate(void*pData);
	void			_RecvXGToCrystalResult(void* pData);
	void			_RecvCrystalToXGResult(void* pData);
	void			_ChangeQuickExpand(void* pData);

	void			_OpenCloseXaiGuageWindow( bool bVisibe );

	void			_SetBalloonTitle(void* pData);

	void			_SetActionKeyNpc(void* pData);
	bool			_CheckTamerState();

public:
	void			StartTalkNpc();

private:
	UINT			m_nFTID;

	//-------------------------------------TamerStatus-------------------------------------------
public:
	enum eTamerStatusTab
	{
		eTamer,
		eDigivice,
		eSkill,
		eCross
	};

	enum eSkillType{ eBasic, eActive, ePassive, };
	enum eAbilType{ eAT, eDE, eHP, eDS, eLV, eFS, eAS, eCT, eHT, eBL, eEV, eMaxCR, eCurCR };

private:
	int					m_nCurTab;
	int					m_nDeleteDigimonID;

private:
	void			_ResetTamerStatus();

	// Event Function
	void			_OpenCloseTamerStatus( void* pData );
	void			_UpdateTamerStatus( void* pData );
	void			_RequestDeleteDigimon( void* pData );

public:
	void			CloseTamerStatus();
	void			DeleteDigimon( std::wstring wsPass );

	int				GetFriendShip() const;

	int				GetTamerStat( eAbilType eType ) const;
	void			GetBaseStat( int& nType, int& nBaseStat, eAbilType eType );

	std::wstring	GetTamerName() const;
	std::wstring	GetDigimonName() const;

	CsCoolTimeSeq*	GetFTItemCoolTime( uint nType ) const;

	cItemInfo*		GetEquipItem( int nEquipType ) const;
	uint			GetEquipItemType( int nEquipType ) const;

	int				GetOpenedChipsetSlot() const;
	cItemInfo*		GetChipsetItem( int nChipsetIndex ) const;

	int				GetSkillType( int nIndex, eSkillType eType ) const;
	int				GetSkillIndex( int nIndex, eSkillType eType ) const;
	CsCoolTimeSeq*	GetSkillCoolTime( int nIndex ) const;

	int				GetTacticsCount() const;
	int				GetTacticsOpenCount() const;
	int				GetTacticsLevel( int nTacticsIndex ) const;
	uint			GetTacticsType( int nTacticsIndex ) const;
	std::string		GetTacticsFileName( int nTacticsIndex ) const;
	int				GetTacticsHatchLv( int nTacticsIndex ) const;

	bool			IsCashSkill( int nSkillIndex );
	bool			IsEquipDigivice() const;

	void			WearEquipItem( int nIconSlot );
	void			MoveEquipItem( int nEquipType );
	void			MoveChipsetItem( int nIconSlot, int nChipsetIndex, bool bToInven, bool bFromChip );

	void			OnClickTactitcs( int nTacticsIndex );

	void			UseSkill( int nSkillIndex );
	void			DeleteSkill( int nSkillIndex );

#ifdef CROSSWARS_SYSTEM
private:
	struct sCrossInfo
	{
		sCrossInfo() : nCrossIndex( -1 ), nCrossRankID( 0 ){}
		~sCrossInfo(){}

		bool operator()( const sCrossInfo& lhs, const sCrossInfo& rhs )
		{
			return lhs.nCrossRankID < rhs.nCrossRankID;
		}

		int				nCrossIndex;
		int				nCrossRankID;
	};
	std::list< sCrossInfo >	m_lSelectedCross;

	CDigimon*		m_pRenderDigimon;
	bool			m_bWaitRecvCross;

public:
	int				GetCrossInvenMaxCount() const;
	int				GetCrossInvenCount() const;
	cItemInfo*		GetCrossInvenItem( int nCrossIndex ) const;

	int				GetCrossTacticsMaxCount() const;
	int				GetCrossTacticsCount() const;
	DWORD			GetCrossTacticsID( int nCrossIndex ) const;
	std::string		GetCrossTacticsFileName( DWORD dwDigimonID ) const;
	cData_CrossTactics::sCross*	GetCrossTacticsInfo( int nCrossIndex ) const;

	bool			IsSelected( int nIndex ) const;
	bool			IsCross() const;
	bool			IsCrossLoader() const;

	CDigimon*		GetRenderDigimon();

	bool			SelectCrossTactics( int nCrossIndex );
	void			OnClickDigiCross();
	void			MoveCrossInvenItem( int nIconSlot, int nCrossIndex, int nIconCount );

private:
	void			_SetCrossTactics();
	void			_SetRenderCrossTactics();

	void			_ReleaseCrossTactics();
	void			_CombineCrossTactics();

	bool			_CheckDigiCross();
	bool			_CheckSelectCrossTactics( int nCrossIndex );

	void			_OpenCrossLoader( void* pData );
	void			_ChangeCrossInvenSize( void* pData );
	void			_ChangeCrossDigimon( void* pData );
#endif

#ifdef USE_BARCODE_REDER
	void			_UseGameScanner( void* pData );
#endif

	//-------------------------------------DigimonStatus-------------------------------------------
private:
	void			_OpenCloseDigimonStatus( void* pData );
	void			_UpdateDigimonStat( void* pData );
	void			_UpdateDigimonSkill( void* pData );
	void			_ChangeDigimon( void* pData );
	void			_RecvDigimonSkillLevelUp(void* pData);

public:
	void			OpenCloseHelpWindow();
	void			CloseDigimonStatus();
	void			LevelUpDigimonSkill( int nIndex );
	void			DeleteDigimonSkill( int nIndex );
	void			OpenEvolveDigimon( int nIndex );
	void			OpenRideDigimon( int nIndex );
	void			RequestEvolveDigimon( int nIndex );
	void			RewardAttributeNormal( int nState );
	void			RewardAttributeNature( int nState );

	bool			IsXAnti() const;
	bool			IsXAnti( DWORD dwID ) const;
	bool			IsUseSkill( int nIndex ) const;
	bool			IsSkillLevelUp( int nIndex );
	bool			IsUpperDigimonBaseStat( eAbilType eType ) const;
	bool			IsActiveEvoSlot( CDigimonEvolveObj* pEvo, int nIndex ) const;
	bool			IsActiveOpenRide( int nIndex ) const;
	bool			IsOpenedRide( int nIndex ) const;
	bool			IsActiveRewardNormal( int nState ) const;
	bool			IsActiveRewardNature( int nState ) const;
	bool			IsActiveDigimonParts( int const& nParts ) const;

	void			GetDigimonSizeInfo( int& nTall, float& fPercent, bool& bIsBuff );
	void			GetDigimonRideInfo( bool& bIsRide, int& nImageIndex );
	void			GetDigimonStatInfo( eAbilType eType, int& nType, int& nBase, int& nChipset, int& nFriendShip );
	void			GetDigimonNatureInfo( int nMyType, int& nUpType, int& nDownType );

	int				GetDigimonHatchLv() const;
	int				GetNormalAttributeType() const;
	int				GetNatureAttributeType() const;
	DWORD			GetNormalAttributeExp( int nState ) const;
	DWORD			GetNatureAttributeExp( int nState ) const;

	std::string		GetEvoFileName( int nIndex ) const;

	CsDigimon::sINFO*			GetDigimonFTInfo() const;
	CsDigimon::sINFO*			GetEvoDigimonFTInfo( int nIndex ) const;
	CDigimonUser::sUSER_STAT*	GetDigimonBaseStat() const;
	CDigimonUser::sENCHENT_STAT*	GetDigimonEnchantStat() const;

	cEvoUnit*					GetEvoUnit() const;
	CDigimonEvolveObj*			GetEvoCurObject() const;
	CDigimonEvolveInfo*			GetEvoInfo() const;
	CsSkill::sINFO*				GetDigimonFTSkillInfo( int nIndex );

	//-------------------------------------TargetUI-------------------------------------------
public:
	enum eCompareResult
	{
		eUpper,
		eLower,
		eEqual,
		eCompError,
	};

private:
	CsC_AvObject*		m_pTarget;

private:
	void				_SetTargetUI( void* pData );
	void				_ReleaseTargetUI( void* pData );
	void				_UpdateTargetUI( void* pData );

	void				_ConnectBuffInfo( CsC_AvObject* pTarget );
	bool				_CheckTargetOwner( CsC_AvObject* pTarget );

public:
	void				CloseTargetUI();
	void				OpenPopUpWindow();
	bool				ReleaseTarget();

	CsC_AvObject*		GetTargetObject() const;
	void				GetTargetName( std::wstring& wsName, std::wstring& wsPenName );
	void				GetTargetStat( int& nLevel, float& fHpRate);
	std::string			GetTargetImgFile();
	UINT				GetExpireTime() const;
	UINT				GetTotalExpireTime() const;

	CsMonster::sINFO*	GetMonsterFTInfo( DWORD const& dwDigimonID ) const;

	int					GetNatureAttributeType( DWORD const& dwDigimonID ) const;
	int					GetNormalAttributeType( DWORD const& dwDigimonID ) const;
	int					GetNatureAttributeIndex( DWORD const& dwDigimonID ) const;
	int					GetNormalAttributeIndex( DWORD const& dwDigimonID ) const;
	eCompareResult		GetNatureCompare( DWORD const& dwDigimonID ) const;
	eCompareResult		GetNormalCompare( DWORD const& dwDigimonID ) const;

private:
	TOwnershipGuard< AdaptTutorialQuest >		m_pAdaptTutorial;
};
