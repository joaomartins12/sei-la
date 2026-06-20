
#pragma once 

BEGIN_NAME_TABLE

class CCreateCharacterTable : public IDataTableEvent
{
public:
	typedef std::list<sTamerCreateInfo>				LIST_TAMER_CREATE_INFO;
	typedef std::list<sDigimonCreateInfo>			LIST_DIGIMON_CREATE_INFO;

public:
	CCreateCharacterTable();
	~CCreateCharacterTable();

	virtual void				InitTable(int const& nServiceType);
	virtual void				ReleaseTable();

	LIST_TAMER_CREATE_INFO const&	GetTamerCreateInfo() const;
	LIST_DIGIMON_CREATE_INFO const&	GetDigimonCreateInfo() const;

private:
	bool						LoadTableData_Tamer(DataStream& pReader);
	bool						LoadTableData_Digimon(DataStream& pReader);

private:
	LIST_TAMER_CREATE_INFO		m_listTamerCreateInfos;
	LIST_DIGIMON_CREATE_INFO	m_listDigimonCreateInfos;
};

END_NAME_TABLE