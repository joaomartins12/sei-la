
#include "stdafx.h"
#include "MapMng.h"

void CsMapMng::Delete()
{
	if( nsCsMapTable::g_pMapListMng )
	{
		nsCsMapTable::g_pMapListMng->Delete();
		SAFE_DELETE( nsCsMapTable::g_pMapListMng );
	}

	if( nsCsMapTable::g_pMapStartMng )
	{
		nsCsMapTable::g_pMapStartMng->Delete();
		SAFE_DELETE( nsCsMapTable::g_pMapStartMng );
	}

	if( nsCsMapTable::g_pMapPortalMng )
	{
		nsCsMapTable::g_pMapPortalMng->Delete();
		SAFE_DELETE( nsCsMapTable::g_pMapPortalMng );
	}

	if( nsCsMapTable::g_pMapMonsterMng )
	{
		nsCsMapTable::g_pMapMonsterMng->Delete();
		SAFE_DELETE( nsCsMapTable::g_pMapMonsterMng );
	}

	if( nsCsMapTable::g_pMapNpcMng )
	{
		nsCsMapTable::g_pMapNpcMng->Delete();
		SAFE_DELETE( nsCsMapTable::g_pMapNpcMng );
	}

	if( nsCsMapTable::g_pMapRegionMng )
	{
		nsCsMapTable::g_pMapRegionMng->Delete();
		SAFE_DELETE( nsCsMapTable::g_pMapRegionMng );
	}

	if( nsCsMapTable::g_pMapResurrectionMng )
	{
		nsCsMapTable::g_pMapResurrectionMng->Delete();
		SAFE_DELETE( nsCsMapTable::g_pMapResurrectionMng );
	}

	if( nsCsMapTable::g_pMapCharLightMng )
	{
		nsCsMapTable::g_pMapCharLightMng->Delete();
		SAFE_DELETE( nsCsMapTable::g_pMapCharLightMng );
	}

	if( nsCsMapTable::g_pWeatherMng )
	{
		nsCsMapTable::g_pWeatherMng->Delete();
		SAFE_DELETE( nsCsMapTable::g_pWeatherMng );
	}

	if( nsCsMapTable::g_pMapTableObjMng )
	{
		nsCsMapTable::g_pMapTableObjMng->Delete();
		SAFE_DELETE( nsCsMapTable::g_pMapTableObjMng );
	}
	if( nsCsMapTable::g_pActObjectMng )
	{
		nsCsMapTable::g_pActObjectMng->Delete();
		SAFE_DELETE( nsCsMapTable::g_pActObjectMng );
	}
}


bool CsMapMng::Init( char* cPath )
{
	assert_cs( nsCsMapTable::g_pMapListMng == NULL );
	if( nsCsMapTable::g_bUseMapList == true )
	{
		nsCsMapTable::g_pMapListMng = csnew CsMapListMng;
		if( nsCsMapTable::g_pMapListMng->Init( cPath ) == false )
			return false;
	}

	assert_cs( nsCsMapTable::g_pMapStartMng == NULL );
	if( nsCsMapTable::g_bUseMapStart == true )
	{
		nsCsMapTable::g_pMapStartMng = csnew CsMapStartMng;
		nsCsMapTable::g_pMapStartMng->Init();
	}

	assert_cs( nsCsMapTable::g_pMapPortalMng == NULL );
	if( nsCsMapTable::g_bUseMapPortal == true )
	{
		nsCsMapTable::g_pMapPortalMng = csnew CsMapPortalMng;
		if( nsCsMapTable::g_pMapPortalMng->Init( cPath ) == false )
			return false;
	}

	assert_cs( nsCsMapTable::g_pMapMonsterMng == NULL );
	if( nsCsMapTable::g_eModeMapMonster != nsCsMapTable::eMode_None )
	{		
		nsCsMapTable::g_pMapMonsterMng = csnew CsMapMonsterMng;
		if( nsCsMapTable::g_pMapMonsterMng->Init( cPath ) == false )
			return false;
	}

	assert_cs( nsCsMapTable::g_pMapNpcMng == NULL );
	if( nsCsMapTable::g_bUseMapNpc == true )
	{
		nsCsMapTable::g_pMapNpcMng = csnew CsMapNpcMng;
		if( nsCsMapTable::g_pMapNpcMng->Init( cPath ) == false )
			return false;
	}

	assert_cs( nsCsMapTable::g_pMapRegionMng == NULL );
	if( nsCsMapTable::g_bUseMapRegion == true )
	{
		nsCsMapTable::g_pMapRegionMng = csnew CsMapRegionMng;
		if( nsCsMapTable::g_pMapRegionMng->Init( cPath ) == false )
			return false;
	}

	assert_cs( nsCsMapTable::g_pMapResurrectionMng == NULL );
	if( nsCsMapTable::g_bUseMapResurrection == true )
	{
		nsCsMapTable::g_pMapResurrectionMng = csnew CsMapResurrectionMng;
		nsCsMapTable::g_pMapResurrectionMng->Init();
	}

	assert_cs( nsCsMapTable::g_pMapCharLightMng == NULL );
	if( nsCsMapTable::g_bUseMapCharLight == true )
	{
		nsCsMapTable::g_pMapCharLightMng = csnew CsMapCharLightMng;
		if( nsCsMapTable::g_pMapCharLightMng->Init( cPath ) == false )
			return false;
	}

	assert_cs( nsCsMapTable::g_pWeatherMng == NULL );
	if( nsCsMapTable::g_bUseWeather == true )
	{
		nsCsMapTable::g_pWeatherMng = csnew CsWeatherMng;
		if( nsCsMapTable::g_pWeatherMng->Init( cPath ) == false )
			return false;
	}

	assert_cs( nsCsMapTable::g_pMapTableObjMng == NULL );
	if( nsCsMapTable::g_bUseTableObj == true )
	{
		nsCsMapTable::g_pMapTableObjMng = csnew CsMapTableObjMng;
		if( nsCsMapTable::g_pMapTableObjMng->Init() == false )
			return false;
	}

	assert_cs( nsCsMapTable::g_pActObjectMng == NULL );
	if( nsCsMapTable::g_bUseActObject == true )
	{
		nsCsMapTable::g_pActObjectMng = csnew CsMapObjectMng;
		if( nsCsMapTable::g_pActObjectMng->Init( cPath ) == false )
			return false;
	}

	return true;
}

