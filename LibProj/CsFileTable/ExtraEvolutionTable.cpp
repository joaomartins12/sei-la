#include "stdafx.h"
#include "ExtraEvolutionTable.h"

CsExtraEvolutionMng::CsExtraEvolutionMng() 
{
}

CsExtraEvolutionMng::~CsExtraEvolutionMng() 
{
}

bool CsExtraEvolutionMng::Init( char* cPath )
{
	switch( nsCsFileTable::g_eFileType )
	{
	case nsCsFileTable::FT_EXCEL:		_LoadExcel();	break;
	case nsCsFileTable::FT_BIN:			_LoadBin( cPath );	break;
	case nsCsFileTable::FT_FILEPACK:	_LoadFilePack( cPath ); break;
	default:
		assert_cs( false );
		return false;
	}

	return true;
}

void CsExtraEvolutionMng::Delete()
{
}

bool CsExtraEvolutionMng::_LoadExcel()
{
	cExcelReader excel;
	bool bSuccess = excel.Open( nsCsFileTable::GetPath_DM_ExtraEvolution(), "ExtraExchange" );
	if( !bSuccess )
	{
		assert_cs( bSuccess == true );
		return false;
	}

	excel.FirstOfSkip( 3 );

	while( excel.MoveNext() )
	{
		DWORD dwNpcIDX = 0;
		excel.Read_Value( 0, dwNpcIDX );
		if( 0 == dwNpcIDX )
			break;

		USHORT usExchangeType = 0;
		excel.Read_Value( 2, usExchangeType );
		if( 0 == usExchangeType )
			continue;

		std::map<DWORD,TB::MAP_EXTRAEVOLUTION_DATA>::iterator it = m_mapExtraEvoData.find( dwNpcIDX );
		if( it == m_mapExtraEvoData.end() )
		{
			auto rib = m_mapExtraEvoData.insert( std::make_pair( dwNpcIDX, TB::MAP_EXTRAEVOLUTION_DATA() ) );
			it = rib.first;
		}

		TB::MAP_EXTRAEVOLUTION_DATA_IT subIT = it->second.find( usExchangeType );
		if( subIT == it->second.end() )
		{
			auto rib = it->second.insert( std::make_pair( usExchangeType, TB::LIST_EXCHAGEDATA() ) );
			subIT = rib.first;
		}


		TB::sExchangeData addData;
		excel.Read_Value( 1, addData.usWayType );
		excel.Read_Value( 4, addData.dwObjectIDX );
		excel.Read_Value( 5, addData.usMaterialType );
		excel.Read_Value( 7, addData.nNeedMaterialValue );
		excel.Read_Value( 28, addData.dwNeedMoney );
		for( int n = 8; n <= 17; ++n )
		{
			TB::sExchangeMaterialData mainMaterialData;
			excel.Read_Value( n, mainMaterialData.dwMaterialIDX );
			if( 0 == mainMaterialData.dwMaterialIDX )
				continue;

			addData.listNeedMainMaterial.push_back( mainMaterialData );
		}

		for( int n = 18; n <= 27; ++n )
		{
			TB::sExchangeMaterialData subMaterialData;
			excel.Read_Value( n, subMaterialData.dwMaterialIDX );
			n++;
			excel.Read_Value( n, subMaterialData.nCount );
			if( 0 == subMaterialData.dwMaterialIDX || 0 == subMaterialData.nCount )
				continue;

			addData.listNeedSubMaterial.push_back( subMaterialData );
		}

		subIT->second.push_back( addData );
	}

	return true;
}

#define EXTRA_EXCHAGE_BIN	"ExtraExchange.bin"

