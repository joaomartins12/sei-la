
#include "stdafx.h"
#include "WorldMapMng.h"

CsWorldMapMng::CsWorldMapMng()
{

}

CsWorldMapMng::~CsWorldMapMng()
{

}

void CsWorldMapMng::Delete()
{
	{
		CsWorldMap::MAP_IT it = m_mapWorld.begin();
		CsWorldMap::MAP_IT itEnd = m_mapWorld.end();
		for( ; it!=itEnd; ++it )
		{
			it->second->Delete();
			delete it->second;
		}
		m_mapWorld.clear();
	}

	{
		CsAreaMap::MAP_IT it = m_mapArea.begin();
		CsAreaMap::MAP_IT itEnd = m_mapArea.end();
		for( ; it!=itEnd; ++it )
		{
			it->second->Delete();
			delete it->second;
		}
		m_mapArea.clear();
	}

	m_listRealWorld.clear();
	m_listDigitalWorld.clear();
}

bool CsWorldMapMng::Init( char* cPath )
{
	switch( nsCsFileTable::g_eFileType )
	{
	case nsCsFileTable::FT_EXCEL:
		_LoadExcel();
		break;
	case nsCsFileTable::FT_BIN:		
		if( _LoadBin( cPath ) == false )
			return false;
		break;
	case nsCsFileTable::FT_FILEPACK:
		_LoadFilePack( cPath );
		break;
	default:
		assert_cs( false );
	}

	_ConnectArea();

	return true;
}

void CsWorldMapMng::AddRealWorld( CsWorldMap* pWorld )
{ 
	m_listRealWorld.push_back( pWorld ); 
}

std::list< CsWorldMap* >* CsWorldMapMng::GetRealWorldList()
{ 
	return &m_listRealWorld; 
}

void CsWorldMapMng::AddDigitalWorld( CsWorldMap* pWorld )
{ 
	m_listDigitalWorld.push_back( pWorld ); 
}

std::list< CsWorldMap* >* CsWorldMapMng::GetDigitalWorldList()
{ 
	return &m_listDigitalWorld; 
}

CsWorldMap* CsWorldMapMng::GetWorld( int nID )
{	
	assert_cs( IsWorld( nID ) == true );
	return m_mapWorld[ nID ];
}

CsAreaMap* CsWorldMapMng::GetArea( int nID )
{	
	assert_cs( IsArea( nID ) == true );
	return m_mapArea[ nID ];
}

CsAreaMap::MAP* CsWorldMapMng::GetAreaList()
{ 
	return &m_mapArea; 
}

CsWorldMap::MAP* CsWorldMapMng::GetWorldList()
{ 
	return &m_mapWorld; 
}		

void CsWorldMapMng::_ConnectArea()
{
	CsAreaMap::MAP_IT areait = m_mapArea.begin();
	for( ; areait !=m_mapArea.end(); ++areait )
	{
		SAFE_POINTER_CON( areait->second );
		CsAreaMap::sINFO* pInfo = areait->second->GetInfo();
		SAFE_POINTER_CON( pInfo );
		CsWorldMap* pWorld = GetWorld( pInfo->s_nAreaType );
		SAFE_POINTER_CON( pWorld );
		pWorld->AddArea( areait->second );		

		//GetWorld( it->second->GetInfo()->s_nAreaType )->AddArea( it->second );
	}

	CsWorldMap::MAP_IT it = m_mapWorld.begin();
	for( ; it != m_mapWorld.end(); ++it )
	{
		SAFE_POINTER_CON( it->second );
		CsWorldMap::sINFO* pInfo = it->second->GetInfo();
		SAFE_POINTER_CON( pInfo );

		if( 1 == pInfo->s_nWorldType )
			AddRealWorld( it->second );
		else
		{
			assert_cs( pInfo->s_nWorldType == 2 );
			AddDigitalWorld( it->second );
		}
	}
}

