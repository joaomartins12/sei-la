//2017-04-20-nova
#pragma once

class UnionStoreContents: public IUIContentSubject< UnionStoreContents >
{
public:
	enum eEvent{
		eShopWithDraw, // 아이템 회수
	};
	UnionStoreContents(void);
	virtual ~UnionStoreContents(void);
	virtual int const GetContentsIdentity(void) const;
	virtual bool Initialize(void)							{ return true; }
	virtual void UnInitialize(void)							{}
	virtual bool IntraConnection(ContentsSystem& System)	{ return true; }
	virtual void Update(float const& fElapsedTime)					{}
	virtual void NotifyEvent(int const& iNotifiedEvt)		{}
	virtual void MakeMainActorData(void)					{}	
	virtual void ClearMainActorData(void)					{}
	virtual void MakeWorldData(void)						{}
	virtual void ClearWorldData(void)						{}

	static int const IsContentsIdentity(void);

protected:
	virtual void NotifyEventStream(int const& iNotifiedEvt, ContentsStream const& kStream) {}

	CsVectorPB< cItemInfo* >	m_vpUnionItem; 
	sMONEY						m_UnionMoney;	

	int			m_nEmploymentChar_MapID;
	NiPoint2	m_vEmploymentChar_Pos;	
	int			m_nEmploymentChar_Channel;
	int			m_nEmploymentChar_ModelID;

public:
	bool		SendDataUniStore(int type);

	void		Add_UnionItem(void* pData);
	void		DeleteUnion();
	cItemInfo*	GetItemInfo(int num);
	int			GetItemCnt();

	void		Set_UnionMoney(void* pData);
	void		SetUnionMoney(u8 money);
	sMONEY		GetUnionMoney();

	int			GetMapID();

	void		Set_ShopData(void* pData);
	void		Set_ModelID(void* pData);
	void		SetModelID(int modelID);

	void		ReleaseEmploymentChar(void* pData);
	void		InitNetOff(void* pData);	
	void		DeleteUnion(void* pData);

	void		Get_CharMapID(void* pData);
	void		Get_CharPos(void* pData);
	void		Get_CharModelID(void* pData);

	bool		CollectItem();
	bool		IHaveUnionStore() const;


	bool		IsEmptySlot();
	void		Recv_ShopWithDraw(void* pData);
};
