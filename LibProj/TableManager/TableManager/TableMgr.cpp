//---------------------------------------------------------------------------
//
// 파일명 : TableMgr.cpp
// 작성일 : 
// 작성자 : 
// 설  명 : 
//
//---------------------------------------------------------------------------
#include "stdafx.h"
#include "TableMgr.h"
#include "TableFactory.h"

BEGIN_NAME_TABLE
//---------------------------------------------------------------------------
CTableMgr::CTableMgr()
:m_nLoadType(FT_NONE), m_fpDataLoadFunc(NULL), m_nLockPackIdx(-1), m_nServiceType(eServiceType::eNone)
{
}
//---------------------------------------------------------------------------
CTableMgr::~CTableMgr()
{
	Destroy();
}
//---------------------------------------------------------------------------
void CTableMgr::Create( std::wstring const& tableRootDir, std::wstring const& LanguageDir, int const& nLoadType, int const& nServiceType )
{
	m_RootDir = DmCS::FileFn::GetFilePath( tableRootDir.c_str() );
	m_LanguageDir = DmCS::FileFn::GetFilePath( LanguageDir.c_str() );
	m_nLoadType = nLoadType;
	m_nServiceType = nServiceType;
}

void CTableMgr::SetPackLockFunc( int const& nLockPackIdx, PackDataLoadFun fpLoadFunc )
{
	m_nLockPackIdx = nLockPackIdx;
	m_fpDataLoadFunc = fpLoadFunc;
}

//---------------------------------------------------------------------------
void CTableMgr::Destroy()
{
	m_nLockPackIdx = -1;
	m_nServiceType = eServiceType::eNone;
	m_fpDataLoadFunc = NULL;
	m_RootDir.clear();
	m_LanguageDir.clear();

	TableCont::iterator it = m_kTables.begin();
	while( it != m_kTables.end() )
	{
		SAFE_POINTER_CON( it->second );
		it->second->ReleaseTable();
		SAFE_DELETE( it->second );
		it = m_kTables.erase( it );
	}
	
	StringTableCont::iterator it2 = m_kStringTable.begin();
	while( it2 != m_kStringTable.end() )
	{
		SAFE_POINTER_CON( it2->second );
		it2->second->ReleaseTable();
		SAFE_DELETE( it2->second );
		it2 = m_kStringTable.erase( it2 );
	}
}
//---------------------------------------------------------------------------
bool CTableMgr::LoadAllData()
{
	for( int n = (ESTRING_BEGIN + 1); n < ESTRING_END; ++n )
		InsertStringLoadTableData( (EStringTableType)n );

	for( int n = (ETABLE_BEGIN + 1); n < ETABLE_END; ++n )
		InsertLoadTableData( (ETableType)n );

	return _LoadTableData();
}

void Tb::CTableMgr::ResetData()
{
	_ReleaseData();
	_ReleaseStringData();	
}

void CTableMgr::_ReleaseData()
{
	TableCont::iterator it = m_kTables.begin();
	for( ; it != m_kTables.end(); ++it )
	{
		IDataTableEvent* pTmpl = it->second;
		SAFE_POINTER_CON( pTmpl );
		pTmpl->ReleaseTable();
	}

	m_kTables.clear();
}

void CTableMgr::_ReleaseStringData()
{
	StringTableCont::iterator it = m_kStringTable.begin();
	for( ; it != m_kStringTable.end(); ++it )
	{
		ITableEvent* pTmpl = it->second;
		SAFE_POINTER_CON( pTmpl );
		pTmpl->ReleaseTable();
	}

	m_kStringTable.clear();
}

int CTableMgr::GetLoadType() const
{
	return m_nLoadType;
}

// 로드할 테이블의 클래스 생성만 함.
bool CTableMgr::InsertLoadTableData( ETableType const& nTablelIdx )
{
	TableCont::const_iterator it = m_kTables.find( nTablelIdx );
	if( it != m_kTables.end() )
	{
		// 이미 로드 된 테이블 데이터 
		return false;
	}

	IDataTableEvent* pTmpl = TFactory::CreateITable( nTablelIdx );
	if( NULL == pTmpl )
	{
		// 정의 되지 않은 테이블 타입
		return false;
	}

	pTmpl->InitTable(m_nServiceType);

	// 포인터만 생성하고 테이블로드는 따로 한다.
	TableCont::_Pairib Rst = m_kTables.insert( std::make_pair( nTablelIdx, pTmpl ) );
	return Rst.second;
}

// 로드할 테이블의 클래스 생성만 함.
bool CTableMgr::InsertStringLoadTableData( EStringTableType const& nTablelIdx )
{
	StringTableCont::const_iterator it = m_kStringTable.find( nTablelIdx );
	if( it != m_kStringTable.end() )
	{
		// 이미 로드 된 테이블 데이터 
		return false;
	}

	ITableEvent* pTmpl = TFactory::CreateISTable( nTablelIdx );
	if( NULL == pTmpl )
	{
		// 정의 되지 않은 테이블 타입
		return false;
	}

	pTmpl->InitTable(m_nServiceType);

	// 포인터만 생성하고 테이블로드는 따로 한다.
	StringTableCont::_Pairib Rst = m_kStringTable.insert( std::make_pair( nTablelIdx, pTmpl ) );
	return Rst.second;
}

