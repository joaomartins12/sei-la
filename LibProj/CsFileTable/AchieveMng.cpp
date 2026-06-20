
#include "stdafx.h"
#include "AchieveMng.h"

void CsAchieveMng::sTYPE::Delete()
{
	std::list< sTYPE* >::iterator it = s_listChild.begin();
	std::list< sTYPE* >::iterator itEnd = s_listChild.end();
	for( ; it!=itEnd; ++it )
	{
		(*it)->Delete();
		delete *it;
	}
	s_listChild.clear();
}

void CsAchieveMng::sTYPE::Save( FILE *fp )
{
	fwrite( &s_Info, sizeof( sINFO ), 1, fp );

	int nCnt = (int)s_listChild.size();
	fwrite( &nCnt, sizeof( int ), 1, fp );

	std::list< sTYPE* >::iterator it = s_listChild.begin();
	std::list< sTYPE* >::iterator itEnd = s_listChild.end();
	for( ; it!=itEnd; ++it )
	{
		(*it)->Save( fp );
	}
}

void CsAchieveMng::sTYPE::Load( FILE* fp )
{
	fread( &s_Info, sizeof( sINFO ), 1, fp );

	int nCnt;
	fread( &nCnt, sizeof( int ), 1, fp );

	for( int i=0; i<nCnt; ++i )
	{
		AddChild()->Load( fp );
	}
}

void CsAchieveMng::sTYPE::Load( int nHandle )
{
	_read( nHandle, &s_Info, sizeof( sINFO ) );

	int nCnt;
	_read( nHandle, &nCnt, sizeof( int ) );

	for( int i=0; i<nCnt; ++i )
	{
		AddChild()->Load( nHandle );
	}
}

CsAchieveMng::sTYPE* CsAchieveMng::sTYPE::GetChild( int nIndex )
{
	assert_cs( nIndex >= 0 );
	if( nIndex > (int)s_listChild.size() - 1 )
	{
		AddChild();
		return GetChild( nIndex );
	}

	std::list< sTYPE* >::iterator it = s_listChild.begin();
	std::list< sTYPE* >::iterator itEnd = s_listChild.end();
	for( ; it!=itEnd; ++it )
	{
		if( nIndex == 0 )
			return *it;

		--nIndex;
	}
	assert_cs( false );
	return NULL;
}

void CsAchieveMng::Delete()
{
	CsAchieve::MAP_IT it = m_Map.begin();
	CsAchieve::MAP_IT itEnd = m_Map.end();
	for( ; it!=itEnd; ++it )
	{
		it->second->Delete();
		delete it->second;
	}
	m_Map.clear();

	m_RootType.Delete();
}

bool CsAchieveMng::Init( char* cPath )
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

void CsAchieveMng::_LoadExcel()
{
	_LoadExcel_Data();
	_LoadExcel_Achieve();
}

void CsAchieveMng::_LoadExcel_Data()
{
	cExcelReader excel;
	if( !excel.Open( nsCsFileTable::GetPath_DM_Achieve(), "Data" ) )
	{
		CsMessageBoxA( MB_OK, "Table : %s, Sheet : %s - 파일이 존재 하지 않습니다.", nsCsFileTable::GetPath_DM_Achieve(), "Data" );
		return ;
	}

	if(!excel.FirstOfSkip(2))
		return;

	sTYPE* pChild = NULL;
	int nChild = 0;
	while( excel.MoveNext() == true )
	{
		int nKey = 0;
		excel.Read_Value( 0, nKey );
		if( 0 == nKey )
			break;

		pChild = m_RootType.GetChild( nChild );

		excel.Read_String( 1, pChild->s_Info.s_szType, 32);
		++nChild;
	}

	int nParent = nChild;
	for( int i=0; i<nParent; ++i )
	{
		sTYPE* pParent = m_RootType.GetChild( i );

		int nCol = 3 + i*3;

		if(!excel.FirstOfSkip(2))
			break;

		nChild = 0;
		while( excel.MoveNext() == true )
		{
			int nKey = 0;
			excel.Read_Value( nCol, nKey );
			if( 0 == nKey )
				break;

			pChild = pParent->GetChild( nChild );
			excel.Read_String( nCol+1, pChild->s_Info.s_szType, 32);
			++nChild;
		}
	}

	excel.Close();
}

