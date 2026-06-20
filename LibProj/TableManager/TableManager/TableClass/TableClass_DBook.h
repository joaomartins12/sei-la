
#pragma once 

BEGIN_NAME_TABLE

class CDBookTable : public IDataTableEvent
{
public:
	CDBookTable();
	~CDBookTable();

	typedef	std::map< DWORD, sDBookDeck >			DeckCont;
	typedef	std::map< int, sDBookOption>			DBookOptionCont;
	typedef	std::list< DWORD >						ExceptionCont;


	virtual void		InitTable(int const& nServiceType) override;
	virtual void		ReleaseTable() override;

private:
	bool				LoadData_Deck(DataStream& pReader);
	bool				LoadData_Compsition(DataStream& pReader);
	bool				LoadData_Option(DataStream& pReader);
	bool				LoadData_Exception(DataStream& pReader);

private:
	DeckCont			m_kDecks;
	DBookOptionCont		m_kDBookOption;
	ExceptionCont		m_kException;
};

END_NAME_TABLE