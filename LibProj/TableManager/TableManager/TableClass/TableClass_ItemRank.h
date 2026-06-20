
#pragma once 

BEGIN_NAME_TABLE

class CItemRankTable : public IDataTableEvent
{
public:
	CItemRankTable();
	~CItemRankTable();

	virtual void				InitTable(int const& nServiceType) override;
	virtual void				ReleaseTable() override;

	typedef std::map<DWORD, sItemRankData>	ItemCont;
	int							GetDropCount(DWORD const& dwItemID) const;
	ItemCont const&				GetItemRank() const;
	sItemRankData const*		GetItemRank( DWORD const& dwItemID ) const;
	bool						IsRankItem( DWORD const& dwItemID ) const;


	typedef std::list<sItemRankEffect>		ChildCont;
	typedef std::map<DWORD, ChildCont>		RankItemCont;
	sItemRankEffect const *		GetRankInfo(DWORD const& dwItemCode, DWORD const&  dwRankItemCode) const;

private:
	bool						ReadItemData(DataStream& pReader);
	bool						ReadItemData_Effect(DataStream& pReader);

private:
	ItemCont					m_kItems;
	RankItemCont				m_kEffectItems;
};

END_NAME_TABLE