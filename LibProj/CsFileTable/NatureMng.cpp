
#include "stdafx.h"
#include "NatureMng.h"


void CsNatureMng::Delete()
{
	CsNature::MAP_IT it = m_mapNatuer.begin();
	CsNature::MAP_IT itEnd = m_mapNatuer.end();
	for( ; it!=itEnd; ++it )
	{
		it->second->Delete();
		delete it->second;
	}
	m_mapNatuer.clear();	
}

bool CsNatureMng::Init( char* cPath )
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

	return true;
}

CsNature* CsNatureMng::GetNature( int nID )
{	
	assert_cs( IsNature( nID ) == true );
	return m_mapNatuer[ nID ];
}

int CsNatureMng::GetValue( int nOwner, int nTarget )
{
	if(m_mapNatuer.find(nOwner) == m_mapNatuer.end())
		return 0;

	return m_mapNatuer[ nOwner ]->GetInfo()->s_nElement[ nTarget - nsCsDigimonTable::NT_ICE ];
}

void CsNatureMng::_LoadExcel()
{

#define IL_COL_START				1

	char cBuffer[ CEXCLE_CELL_LEN ];

	// Tamer
	{
		#define NATURE_SHEET_NAME			"New자연속성"
		cExcelReader excel;
		bool bSuccess;
		switch( nsCsFileTable::g_eLanguage )
		{
		case nsCsFileTable::KOREA_ORIGINAL:
		case nsCsFileTable::KOREA_TRANSLATION:
		case nsCsFileTable::ENGLISH:
		case nsCsFileTable::ENGLISH_A:
			bSuccess = excel.Open( nsCsFileTable::GetPath_DM_Base(), NATURE_SHEET_NAME );
			assert_cs( bSuccess == true );
			break;
		case nsCsFileTable::THAILAND:	// 태국 신규 속성 시스템 들어가면서 sheet 이름 변경 14.04.16
		case nsCsFileTable::HONGKONG:
			bSuccess = excel.Open( nsCsFileTable::GetPath_DM_Base(), "New_Natural_Nature" );
			assert_cs( bSuccess == true );
			break;
		case nsCsFileTable::TAIWAN:		
			bSuccess = excel.Open( nsCsFileTable::GetPath_DM_Base(), "New_Natural_Nature" );
			assert_cs( bSuccess == true );
			break;
		default:
			assert_cs( false );
		}
		// 2줄은 버린다.
		bSuccess = excel.MoveFirst();
		assert_cs( bSuccess == true );	
		bSuccess = excel.MoveNext();
		assert_cs( bSuccess == true );

		CsNature::sINFO info;
		int nCnt = 0;
			
		while( excel.MoveNext() == true )
		{
			// Level
			info.s_nID = nsCsDigimonTable::NT_ICE + nCnt;
			++nCnt;

			for( int i=0; i<CsNature::MaxCount; ++i )
			{
				info.s_nElement[ i ] = atoi( excel.GetData( IL_COL_START + i , cBuffer ) );
			}

			CsNature* pNautre = csnew CsNature;
			pNautre->Init( &info );
			m_mapNatuer[ info.s_nID ] = pNautre;
		}
		excel.Close();
	}

	_LoadExcelBaseElement();
}



#define NATURE_BIN		"Nature.bin"
void CsNatureMng::SaveBin( char* cPath )
{
	char cName[ MAX_PATH ];
	sprintf_s( cName, MAX_PATH, "%s\\%s", cPath, NATURE_BIN );

	FILE *fp = NULL;
	fopen_s( &fp, cName, "wb" );
	assert_cs( fp != NULL );

	int nCount = (int)m_mapNatuer.size();
	fwrite( &nCount, sizeof( int ), 1, fp );

	CsNature::MAP_IT it = m_mapNatuer.begin();
	CsNature::MAP_IT itEnd = m_mapNatuer.end();
	for( ; it!=itEnd; ++it )
	{
		fwrite( it->second->GetInfo(), sizeof( CsNature::sINFO ), 1, fp );
	}

	_SaveBinBaseElement( fp );

	fclose( fp );
}

bool CsNatureMng::_LoadBin( char* cPath )
{
	char cName[ MAX_PATH ];
	sprintf_s( cName, MAX_PATH, "%s\\%s", cPath, NATURE_BIN );

	FILE *fp = NULL;
	fopen_s( &fp, cName, "rb" );
	if( fp == NULL )
	{
		CsMessageBoxA( MB_OK, "%s 파일이 존재 하지 않습니다.", cName );
		return false;
	}

	int nCount;
	fread( &nCount, sizeof( int ), 1, fp );

	CsNature::sINFO info;
	for( int i=0; i<nCount; ++i )
	{
		fread( &info, sizeof( CsNature::sINFO ), 1, fp );

		CsNature* pObject = csnew CsNature;
		pObject->Init( &info );
		assert_cs( m_mapNatuer.find( info.s_nID ) == m_mapNatuer.end() );
		m_mapNatuer[ info.s_nID ] = pObject;
	}

	_LoadBinBaseElement( fp );

	fclose( fp );
	return true;
}

