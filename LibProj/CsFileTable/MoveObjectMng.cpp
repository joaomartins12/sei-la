
#include "stdafx.h"
#include "MoveObjectMng.h"
#include <assert.h>


void CsMoveObjectMng::Delete()
{
	CsMoveObject::MAP_IT it = m_mapObject.begin();
	CsMoveObject::MAP_IT itEnd = m_mapObject.end();
	for( ; it!=itEnd; ++it )
	{
		it->second->Delete();
		delete it->second;
	}
	m_mapObject.clear();
}

bool CsMoveObjectMng::Init()
{	
#ifdef _GIVE
	switch( nsCsFileTable::g_eFileType )
	{
	case nsCsFileTable::FT_BIN:
	case nsCsFileTable::FT_EXCEL:
		return Load();	
	case nsCsFileTable::FT_FILEPACK:
		LoadFilePack();
		return true;
	default:
		assert_cs( false );
	}	
#else
	switch( nsCsFileTable::g_eFileType )
	{
	case nsCsFileTable::FT_BIN:
	case nsCsFileTable::FT_EXCEL:
		Load();
		break;
	case nsCsFileTable::FT_FILEPACK:
		LoadFilePack();
		break;
	default:
		assert_cs( false );
	}	
#endif

	return true;
}

bool CsMoveObjectMng::IsObject( CsMoveObject::eKIND kind, DWORD dwID )
{
	assert( dwID < CsMoveObject::MO_NPC );
	return ( m_mapObject.find( TO_MO_ID( kind, dwID ) ) != m_mapObject.end() );
}

bool CsMoveObjectMng::IsObject( DWORD dwMoveObjectID )
{
	return ( m_mapObject.find( dwMoveObjectID ) != m_mapObject.end() );
}

CsMoveObject* CsMoveObjectMng::GetObject( CsMoveObject::eKIND kind, DWORD dwID )
{
	return GetObject( TO_MO_ID( kind, dwID ) );
}

CsMoveObject* CsMoveObjectMng::GetObject( DWORD dwMoveObjectID )
{
	if( IsObject( dwMoveObjectID ) == false )
	{
		CsMoveObject* pNew = new CsMoveObject;
		pNew->Setkind( MOTO_KIND( dwMoveObjectID ) );
		pNew->SetFTID( MOTO_ID( dwMoveObjectID ) );
		m_mapObject[ dwMoveObjectID ] = pNew;
	}
	return m_mapObject[ dwMoveObjectID ];
}

#define MOVEOBJECT_BIN_PATH				"Data\\MO.bin"

void CsMoveObjectMng::Save()
{
	FILE *fp = NULL;
	fopen_s( &fp, MOVEOBJECT_BIN_PATH, "wb" );
	assert_cs( fp != NULL );
	// °¹¼ö Ã¼Å©
	{
		int nCount = 0;
		CsMoveObject::MAP_IT it = m_mapObject.begin();
		CsMoveObject::MAP_IT itEnd = m_mapObject.end();
		for( ; it!=itEnd; ++it )
		{
			if( it->second->GetActList()->size() > 0 )
				++nCount;
		}
		fwrite( &nCount, sizeof( int ), 1, fp );
	}


	{
		int nMOID;
		CsMoveObject::MAP_IT it = m_mapObject.begin();
		CsMoveObject::MAP_IT itEnd = m_mapObject.end();
		for( ; it!=itEnd; ++it )
		{
			if( it->second->GetActList()->size() == 0 )
				continue;

			nMOID = it->second->GetMoveObjectID();
			fwrite( &nMOID, sizeof( int ), 1, fp );
			it->second->Save( fp );
		}
	}
	
	fclose( fp );
}

