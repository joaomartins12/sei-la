
#pragma once 

BEGIN_NAME_TABLE

class CSTable_UIText : public ITableEvent
{
public:
	typedef std::map< UINT64, std::wstring > ItemCont;

public:
	CSTable_UIText();
	~CSTable_UIText();

	virtual void	InitTable(int const& nServiceType);
	virtual void	ReleaseTable();

	void			DataSwap( ItemCont& Dist ) const;

private:
	bool			ReadItemData( DataStream& pReader );

private:
	mutable ItemCont	m_kItems;
};

END_NAME_TABLE