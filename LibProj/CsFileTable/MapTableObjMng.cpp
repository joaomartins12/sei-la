
#include "stdafx.h"
#include "MapTableObjMng.h"


void CsMapTableObjMng::Delete()
{
	CsMapTableObjGroup::MAP_IT it = m_mapGroup.begin();
	CsMapTableObjGroup::MAP_IT itEnd = m_mapGroup.end();
	for( ; it!=itEnd; ++it )
	{
		it->second->Delete();
		delete it->second;
	}
	m_mapGroup.clear();
}

bool CsMapTableObjMng::Init()
{
	switch( nsCsFileTable::g_eFileType )
	{
	case nsCsFileTable::FT_EXCEL:
	case nsCsFileTable::FT_BIN:
		_LoadBin();
		break;
	case nsCsFileTable::FT_FILEPACK:
		_LoadFilePack();
		break;
	default:
		assert_cs( false );
	}
	return true;
}

void CsMapTableObjMng::Save()
{
	_SaveBin();
}

CsMapTableObjGroup* CsMapTableObjMng::GetGroup( DWORD dwMapID )
{
	if( IsGroup( dwMapID ) == false )
	{
		CsMapTableObjGroup* pGroup = csnew CsMapTableObjGroup;
		pGroup->Init( dwMapID );		
		m_mapGroup[ dwMapID ] = pGroup;
	}
	return m_mapGroup[ dwMapID ];
}

#define MAP_TABLE_OBJ_BIN		"Data\\Map\\TableObj.bin"
#define MAP_TABLE_OBJ_BIN2		"Data\\Map\\TableObj2.bin"

void CsMapTableObjMng::_SaveBin()
{
	// 정보 없는 그룹은 제거
	CsMapTableObjGroup::MAP_IT itCheck = m_mapGroup.begin();
	CsMapTableObjGroup::MAP_IT itCheckEnd = m_mapGroup.end();
	while( itCheck!=itCheckEnd )
	{
		itCheck->second->CheckEmpty();

		if( itCheck->second->GetNodeSize() == 0 )
		{
			itCheck->second->Delete();
			delete itCheck->second;
			DWORD nDeleteID = itCheck->first;
			++itCheck;
			m_mapGroup.erase( nDeleteID );
		}
		else
		{
			++itCheck;
		}
	}

	FILE *fp = NULL;
	
	CsMapTableObjGroup::MAP_IT it = m_mapGroup.begin();
	CsMapTableObjGroup::MAP_IT itEnd = m_mapGroup.end();

	bool bTrue = false;

	int nCount;

	for(; it != itEnd; ++it)
	{
		if(it->second->GetMapID() == 1)
		{
			bTrue = true;

			fopen_s( &fp, MAP_TABLE_OBJ_BIN2, "wb" );
			assert_cs( fp != NULL );

			nCount = 1;

			fwrite( &nCount, sizeof( int ), 1, fp );

			it->second->SaveBin( fp );

			fclose( fp );
		}
	}

	fopen_s( &fp, MAP_TABLE_OBJ_BIN, "wb" );
	assert_cs( fp != NULL );

	nCount = (int)m_mapGroup.size();

	if(bTrue)
	{
		nCount -= 1;
		bTrue = false;
	}

	fwrite( &nCount, sizeof( int ), 1, fp );

//	CsMapTableObjGroup::MAP_IT it = m_mapGroup.begin();
//	CsMapTableObjGroup::MAP_IT itEnd = m_mapGroup.end();

	it = m_mapGroup.begin();

	for( ; it!=itEnd; ++it )
	{
		if(it->second->GetMapID() == 1) continue;

		it->second->SaveBin( fp );
	}

	fclose( fp );
}

bool CsMapTableObjMng::_LoadBin()
{
	FILE *fp = NULL;
	fopen_s( &fp, MAP_TABLE_OBJ_BIN, "rb" );
//	if( fp == NULL )
//		return false;

	if(fp != NULL)
	{
		int nCount;
		fread( &nCount, sizeof( int ), 1, fp );

		for( int i=0; i<nCount; ++i )
		{
			CsMapTableObjGroup* pObj = csnew CsMapTableObjGroup;
			pObj->LoadBin( fp );

			assert_cs( IsGroup( pObj->GetMapID() ) == false );
			m_mapGroup[ pObj->GetMapID() ] = pObj;
		}

		fclose( fp );
	}

    fp = NULL;

	fopen_s( &fp, MAP_TABLE_OBJ_BIN2, "rb");
	
//	if(fp == NULL)
//		return false;

	if(fp != NULL)
	{
		int nCount;
		fread( &nCount, sizeof( int ), 1, fp );

		for( int i=0; i<nCount; ++i )
		{
			CsMapTableObjGroup* pObj = csnew CsMapTableObjGroup;
			pObj->LoadBin( fp );

			assert_cs( IsGroup( pObj->GetMapID() ) == false );
			m_mapGroup[ pObj->GetMapID() ] = pObj;
		}

		fclose( fp );
	}

	return true;
}

bool CsMapTableObjMng::_LoadFilePack()
{
	if( !CsFPS::CsFPSystem::IsExistOnlyPack( FT_RESOURCE_HANDLE, MAP_TABLE_OBJ_BIN ) ||
		!CsFPS::CsFPSystem::IsExistOnlyPack( FT_RESOURCE_HANDLE, MAP_TABLE_OBJ_BIN2 ) )
	{
		return _LoadBin();    
	}

	if( !CsFPS::CsFPSystem::SeekLock( FT_RESOURCE_HANDLE ) )
		return false;

	int nHandle = CsFPS::CsFPSystem::GetFileHandle( FT_RESOURCE_HANDLE, MAP_TABLE_OBJ_BIN );

	int nCount;
	_read( nHandle, &nCount, sizeof( int ) );

	for( int i=0; i<nCount; ++i )
	{
		CsMapTableObjGroup* pObj = csnew CsMapTableObjGroup;
		pObj->LoadFilePack( nHandle );

		assert_cs( IsGroup( pObj->GetMapID() ) == false );
		m_mapGroup[ pObj->GetMapID() ] = pObj;
	}

	CsFPS::CsFPSystem::SeekUnLock( FT_RESOURCE_HANDLE );

	if( !CsFPS::CsFPSystem::SeekLock( FT_RESOURCE_HANDLE ) )
		return false;

	nHandle = CsFPS::CsFPSystem::GetFileHandle( FT_RESOURCE_HANDLE, MAP_TABLE_OBJ_BIN2 );

	_read(nHandle, &nCount, sizeof( int ));

	for( int i = 0; i < nCount; ++i )
	{
		CsMapTableObjGroup* pObj = csnew CsMapTableObjGroup;
		pObj->LoadFilePack(nHandle);

		assert_cs(IsGroup(pObj->GetMapID()) == false);
		m_mapGroup[pObj->GetMapID()] = pObj;
	}

	CsFPS::CsFPSystem::SeekUnLock( FT_RESOURCE_HANDLE );

	return true;
}