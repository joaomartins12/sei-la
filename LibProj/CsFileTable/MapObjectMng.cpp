
#include "stdafx.h"
#include "MapObjectMng.h"

#define MAP_OBJECT_BIN			"MapObject.bin"

void CsMapObjectMng::Delete()
{
	GROUP_MAP_IT itGroup = m_mapMapObjectGroup.begin();
	for( ; itGroup!=m_mapMapObjectGroup.end(); ++itGroup )
	{
		itGroup->second->Delete();
		delete itGroup->second;
	}
	m_mapMapObjectGroup.clear();
}


void CsMapObjectMng::Reload( char* cPath )
{ 
	if( NULL == cPath )
		return;

	_LoadBin( cPath ); 
}

int CsMapObjectMng::GetMaxColCount() const
{
	int nMaxColCount = 0;
	GROUP_MAP_CIT it = m_mapMapObjectGroup.begin();
	for( ; it != m_mapMapObjectGroup.end(); ++it )
	{
		int nMFC = it->second->GetMaxFactorCount();
		if( nMFC > nMaxColCount )
			nMaxColCount = nMFC;
	}

	return nMaxColCount;
}

bool CsMapObjectMng::Init( char* cPath, nsCsFileTable::eFILE_TYPE ft /*=nsCsFileTable::g_eFileType*/ )
{
	switch( ft )
	{	
	case nsCsFileTable::FT_EXCEL:		_LoadExcel();						break;
	case nsCsFileTable::FT_BIN:
		if( _LoadBin( cPath ) == false )
			return false;
		break;
	case nsCsFileTable::FT_FILEPACK:	_LoadFilePack( cPath );		break;
	default:
		assert_cs( false );
	}
	return true;
}


#define MAP_OBJ_COL_MAPID			0
#define MAP_OBJ_COL_OBJECTID		MAP_OBJ_COL_MAPID + 1		// 1
#define MAP_OBJ_COL_ORDERNUM		MAP_OBJ_COL_OBJECTID + 1	// 2 // 순번
#define MAP_OBJ_COL_OBJTYPE			MAP_OBJ_COL_ORDERNUM + 1	// 3 , 6 , 9
#define MAP_OBJ_COL_OBJFACTOR		MAP_OBJ_COL_OBJTYPE + 1		// 4 , 7 , 10
#define MAP_OBJ_COL_OBJFACTOR_CNT	MAP_OBJ_COL_OBJFACTOR + 1	// 5 , 8 , 11

void CsMapObjectMng::_LoadExcel()
{
	cExcelReader excel;
	bool bSuccess = excel.Open( nsCsFileTable::GetPath_MAP(), ME_OBJECT_SHEET_NAME );
	if( bSuccess == false )
		return;

	// 2줄은 버린다.
	bSuccess = excel.MoveFirst();
	if( bSuccess == false )
		return;

	bSuccess = excel.MoveNext();
	if( bSuccess == false )
		return;

	m_mapMapObjectGroup.clear();

	UINT unColCount = excel.GetColCount();
	char cBuffer[ CEXCLE_CELL_LEN ] = {0,};
	// 데이터 입력
	while( excel.MoveNext() == true )
	{
		DWORD dwMapID = 0;
		excel.Read_Value( MAP_OBJ_COL_MAPID, dwMapID );
		if( 0 == dwMapID )
			break;

		GROUP_MAP_IT it = m_mapMapObjectGroup.find( dwMapID );
		if( it == m_mapMapObjectGroup.end() )
		{
			auto result = m_mapMapObjectGroup.insert( make_pair( dwMapID, csnew CsMapObjectGroup ) );
			if( !result.second )
				break;

			it = result.first;
		}

		DWORD dwObjectId = 0;
		excel.Read_Value( MAP_OBJ_COL_OBJECTID, dwObjectId );
		CsMapObjectGroup::LIST_OBJECT * pFindObj = const_cast<CsMapObjectGroup::LIST_OBJECT*>(it->second->FindObjectList(dwObjectId));
		if( NULL == pFindObj )
		{
			pFindObj = it->second->AddNewObject(dwObjectId);
			if( NULL == pFindObj )
				break;
		}

		pFindObj->push_back( csnew CsMapObject );

		int nOderNumer = 0;
		excel.Read_Value( MAP_OBJ_COL_ORDERNUM, nOderNumer );
		pFindObj->back()->SetOrderNumber( nOderNumer );

		for( int n = MAP_OBJ_COL_OBJTYPE; n < unColCount;  )
		{
			CsMapObject::sFACTOR_INFO addInfo;
			excel.Read_Value( n, addInfo.s_nOpenType );
			if( 0 == addInfo.s_nOpenType )
				break;
			++n;
			excel.Read_Value( n, addInfo.s_nFactor );
			++n;
			excel.Read_Value( n, addInfo.s_nFactorCnt );
			++n;

			pFindObj->back()->AddFactorInfo( addInfo );
		}
	}

	excel.Close();
}

