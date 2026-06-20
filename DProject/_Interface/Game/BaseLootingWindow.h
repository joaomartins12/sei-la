
#pragma once 

class cBaseLoot : public cBaseWindow , public CPartyContents::ObserverType
{
public:
	cBaseLoot();
	virtual ~cBaseLoot();

protected:
	struct sGetItemInfo
	{
		sGetItemInfo():m_nDiceNumber(0),m_dwItemIconType(0),m_dwItemIconIdx(0),m_dwItemType(0),m_nItemCount(1),m_fLifeTime(3.0f)
		{}		
		~sGetItemInfo(){}

		float			m_fLifeTime;
		int				m_nItemCount;
		int				m_dwItemType;
		DWORD			m_dwItemIconType;
		int				m_dwItemIconIdx;
		int				m_nDiceNumber;
		NiColor			m_ItemColor;
		std::wstring	m_wsItemName;
		sLootingRule	m_sLootingrule;	// 아이템을 획득 하였을 때의 루팅 룰
	};

	struct sViewControls
	{
		sViewControls();
		~sViewControls();

		cString::sICON* m_pItemIcon;
		cString::sICON* m_pLootingRuleIcon;
		cString::sTEXT*	m_pItemName;
		cString::sTEXT*	m_pDiceValue;
		cListBoxItem *	m_BoxItem;
	};

	struct sMemberIdx : public CUSTOMDATA
	{
		sMemberIdx(int nIdx) : m_nMemberIdx(nIdx)
		{}
		~sMemberIdx() {}

		int		m_nMemberIdx;
	};

public:
	virtual void			Destroy();
	virtual void			DeleteResource();	
	virtual void			ResetDevice();	

	virtual void			Create( int nValue = 0 );

	virtual void			Update(float const& fDeltaTime);	
	virtual void			Render();
	virtual eMU_TYPE		Update_ForMouse();

protected:
	virtual	bool			Construct(void);
	virtual	void			Notify(int const& iNotifiedEvt, ContentsStream const& kStream) {};

protected:
	void					_AddLootingWindow(SMemberInfo const* pInfo);
	void					_AddGetItemInfo(CPartyContents::sLootingItems const& getItem, std::wstring const& wsItemName);
	void					_AddViewItem( int const& nSlotNum, sGetItemInfo const& addItem );
	void					_HideLootingView( int const& nSlotNum );
	void					_DeleteMember(int const& nSlotNum);

protected:
	cListBox*								m_pLootingList;
	std::map<int,sViewControls>				m_mapViewControls;

	typedef std::list<sGetItemInfo>			SubItemCont;
	typedef std::map< short, SubItemCont >	ItemCont;

	ItemCont								m_mapItems;
};