void CsWorldMapMng::_LoadExcel()
{	

	char cBuffer[ CEXCLE_CELL_LEN ];
	wchar_t wBuffer[ CEXCLE_CELL_LEN ];

	// 월드
	{
#define WORLD_SHEET_NAME			"DM_WorldMap"
#define IL_COL_W_ID				1
#define IL_COL_W_NAME_KOR		2
#define IL_COL_W_NAME_ENG		3
#define IL_COL_W_TYPE			4
#define IL_COL_W_UI_X			6
#define IL_COL_W_UI_Y			7
#define IL_COL_W_COMMENT_KOR	8
#define IL_COL_W_COMMENT_ENG	9

		cExcelReader excel;
		bool bSuccess = excel.Open( nsCsFileTable::GetPath_DM_WorldMap(), WORLD_SHEET_NAME );
		assert_cs( bSuccess == true );
		// 2줄은 버린다.
		bSuccess = excel.MoveFirst();
		assert_cs( bSuccess == true );	
		bSuccess = excel.MoveNext();
		assert_cs( bSuccess == true );

		CsWorldMap::sINFO info;

		while( excel.MoveNext() == true )
		{
			// id
			info.s_nID = atoi( excel.GetData( IL_COL_W_ID , cBuffer ) );
			
			switch( nsCsFileTable::g_eLanguage )
			{
			case nsCsFileTable::KOREA_ORIGINAL:
			case nsCsFileTable::KOREA_TRANSLATION:
				// name
				M2W( info.s_szName, excel.GetData( IL_COL_W_NAME_KOR, cBuffer ), FT_WORLDMAP_NAME_LEN );
				// comment
				M2W( info.s_szComment, excel.GetData( IL_COL_W_COMMENT_KOR, cBuffer ), FT_WORLDMAP_COMMENT_LEN );
				break;
			case nsCsFileTable::ENGLISH:
			case nsCsFileTable::ENGLISH_A:
				// name
				M2W( info.s_szName, excel.GetData( IL_COL_W_NAME_ENG, cBuffer ), FT_WORLDMAP_NAME_LEN );
				// comment
				M2W( info.s_szComment, excel.GetData( IL_COL_W_COMMENT_ENG, cBuffer ), FT_WORLDMAP_COMMENT_LEN );
				break;
			case nsCsFileTable::THAILAND:
			case nsCsFileTable::TAIWAN:
			case nsCsFileTable::HONGKONG:
				// name
				//M2W( info.s_szName, excel.GetData( IL_COL_W_NAME_ENG, cBuffer ), FT_WORLDMAP_NAME_LEN );
				wcscpy_s(info.s_szName, FT_WORLDMAP_NAME_LEN, excel.GetData( IL_COL_W_NAME_ENG, wBuffer ));
				// comment
				//M2W( info.s_szComment, excel.GetData( IL_COL_W_COMMENT_ENG, cBuffer ), FT_WORLDMAP_COMMENT_LEN );
				wcscpy_s(info.s_szComment, FT_WORLDMAP_COMMENT_LEN, excel.GetData( IL_COL_W_COMMENT_ENG, wBuffer ));
				break;
			default:
				assert_cs( false );
			}			

			// type
			info.s_nWorldType = atoi( excel.GetData( IL_COL_W_TYPE , cBuffer ) );

			// ui
			info.s_nUI_X = atoi( excel.GetData( IL_COL_W_UI_X , cBuffer ) );
			info.s_nUI_Y = atoi( excel.GetData( IL_COL_W_UI_Y , cBuffer ) );

			CsWorldMap* pData = csnew CsWorldMap;
			pData->Init( &info );
			m_mapWorld[ info.s_nID ] = pData;
		}
		excel.Close();
	}

	// 에이리어
	{

#define AREA_SHEET_NAME			"DM_AreaMap"
#define IL_COL_A_ID				1
#define IL_COL_A_NAME_KOR		2
#define IL_COL_A_NAME_ENG		3
#define IL_COL_A_TYPE			6
#define IL_COL_A_FIELD			8
#define IL_COL_A_DETAIL			10
#define IL_COL_A_UI_X			12
#define IL_COL_A_UI_Y			13
#define IL_COL_A_COMMENT_KOR	14
#define IL_COL_A_COMMENT_ENG	15

#define IL_COL_A_GAUSSIAN_1		17
#define IL_COL_A_GAUSSIAN_2		18
#define IL_COL_A_GAUSSIAN_3		19

		cExcelReader excel;
		bool bSuccess = excel.Open( nsCsFileTable::GetPath_DM_WorldMap(), AREA_SHEET_NAME );
		assert_cs( bSuccess == true );
		// 2줄은 버린다.
		bSuccess = excel.MoveFirst();
		assert_cs( bSuccess == true );	
		bSuccess = excel.MoveNext();
		assert_cs( bSuccess == true );

		CsAreaMap::sINFO info;

		while( excel.MoveNext() == true )
		{
			// id
			info.s_nMapID = atoi( excel.GetData( IL_COL_A_ID , cBuffer ) );

			switch( nsCsFileTable::g_eLanguage )
			{
			case nsCsFileTable::KOREA_ORIGINAL:
			case nsCsFileTable::KOREA_TRANSLATION:
				// name
				M2W( info.s_szName, excel.GetData( IL_COL_A_NAME_KOR, cBuffer ), FT_WORLDMAP_NAME_LEN );
				// comment
				M2W( info.s_szComment, excel.GetData( IL_COL_A_COMMENT_KOR, cBuffer ), FT_WORLDMAP_COMMENT_LEN );
				break;
			case nsCsFileTable::ENGLISH:
			case nsCsFileTable::ENGLISH_A:
				// name
				M2W( info.s_szName, excel.GetData( IL_COL_A_NAME_ENG, cBuffer ), FT_WORLDMAP_NAME_LEN );
				// comment
				M2W( info.s_szComment, excel.GetData( IL_COL_A_COMMENT_ENG, cBuffer ), FT_WORLDMAP_COMMENT_LEN );
				break;
			case nsCsFileTable::THAILAND:
			case nsCsFileTable::TAIWAN:
			case nsCsFileTable::HONGKONG:
				// name
				//M2W( info.s_szName, excel.GetData( IL_COL_A_NAME_ENG, cBuffer ), FT_WORLDMAP_NAME_LEN );
				wcscpy_s(info.s_szName, FT_WORLDMAP_NAME_LEN, excel.GetData( IL_COL_A_NAME_ENG, wBuffer ));
				// comment
				//M2W( info.s_szComment, excel.GetData( IL_COL_A_COMMENT_ENG, cBuffer ), FT_WORLDMAP_COMMENT_LEN );
				wcscpy_s(info.s_szComment, FT_WORLDMAP_COMMENT_LEN, excel.GetData( IL_COL_A_COMMENT_ENG, wBuffer ));
				break;
			default:
				assert_cs( false );
			}

			// type
			info.s_nAreaType = atoi( excel.GetData( IL_COL_A_TYPE , cBuffer ) );

			// FieldType
			info.s_nFieldType = atoi( excel.GetData( IL_COL_A_FIELD , cBuffer ) );

			// FieldDetail
			info.s_nFTDetail = atoi( excel.GetData( IL_COL_A_DETAIL, cBuffer ) );

			// ui
			info.s_nUI_X = atoi( excel.GetData( IL_COL_A_UI_X , cBuffer ) );
			info.s_nUI_Y = atoi( excel.GetData( IL_COL_A_UI_Y , cBuffer ) );

			info.s_fGaussianBlur[ 0 ] = atoi( excel.GetData( IL_COL_A_GAUSSIAN_1 , cBuffer ) )*0.001f;
			info.s_fGaussianBlur[ 1 ] = atoi( excel.GetData( IL_COL_A_GAUSSIAN_2 , cBuffer ) )*0.001f;
			info.s_fGaussianBlur[ 2 ] = atoi( excel.GetData( IL_COL_A_GAUSSIAN_3 , cBuffer ) )*0.001f;

			CsAreaMap* pData = csnew CsAreaMap;
			pData->Init( &info );
			m_mapArea[ info.s_nMapID ] = pData;
		}
		excel.Close();
	}
}