void CsMapObjectMng::Save()
{
	YExcel::BasicExcel excel;
	bool bLoad = excel.Load( nsCsFileTable::GetPath_MAP() );
	assert_cs( bLoad == true );

	YExcel::BasicExcelWorksheet* pSheet = excel.GetWorksheet( ME_OBJECT_SHEET_NAME );
	assert_cs( pSheet != NULL );

	pSheet->DeleteAllRows(1);

	pSheet->Cell( 0, ME_VERSION_COL )->SetInteger( ME_OBJECT_VERSION );
	for( int i=1; i<=11; ++i )
		pSheet->Cell( 0, i )->SetInteger( 1 );

	pSheet->Cell( 1, MAP_OBJ_COL_MAPID )->SetWString( _T( "mapID" ) );
	pSheet->Cell( 1, MAP_OBJ_COL_OBJECTID )->SetWString( _T( "ObjectID" ) );
	pSheet->Cell( 1, MAP_OBJ_COL_ORDERNUM )->SetWString( _T( "OrderNumber" ) );

	int nMaxCount = GetMaxColCount() + MAP_OBJ_COL_OBJTYPE;
	for( int n = MAP_OBJ_COL_OBJTYPE; n < nMaxCount; )
	{
		pSheet->Cell( 1, n )->SetWString( _T( "Open_Type" ) );
		++n;
		pSheet->Cell( 1, n )->SetWString( _T( "Open_Factor" ) );
		++n;
		pSheet->Cell( 1, n )->SetWString( _T( "Factor_Number" ) );
		++n;
	}

	// 컬럼 설명 -> 기능 추가 시 or 기획자 수정 요청 시 수정
	size_t siInputRow = 2;
	GROUP_MAP_IT  itGroup = m_mapMapObjectGroup.begin();
	for( ; itGroup!=m_mapMapObjectGroup.end(); ++itGroup )
	{		
		CsMapObjectGroup::OBJECT_MAP const & mapObj = itGroup->second->GetMapObject();
		CsMapObjectGroup::OBJECT_MAP_CIT itObj =  mapObj.begin();
		for( ; itObj != mapObj.end(); ++itObj )
		{
			CsMapObjectGroup::LIST_OBJECT_CIT cItObject = itObj->second.begin();
			for( ; cItObject != itObj->second.end(); ++cItObject, ++siInputRow )
			{
				pSheet->Cell( siInputRow, MAP_OBJ_COL_MAPID )->SetInteger( itGroup->first );
				pSheet->Cell( siInputRow, MAP_OBJ_COL_OBJECTID )->SetInteger( itObj->first );
				pSheet->Cell( siInputRow, MAP_OBJ_COL_ORDERNUM )->SetInteger( (*cItObject)->GetOrderNumber() );

				CsMapObject::FACT_LIST const& factList = (*cItObject)->GetFactInfoList();
				CsMapObject::FACT_LIST_CIT factIt = factList.begin();
				int n = MAP_OBJ_COL_OBJTYPE;
				for( ; factIt != factList.end(); ++factIt )
				{
					pSheet->Cell( siInputRow, n )->SetInteger( (*factIt).s_nOpenType );
					++n;
					pSheet->Cell( siInputRow, n )->SetInteger( (*factIt).s_nFactor );
					++n;
					pSheet->Cell( siInputRow, n )->SetInteger( (*factIt).s_nFactorCnt );
					++n;
				}

				for( ; n < nMaxCount; ++n )
					pSheet->Cell( siInputRow, n )->SetInteger( 0 );
			}
		}
	}

	excel.SaveAs( nsCsFileTable::GetPath_MAP() );
}

