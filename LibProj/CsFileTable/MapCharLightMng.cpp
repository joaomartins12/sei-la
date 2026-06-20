

#include "stdafx.h"
#include "MapCharLightMng.h"


void CsMapCharLightMng::Delete()
{
	CsMapCharLightGroup::MAP_IT it = m_mapGroup.begin();
	CsMapCharLightGroup::MAP_IT itEnd = m_mapGroup.end();
	for( ; it!=itEnd; ++it )
	{
		it->second->Delete();
		delete it->second;
	}
	m_mapGroup.clear();
}

bool CsMapCharLightMng::Init( char* cPath, nsCsFileTable::eFILE_TYPE ft /*=nsCsFileTable::g_eFileType*/ )
{
	switch( ft )
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

	return true;
}

CsMapCharLightGroup* CsMapCharLightMng::GetGroup( DWORD dwMapID )
{
	if( IsGroup( dwMapID ) == false )
	{
		CsMapCharLightGroup* pGroup = csnew CsMapCharLightGroup;
		pGroup->Init();
		m_mapGroup[ dwMapID ] = pGroup;
	}
	return m_mapGroup[ dwMapID ];
}

void CsMapCharLightMng::DeleteGroup( DWORD dwMapID )
{
	if( IsGroup( dwMapID ) == false )
		return;

	m_mapGroup[ dwMapID ]->Delete();
	delete m_mapGroup[ dwMapID ];
	m_mapGroup.erase( dwMapID );
}

#define MES_COL_MAPID				0
#define MES_COL_IS_CHAR				1
#define MES_COL_IS_SHADOW			2
#define MES_COL_ENABLE				3
#define MES_COL_TYPE				4
#define MES_COL_DIFFUSE_R			5
#define MES_COL_DIFFUSE_G			6
#define MES_COL_DIFFUSE_B			7
#define MES_COL_AMBIENT_R			8
#define MES_COL_AMBIENT_G			9
#define MES_COL_AMBIENT_B			10
#define MES_COL_SPECULAR_R			11
#define MES_COL_SPECULAR_G			12
#define MES_COL_SPECULAR_B			13
#define MES_COL_POWER				14

#define MES_COL_DIR_ROT00			15
#define MES_COL_DIR_ROT01			16
#define MES_COL_DIR_ROT02			17
#define MES_COL_DIR_ROT10			18
#define MES_COL_DIR_ROT11			19
#define MES_COL_DIR_ROT12			20
#define MES_COL_DIR_ROT20			21
#define MES_COL_DIR_ROT21			22
#define MES_COL_DIR_ROT22			23
#define MES_COL_DIR_POS0			24
#define MES_COL_DIR_POS1			25
#define MES_COL_DIR_POS2			26
#define MES_COL_DIR_SCALE			27

#define MES_COL_POINT_POS0			15
#define MES_COL_POINT_POS1			16
#define MES_COL_POINT_POS2			17
#define MES_COL_POINT_C				18
#define MES_COL_POINT_L				19
#define MES_COL_POINT_Q				20
#define MES_COL_POINT_RANGE			21