#define WORLD_BIN			"WorldMap.bin"
void CsWorldMapMng::SaveBin( char* cPath )
{
	char cName[ MAX_PATH ];
	sprintf_s( cName, MAX_PATH, "%s\\%s", cPath, WORLD_BIN );

	FILE *fp = NULL;
	fopen_s( &fp, cName, "wb" );
	assert_cs( fp != NULL );

	{
		int nCount = (int)m_mapWorld.size();
		fwrite( &nCount, sizeof( int ), 1, fp );

		CsWorldMap::MAP_IT it = m_mapWorld.begin();
		CsWorldMap::MAP_IT itEnd = m_mapWorld.end();
		for( ; it!=itEnd; ++it )
		{
			fwrite( it->second->GetInfo(), sizeof( CsWorldMap::sINFO ), 1, fp );
		}
	}

	{
		int nCount = (int)m_mapArea.size();
		fwrite( &nCount, sizeof( int ), 1, fp );

		CsAreaMap::MAP_IT it = m_mapArea.begin();
		CsAreaMap::MAP_IT itEnd = m_mapArea.end();
		for( ; it!=itEnd; ++it )
		{
			fwrite( it->second->GetInfo(), sizeof( CsAreaMap::sINFO ), 1, fp );
		}
	}

	fclose( fp );
}