void CsMapMng::Save( bool bOther, bool bList, bool bPortal, bool bCharLight )
{
	if( _access_s( nsCsFileTable::GetPath_MAP(), 0 ) != 0 )
	{
		{
			YExcel::BasicExcel excel;
			excel.New( MAPEXCEL_MAX_SHEET_COUNT );
			excel.GetWorksheet( ME_START_SHEET_INDEX )->Rename( ME_START_SHEET_NAME );
			excel.GetWorksheet( ME_MONSTER_SHEET_INDEX )->Rename( ME_MONSTER_SHEET_NAME );
			excel.GetWorksheet( ME_NPC_SHEET_INDEX )->Rename( ME_NPC_SHEET_NAME );
			excel.GetWorksheet( ME_REGION_SHEET_INDEX )->Rename( ME_REGION_SHEET_NAME );
			excel.GetWorksheet( ME_RESURRECTION_SHEET_INDEX )->Rename( ME_RESURRECTION_SHEET_NAME );
			excel.GetWorksheet( ME_LIMITEVO_SHEET_INDEX )->Rename( ME_LIMITEVO_SHEET_NAME );
			excel.GetWorksheet( ME_WEATHER_SHEET_INDEX )->Rename( ME_WEATHER_SHEET_NAME );
			//Object 시트 추가
			excel.GetWorksheet( ME_OBJECT_SHEET_INDEX )->Rename( ME_OBJECT_SHEET_NAME );
			excel.GetWorksheet( ME_BUFF_REGION_SHEET_INDEX )->Rename( ME_BUFF_REGION_SHEET_NAME );
			excel.SaveAs( nsCsFileTable::GetPath_MAP() );
		}

		{
			YExcel::BasicExcel excel;
			excel.New( 1 );
			excel.GetWorksheet( ME_LIST_SHEET_INDEX )->Rename( ME_LIST_SHEET_NAME );
			excel.SaveAs( nsCsFileTable::GetPath_MAP_List() );
		}

		{
			YExcel::BasicExcel excel;
			excel.New( 1 );
			excel.GetWorksheet( ME_PORTAL_SHEET_INDEX )->Rename( ME_PORTAL_SHEET_NAME );
			excel.SaveAs( nsCsFileTable::GetPath_MAP_Portal() );
		}

		{
			YExcel::BasicExcel excel;
			excel.New( 1 );
			excel.GetWorksheet( ME_CHARLIGHT_SHEET_INDEX )->Rename( ME_CHARLIGHT_SHEET_NAME );
			excel.SaveAs( nsCsFileTable::GetPath_MAP_CharLight() );
		}
	}
	else
	{
		YExcel::BasicExcel excel;
		bool bLoad = excel.Load( nsCsFileTable::GetPath_MAP() );
		assert_cs( bLoad == true );

		if( excel.GetWorksheet( ME_START_SHEET_NAME ) == NULL )
			excel.AddWorksheet( ME_START_SHEET_NAME, ME_START_SHEET_INDEX );

		if( excel.GetWorksheet( ME_MONSTER_SHEET_NAME ) == NULL )
			excel.AddWorksheet( ME_MONSTER_SHEET_NAME, ME_MONSTER_SHEET_INDEX );

		if( excel.GetWorksheet( ME_NPC_SHEET_NAME ) == NULL )
			excel.AddWorksheet( ME_NPC_SHEET_NAME, ME_NPC_SHEET_INDEX );

		if( excel.GetWorksheet( ME_REGION_SHEET_NAME ) == NULL )
			excel.AddWorksheet( ME_REGION_SHEET_NAME, ME_REGION_SHEET_INDEX );

		if( excel.GetWorksheet( ME_RESURRECTION_SHEET_NAME ) == NULL )
			excel.AddWorksheet( ME_RESURRECTION_SHEET_NAME, ME_RESURRECTION_SHEET_INDEX );

		if( excel.GetWorksheet( ME_LIMITEVO_SHEET_NAME ) == NULL )
			excel.AddWorksheet( ME_LIMITEVO_SHEET_NAME, ME_LIMITEVO_SHEET_INDEX );

		if( excel.GetWorksheet( ME_BUFF_REGION_SHEET_NAME ) == NULL )
			excel.AddWorksheet( ME_BUFF_REGION_SHEET_NAME, ME_BUFF_REGION_SHEET_INDEX );

		if( excel.GetWorksheet( ME_WEATHER_SHEET_NAME ) == NULL )
			excel.AddWorksheet( ME_WEATHER_SHEET_NAME, ME_WEATHER_SHEET_INDEX );
		if( excel.GetWorksheet( ME_OBJECT_SHEET_NAME ) == NULL )
			excel.AddWorksheet( ME_OBJECT_SHEET_NAME, ME_OBJECT_SHEET_INDEX );
		// 시트 제거
		if( excel.GetWorksheet( ME_PORTAL_SHEET_NAME ) != NULL )
		{
			excel.DeleteWorksheet( ME_PORTAL_SHEET_NAME );
		}
		if( excel.GetWorksheet( ME_LIST_SHEET_NAME ) != NULL )
		{
			excel.DeleteWorksheet( ME_LIST_SHEET_NAME );
		}
		if( excel.GetWorksheet( ME_CHARLIGHT_SHEET_NAME ) != NULL )
		{
			excel.DeleteWorksheet( ME_CHARLIGHT_SHEET_NAME );
		}

		excel.Save();
	}

	if( ( nsCsMapTable::g_pMapListMng )&&( bList ) )
		nsCsMapTable::g_pMapListMng->Save();

	if( ( nsCsMapTable::g_pMapStartMng )&&( bOther ) )
		nsCsMapTable::g_pMapStartMng->Save();

	if( ( nsCsMapTable::g_pMapPortalMng )&&( bPortal ) )
		nsCsMapTable::g_pMapPortalMng->Save();

	if( ( nsCsMapTable::g_pMapMonsterMng )&&( bOther ) )
		nsCsMapTable::g_pMapMonsterMng->Save();

	if( ( nsCsMapTable::g_pMapNpcMng )&&( bOther ) )
		nsCsMapTable::g_pMapNpcMng->Save();

	if( ( nsCsMapTable::g_pMapRegionMng )&&( bOther ) )
		nsCsMapTable::g_pMapRegionMng->Save();

	if( ( nsCsMapTable::g_pMapResurrectionMng )&&( bOther ) )
		nsCsMapTable::g_pMapResurrectionMng->Save();

	if( ( nsCsMapTable::g_pMapCharLightMng )&&( bCharLight ) )
		nsCsMapTable::g_pMapCharLightMng->Save();

	if( (nsCsMapTable::g_pWeatherMng )&&( bOther ) )
		nsCsMapTable::g_pWeatherMng->Save();

	if( (nsCsMapTable::g_pActObjectMng )&&( bOther ) )
		nsCsMapTable::g_pActObjectMng->Save();
}