void CsMapCharLightMng::_LoadExcel()
{
	char cBuffer[ CEXCLE_CELL_LEN ];

	cExcelReader excel;
	bool bSuccess = excel.Open( nsCsFileTable::GetPath_MAP_CharLight(), ME_CHARLIGHT_SHEET_NAME );
	assert_cs( bSuccess == true );
	// 2줄은 버린다.
	bSuccess = excel.MoveFirst();
	assert_cs( bSuccess == true );

	if( atoi( excel.GetData( ME_VERSION_COL, cBuffer ) ) != ME_CHARLIGHT_VERSION )
	{
		MessageBox( NULL, _T( "Map.xls Region Sheet 의 버젼이 틀립니다.\n송명수에게 문의해주세요." ), NULL, MB_OK );
		return;
	}

	bSuccess = excel.MoveNext();
	assert_cs( bSuccess == true );

	// 데이터 입력
	CsMapCharLightGroup* pMapGroup = NULL;
	CsMapCharLight::sINFO_BASE infoBase;
	CsMapCharLight::sINFO_DIR infoDir;
	CsMapCharLight::sINFO_POINT infoPoint;

	while( excel.MoveNext() == true )
	{
		infoBase.s_dwMapID = atoi( excel.GetData( MES_COL_MAPID, cBuffer ) );
		if( infoBase.s_dwMapID == 0 )
			continue;

		infoBase.s_bChar = ( atoi( excel.GetData( MES_COL_IS_CHAR, cBuffer ) ) == 1 );
		infoBase.s_bShadow = ( atoi( excel.GetData( MES_COL_IS_SHADOW, cBuffer ) ) == 1 );
		infoBase.s_bEnableLight = ( atoi( excel.GetData( MES_COL_ENABLE, cBuffer ) ) != 0 );
		infoBase.s_nType = atoi( excel.GetData( MES_COL_TYPE, cBuffer ) );

		infoBase.s_fDiffuse[ 0 ] = (float)atof( excel.GetData( MES_COL_DIFFUSE_R, cBuffer ) );
		infoBase.s_fDiffuse[ 1 ] = (float)atof( excel.GetData( MES_COL_DIFFUSE_G, cBuffer ) );
		infoBase.s_fDiffuse[ 2 ] = (float)atof( excel.GetData( MES_COL_DIFFUSE_B, cBuffer ) );
		infoBase.s_fAmbient[ 0 ] = (float)atof( excel.GetData( MES_COL_AMBIENT_R, cBuffer ) );
		infoBase.s_fAmbient[ 1 ] = (float)atof( excel.GetData( MES_COL_AMBIENT_G, cBuffer ) );
		infoBase.s_fAmbient[ 2 ] = (float)atof( excel.GetData( MES_COL_AMBIENT_B, cBuffer ) );
		infoBase.s_fSpecular[ 0 ] = (float)atof( excel.GetData( MES_COL_SPECULAR_R, cBuffer ) );
		infoBase.s_fSpecular[ 1 ] = (float)atof( excel.GetData( MES_COL_SPECULAR_G, cBuffer ) );
		infoBase.s_fSpecular[ 2 ] = (float)atof( excel.GetData( MES_COL_SPECULAR_B, cBuffer ) );
		infoBase.s_fPower = (float)atof( excel.GetData( MES_COL_POWER, cBuffer ) );

		if( infoBase.s_nType == 0 )		// Direction
		{
			memcpy( &infoDir, &infoBase, sizeof( CsMapCharLight::sINFO_BASE ) );

			infoDir.s_fRot[ 0 ][ 0 ] = (float)atof( excel.GetData( MES_COL_DIR_ROT00, cBuffer ) );
			infoDir.s_fRot[ 0 ][ 1 ] = (float)atof( excel.GetData( MES_COL_DIR_ROT01, cBuffer ) );
			infoDir.s_fRot[ 0 ][ 2 ] = (float)atof( excel.GetData( MES_COL_DIR_ROT02, cBuffer ) );
			infoDir.s_fRot[ 1 ][ 0 ] = (float)atof( excel.GetData( MES_COL_DIR_ROT10, cBuffer ) );
			infoDir.s_fRot[ 1 ][ 1 ] = (float)atof( excel.GetData( MES_COL_DIR_ROT11, cBuffer ) );
			infoDir.s_fRot[ 1 ][ 2 ] = (float)atof( excel.GetData( MES_COL_DIR_ROT12, cBuffer ) );
			infoDir.s_fRot[ 2 ][ 0 ] = (float)atof( excel.GetData( MES_COL_DIR_ROT20, cBuffer ) );
			infoDir.s_fRot[ 2 ][ 1 ] = (float)atof( excel.GetData( MES_COL_DIR_ROT21, cBuffer ) );
			infoDir.s_fRot[ 2 ][ 2 ] = (float)atof( excel.GetData( MES_COL_DIR_ROT22, cBuffer ) );
			infoDir.s_fPos[ 0 ] = (float)atof( excel.GetData( MES_COL_DIR_POS0, cBuffer ) );
			infoDir.s_fPos[ 1 ] = (float)atof( excel.GetData( MES_COL_DIR_POS1, cBuffer ) );
			infoDir.s_fPos[ 2 ] = (float)atof( excel.GetData( MES_COL_DIR_POS2, cBuffer ) );
			infoDir.s_fScale = (float)atof( excel.GetData( MES_COL_DIR_SCALE, cBuffer ) );

			GetGroup( infoDir.s_dwMapID )->Add( &infoDir );
		}
		else
		{
			assert_cs( infoBase.s_nType == 1 ); // Point
			memcpy( &infoPoint, &infoBase, sizeof( CsMapCharLight::sINFO_BASE ) );

			infoPoint.s_Pos[ 0 ] = (float)atof( excel.GetData( MES_COL_POINT_POS0, cBuffer ) );
			infoPoint.s_Pos[ 1 ] = (float)atof( excel.GetData( MES_COL_POINT_POS1, cBuffer ) );
			infoPoint.s_Pos[ 2 ] = (float)atof( excel.GetData( MES_COL_POINT_POS2, cBuffer ) );
			infoPoint.s_C = (float)atof( excel.GetData( MES_COL_POINT_C, cBuffer ) );
			infoPoint.s_L = (float)atof( excel.GetData( MES_COL_POINT_L, cBuffer ) );
			infoPoint.s_Q = (float)atof( excel.GetData( MES_COL_POINT_Q, cBuffer ) );
			infoPoint.s_Range = (float)atof( excel.GetData( MES_COL_POINT_RANGE, cBuffer ) );

			GetGroup( infoPoint.s_dwMapID )->Add( &infoPoint );
		}
	}
	excel.Close();
}