bool CsWorldMapMng::_LoadBin( char* cPath )
{
	char cName[ MAX_PATH ];
	sprintf_s( cName, MAX_PATH, "%s\\%s", cPath, WORLD_BIN );

	FILE *fp = NULL;
	fopen_s( &fp, cName, "rb" );
	if( fp == NULL )
	{
		CsMessageBoxA( MB_OK, "%s 파일이 존재 하지 않습니다.", cName );
		return false;
	}

	{
		int nCount;
		fread( &nCount, sizeof( int ), 1, fp );
		CsWorldMap::sINFO info;
		for( int i=0; i<nCount; ++i )
		{
			fread( &info, sizeof( CsWorldMap::sINFO ), 1, fp );

			CsWorldMap* pObject = csnew CsWorldMap;
			pObject->Init( &info );
			assert_cs( m_mapWorld.find( info.s_nID ) == m_mapWorld.end() );
			m_mapWorld[ info.s_nID ] = pObject;
		}
	}

	{
		int nCount;
		fread( &nCount, sizeof( int ), 1, fp );
		CsAreaMap::sINFO info;
		for( int i=0; i<nCount; ++i )
		{
			fread( &info, sizeof( CsAreaMap::sINFO ), 1, fp );

			CsAreaMap* pObject = csnew CsAreaMap;
			pObject->Init( &info );
			assert_cs( m_mapArea.find( info.s_nMapID ) == m_mapArea.end() );
			m_mapArea[ info.s_nMapID ] = pObject;
		}
	}

	fclose( fp );

	return true;
}


