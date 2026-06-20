#pragma once
#include "../../ContentsSystem/ContentsSystem.h"

class cEvolutionStoreContents
	: public IUIContentSubject< cEvolutionStoreContents >
{
public:
	static int const IsContentsIdentity(void);


	struct sItemToDigimonData
	{
	public:
		sItemToDigimonData():m_dwMakeDigimonTbIDX(0)/*, m_bIsSenderData(false)*/	{};
		~sItemToDigimonData() {};

		//bool IsEnableSend() const;
		void ResetData();
		void SetMakeDigimonTable( DWORD const& dwDigimonTbIDX );
		void SetMakeDigmonName(std::wstring const& wsName);
		//void SetSender();
		//bool IsSend() const;

	private:
		DWORD			m_dwMakeDigimonTbIDX;		// 만들려고 하는 디지몬의 테이블 인덱스 번호
		std::wstring	m_wsMakeDigimonName;	// 만들려고 하는 디지몬의 이름
		//bool			m_bIsSenderData;		//  서버에 패킷을 요청한 상태 체크
	};

	struct sDigimonToItemData
	{
	public:
		sDigimonToItemData():m_nTacticsSlotNum(-1)/*, m_bIsSenderData(false)*/ {};
		~sDigimonToItemData() {};

		//bool IsEnableSend() const;
		void ResetData();
		void SetTacticsSlotNum( int const& nTacticsSlotNum );
		void SetPassword( std::wstring const& sPassword );
		void SetPassword( std::string const& sPassword );
		//void SetSender();
		//bool IsSend() const;

		int GetTacticsSlotNum() const;
		std::string GetPassword() const;

	private:
		int				m_nTacticsSlotNum;	// 아이템으로 변경 시킬 디지몬 텍틱스 슬롯 번호
		std::string		m_sPassword;	// 디지몬을 아이템으로 변경할 때 2차 비밀 번호 입력
		//bool			m_bIsSenderData;	//  서버에 패킷을 요청한 상태 체크
	};

	struct sMaterialData
	{
		enum eMaterialItemType
		{
			eNone	=	0,
			eItemMaterial = 1,
			eDigimonMaterial,
		};

		sMaterialData() : dwMaterialIDX(0), nCount(0),bEnable(false),eMaterialType(eNone)
		{
		}
		~sMaterialData() {};

		void				SetMaterialEnable();
		bool				IsMaterialEnable() const;

		bool				bEnable;
		std::string			sDigimonIconName;
		DWORD				dwMaterialIDX;
		int					nCount;
		eMaterialItemType	eMaterialType;
		std::wstring		wsObjectName;
	};

	struct sExtraExchageData
	{
		sExtraExchageData():dwObjectIDX(0),usMaterialType(0),dwNeedMoney(0),usWayType(0)
		{};
		~sExtraExchageData() {};

		void			SetAllMaterialEnable();
		bool			IsEnableTrade() const; // 선택된 아이템 및 디지몬으로 교환이 가능하지 체크


		std::wstring	wsExchageObjectName;
		std::string		sDigimonIconName;

		DWORD			dwObjectIDX;			// 디지몬 테에이블 번호 이거나 아이템 테이블 번호
		USHORT			usMaterialType;			// 재료의 타입
		DWORD			dwNeedMoney;			// 필요 금액
		USHORT			usWayType;				// 1: 모든 재료가 있어야만 교환 가능, 2 : 재료중에 하나만 있어도 교환 가능
		

		std::list<sMaterialData>	MainNeedMaterial;	// 메인 재료
		std::list<sMaterialData>	SubNeedMaterial;	// 보조 재료
		
	};

public:
	cEvolutionStoreContents(void);
	virtual ~cEvolutionStoreContents(void);

	enum eEventType
	{
		eUpdateNeedItemCount,
		eOpenPasswordInputUI,
		eClosePasswordInputUI,
		eOpenOverlapSelectUI,
		eCloseOverlapSelectUI,
	};

public:
	virtual int const				GetContentsIdentity(void) const;
	virtual bool					Initialize(void);
	virtual void					UnInitialize(void);
	virtual bool					IntraConnection(ContentsSystem& System);
	virtual void					Update(float const& fElapsedTime);
	virtual void					NotifyEvent(int const& iNotifiedEvt);
	virtual void					MakeMainActorData(void);
	virtual void					ClearMainActorData(void);
	virtual void					MakeWorldData(void);
	virtual void					ClearWorldData(void);

	void							_ResetData();
	void							CloseWindow();

	std::wstring					getNpcTypeFromUITitleName();
	CsC_AvObject*					GetTargetNpc();
	std::wstring					GetDigimonName( DWORD const& dwDigimonIDX );
	std::wstring					GetItemNameName( DWORD const& dwItemIDX );
	std::string						GetDigimonIconFile( DWORD const& dwDigimonIDX );
	DWORD							GetItemIconIndex( DWORD const& dwItemIDX );

	std::list<sExtraExchageData> const* GetExchageTargetList( unsigned short const& nExchageType ) const;
	sExtraExchageData const*		GetExchageData( unsigned short const& nExchageType, DWORD const& dwObjectIDX, unsigned short const& usWayType );
	void							SendExtraEvolution( unsigned short const& nExchageType, unsigned short const& usWayType, DWORD const& dwObjectIDX );

	void							ClosePasswordInput();
	void							CloseOverlapDigimonSelectUI();
	void							DigimonToItem_SelectOverlapDigimon( int const& nSlotNum, DWORD const& dwDigimonTBIDX );

	int								GetTacticsSlotNum( DWORD const& dwDigimonTbIDX, int const& nLv );

	void							GetTacticsDigimonSlotInfo( DWORD const& dwFindDigimonTBIDX, int const& nLv, std::list<std::pair<int,std::string>>& slotInfo );

	void							SendDigimonToItem(std::wstring const& wsPassword);

private:
	void							_Recv_ItemToDigimon(void* pData);
	void							_Recv_DigimonToItem(void* pData);

	void							openEvolutionStoreWindow(void* pData);
	void							closeEvolutionStoreWindow(void* pData);
	void							_MessageAsk_DigimonToItem(void* pData);
	void							_MessageAsk_ItemToDigimon(void* pData);
	void							_SendItemToDigimon(void* pData);

	void							changeMyItemCount(void* pData);
	void							_LoadExchageData( DWORD const& dwTargetNpcIDX );
	void							_CheckAllEnableMaterial();

	bool							IsTacticsOverlapDigimon(DWORD const& dwDigimonTBIdx, int const& nLv);
	

private:
	DWORD								m_dwTargetNpcIdx;
	CsC_AvObject*						m_pTarget;
	std::map<int, std::list<sExtraExchageData>>	m_mapExchangeData;

	sItemToDigimonData					m_ItemToDigimon;
	sDigimonToItemData					m_DigimonToItem;
};