void CsMapCharLightMng::Save()
{
	YExcel::BasicExcel excel;
	bool bLoad = excel.Load( nsCsFileTable::GetPath_MAP_CharLight() );
	assert_cs( bLoad == true );

	YExcel::BasicExcelWorksheet* pSheet = excel.GetWorksheet( ME_CHARLIGHT_SHEET_NAME );
	assert_cs( pSheet != NULL );

	pSheet->DeleteAllRows();

	pSheet->Cell( 0, ME_VERSION_COL )->SetInteger( ME_CHARLIGHT_VERSION );
	for( int i=1; i<=CsMax(MES_COL_DIR_SCALE,MES_COL_POINT_RANGE); ++i )
	{
		pSheet->Cell( 0, i )->SetInteger( 1 );
	}

	pSheet->Cell( 1, MES_COL_MAPID )->SetWString( _T( "map idx" ) );
	pSheet->Cell( 1, MES_COL_IS_CHAR )->SetWString( _T( "charLight" ) );
	pSheet->Cell( 1, MES_COL_IS_SHADOW )->SetWString( _T( "shadowLight" ) );
	pSheet->Cell( 1, MES_COL_ENABLE )->SetWString( _T( "enable" ) );
	pSheet->Cell( 1, MES_COL_TYPE )->SetWString( _T( "type" ) );
	pSheet->Cell( 1, MES_COL_DIFFUSE_R )->SetWString( _T( "dr" ) );
	pSheet->Cell( 1, MES_COL_DIFFUSE_G )->SetWString( _T( "dg" ) );
	pSheet->Cell( 1, MES_COL_DIFFUSE_B )->SetWString( _T( "db" ) );
	pSheet->Cell( 1, MES_COL_AMBIENT_R )->SetWString( _T( "ar" ) );
	pSheet->Cell( 1, MES_COL_AMBIENT_G )->SetWString( _T( "ag" ) );
	pSheet->Cell( 1, MES_COL_AMBIENT_B )->SetWString( _T( "ab" ) );
	pSheet->Cell( 1, MES_COL_SPECULAR_R )->SetWString( _T( "sr" ) );
	pSheet->Cell( 1, MES_COL_SPECULAR_G )->SetWString( _T( "sg" ) );
	pSheet->Cell( 1, MES_COL_SPECULAR_B )->SetWString( _T( "sb" ) );
	pSheet->Cell( 1, MES_COL_POWER )->SetWString( _T( "power" ) );
	for( int i=MES_COL_POWER; i<=CsMax(MES_COL_DIR_SCALE,MES_COL_POINT_RANGE); ++i )
		pSheet->Cell( 1, i )->SetWString( _T( "sub" ) );

	size_t siInputRow = 2;
	CsMapCharLightGroup::MAP_IT itGroup = m_mapGroup.begin();
	CsMapCharLightGroup::MAP_IT itGroupEnd = m_mapGroup.end();

	CsMapCharLight::sINFO_BASE* pInfoBase = NULL;
	CsMapCharLight::sINFO_DIR* pInfoDir = NULL;
	CsMapCharLight::sINFO_POINT* pInfoPoint = NULL;

	for( ; itGroup!=itGroupEnd; ++itGroup )
	{
		CsMapCharLight::LIST* pList = itGroup->second->GetList();
		CsMapCharLight::LIST_IT it = pList->begin();
		CsMapCharLight::LIST_IT itEnd = pList->end();
		for( ; it!=itEnd; ++it )
		{
			pInfoBase = ( *it )->GetInfo();
			pSheet->Cell( siInputRow, MES_COL_MAPID )->SetInteger( pInfoBase->s_dwMapID );
			pSheet->Cell( siInputRow, MES_COL_IS_CHAR )->SetInteger( pInfoBase->s_bChar == true ? 1 : 0 );
			pSheet->Cell( siInputRow, MES_COL_IS_SHADOW )->SetInteger( pInfoBase->s_bShadow == true ? 1 : 0 );
			pSheet->Cell( siInputRow, MES_COL_ENABLE )->SetInteger(  pInfoBase->s_bEnableLight == true ? 1 : 0 );
			pSheet->Cell( siInputRow, MES_COL_TYPE )->SetInteger( pInfoBase->s_nType );
			pSheet->Cell( siInputRow, MES_COL_DIFFUSE_R )->SetDouble( pInfoBase->s_fDiffuse[ 0 ] );
			pSheet->Cell( siInputRow, MES_COL_DIFFUSE_G )->SetDouble( pInfoBase->s_fDiffuse[ 1 ] );
			pSheet->Cell( siInputRow, MES_COL_DIFFUSE_B )->SetDouble( pInfoBase->s_fDiffuse[ 2 ] );
			pSheet->Cell( siInputRow, MES_COL_AMBIENT_R )->SetDouble( pInfoBase->s_fAmbient[ 0 ] );
			pSheet->Cell( siInputRow, MES_COL_AMBIENT_G )->SetDouble( pInfoBase->s_fAmbient[ 1 ] );
			pSheet->Cell( siInputRow, MES_COL_AMBIENT_B )->SetDouble( pInfoBase->s_fAmbient[ 2 ] );
			pSheet->Cell( siInputRow, MES_COL_SPECULAR_R )->SetDouble( pInfoBase->s_fSpecular[ 0 ] );
			pSheet->Cell( siInputRow, MES_COL_SPECULAR_G )->SetDouble( pInfoBase->s_fSpecular[ 1 ] );
			pSheet->Cell( siInputRow, MES_COL_SPECULAR_B )->SetDouble( pInfoBase->s_fSpecular[ 2 ] );
			pSheet->Cell( siInputRow, MES_COL_POWER )->SetDouble( pInfoBase->s_fPower );

			if( pInfoBase->s_nType == 0 )		// Direction
			{
				pInfoDir = (CsMapCharLight::sINFO_DIR*)pInfoBase;
				pSheet->Cell( siInputRow, MES_COL_DIR_ROT00 )->SetDouble( pInfoDir->s_fRot[ 0 ][ 0 ] );
				pSheet->Cell( siInputRow, MES_COL_DIR_ROT01 )->SetDouble( pInfoDir->s_fRot[ 0 ][ 1 ] );
				pSheet->Cell( siInputRow, MES_COL_DIR_ROT02 )->SetDouble( pInfoDir->s_fRot[ 0 ][ 2 ] );
				pSheet->Cell( siInputRow, MES_COL_DIR_ROT10 )->SetDouble( pInfoDir->s_fRot[ 1 ][ 0 ] );
				pSheet->Cell( siInputRow, MES_COL_DIR_ROT11 )->SetDouble( pInfoDir->s_fRot[ 1 ][ 1 ] );
				pSheet->Cell( siInputRow, MES_COL_DIR_ROT12 )->SetDouble( pInfoDir->s_fRot[ 1 ][ 2 ] );
				pSheet->Cell( siInputRow, MES_COL_DIR_ROT20 )->SetDouble( pInfoDir->s_fRot[ 2 ][ 0 ] );
				pSheet->Cell( siInputRow, MES_COL_DIR_ROT21 )->SetDouble( pInfoDir->s_fRot[ 2 ][ 1 ] );
				pSheet->Cell( siInputRow, MES_COL_DIR_ROT22 )->SetDouble( pInfoDir->s_fRot[ 2 ][ 2 ] );
				pSheet->Cell( siInputRow, MES_COL_DIR_POS0 )->SetDouble( pInfoDir->s_fPos[ 0 ] );
				pSheet->Cell( siInputRow, MES_COL_DIR_POS1 )->SetDouble( pInfoDir->s_fPos[ 1 ] );
				pSheet->Cell( siInputRow, MES_COL_DIR_POS2 )->SetDouble( pInfoDir->s_fPos[ 2 ] );
				pSheet->Cell( siInputRow, MES_COL_DIR_SCALE )->SetDouble( pInfoDir->s_fScale );
			}
			else
			{
				assert_cs( pInfoBase->s_nType == 1 );		// Point
				pInfoPoint = (CsMapCharLight::sINFO_POINT*)pInfoBase;

				pSheet->Cell( siInputRow, MES_COL_POINT_POS0 )->SetDouble( pInfoPoint->s_Pos[ 0 ] );
				pSheet->Cell( siInputRow, MES_COL_POINT_POS1 )->SetDouble( pInfoPoint->s_Pos[ 1 ] );
				pSheet->Cell( siInputRow, MES_COL_POINT_POS2 )->SetDouble( pInfoPoint->s_Pos[ 2 ] );
				pSheet->Cell( siInputRow, MES_COL_POINT_C )->SetDouble( pInfoPoint->s_C );
				pSheet->Cell( siInputRow, MES_COL_POINT_L )->SetDouble( pInfoPoint->s_L );
				pSheet->Cell( siInputRow, MES_COL_POINT_Q )->SetDouble( pInfoPoint->s_Q );
				pSheet->Cell( siInputRow, MES_COL_POINT_RANGE )->SetDouble( pInfoPoint->s_Range );
			}

			++siInputRow;
		}		
	}

	excel.SaveAs( nsCsFileTable::GetPath_MAP_CharLight() );
}