void CsMapObjectMng::SaveBin( char* cPath )
{
	char cName[ MAX_PATH ];
	sprintf_s( cName, MAX_PATH, "%s\\%s", cPath, MAP_OBJECT_BIN );

	FILE *fp = NULL;
	fopen_s( &fp, cName, "wb" );
	assert_cs( fp != NULL );

	int nCount = (int)m_mapMapObjectGroup.size();
	fwrite( &nCount, sizeof( int ), 1, fp );

	GROUP_MAP_CIT itGroup = m_mapMapObjectGroup.begin();
	for( ; itGroup!= m_mapMapObjectGroup.end(); ++itGroup )
	{
		fwrite( &itGroup->first, sizeof( DWORD ), 1, fp ); // 맵 ID 키값

		CsMapObjectGroup::OBJECT_MAP mapObj = itGroup->second->GetMapObject();
		int ObjCount = (int)mapObj.size();
		fwrite( &ObjCount, sizeof( int ), 1, fp ); // 맵 갯수

		CsMapObjectGroup::OBJECT_MAP_CIT itmapObj = mapObj.begin();
		for( ; itmapObj != mapObj.end(); ++itmapObj )
		{
			fwrite( &itmapObj->first, sizeof( DWORD ), 1, fp ); // 오브젝트 ID 키값		

			int nObjListCount = (int)itmapObj->second.size();
			fwrite( &nObjListCount, sizeof( DWORD ), 1, fp ); // 오브젝트 갯수

			CsMapObjectGroup::LIST_OBJECT_CIT cITObjList = itmapObj->second.begin();
			for( ; cITObjList != itmapObj->second.end(); ++cITObjList )
			{
				int nOrderIndx = (*cITObjList)->GetOrderNumber();
				fwrite( &nOrderIndx, sizeof( int ), 1, fp ); // 열리는 순서 - 서버에서만 사용 함.

				CsMapObject::FACT_LIST factList = (*cITObjList)->GetFactInfoList();
				int nFactCount = (int)factList.size();
				fwrite( &nFactCount, sizeof( int ), 1, fp ); // 요건 갯수

				CsMapObject::FACT_LIST_CIT itFact = factList.begin();
				for( ; itFact != factList.end(); ++itFact )
				{
					fwrite( &(*itFact).s_nOpenType, sizeof( int ), 1, fp ); 
					fwrite( &(*itFact).s_nFactorCnt, sizeof( int ), 1, fp );
					fwrite( &(*itFact).s_nFactor, sizeof( unsigned int ), 1, fp );
				}
			}
		}
	}

	fclose( fp );
}

