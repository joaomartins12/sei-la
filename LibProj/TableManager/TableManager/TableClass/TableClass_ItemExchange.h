
#pragma once 

BEGIN_NAME_TABLE

class CItemExchangeTable : public IDataTableEvent
{
public:
	CItemExchangeTable();
	~CItemExchangeTable();

	typedef std::list<sItemExchange>					ItemListCont;
	typedef std::map<DWORD/*Npc Idx*/,ItemListCont>		ItemCont;

	virtual void				InitTable(int const& nServiceType) override;
	virtual void				ReleaseTable() override;
	sItemExchange const*		GetExchangeItem( DWORD const& dwNpcID, DWORD const& dwItemID ) const;

private:
	bool						ReadItemData(DataStream& pReader);

private:
	ItemCont					m_kItems;
};

END_NAME_TABLE