#define MAP_CHARLIGHT_BIN			"MapCharLight.bin"
void CsMapCharLightMng::SaveBin( char* cPath )
{
	char cName[ MAX_PATH ];
	sprintf_s( cName, MAX_PATH, "%s\\%s", cPath, MAP_CHARLIGHT_BIN );

	FILE *fp = NULL;
	fopen_s( &fp, cName, "wb" );
	assert_cs( fp != NULL );

	int nCount = (int)m_mapGroup.size();
	fwrite( &nCount, sizeof( int ), 1, fp );

	CsMapCharLightGroup::MAP_IT itGroup = m_mapGroup.begin();
	CsMapCharLightGroup::MAP_IT itGroupEnd = m_mapGroup.end();
	for( ; itGroup!=itGroupEnd; ++itGroup )
	{		
		CsMapCharLight::LIST* pList = itGroup->second->GetList();

		int nCountLight = (int)pList->size();
		fwrite( &nCountLight, sizeof( int ), 1, fp );

		CsMapCharLight::LIST_IT it = pList->begin();
		CsMapCharLight::LIST_IT itEnd = pList->end();
		for( ; it!=itEnd; ++it )
		{
			int nType = (*it)->GetInfo()->s_nType;
			fwrite( &nType, sizeof( int ), 1, fp );

			switch( nType )
			{
			case 0:
				fwrite( (*it)->GetInfo(), sizeof( CsMapCharLight::sINFO_DIR ), 1, fp );
				break;
			case 1:
				fwrite( (*it)->GetInfo(), sizeof( CsMapCharLight::sINFO_POINT ), 1, fp );
				break;
			default:
				assert_cs( false );
			}			
		}
	}
	fclose( fp );
}

