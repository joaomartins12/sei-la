
#pragma once 

BEGIN_NAME_TABLE

class CSTable_Digimon : public ITableEvent
{
public:
	typedef std::map< DWORD, std::wstring> ItemCont;

public:
	CSTable_Digimon();
	~CSTable_Digimon();

	virtual void	InitTable(int const& nServiceType);
	virtual void	ReleaseTable();

	std::wstring	GetName( DWORD const& dwDigimonID ) const;

	ItemCont const&	GetItem() const;

private:
	bool			ReadItemData( DataStream& pReader );

private:
	ItemCont		m_kItems;
};

END_NAME_TABLE