void CsExtraEvolutionMng::SaveBin( char* cPath )
{
	char cName[ MAX_PATH ];
	sprintf_s( cName, MAX_PATH, "%s\\%s", cPath, EXTRA_EXCHAGE_BIN );

	FILE *fp = NULL;
	fopen_s( &fp, cName, "wb" );
	assert_cs( fp != NULL );

	int nCount = (int)m_mapExtraEvoData.size();
	CmUtil::CmBinSave::Save_Value( fp, &nCount );
	
	std::map<DWORD,TB::MAP_EXTRAEVOLUTION_DATA>::const_iterator it = m_mapExtraEvoData.begin();
	for( ; it != m_mapExtraEvoData.end(); ++it )
	{
		CmUtil::CmBinSave::Save_Value( fp, &it->first );
		
		int nCount = (int)it->second.size();
		CmUtil::CmBinSave::Save_Value( fp, &nCount );

		TB::MAP_EXTRAEVOLUTION_DATA_CIT subIT = it->second.begin();
		for( ; subIT != it->second.end(); ++subIT )
		{
			CmUtil::CmBinSave::Save_Value( fp, &subIT->first );

			int nCount = (int)subIT->second.size();
			CmUtil::CmBinSave::Save_Value( fp, &nCount );
			TB::LIST_EXCHAGEDATA_CIT forIT = subIT->second.begin();
			for( ; forIT != subIT->second.end(); ++forIT )
			{
				CmUtil::CmBinSave::Save_Value( fp, &(*forIT).dwObjectIDX );
				CmUtil::CmBinSave::Save_Value( fp, &(*forIT).usMaterialType );
				CmUtil::CmBinSave::Save_Value( fp, &(*forIT).nNeedMaterialValue );
				CmUtil::CmBinSave::Save_Value( fp, &(*forIT).dwNeedMoney );
				CmUtil::CmBinSave::Save_Value( fp, &(*forIT).usWayType );

				int nCount = (int)(*forIT).listNeedMainMaterial.size();
				CmUtil::CmBinSave::Save_Value( fp, &nCount );

				TB::LIST_MATERIALDATA_CIT mainMtIt = (*forIT).listNeedMainMaterial.begin();
				for( ; mainMtIt != (*forIT).listNeedMainMaterial.end(); ++mainMtIt )
				{
					CmUtil::CmBinSave::Save_Value( fp, &(*mainMtIt).dwMaterialIDX );
					CmUtil::CmBinSave::Save_Value( fp, &(*mainMtIt).nCount );
				}

				nCount = (int)(*forIT).listNeedSubMaterial.size();
				CmUtil::CmBinSave::Save_Value( fp, &nCount );
				TB::LIST_MATERIALDATA_CIT subMtIt = (*forIT).listNeedSubMaterial.begin();
				for( ; subMtIt != (*forIT).listNeedSubMaterial.end(); ++subMtIt )
				{
					CmUtil::CmBinSave::Save_Value( fp, &(*subMtIt).dwMaterialIDX );
					CmUtil::CmBinSave::Save_Value( fp, &(*subMtIt).nCount );
				}				
			}
		}
	}


	fclose( fp );
}

bool CsExtraEvolutionMng::_LoadBin(char* cPath)
{
	char cName[ MAX_PATH ];
	sprintf_s( cName, MAX_PATH, "%s\\%s", cPath, EXTRA_EXCHAGE_BIN );

	FILE *fp = NULL;
	fopen_s( &fp, cName, "rb" );
	if( fp == NULL )
	{
		//CsMessageBoxA( MB_OK, "%s 파일이 존재 하지 않습니다.", cName );
		return false;
	}

	//////////////////////////////////////////////////////////////////////////
	int nMainCount = 0;
	CmUtil::CmBinRead::Read_Value( fp, &nMainCount );
	for( int i=0; i<nMainCount; ++i )
	{
		DWORD dwNPCIdx = 0;
		CmUtil::CmBinRead::Read_Value( fp, &dwNPCIdx );
		std::map<DWORD,TB::MAP_EXTRAEVOLUTION_DATA>::iterator it = m_mapExtraEvoData.find( dwNPCIdx );
		if( it == m_mapExtraEvoData.end() )
		{
			auto rib = m_mapExtraEvoData.insert( std::make_pair( dwNPCIdx, TB::MAP_EXTRAEVOLUTION_DATA() ) );
			it = rib.first;
		}

		int nSecCount = 0;
		CmUtil::CmBinRead::Read_Value( fp, &nSecCount );
		for( int n = 0; n < nSecCount; ++n )
		{
			USHORT usExchangeType = 0;
			CmUtil::CmBinRead::Read_Value( fp, &usExchangeType );
			TB::MAP_EXTRAEVOLUTION_DATA_IT secIT = it->second.find( usExchangeType );
			if( secIT == it->second.end() )
			{
				auto rib = it->second.insert( std::make_pair( usExchangeType, TB::LIST_EXCHAGEDATA() ) );
				secIT = rib.first;
			}

			int nListCount = 0;
			CmUtil::CmBinRead::Read_Value( fp, &nListCount );
			for( int m = 0; m < nListCount; ++m )
			{
				TB::sExchangeData addData;

				CmUtil::CmBinRead::Read_Value( fp, &addData.dwObjectIDX );
				CmUtil::CmBinRead::Read_Value( fp, &addData.usMaterialType );
				CmUtil::CmBinRead::Read_Value( fp, &addData.nNeedMaterialValue );
				CmUtil::CmBinRead::Read_Value( fp, &addData.dwNeedMoney );
				CmUtil::CmBinRead::Read_Value( fp, &addData.usWayType );

				int nCount = 0;
				CmUtil::CmBinRead::Read_Value( fp, &nCount );
				for( int j = 0; j < nCount; ++j )
				{
					TB::sExchangeMaterialData addMaterialData;
					CmUtil::CmBinRead::Read_Value( fp, &addMaterialData.dwMaterialIDX );
					CmUtil::CmBinRead::Read_Value( fp, &addMaterialData.nCount );
					addData.listNeedMainMaterial.push_back( addMaterialData );
				}

				nCount = 0;
				CmUtil::CmBinRead::Read_Value( fp, &nCount );
				for( int j = 0; j < nCount; ++j )
				{
					TB::sExchangeMaterialData addMaterialData;
					CmUtil::CmBinRead::Read_Value( fp, &addMaterialData.dwMaterialIDX );
					CmUtil::CmBinRead::Read_Value( fp, &addMaterialData.nCount );
					addData.listNeedSubMaterial.push_back( addMaterialData );
				}

				secIT->second.push_back( addData );
			}
		}		
	}
	//////////////////////////////////////////////////////////////////////////
	fclose( fp );
	return true;
}