bool CsMapCharLightMng::_LoadBin( char* cPath )
{
	char cName[ MAX_PATH ];
	sprintf_s( cName, MAX_PATH, "%s\\%s", cPath, MAP_CHARLIGHT_BIN );

	FILE *fp = NULL;
	fopen_s( &fp, cName, "rb" );
	if( fp == NULL )
	{
		CsMessageBoxA( MB_OK, "%s 파일이 존재 하지 않습니다.", cName );
		return false;
	}

	int nCount;
	fread( &nCount, sizeof( int ), 1, fp );

	for( int i=0; i<nCount; ++i )
	{
		int nCountLight;
		fread( &nCountLight, sizeof( int ), 1, fp );
		for( int l=0; l<nCountLight; ++l )
		{
			int nType;
			fread( &nType, sizeof( int ), 1, fp );

			switch( nType )
			{
			case 0:
				{
					CsMapCharLight::sINFO_DIR info;
					fread( &info, sizeof( CsMapCharLight::sINFO_DIR ), 1, fp );
					GetGroup( info.s_dwMapID )->Add( &info );
				}				
				break;
			case 1:
				{
					CsMapCharLight::sINFO_POINT info;
					fread( &info, sizeof( CsMapCharLight::sINFO_POINT ), 1, fp );
					GetGroup( info.s_dwMapID )->Add( &info );
				}
				break;
			default:
				assert_cs( false );
			}
		}
	}

	fclose( fp );

	return true;
}


