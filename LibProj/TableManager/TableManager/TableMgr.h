//---------------------------------------------------------------------------
//
// 파일명 : TableMgr.h
// 작성일 : 
// 작성자 : 
// 설  명 : 
//
//---------------------------------------------------------------------------

#pragma once
BEGIN_NAME_TABLE
//---------------------------------------------------------------------------

//class ITableTmpl;
class CTableMgr : public CSingleton< CTableMgr >
{
public:
	CTableMgr();
	~CTableMgr();
	enum
	{
		FT_NONE,
		FT_TEXT,
		FT_PACK
	};

	// Create, Destroy
	void Create(std::wstring const& tableRootDir, std::wstring const& LanguageDir, int const& nLoadType, int const& nServiceType);
	bool ChangeLanguage( std::wstring const& LanguageDir );
	void Destroy();

	// Load 
	bool LoadAllData();
	void ResetData();
	int GetLoadType() const;

	void SetPackLockFunc( int const& nLockPackIdx, PackDataLoadFun fpLoadFunc );
	bool InsertLoadTableData( ETableType const& nTablelIdx );
	bool InsertStringLoadTableData( EStringTableType const& nTablelIdx );

	std::wstring GetUIString( std::string const& value ) const;
	std::wstring GetUIStringVA( std::string const& value, ... ) const;

	template<class T>
	T const* GetTable( int const& nType ) const
	{
		return dynamic_cast<T const*>(_GetITable( nType ));
	}

private:
	typedef std::map< int, IDataTableEvent* >		TableCont;
	typedef std::map< int, ITableEvent* >			StringTableCont;

	IDataTableEvent const*	_GetITable(int TableType) const;
	ITableEvent const*		_GetISTable(int TableType) const;
	
	bool				_LoadTableData();
	
	bool				_LoadTable( std::wstring const& wsRootDir );
	bool				_LoadPack( std::wstring const& wsRootDir );

	bool				_LoadStringTable( std::wstring const& wsRootDir );
	bool				_LoadStringPack( std::wstring const& wsRootDir );

	void				_SetStringTable();

	void				_ReleaseData();
	void				_ReleaseStringData();

	bool				_ReadPack( std::wstring const& readFile, dm_bytevec& readData );
	bool				_ReadFile( std::wstring const& readFile, dm_bytevec& readData );
	bool				_UnDecAndComp( dm_bytevec& readData );

private:
	int					m_nLockPackIdx;
	PackDataLoadFun		m_fpDataLoadFunc;

	int					m_nLoadType;
	int					m_nServiceType;	
	std::wstring		m_RootDir;
	std::wstring		m_LanguageDir;

	TableCont			m_kTables;
	StringTableCont		m_kStringTable;
};
END_NAME_TABLE
//---------------------------------------------------------------------------
// Singleton
#define TABLEMGR_ST			Tb::CTableMgr::GetSingleton()
#define TABLEMGR_STPTR		Tb::CTableMgr::GetSingletonPtr()


#define UISTRING_TEXT(value)	TABLEMGR_ST.GetUIString(value)
#define UISTRING_VA(value,...)	TABLEMGR_ST.GetUIStringVA(value, __VA_ARGS__)
//---------------------------------------------------------------------------