#define MOVEMONSTER_SHEET_NAME			"MoveMonsters"
void CsMoveObjectMng::ExcelSave()
{
	
	YExcel::BasicExcel excel;
	bool bLoad = excel.Load( nsCsFileTable::GetPath_DM_MoveObject() );
	assert_cs( bLoad == true );

	YExcel::BasicExcelWorksheet* pSheet = excel.GetWorksheet( MOVEMONSTER_SHEET_NAME );
	assert_cs( pSheet != NULL );

	pSheet->DeleteAllRows();

	{
		size_t siInputCol = 0;
		pSheet->Cell( 1, siInputCol++ )->SetWString( _T( "MapID" ) );
		pSheet->Cell( 1, siInputCol++ )->SetWString( _T( "MonsterID" ) );
		//pSheet->Cell( 1, siInputCol++ )->SetWString( _T( "TotalTime" ) );
		pSheet->Cell( 1, siInputCol++ )->SetWString( _T( "ActSize" ) );
		pSheet->Cell( 1, siInputCol++ )->SetWString( _T( "ActType" ) );
		pSheet->Cell( 1, siInputCol++ )->SetWString( _T( "ActTime" ) );
		pSheet->Cell( 1, siInputCol++ )->SetWString( _T( "AniID" ) );
		pSheet->Cell( 1, siInputCol++ )->SetWString( _T( "PosX" ) );
		pSheet->Cell( 1, siInputCol++ )->SetWString( _T( "PosY" ) );
	}

	size_t siInputRow = 3;
	CsMoveObject::MAP_IT it = m_mapObject.begin();
	CsMoveObject::MAP_IT itEnd = m_mapObject.end();
	for( ; it!=itEnd; ++it )
	{
		if( CsMoveObject::MO_MONSTER == it->second->GetKind())
		{	
			size_t siInputCol = 0;			


			pSheet->Cell( siInputRow, siInputCol++ )->SetInteger(  it->second->GetMapID() );
			pSheet->Cell( siInputRow, siInputCol++ )->SetInteger(  it->second->GetFTID() );
			//pSheet->Cell( siInputRow, siInputCol++ )->SetDouble( (double)it->second->GetTotalTime() );

			int SIZE = (int)it->second->GetActList()->size();
			pSheet->Cell( siInputRow, siInputCol++ )->SetInteger(  SIZE );

			CsMoveObject::sACT_BASE::LIST* pList = it->second->GetActList();
			CsMoveObject::sACT_BASE::LIST_IT itAct = pList->begin();
			
			for( ; itAct != pList->end(); ++itAct )
			{				
				pSheet->Cell( siInputRow, siInputCol++ )->SetInteger(  (*itAct)->GetType() );
				pSheet->Cell( siInputRow, siInputCol++ )->SetInteger(  (*itAct)->GetTime() );	
				pSheet->Cell( siInputRow, siInputCol++ )->SetInteger(  (*itAct)->GetAni() );				
				pSheet->Cell( siInputRow, siInputCol++ )->SetInteger(  (*itAct)->GetPosX() );	
				pSheet->Cell( siInputRow, siInputCol++ )->SetInteger(  (*itAct)->GetPosY() );	
			}
			++siInputRow;
		}
		
	}

	excel.SaveAs( nsCsFileTable::GetPath_DM_MoveObject() );
	
}

bool CsMoveObjectMng::Load()
{
	FILE *fp = NULL;
	fopen_s( &fp, MOVEOBJECT_BIN_PATH, "rb" );

	if( fp == NULL )
		return false;

	int nCount;
	fread( &nCount, sizeof( int ), 1, fp );

	int nMOID;
	for( int i=0; i<nCount; ++i )
	{
		fread( &nMOID, sizeof( int ), 1, fp );
		GetObject( nMOID )->Load( fp );
	}

	fclose( fp );
	return true;
}

void CsMoveObjectMng::LoadFilePack()
{
	if( !CsFPS::CsFPSystem::IsExistOnlyPack( FT_RESOURCE_HANDLE, MOVEOBJECT_BIN_PATH ) )
	{
		Load();
		return;
	}

	if( !CsFPS::CsFPSystem::SeekLock( FT_RESOURCE_HANDLE ) )
		return;

	int nHandle = CsFPS::CsFPSystem::GetFileHandle( FT_RESOURCE_HANDLE, MOVEOBJECT_BIN_PATH );

	int nCount;
	_read( nHandle, &nCount, sizeof( int ) );

	int nMOID;
	for( int i=0; i<nCount; ++i )
	{
		_read( nHandle, &nMOID, sizeof( int ) );
		GetObject( nMOID )->LoadFilePack( nHandle );
	}
	CsFPS::CsFPSystem::SeekUnLock( FT_RESOURCE_HANDLE );
}