void CsNatureMng::_LoadFilePack( char* cPath )
{
	char cName[ MAX_PATH ];
	sprintf_s( cName, MAX_PATH, "%s\\%s", cPath, NATURE_BIN );

	if( !CsFPS::CsFPSystem::SeekLock( FT_PACKHANDLE ) )
		return;

	int nHandle = CsFPS::CsFPSystem::GetFileHandle( FT_PACKHANDLE, cName );

	int nCount;
	_read( nHandle, &nCount, sizeof( int ) );

	CsNature::sINFO info;
	for( int i=0; i<nCount; ++i )
	{
		_read( nHandle, &info, sizeof( CsNature::sINFO ) );

		CsNature* pObject = csnew CsNature;
		pObject->Init( &info );
		assert_cs( m_mapNatuer.find( info.s_nID ) == m_mapNatuer.end() );
		m_mapNatuer[ info.s_nID ] = pObject;
	}

	_LoadFilePackBaseElement( nHandle );

	CsFPS::CsFPSystem::SeekUnLock( FT_PACKHANDLE );
}

void CsNatureMng::_LoadExcelBaseElement()
{
	cExcelReader excel;
	bool bSuccess;
	switch( nsCsFileTable::g_eLanguage )
	{
	case nsCsFileTable::KOREA_ORIGINAL:
	case nsCsFileTable::KOREA_TRANSLATION:
	case nsCsFileTable::ENGLISH:
	case nsCsFileTable::ENGLISH_A:
		bSuccess = excel.Open( nsCsFileTable::GetPath_DM_Base(), "기본속성" );
		break;
	case nsCsFileTable::THAILAND:	// 태국 신규 속성 시스템 들어가면서 sheet 이름 변경 14.04.16
	case nsCsFileTable::HONGKONG:
		bSuccess = excel.Open( nsCsFileTable::GetPath_DM_Base(), "Basic_Nature" );
		break;
	case nsCsFileTable::TAIWAN:		
		bSuccess = excel.Open( nsCsFileTable::GetPath_DM_Base(), "Basic_Nature" );
		break;
	default:
		assert_cs( false );
	}

	if( !bSuccess )
		return;

	if( !excel.FirstOfSkip(2) )
		return;

	MAP_BASE_ELEMENT mapAttElement;
	int nType = 0;
	while( excel.MoveNext() )
	{
		VEC_CMP_POINT vecCmpPoint;
		for( int i = 1; i < 6; ++i )
		{
			sBaseElement sElem;
			sElem.nTargetType = i - 1;
			excel.Read_Value( i, sElem.nPoint );
			if( 0 == sElem.nPoint )
				break;

			vecCmpPoint.push_back( sElem );
		}

		mapAttElement.insert( std::make_pair( nType++, vecCmpPoint ) );
	}
	m_mapCmpElement.insert( std::make_pair( nsBaseElement::eAtt, mapAttElement ) );

	excel.Close();

	MAP_BASE_ELEMENT mapDefElement;
	for( int i = 0; i < nsBaseElement::eMax; ++i )
	{
		VEC_CMP_POINT vecCmpPoint;
		for( int j = 0; j < nsBaseElement::eMax; ++j )
		{
			MAP_BASE_ELEMENT_IT it = mapAttElement.find( j );
			if( it == mapAttElement.end() )
				continue;

			sBaseElement sElem = it->second.at( i );
			sElem.nTargetType = j;
			vecCmpPoint.push_back( sElem );
		}
		mapDefElement.insert( std::make_pair( i, vecCmpPoint ) );
	}
	m_mapCmpElement.insert( std::make_pair( nsBaseElement::eDef, mapDefElement ) );
}

void CsNatureMng::_LoadBinBaseElement(FILE* fp)
{
	int nCount = 0;
	CmUtil::CmBinRead::Read_Value( fp, &nCount );
	for( int i = 0 ; i < nCount; ++i )
	{
		int nCmpType;
		CmUtil::CmBinRead::Read_Value( fp, &nCmpType );

		MAP_CMP_ELEMENT_IT it = m_mapCmpElement.find( nCmpType );
		if( it == m_mapCmpElement.end() )
		{
			auto ib = m_mapCmpElement.insert( std::make_pair( nCmpType, MAP_BASE_ELEMENT() ) );
			if( !ib.second )
			{
				assert_csm( false, _T( "Insertion failure." ) );
				continue;
			}
			it = ib.first;
		}

		int nBaseCount = 0;
		CmUtil::CmBinRead::Read_Value( fp, &nBaseCount );
		for( int j = 0; j < nBaseCount; ++j )
		{
			int nBaseType;
			CmUtil::CmBinRead::Read_Value( fp, &nBaseType );

			MAP_BASE_ELEMENT_IT itBase = it->second.find( nBaseType );
			if( itBase == it->second.end() )
			{
				auto ib = it->second.insert( std::make_pair( nBaseType, VEC_CMP_POINT() ) );
				if( !ib.second )
				{
					assert_csm( false, _T( "Insertion failure." ) );
					continue;
				}
				itBase = ib.first;
			}

			int nVecCount = 0;
			CmUtil::CmBinRead::Read_Value( fp, &nVecCount );
			for( int k = 0; k < nVecCount; ++k )
			{
				sBaseElement sElem;
				CmUtil::CmBinRead::Read_Value( fp, &sElem.nTargetType );
				CmUtil::CmBinRead::Read_Value( fp, &sElem.nPoint );

				itBase->second.push_back( sElem );
			}
		}
	}
}