bool CsMapObjectMng::_LoadBin( char* cPath )
{
	char cName[ MAX_PATH ];
	sprintf_s( cName, MAX_PATH, "%s\\%s", cPath, MAP_OBJECT_BIN );

	FILE *fp = NULL;
	fopen_s( &fp, cName, "rb" );
	if( fp == NULL )
	{
		CsMessageBoxA( MB_OK, "%s 파일이 존재 하지 않습니다.", cName );
		return false;
	}


	int nCount = 0;
	fread( &nCount, sizeof( int ), 1, fp );
	for( int i = 0; i < nCount; i++)
	{
		DWORD dwMapID = 0;
		fread( &dwMapID, sizeof( DWORD ), 1, fp );		

		GROUP_MAP_IT it = m_mapMapObjectGroup.find( dwMapID );
		if( it == m_mapMapObjectGroup.end() )
		{
			auto result = m_mapMapObjectGroup.insert( make_pair( dwMapID, csnew CsMapObjectGroup ) );
			if( !result.second )
				break;

			it = result.first;
		}

		int nSize = 0;
		fread( &nSize, sizeof( int ), 1, fp );		

		for( int j = 0; j < nSize; ++j )
		{
			DWORD dwObjectId = 0;
			fread( &dwObjectId, sizeof( DWORD ), 1, fp );	

			CsMapObjectGroup::LIST_OBJECT * pFindObj = const_cast<CsMapObjectGroup::LIST_OBJECT*>(it->second->FindObjectList(dwObjectId));
			if( NULL == pFindObj )
			{
				pFindObj = it->second->AddNewObject(dwObjectId);
				if( NULL == pFindObj )
					break;
			}

			int nObjCount = 0;
			fread( &nObjCount, sizeof( int ), 1, fp );	

			for( int l = 0; l < nObjCount; ++l )
			{
				pFindObj->push_back( csnew CsMapObject );

				int nOrderIndx;
				fread( &nOrderIndx, sizeof( int ), 1, fp ); // 열리는 순서 - 서버에서만 사용 함.

				int nFactSize = 0;
				fread( &nFactSize, sizeof( int ), 1, fp );	

				pFindObj->back()->SetOrderNumber( nOrderIndx );

				for( int k = 0; k < nFactSize; ++k )
				{
					CsMapObject::sFACTOR_INFO addInfo;
					fread( &addInfo.s_nOpenType, sizeof( int ), 1, fp );
					fread( &addInfo.s_nFactorCnt, sizeof( int ), 1, fp );
					fread( &addInfo.s_nFactor, sizeof( unsigned int ), 1, fp );

					pFindObj->back()->AddFactorInfo( addInfo );
				}
			}
		}
	}

	fclose( fp );

	return true;
}

void CsMapObjectMng::_LoadFilePack( char* cPath )
{
	char cName[ MAX_PATH ];
	sprintf_s( cName, MAX_PATH, "%s\\%s", cPath, MAP_OBJECT_BIN );

	if( !CsFPS::CsFPSystem::SeekLock( FT_PACKHANDLE ) )
		return;

	int nHandle = CsFPS::CsFPSystem::GetFileHandle( FT_PACKHANDLE, cName );

	int nCount = 0;
	_read( nHandle, &nCount, sizeof( int ) );
	for( int i = 0; i < nCount; i++)
	{
		DWORD dwMapID = 0;
		_read( nHandle, &dwMapID, sizeof( int ) );

		GROUP_MAP_IT it = m_mapMapObjectGroup.find( dwMapID );
		if( it == m_mapMapObjectGroup.end() )
		{
			auto result = m_mapMapObjectGroup.insert( make_pair( dwMapID, csnew CsMapObjectGroup ) );
			if( !result.second )
				break;

			it = result.first;
		}

		int nSize = 0;
		_read( nHandle, &nSize, sizeof( int ) );

		for( int j = 0; j < nSize; ++j )
		{
			DWORD dwObjectId = 0;
			_read( nHandle, &dwObjectId, sizeof( DWORD ) );

			CsMapObjectGroup::LIST_OBJECT * pFindObj = const_cast<CsMapObjectGroup::LIST_OBJECT*>(it->second->FindObjectList(dwObjectId));
			if( NULL == pFindObj )
			{
				pFindObj = it->second->AddNewObject(dwObjectId);
				if( NULL == pFindObj )
					break;
			}

			int nObjCount = 0;
			_read( nHandle, &nObjCount, sizeof( int ) );
			for( int l = 0; l < nObjCount; ++l )
			{
				pFindObj->push_back( csnew CsMapObject );

				int nOrderIndx = 0;
				_read( nHandle, &nOrderIndx, sizeof( int ) );
				pFindObj->back()->SetOrderNumber( nOrderIndx );

				int nFactSize = 0;
				_read( nHandle, &nFactSize, sizeof( int ) );

				for( int k = 0; k < nFactSize; ++k )
				{
					CsMapObject::sFACTOR_INFO addInfo;
					_read( nHandle, &addInfo.s_nOpenType, sizeof( int ) );
					_read( nHandle, &addInfo.s_nFactorCnt, sizeof( int ) );
					_read( nHandle, &addInfo.s_nFactor, sizeof( unsigned int ) );

					pFindObj->back()->AddFactorInfo( addInfo );
				}
			}
		}
	}
	CsFPS::CsFPSystem::SeekUnLock( FT_PACKHANDLE );
}
//==============================================================================================
//
//		Group
//
//==============================================================================================
CsMapObjectGroup const* CsMapObjectMng::GetGroup( DWORD const& dwMapID ) const
{
	GROUP_MAP_CIT it = m_mapMapObjectGroup.find( dwMapID );
	if( it == m_mapMapObjectGroup.end() )
		return NULL;

	return it->second;
}

