
#pragma once 

BEGIN_NAME_TABLE


class CItemCreationTable : public IDataTableEvent
{
public:
	CItemCreationTable();
	~CItemCreationTable();

	virtual void	InitTable(int const& nServiceType) override;
	virtual void	ReleaseTable() override;

	MAP_Main_Categoty const*		GetItemProductionCategotyTable( DWORD const& dwNpcIdx ) const;
	STL_MAP_ITEMPRODUCTION const&	GetItemProductionTable() const;
	sAssistanceItemGroup const*		GetAssistItemGroup(int const& nGroupID) const;
	LIST_AssistItemList const*		GetAssistGroupItemList(int const& nGroupID) const;

private:
	bool							ReadCreationTable( DataStream& pReader );
	bool							ReadAssistGroupTable( DataStream& pReader );
	bool							_InsertString(ITbImpl const* pData);

private:
	STL_MAP_ITEMPRODUCTION			m_kItems;	// 아이템 제작 테이블
	MAP_AssistItemGroup				m_kAssistGroup;
};

END_NAME_TABLE