void CsMapCharLightMng::_LoadFilePack( char* cPath )
{
	char cName[ MAX_PATH ];
	sprintf_s( cName, MAX_PATH, "%s\\%s", cPath, MAP_CHARLIGHT_BIN );

	if( !CsFPS::CsFPSystem::SeekLock( FT_PACKHANDLE ) )
		return;

	int nHandle = CsFPS::CsFPSystem::GetFileHandle( FT_PACKHANDLE, cName );

	int nCount;
	_read( nHandle, &nCount, sizeof( int ) );

	for( int i=0; i<nCount; ++i )
	{
		int nCountLight;
		_read( nHandle, &nCountLight, sizeof( int ) );
		for( int l=0; l<nCountLight; ++l )
		{
			int nType;
			_read( nHandle, &nType, sizeof( int ) );

			switch( nType )
			{
			case 0:
				{
					CsMapCharLight::sINFO_DIR info;
					_read( nHandle, &info, sizeof( CsMapCharLight::sINFO_DIR ) );
					GetGroup( info.s_dwMapID )->Add( &info );
				}				
				break;
			case 1:
				{
					CsMapCharLight::sINFO_POINT info;
					_read( nHandle, &info, sizeof( CsMapCharLight::sINFO_POINT ) );
					GetGroup( info.s_dwMapID )->Add( &info );
				}
				break;
			default:
				assert_cs( false );
			}
		}
	}
	CsFPS::CsFPSystem::SeekUnLock( FT_PACKHANDLE );
}