void CsWorldMapMng::_LoadFilePack( char* cPath )
{
	char cName[ MAX_PATH ];
	sprintf_s( cName, MAX_PATH, "%s\\%s", cPath, WORLD_BIN );

	if( !CsFPS::CsFPSystem::SeekLock( FT_PACKHANDLE ) )
		return;

	int nHandle = CsFPS::CsFPSystem::GetFileHandle( FT_PACKHANDLE, cName );

	{
		int nCount;
		_read( nHandle, &nCount, sizeof( int ) );
		CsWorldMap::sINFO info;
		for( int i=0; i<nCount; ++i )
		{
			_read( nHandle, &info, sizeof( CsWorldMap::sINFO ) );

			CsWorldMap* pObject = csnew CsWorldMap;
			pObject->Init( &info );
			assert_cs( m_mapWorld.find( info.s_nID ) == m_mapWorld.end() );
			m_mapWorld[ info.s_nID ] = pObject;
		}
	}

	{
		int nCount;
		_read( nHandle, &nCount, sizeof( int ) );
		CsAreaMap::sINFO info;
		for( int i=0; i<nCount; ++i )
		{
			_read( nHandle, &info, sizeof( CsAreaMap::sINFO ) );

			CsAreaMap* pObject = csnew CsAreaMap;
			pObject->Init( &info );
			assert_cs( m_mapArea.find( info.s_nMapID ) == m_mapArea.end() );
			m_mapArea[ info.s_nMapID ] = pObject;
		}
	}

	CsFPS::CsFPSystem::SeekUnLock( FT_PACKHANDLE );
}

void CsWorldMapMng::ReloadGaussianBlur()
{

#define AREA_SHEET_NAME			"DM_AreaMap"
#define IL_COL_A_GAUSSIAN_1		17
#define IL_COL_A_GAUSSIAN_2		18
#define IL_COL_A_GAUSSIAN_3		19

	cExcelReader excel;
	bool bSuccess = excel.Open( nsCsFileTable::GetPath_DM_WorldMap(), AREA_SHEET_NAME );
	assert_cs( bSuccess == true );
	// 2줄은 버린다.
	bSuccess = excel.MoveFirst();
	assert_cs( bSuccess == true );	
	bSuccess = excel.MoveNext();
	assert_cs( bSuccess == true );

	char cBuffer[ CEXCLE_CELL_LEN ];
	while( excel.MoveNext() == true )
	{
		// id
		DWORD nMapID = atoi( excel.GetData( IL_COL_A_ID , cBuffer ) );

		if( IsArea( nMapID ) == false )
			continue;

		GetArea( nMapID )->GetInfo()->s_fGaussianBlur[ 0 ] = atoi( excel.GetData( IL_COL_A_GAUSSIAN_1 , cBuffer ) )*0.001f;
		GetArea( nMapID )->GetInfo()->s_fGaussianBlur[ 1 ] = atoi( excel.GetData( IL_COL_A_GAUSSIAN_2 , cBuffer ) )*0.001f;
		GetArea( nMapID )->GetInfo()->s_fGaussianBlur[ 2 ] = atoi( excel.GetData( IL_COL_A_GAUSSIAN_3 , cBuffer ) )*0.001f;
	}
	excel.Close();
}

bool CsWorldMapMng::IsWorld( int nID ) const
{ 
	CsWorldMap::MAP_CIT it = m_mapWorld.find( nID );
	if( it == m_mapWorld.end() ) 
		return false;

	return true; 
}

bool CsWorldMapMng::IsArea( int nID ) const
{ 
	CsAreaMap::MAP_CIT it = m_mapArea.find( nID );
	if( it == m_mapArea.end() ) 
		return false;

	return true;
}

std::wstring CsWorldMapMng::GetWorldName( DWORD const& dwWorldID )
{
	std::wstring name;
	CsWorldMap::MAP_IT it = m_mapWorld.find(dwWorldID);
	if( it == m_mapWorld.end() )
		return name;

	SAFE_POINTER_RETVAL( it->second, name );
	CsWorldMap::sINFO* pInfo = it->second->GetInfo();
	SAFE_POINTER_RETVAL( pInfo, name );

	name = pInfo->s_szName;
	return name;
}

std::wstring CsWorldMapMng::GetAreaName( DWORD const& dwAreaID )
{
	std::wstring name;
	CsAreaMap::MAP_IT it = m_mapArea.find(dwAreaID);
	if( it == m_mapArea.end() )
		return name;

	SAFE_POINTER_RETVAL( it->second, name );
	CsAreaMap::sINFO* pInfo = it->second->GetInfo();
	SAFE_POINTER_RETVAL( pInfo, name );

	name = pInfo->s_szName;
	return name;
}