void CsMapObjectMng::DeleteGroup( DWORD const& dwMapID )
{
	GROUP_MAP_IT it = m_mapMapObjectGroup.find( dwMapID );
	if( it == m_mapMapObjectGroup.end() )
		return;
	
	if( it->second )
		it->second->Delete();

	SAFE_DELETE( it->second );
	m_mapMapObjectGroup.erase( it );
}

bool CsMapObjectMng::IsGroup( DWORD const& dwMapID ) const
{
	GROUP_MAP_CIT it = m_mapMapObjectGroup.find( dwMapID );
	if( it == m_mapMapObjectGroup.end() )
		return false;

	return true;
}

bool CsMapObjectMng::IsGroup( DWORD const& dwMapID, DWORD const& dwObjectID ) const
{
	GROUP_MAP_CIT it = m_mapMapObjectGroup.find( dwMapID );
	if( it == m_mapMapObjectGroup.end() )
		return false;

	return it->second->IsObject(dwObjectID);
}

void CsMapObjectMng::DeleteActorObject( DWORD const& dwMapID, DWORD const& dwObjId )
{
	GROUP_MAP_IT it = m_mapMapObjectGroup.find( dwMapID );
	if( it == m_mapMapObjectGroup.end() )
		return;

	it->second->DeleteObjectFormIdx(dwObjId);
}

void CsMapObjectMng::DeleteActorFactor1( DWORD const& dwMapID, DWORD const& dwObjId, BYTE const& byFactor1 )
{
	GROUP_MAP_IT it = m_mapMapObjectGroup.find( dwMapID );
	if( it == m_mapMapObjectGroup.end() )
		return;

	it->second->DeleteActorFactor(dwObjId, byFactor1);
}

void CsMapObjectMng::DeleteActorFactor2( DWORD const& dwMapID, DWORD const& dwObjId, BYTE const& byFactor1, BYTE const& byFactor2 )
{
	GROUP_MAP_IT it = m_mapMapObjectGroup.find( dwMapID );
	if( it == m_mapMapObjectGroup.end() )
		return;

	it->second->DeleteActorFactor(dwObjId, byFactor1, byFactor2);
}


CsMapObjectGroup::LIST_OBJECT const* CsMapObjectMng::GetFactorList( DWORD const& dwMapID, DWORD const& dwObjId ) const
{
	GROUP_MAP_CIT it = m_mapMapObjectGroup.find( dwMapID );
	if( it == m_mapMapObjectGroup.end() )
		return NULL;

	return it->second->FindObjectList(dwObjId);
}

void CsMapObjectMng::AddActorOrFactor( DWORD const& dwMapID, DWORD const& dwObjId, CsMapObject::sFACTOR_INFO addInfo )
{
	GROUP_MAP_CIT it = m_mapMapObjectGroup.find( dwMapID );
	if( it == m_mapMapObjectGroup.end() )
		return;

	CsMapObjectGroup::LIST_OBJECT * pFindObj = const_cast<CsMapObjectGroup::LIST_OBJECT*>(it->second->FindObjectList(dwObjId));
	if( NULL == pFindObj )
	{
		pFindObj = it->second->AddNewObject(dwObjId);
		if( NULL == pFindObj )
			return;
	}

	pFindObj->push_back( csnew CsMapObject );
	pFindObj->back()->AddFactorInfo( addInfo );
}