bool CsExtraEvolutionMng::_LoadFilePack(char* cPath)
{
	char cName[ MAX_PATH ];
	sprintf_s( cName, MAX_PATH, "%s\\%s", cPath, EXTRA_EXCHAGE_BIN );

	if( !CsFPS::CsFPSystem::IsExistOnlyPack( FT_PACKHANDLE, cName ) )
		return false;

	if( !CsFPS::CsFPSystem::SeekLock( FT_PACKHANDLE ) )
		return false;

	int nHandle = CsFPS::CsFPSystem::GetFileHandle( FT_PACKHANDLE, cName );	

	int nCount = 0;
	CmUtil::CmPackRead::Read_Value( nHandle, &nCount );
	for( int i=0; i<nCount; ++i )
	{
		DWORD dwNPCIdx = 0;
		CmUtil::CmPackRead::Read_Value( nHandle, &dwNPCIdx );
		std::map<DWORD,TB::MAP_EXTRAEVOLUTION_DATA>::iterator it = m_mapExtraEvoData.find( dwNPCIdx );
		if( it == m_mapExtraEvoData.end() )
		{
			auto rib = m_mapExtraEvoData.insert( std::make_pair( dwNPCIdx, TB::MAP_EXTRAEVOLUTION_DATA() ) );
			it = rib.first;
		}

		int nCount = 0;
		CmUtil::CmPackRead::Read_Value( nHandle, &nCount );
		for( int n = 0; n < nCount; ++n )
		{
			USHORT usExchangeType = 0;
			CmUtil::CmPackRead::Read_Value( nHandle, &usExchangeType );
			TB::MAP_EXTRAEVOLUTION_DATA_IT secIT = it->second.find( usExchangeType );
			if( secIT == it->second.end() )
			{
				auto rib = it->second.insert( std::make_pair( usExchangeType, TB::LIST_EXCHAGEDATA() ) );
				secIT = rib.first;
			}

			int nCount = 0;
			CmUtil::CmPackRead::Read_Value( nHandle, &nCount );
			for( int m = 0; m < nCount; ++m )
			{
				TB::sExchangeData addData;

				CmUtil::CmPackRead::Read_Value( nHandle, &addData.dwObjectIDX );
				CmUtil::CmPackRead::Read_Value( nHandle, &addData.usMaterialType );
				CmUtil::CmPackRead::Read_Value( nHandle, &addData.nNeedMaterialValue );
				CmUtil::CmPackRead::Read_Value( nHandle, &addData.dwNeedMoney );
				CmUtil::CmPackRead::Read_Value( nHandle, &addData.usWayType );

				int nCount = 0;
				CmUtil::CmPackRead::Read_Value( nHandle, &nCount );
				for( int j = 0; j < nCount; ++j )
				{
					TB::sExchangeMaterialData addMaterialData;
					CmUtil::CmPackRead::Read_Value( nHandle, &addMaterialData.dwMaterialIDX );
					CmUtil::CmPackRead::Read_Value( nHandle, &addMaterialData.nCount );
					addData.listNeedMainMaterial.push_back( addMaterialData );
				}

				nCount = 0;
				CmUtil::CmPackRead::Read_Value( nHandle, &nCount );
				for( int j = 0; j < nCount; ++j )
				{
					TB::sExchangeMaterialData addMaterialData;
					CmUtil::CmPackRead::Read_Value( nHandle, &addMaterialData.dwMaterialIDX );
					CmUtil::CmPackRead::Read_Value( nHandle, &addMaterialData.nCount );
					addData.listNeedSubMaterial.push_back( addMaterialData );
				}

				secIT->second.push_back( addData );
			}
		}
	}

	CsFPS::CsFPSystem::SeekUnLock( FT_PACKHANDLE );
	return true;
}


TB::MAP_EXTRAEVOLUTION_DATA const*	CsExtraEvolutionMng::GetExtraEvolData( DWORD const& dwNpcID ) const
{
	std::map<DWORD,TB::MAP_EXTRAEVOLUTION_DATA>::const_iterator it = m_mapExtraEvoData.find( dwNpcID );
	if( it == m_mapExtraEvoData.end() )
		return NULL;

	return &it->second;
}