void CsAchieveMng::_LoadExcel_Achieve()
{
	cExcelReader excel;
	if( !excel.Open( nsCsFileTable::GetPath_DM_Achieve(), "DM_Achieve" ) )
	{
		CsMessageBoxA( MB_OK, "Table : %s, Sheet : %s - 파일이 존재 하지 않습니다.", nsCsFileTable::GetPath_DM_Achieve(), "DM_Achieve" );
		return ;
	}

	if(!excel.FirstOfSkip(2))
		return;

	CsAchieve::sINFO info;
	int nCnt = 0;
	CsQuestCompRequireGroup* pFTQuestCompReq = NULL;
	while( excel.MoveNext() == true )
	{
		excel.Read_Value( 0, info.s_nQuestID );
		if( info.s_nQuestID == 0 )
			continue;

		switch( nsCsFileTable::g_eLanguage )
		{
		case nsCsFileTable::KOREA_ORIGINAL:
		case nsCsFileTable::KOREA_TRANSLATION:
			excel.Read_String( 1, info.s_szName, FT_ACHIEVE_NAME );
			excel.Read_String( 4, info.s_szComment, FT_ACHIEVE_COMMENT );
			excel.Read_String( 6, info.s_szTitle, FT_ACHIEVE_TITLE );
			break;
		default:
			excel.Read_String( 2, info.s_szName, FT_ACHIEVE_NAME );
			excel.Read_String( 5, info.s_szComment, FT_ACHIEVE_COMMENT );
			excel.Read_String( 7, info.s_szTitle, FT_ACHIEVE_TITLE );
			break;
		}

		excel.Read_Value( 9, info.s_nIcon );
		excel.Read_Value( 10, info.s_nPoint );
		excel.Read_Value( 11, info.s_bDisplay );

		excel.Read_Value( 12, info.s_nGroup );
		excel.Read_Value( 14, info.s_nSubGroup );
		excel.Read_Value( 22, info.s_nBuffCode );

		assert_csm1( nsCsFileTable::g_pQuestMng->IsQuest( info.s_nQuestID ), _T( "QuestID = %d" ), info.s_nQuestID );

		pFTQuestCompReq = nsCsFileTable::g_pQuestMng->GetQuest( info.s_nQuestID )->GetCompRequireGroup();
		assert_cs( nsCsFileTable::g_pQuestMng->GetQuest( info.s_nQuestID )->GetQuestType() == CsQuest::QT_ACHIEVE );
		assert_csm1( pFTQuestCompReq->GetListCount() == 1, "QuestID = %d", info.s_nQuestID );
		assert_cs( pFTQuestCompReq->Get( 0 )->GetType() == CsQuestCompRequire::CLIENT );
		info.s_nType = pFTQuestCompReq->Get( 0 )->GetTypeID();

		CsAchieve* pData = csnew CsAchieve;
		pData->Init( &info );
		m_Map[ info.s_nQuestID ] = pData;
	}
	excel.Close();
}


#define ACHIEVE_BIN		"Achieve.bin"
void CsAchieveMng::SaveBin( char* cPath )
{
	char cName[ MAX_PATH ];
	sprintf_s( cName, MAX_PATH, "%s\\%s", cPath, ACHIEVE_BIN );

	FILE *fp = NULL;
	fopen_s( &fp, cName, "wb" );
	assert_cs( fp != NULL );

	// 데이터
	m_RootType.Save( fp );
	
	// 실제 업적
	int nCount = (int)m_Map.size();
	fwrite( &nCount, sizeof( int ), 1, fp );

	CsAchieve::MAP_IT it = m_Map.begin();
	CsAchieve::MAP_IT itEnd = m_Map.end();
	for( ; it!=itEnd; ++it )
	{
		fwrite( it->second->GetInfo(), sizeof( CsAchieve::sINFO ), 1, fp );
	}
	fclose( fp );
}

bool CsAchieveMng::_LoadBin( char* cPath )
{
	char cName[ MAX_PATH ];
	sprintf_s( cName, MAX_PATH, "%s\\%s", cPath, ACHIEVE_BIN );

	FILE *fp = NULL;
	fopen_s( &fp, cName, "rb" );
	if( fp == NULL )
	{
		CsMessageBoxA( MB_OK, "%s 파일이 존재 하지 않습니다.", cName );
		return false;
	}

	// 데이터
	m_RootType.Load( fp );

	// 실제 업적
	int nCount;
	fread( &nCount, sizeof( int ), 1, fp );

	CsAchieve::sINFO info;
	for( int i=0; i<nCount; ++i )
	{
		fread( &info, sizeof( CsAchieve::sINFO ), 1, fp );

		CsAchieve* pObject = csnew CsAchieve;
		pObject->Init( &info );
		assert_cs( IsAchieve( info.s_nQuestID ) == false );
		m_Map[ info.s_nQuestID ] = pObject;
	}

	fclose( fp );
	return true;
}

void CsAchieveMng::_LoadFilePack( char* cPath )
{
	char cName[ MAX_PATH ];
	sprintf_s( cName, MAX_PATH, "%s\\%s", cPath, ACHIEVE_BIN );

	if( !CsFPS::CsFPSystem::SeekLock( FT_PACKHANDLE ) )
		return;

	int nHandle = CsFPS::CsFPSystem::GetFileHandle( FT_PACKHANDLE, cName );

	// 데이터
	m_RootType.Load( nHandle );

	// 실제 업적
	int nCount;
	_read( nHandle, &nCount, sizeof( int ) );

	CsAchieve::sINFO info;
	for( int i=0; i<nCount; ++i )
	{
		_read( nHandle, &info, sizeof( CsAchieve::sINFO ) );

		CsAchieve* pObject = csnew CsAchieve;
		pObject->Init( &info );
		assert_cs( IsAchieve( info.s_nQuestID ) == false );
		m_Map[ info.s_nQuestID ] = pObject;
	}

	CsFPS::CsFPSystem::SeekUnLock( FT_PACKHANDLE );	
}

bool CsAchieveMng::IsAchieve( int nQuestID )
{
	return ( m_Map.find( nQuestID ) != m_Map.end() );
}

CsAchieve* CsAchieveMng::GetAchieve( int nQuestID )
{ 
	CsAchieve::MAP_IT it = m_Map.find( nQuestID );
	if( it == m_Map.end() )
		return NULL;
	return it->second;
}