void CsMapObjectMng::AddActorAndFactor( DWORD const& dwMapID, DWORD const& dwObjId, BYTE const& byFactor1, CsMapObject::sFACTOR_INFO addInfo )
{
	GROUP_MAP_CIT it = m_mapMapObjectGroup.find( dwMapID );
	if( it == m_mapMapObjectGroup.end() )
		return;

	CsMapObjectGroup::LIST_OBJECT * pFindObj = const_cast<CsMapObjectGroup::LIST_OBJECT*>(it->second->FindObjectList(dwObjId));
	if( NULL == pFindObj )
		return;

	CsMapObjectGroup::LIST_OBJECT_IT itObj = pFindObj->begin();
	for( int n=0 ; itObj != pFindObj->end(); ++itObj, ++n )
	{
		if( n == byFactor1 )
		{
			(*itObj)->AddFactorInfo( addInfo );
			break;
		}
	}
}

void CsMapObjectMng::ChangeActorOrFactor( DWORD const& dwMapID, DWORD const& dwObjId, BYTE const& byFactor1, CsMapObject::sFACTOR_INFO addInfo )
{
	GROUP_MAP_CIT it = m_mapMapObjectGroup.find( dwMapID );
	if( it == m_mapMapObjectGroup.end() )
		return;

	CsMapObjectGroup::LIST_OBJECT * pFindObj = const_cast<CsMapObjectGroup::LIST_OBJECT*>(it->second->FindObjectList(dwObjId));
	if( NULL == pFindObj )
		return;

	CsMapObjectGroup::LIST_OBJECT_IT itObj = pFindObj->begin();
	for( int n=0 ; itObj != pFindObj->end(); ++itObj, ++n )
	{
		if( n == byFactor1 )
		{
			(*itObj)->ChangeFactInfo( addInfo );
			break;
		}
	}
}

void CsMapObjectMng::ChangeActorAndFactor( DWORD const& dwMapID, DWORD const& dwObjId, BYTE const& byFactor1, BYTE const& byFactor2, CsMapObject::sFACTOR_INFO addInfo )
{
	GROUP_MAP_CIT it = m_mapMapObjectGroup.find( dwMapID );
	if( it == m_mapMapObjectGroup.end() )
		return;

	CsMapObjectGroup::LIST_OBJECT * pFindObj = const_cast<CsMapObjectGroup::LIST_OBJECT*>(it->second->FindObjectList(dwObjId));
	if( NULL == pFindObj )
		return;

	CsMapObjectGroup::LIST_OBJECT_IT itObj = pFindObj->begin();
	for( int n=0 ; itObj != pFindObj->end(); ++itObj, ++n )
	{
		if( n == byFactor1 )
		{
			(*itObj)->ChangeFactInfo( addInfo, byFactor2 );
			break;
		}
	}
}

void CsMapObjectMng::ChangeOrderNumber( DWORD const& dwMapID, DWORD const& dwObjId, int const& nOrderNum )
{
	CsMapObjectGroup::LIST_OBJECT const* pObjList = GetFactorList( dwMapID, dwObjId );
	if( NULL == pObjList )
		return ;

	CsMapObjectGroup::LIST_OBJECT_CIT it = pObjList->begin();
	for( ; it != pObjList->end(); ++it )
		(*it)->SetOrderNumber( nOrderNum );
}

CsMapObject::sFACTOR_INFO const* CsMapObjectMng::GetFactorInfo( DWORD const& dwMapID, DWORD const& dwObjId, BYTE const& byFactor1, BYTE const& byFactor2 ) const
{
	CsMapObjectGroup::LIST_OBJECT const* pObjList = GetFactorList( dwMapID, dwObjId );
	if( NULL == pObjList )
		return NULL;

	CsMapObjectGroup::LIST_OBJECT_CIT it = pObjList->begin();
	for( int nCount = 0 ; it != pObjList->end(); ++it, ++nCount )
	{
		if( nCount != byFactor1 )
			continue;

		CsMapObject::sFACTOR_INFO const* pInfo = (*it)->GetFactorInfo( byFactor2 );
		return pInfo;
	}

	return NULL;
}

