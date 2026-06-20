#pragma once

#include "../../ContentsSystem/ContentsSystem.h"

struct sDamageMeterInfo
{
	sDamageMeterInfo(UINT hitter, UINT target, int damage):m_nHitterUidx(hitter),m_nTargetUidx(target),m_nDamage(damage)
	{};
	~sDamageMeterInfo() {};

	UINT		m_nHitterUidx;
	UINT		m_nTargetUidx;
	int			m_nDamage;
};

//-------------------------------------------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------------------------------------------
class DamageMeterContents
	: public IUIContentSubject< DamageMeterContents >
{
public:
	enum eDISP_TYPE
	{ 
		DAMAGE_DONE,			// 준 데미지
		DAMAGE_DPS,				// DPS
		DAMAGE_TAKEN			// 받은 데미지
	};

	enum eEventType
	{
		eClear_Damage_Data,
		eUpdate_View_Type,
		eChange_ViewType,
	};

	struct sRankBase
	{
	public:
		sRankBase();
		virtual ~sRankBase();

		virtual bool	Update(float const& fDeltime);
		void			SetName( std::wstring name );
		std::wstring	GetName() const;

		virtual void	AddValue( uint const& nValue ) = 0;
		virtual __int64 GetValue() const = 0;

		void			SetIsMe( bool bValue );
		void			SetFaraway( bool bValue );

		bool			IsMe() const;
		bool			IsFaraway() const;

	protected:
		std::wstring	m_wsName;	// 테이머 이름
		__int64			m_nValue;	// 값
		bool			m_bIsMe;	// 나 인가?
		bool			m_bFaraway;	// 멀리 있는가?		
	};

	struct sRankData : public sRankBase
	{
	public:
		sRankData();
		~sRankData();

		virtual void	AddValue( uint const& nValue );
		virtual __int64	GetValue() const;
	};

	struct sDPSRankData : public sRankBase
	{
		sDPSRankData();
		~sDPSRankData();

		virtual bool		Update(float const& fDeltime);
		virtual void		AddValue( uint const& nValue );
		virtual __int64		GetValue() const;

		void				SetDPSOn();
		void				SetDPSOff();

	private:
		bool				m_bBattleOn;
		float				m_fBattleTime;	// 전투가 진행 중인 시간
		__int64				m_nBattleOnDPS;	// 전투중 평균 DPS
		__int64				m_nBattleOnDamage;	// 전투중에 준 총 데미지
	};

	typedef std::list<sRankBase*>					LIST_RANKDATA;
	typedef std::list<sRankBase*>::iterator			LIST_RANKDATA_IT;
	typedef std::list<sRankBase*>::const_iterator	LIST_RANKDATA_CIT;

public:
	static int const IsContentsIdentity(void);

public:
	DamageMeterContents(void);
	virtual ~DamageMeterContents(void);

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

public:
	void					ClearDamageMeterData();
	void					ChangePrevViewType();
	void					ChangeNextViewType();
	int						GetCurrentViewType() const;
	LIST_RANKDATA const&	GetViewData(int const& nViewType) const;

	void					CloseMeterWindow();

private:
	void					_ResetData();
	void					toggleWindow(void* pData);
	void					Add_DamageMeterData(void* pData);

	void					DigimonBattleOn(void* pData);
	void					DigimonBattleOff(void* pData);
	void					DPSAllStop(void* pData);

	void					_SetBattleOn( std::wstring const& name );
	void					_SetBattleOff( std::wstring const& name );

	void					_AddDoneDamage( std::wstring const& name, uint const& damage, bool bIsMe );
	void					_AddTakenDamage( std::wstring const& name, uint const& damage, bool bIsMe );

	void					_NotifyUIUpdate(int const& nUpdateType);

	sRankBase*				_FindRankData( LIST_RANKDATA& findList, std::wstring const& name );

	void					_ClearListData( LIST_RANKDATA& delList );
	void					_SortRankData(LIST_RANKDATA & listData);

	void					_AllStopDPS();
	void					_StopDPSOther();
	void					_StopDPSFormName(std::wstring const& name);
	void					_RankDataSort(int const& nViewType);

	template <typename T>
	sRankBase* _AddRankData( LIST_RANKDATA& findList, std::wstring const& name )
	{
		T * pNewRank = new T;
		pNewRank->SetName(name);
		findList.push_back( pNewRank );
		return pNewRank;
	}

private:
	TOwnershipGuard<AdaptPartySystem>		m_pAdaptParty;

	float									m_fUpdateTime;
	int										m_nCurrentViewType;

	LIST_RANKDATA							m_DamageDoneRank;		// 준 데미지 랭킹
	LIST_RANKDATA							m_DamageDPSRank;		// 데미지 DPS 랭킹
	LIST_RANKDATA							m_DamageTakenRank;		// 입은 데미지 랭킹
	bool									m_bOpenedMeter;
};

