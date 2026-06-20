
#include "stdafx.h"
#include "Resist.h"

cResist				g_Resist;

#define SIZE_SET	( s_dwStructSize = sizeof( *this ) - sizeof( sSTRUCT_HEADER ) )

void cResist::sHEADER::Reset()
{
	SIZE_SET;
	s_dwVersion = RESIST_VERSION;
	s_dwStructCount = RESIST_STRUCT_COUNT;
}

void cResist::sVIEW_STATE::Reset()
{
	SIZE_SET;
}

void cResist::sMAINMENU::Reset()
{
	SIZE_SET;

	_stprintf_s( s_szSaveLoad, MAX_PATH, L".\\Data\\Map" );
	_stprintf_s( s_szCMI, MAX_PATH, L".\\Data\\Map" );
}

void cResist::sNEW_TERRAIN::Reset()
{
	SIZE_SET;

	s_nWidth = 0;
	s_nHeight = 0;
	s_nGrid = 200;
	s_nTreeLevel = 2;

	s_nBaseMPPG = 1;
	s_nAlphaMPPG = 2;
}

void cResist::sHEIGHT_MAP::Reset()
{
	SIZE_SET;

	s_fHeightMapRate = 1.0f;
	memset( s_szFile, 0, sizeof(TCHAR)*MAX_PATH );
	_stprintf_s( s_szPath, MAX_PATH, L".\\Data\\Map" );
}

void cResist::sMAPPING::Reset()
{
	SIZE_SET;
	_stprintf_s( s_szTexturePath, MAX_PATH, L".\\Data\\Map" );
	s_fAlphaDecreaseConstant = 0.7f;
	s_nMappingSize = 17;
	s_nMappingDecrement = 20;
	s_fGrayDecreaseConstant = 0.7f;
	s_nGraySize = 17;
	s_nGrayDecrement = 20;
	s_GrayColor = NiColor( 0.78f, 0.78f, 0.78f );
}

void cResist::sOBJECT::Reset()
{
	SIZE_SET;
	_stprintf_s( s_szObjectPath, MAX_PATH, L".\\Data\\Map" );
}

void cResist::sSKYBOX::Reset()
{
	SIZE_SET;
	_stprintf_s( s_szSkyBoxPath, MAX_PATH, L".\\Data\\Map" );
}

void cResist::sETC::Reset()
{
	SIZE_SET;
	_stprintf_s( s_szMinimapPath, MAX_PATH, L".\\Data\\Map" );
	_stprintf_s( s_szEMRPath, MAX_PATH, L".\\" );
}

void cResist::sMAP::Reset()
{
	SIZE_SET;
	_stprintf_s( s_szBGSound, MAX_PATH, L".\\Data\\Music" );
}

void cResist::sPATH::Reset()
{
	SIZE_SET;
	s_nTerrainPathHeight = 20;
	s_nObjectPathHeight = 20;
	s_nMaxPathHeight = 20;
}

void cResist::sMONSTER::Reset()
{
	SIZE_SET;
	s_nDefRadius = 1000;
	s_nDefMonCount = 10;
	s_nDefMonResfonTime = 60;
	s_fViewMonScale = 15.0f;
	s_nDefMove = 0;
	s_bInsRegen = false;
}


//==============================================================================================
//==============================================================================================
//==============================================================================================

cResist::cResist()
{
}

cResist::~cResist()
{

}

void cResist::Reset()
{
	m_Header.Reset();
	m_ViewState.Reset();
	m_MainMenu.Reset();
	m_NewTerrain.Reset();
	m_HeightMap.Reset();	
	m_Mapping.Reset();
	m_Object.Reset();
	m_SkyBox.Reset();
	m_Etc.Reset();
	m_Map.Reset();
	m_Path.Reset();
	m_Monster.Reset();
}

void cResist::Save()
{
	TCHAR szFolder[ MAX_PATH ] = {0, };
	nsCSFILE::GetFilePathOnly( szFolder, MAX_PATH, RESIST_PATH );
	// 폴더가 존재 하지 않는다면 폴더 생성
	nsCSFILE::CreateDirectory( szFolder );
	assert_cs( m_Header.s_dwVersion == RESIST_VERSION );	
	m_Header.s_dwStructCount = RESIST_STRUCT_COUNT;

	FILE* fp = NULL;
	_tfopen_s( &fp, RESIST_PATH, L"wb" );

	fwrite( &m_Header, sizeof( sHEADER ), 1, fp );
	fwrite( &m_MainMenu, sizeof( sMAINMENU ), 1, fp );
	fwrite( &m_NewTerrain, sizeof( sNEW_TERRAIN ), 1, fp );
	fwrite( &m_HeightMap, sizeof( sHEIGHT_MAP ), 1, fp );
	fwrite( &m_ViewState, sizeof( sVIEW_STATE ), 1, fp );
	fwrite( &m_Mapping, sizeof( sMAPPING ), 1, fp );
	fwrite( &m_Object, sizeof( sOBJECT ), 1, fp );
	fwrite( &m_Etc, sizeof( sETC ), 1, fp );
	fwrite( &m_SkyBox, sizeof( sSKYBOX ), 1, fp );
	fwrite( &m_Map, sizeof( sMAP ), 1, fp );
	fwrite( &m_Path, sizeof( sPATH ), 1, fp );
	fwrite( &m_Monster, sizeof( sMONSTER ), 1, fp );

	fclose( fp );
}

void cResist::Load()
{
				#define READ_RESIST_FILE( ClassMember )	{\
					if( --nStructCount == 0 )\
					{\
						fclose( fp );\
						return;\
					}\
					fread( &sh, sizeof( sSTRUCT_HEADER ), 1, fp );\
					fread( ( (BYTE*)(&##ClassMember ) ) + sizeof( sSTRUCT_HEADER ), sh.s_dwStructSize, 1, fp );\
				}


	Reset();

	if( _taccess_s( RESIST_PATH, 0 ) != 0 )
	{
		return;
	}
	
	// 파일이 존재 한다면 값을 읽어 온다
	FILE* fp = NULL;
	_tfopen_s( &fp, RESIST_PATH, L"rb" );
	
	sSTRUCT_HEADER sh;
	int nStructCount = RESIST_STRUCT_COUNT;
	READ_RESIST_FILE( m_Header );
	nStructCount = m_Header.s_dwStructCount;
	// 버젼 체크
	if( m_Header.s_dwVersion != RESIST_VERSION )
	{
		MessageBox( NULL, L"저장된 레지스트 파일은 예전버젼 이므로\n제거 하고 새로 만들겠습니다.", L"Resist", MB_OK );
		m_Header.Reset();
		fclose( fp );
		return;
	}

	READ_RESIST_FILE( m_MainMenu );
	READ_RESIST_FILE( m_NewTerrain );
	READ_RESIST_FILE( m_HeightMap );
	READ_RESIST_FILE( m_ViewState );
	READ_RESIST_FILE( m_Mapping );
	READ_RESIST_FILE( m_Object );
	READ_RESIST_FILE( m_Etc );
	READ_RESIST_FILE( m_SkyBox );
	READ_RESIST_FILE( m_Map );
	READ_RESIST_FILE( m_Path );
	READ_RESIST_FILE( m_Monster );

	fclose( fp );
}