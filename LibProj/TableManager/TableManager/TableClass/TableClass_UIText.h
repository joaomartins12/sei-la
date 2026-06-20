
#pragma once 

BEGIN_NAME_TABLE

class CUITextTable : public IDataTableEvent
{
public:
	typedef std::map< UINT64, std::wstring > ItemCont;

public:
	CUITextTable();
	~CUITextTable();

	virtual void	InitTable(int const& nServiceType) override;
	virtual void	ReleaseTable() override;

	std::wstring	GetUIText( char const* pKey ) const;

private:
	bool			_InsertString(ITbImpl const* pData);

private:
	ItemCont		m_kItems;
};

END_NAME_TABLE