void CsMapMng::SaveBin( char* cPath )
{
	if( nsCsMapTable::g_pMapListMng )
		nsCsMapTable::g_pMapListMng->SaveBin( cPath );

	if( nsCsMapTable::g_pMapStartMng )
		assert_cs( false );

	if( nsCsMapTable::g_pMapPortalMng )
		nsCsMapTable::g_pMapPortalMng->SaveBin( cPath );

	if( nsCsMapTable::g_pMapMonsterMng )
		nsCsMapTable::g_pMapMonsterMng->SaveBin( cPath );

	if( nsCsMapTable::g_pMapNpcMng )
		nsCsMapTable::g_pMapNpcMng->SaveBin( cPath );

	if( nsCsMapTable::g_pMapRegionMng )
		nsCsMapTable::g_pMapRegionMng->SaveBin( cPath );

	if( nsCsMapTable::g_pMapResurrectionMng )
		assert_cs( false );

	if( nsCsMapTable::g_pMapCharLightMng )
		nsCsMapTable::g_pMapCharLightMng->SaveBin( cPath );

	if( nsCsMapTable::g_pWeatherMng )
		nsCsMapTable::g_pWeatherMng->SaveBin( cPath );
	if( nsCsMapTable::g_pActObjectMng )
		nsCsMapTable::g_pActObjectMng->SaveBin( cPath );
}