void CsNatureMng::_LoadFilePackBaseElement(int nHandle)
{
	int nCount = 0;
	CmUtil::CmPackRead::Read_Value( nHandle, &nCount );
	for( int i = 0 ; i < nCount; ++i )
	{
		int nCmpType;
		CmUtil::CmPackRead::Read_Value( nHandle, &nCmpType );

		MAP_CMP_ELEMENT_IT it = m_mapCmpElement.find( nCmpType );
		if( it == m_mapCmpElement.end() )
		{
			auto ib = m_mapCmpElement.insert( std::make_pair( nCmpType, MAP_BASE_ELEMENT() ) );
			if( !ib.second )
			{
				assert_csm( false, _T( "Insertion failure." ) );
				continue;
			}
			it = ib.first;
		}

		int nBaseCount = 0;
		CmUtil::CmPackRead::Read_Value( nHandle, &nBaseCount );
		for( int j = 0; j < nBaseCount; ++j )
		{
			int nBaseType;
			CmUtil::CmPackRead::Read_Value( nHandle, &nBaseType );

			MAP_BASE_ELEMENT_IT itBase = it->second.find( nBaseType );
			if( itBase == it->second.end() )
			{
				auto ib = it->second.insert( std::make_pair( nBaseType, VEC_CMP_POINT() ) );
				if( !ib.second )
				{
					assert_csm( false, _T( "Insertion failure." ) );
					continue;
				}
				itBase = ib.first;
			}

			int nVecCount = 0;
			CmUtil::CmPackRead::Read_Value( nHandle, &nVecCount );
			for( int k = 0; k < nVecCount; ++k )
			{
				sBaseElement sElem;
				CmUtil::CmPackRead::Read_Value( nHandle, &sElem.nTargetType );
				CmUtil::CmPackRead::Read_Value( nHandle, &sElem.nPoint );

				itBase->second.push_back( sElem );
			}
		}
	}
}

void CsNatureMng::_SaveBinBaseElement(FILE* fp)
{
	int nCount = static_cast< int >( m_mapCmpElement.size() );
	CmUtil::CmBinSave::Save_Value( fp, &nCount );

	MAP_CMP_ELEMENT_IT it = m_mapCmpElement.begin();
	MAP_CMP_ELEMENT_IT itEnd = m_mapCmpElement.end();
	for( ; it != itEnd; ++it )
	{
		CmUtil::CmBinSave::Save_Value( fp, &it->first );

		nCount = (int)it->second.size();
		CmUtil::CmBinSave::Save_Value( fp, &nCount );

		MAP_BASE_ELEMENT_IT itBase = it->second.begin();
		MAP_BASE_ELEMENT_IT itBaseEnd = it->second.end();
		for( ; itBase != itBaseEnd; ++itBase )
		{
			CmUtil::CmBinSave::Save_Value( fp, &itBase->first );

			nCount = (int)itBase->second.size();
			CmUtil::CmBinSave::Save_Value( fp, &nCount );
			for( int i = 0; i < nCount; ++i )
			{
				sBaseElement sElem = itBase->second.at( i );
				CmUtil::CmBinSave::Save_Value( fp, &sElem.nTargetType );
				CmUtil::CmBinSave::Save_Value( fp, &sElem.nPoint );
			}
		}
	}
}

bool CsNatureMng::IsSuperiority(nsBaseElement::eCompareType eCmpType, nsBaseElement::eElementType eOrgType, nsBaseElement::eElementType eSrcType)
{
	MAP_CMP_ELEMENT_IT it = m_mapCmpElement.find( eCmpType );
	if( it == m_mapCmpElement.end() )
		return false;

	MAP_BASE_ELEMENT_IT itOrg = it->second.find( eOrgType );
	if( itOrg == it->second.end() )
		return false;

	sBaseElement sElem = itOrg->second.at( eSrcType );
	if( ELEMENT_EQAUL < sElem.nPoint )
		return true;

	return false;
}

bool CsNatureMng::IsInferiority(nsBaseElement::eCompareType eCmpType, nsBaseElement::eElementType eOrgType, nsBaseElement::eElementType eSrcType)
{
	MAP_CMP_ELEMENT_IT it = m_mapCmpElement.find( eCmpType );
	if( it == m_mapCmpElement.end() )
		return false;

	MAP_BASE_ELEMENT_IT itOrg = it->second.find( eOrgType );
	if( itOrg == it->second.end() )
		return false;

	sBaseElement sElem = itOrg->second.at( eSrcType );
	if( ELEMENT_EQAUL > sElem.nPoint )
		return true;

	return false;
}