// 로드할 테이블의 목록만 테이터 로드 함.
bool CTableMgr::_LoadTableData()
{
	if( m_kTables.empty() )
		return false;

	switch( m_nLoadType )
	{
	case FT_TEXT:	
		{
			if( !_LoadStringTable( m_LanguageDir ) )
				return false;
			if( !_LoadTable( m_RootDir ) )
				return false;
		}break;
	case FT_PACK:
		{
			if( !_LoadStringPack( m_LanguageDir ) )
				return false;
			if( !_LoadPack( m_RootDir ) )
				return false;
		}
		break;
	default:
		return false;
	}

	_SetStringTable();
	// 테이블 데이터 로드 후 문자열 테이블 데이터를 삭제 함.
	_ReleaseStringData();
	return true;
}

std::wstring CTableMgr::GetUIString( std::string const& value ) const
{
	CUITextTable const* pUITable = static_cast<CUITextTable const*>(_GetITable( ETABLE_UITEXT ));
	return pUITable->GetUIText( value.c_str() );
}

std::wstring CTableMgr::GetUIStringVA( std::string const& value, ... ) const
{
	std::wstring getValue = GetUIString(value);
	if( getValue.empty() )
		return getValue;

	wchar_t tmpStr[1024] = {0,};
	va_list argList;
	va_start( argList, value );
	_vstprintf_s( tmpStr, 1024, getValue.c_str(), argList );
	tmpStr[1023] = L'\0';
	va_end( argList );

	getValue = tmpStr;
	return getValue;
}

bool CTableMgr::ChangeLanguage(std::wstring const& LanguageDir)
{
	m_LanguageDir = DmCS::FileFn::GetFilePath( LanguageDir.c_str() );

	if( m_kTables.empty() || !m_kStringTable.empty() || m_LanguageDir.empty() )
		return false;

	switch( m_nLoadType )
	{
	case FT_TEXT:	
		{
			if( !_LoadStringTable( m_LanguageDir ) )
				return false;
		}break;
	case FT_PACK:
		{
			if( !_LoadStringPack( m_LanguageDir ) )
				return false;
		}
		break;
	default:
		return false;
	}

	_SetStringTable();
	// 테이블 데이터 로드 후 문자열 테이블 데이터를 삭제 함.
	_ReleaseStringData();
	return true;
}

void CTableMgr::_SetStringTable()
{
	TableCont::iterator it = m_kTables.begin();
	for( ; it != m_kTables.end(); ++it )
	{
		IDataTableEvent* pTmpl = it->second;
		SAFE_POINTER_CON( pTmpl );
		ITableEvent::EventCont::iterator it = pTmpl->m_ChangeStringEvent.begin();
		for( ; it != pTmpl->m_ChangeStringEvent.end(); ++it )
		{
			int nType = (*it)->GetSTableIdx();
			ITableEvent const* pSData = _GetISTable( nType );
			SAFE_POINTER_CON(pSData);
			(*it)->OnExecute( pSData );
		}
	}
}

bool CTableMgr::_LoadTable( std::wstring const& wsRootDir )
{
	TableCont::iterator it = m_kTables.begin();
	for( ; it != m_kTables.end(); ++it )
	{
		IDataTableEvent* pTmpl = it->second;
		SAFE_POINTER_RETVAL( pTmpl, false );

		ITableEvent::EventCont::iterator it = pTmpl->m_LoadTableEvent.begin();
		for( ; it != pTmpl->m_LoadTableEvent.end(); ++it )
		{
			std::wstring filePath = wsRootDir;
			filePath += (*it)->GetFile();

			dm_bytevec binary_data;
			if(!_ReadFile( filePath, binary_data ))
				return false;

			// 암호 및 압축 해제
			if( !_UnDecAndComp( binary_data ) )
				return false;		

			dm_stringlist_t read_Data;
			dm_size_t size = 0;
			if( !DmCS::Read::exec( read_Data, binary_data, size ) )
				return false;

			DataStream conveter;
			conveter.SetData( read_Data );
			(*it)->OnExecute( conveter );
			conveter.Close();
		}
	}
	return true;
}

bool CTableMgr::_LoadStringTable( std::wstring const& wsRootDir )
{
	StringTableCont::iterator it = m_kStringTable.begin();
	for( ; it != m_kStringTable.end(); ++it )
	{
		ITableEvent* pTmpl = it->second;
		SAFE_POINTER_RETVAL( pTmpl, false );
		ITableEvent::EventCont::iterator it2 = pTmpl->m_LoadTableEvent.begin();
		for( ; it2 != pTmpl->m_LoadTableEvent.end(); ++it2 )
		{
			std::wstring filePath = wsRootDir;
			filePath += (*it2)->GetFile();

			dm_bytevec binary_data;
			if(!_ReadFile( filePath, binary_data ))
				return false;

			// 암호 및 압축 해제
			if( !_UnDecAndComp( binary_data ) )
				return false;		

			dm_stringlist_t read_Data;
			dm_size_t size = 0;
			if( !DmCS::Read::exec( read_Data, binary_data, size ) )
				return false;

			DataStream conveter;
			conveter.SetData( read_Data );
			(*it2)->OnExecute( conveter );
			conveter.Close();
		}

	}
	return true;
}

bool CTableMgr::_LoadPack( std::wstring const& wsRootDir )
{
	if( FT_PACK != m_nLoadType )
		return false;

	SAFE_POINTER_RETVAL( m_fpDataLoadFunc, false );

	TableCont::iterator it = m_kTables.begin();
	for( ; it != m_kTables.end(); ++it )
	{
		IDataTableEvent* pTmpl = it->second;
		SAFE_POINTER_RETVAL( pTmpl, false );
		ITableEvent::EventCont::iterator it2 = pTmpl->m_LoadTableEvent.begin();
		for( ; it2 != pTmpl->m_LoadTableEvent.end(); ++it2 )
		{
			std::wstring filePath = wsRootDir;
			filePath += (*it2)->GetFile();

			// 패킹 파일에서 데이터 읽기
			dm_bytevec read_buffer;
			if( !_ReadPack( filePath, read_buffer ) )
				return false;

			// 암호 및 압축 해제
			if( !_UnDecAndComp( read_buffer ) )
				return false;

			dm_stringlist_t read_data;
			dm_size_t offset = 0;
			if( !DmCS::Read::exec( read_data, read_buffer, offset ) )
				return false;

			DataStream tableRead;
			tableRead.SetData( read_data );
			(*it2)->OnExecute( tableRead );
			tableRead.Close();
		}
	}

	return true;
}

bool CTableMgr::_LoadStringPack( std::wstring const& wsRootDir )
{
	if( FT_PACK != m_nLoadType )
		return false;

	SAFE_POINTER_RETVAL( m_fpDataLoadFunc, false );

	StringTableCont::iterator it = m_kStringTable.begin();
	for( ; it != m_kStringTable.end(); ++it )
	{
		ITableEvent* pTmpl = it->second;
		SAFE_POINTER_RETVAL( pTmpl, false );
		ITableEvent::EventCont::iterator it2 = pTmpl->m_LoadTableEvent.begin();
		for( ; it2 != pTmpl->m_LoadTableEvent.end(); ++it2 )
		{
			std::wstring filePath = wsRootDir;
			filePath += (*it2)->GetFile();

			dm_bytevec read_buffer;
			if( !_ReadPack( filePath, read_buffer ) )
				return false;

			// 암호 및 압축 해제
			if( !_UnDecAndComp( read_buffer ) )
				return false;

			dm_stringlist_t read_data;
			dm_size_t offset = 0;
			if( !DmCS::Read::exec( read_data, read_buffer, offset ) )
				return false;

			DataStream tableRead;
			tableRead.SetData( read_data );
			(*it2)->OnExecute( tableRead );
			tableRead.Close();
		}
	}

	return true;
}
//---------------------------------------------------------------------------

bool CTableMgr::_ReadPack( std::wstring const& readFile, dm_bytevec& readData )
{
	std::string file;
	DmCS::StringFn::ToMB( readFile, file );

	size_t buffSize = (*m_fpDataLoadFunc)(m_nLockPackIdx, file.c_str(), readData );
	if( 0 == buffSize )
		return false;

	return true;
}

bool CTableMgr::_ReadFile( std::wstring const& readFile, dm_bytevec& readData )
{
	DmCS::Storage::File file;
	if( !file.Open( readFile ) )
		return false;

	readData.resize( file.Size() );
	bool bSuccess = file.Read( readData );
	file.Close();
	return bSuccess;
}

// 암호 및 압축 해제
bool CTableMgr::_UnDecAndComp( dm_bytevec& readData )
{
	// 암호 해제
	if( !DmFS::SecuritySelf::Decript( 0xFF, readData, readData ) )
		return false;

	// 압축 해제
	if( !DmCS::CompressZip::UnCompress( readData, readData ) )
		return false;

	return true;
}

//---------------------------------------------------------------------------
IDataTableEvent const* CTableMgr::_GetITable(int TableType) const
{
	TableCont::const_iterator it = m_kTables.find( TableType );
	if( it != m_kTables.end() )
		return it->second;

	return NULL;
}

ITableEvent const* CTableMgr::_GetISTable(int TableType) const
{
	StringTableCont::const_iterator it = m_kStringTable.find( TableType );
	if( it != m_kStringTable.end() )
		return it->second;

	return NULL;
}

END_NAME_TABLE
//---------------------------------------------------------------------------
