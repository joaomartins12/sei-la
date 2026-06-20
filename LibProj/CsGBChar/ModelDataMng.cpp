
#include "stdafx.h"
#include "ModelDataMng.h"

CsModelDataMng*		g_pModelDataMng = NULL;

#define MODELDATA_MNG_FILENAME		_T( "Data\\Model.dat" )

CsModelDataMng::CsModelDataMng()
{

}

CsModelDataMng::~CsModelDataMng()
{

}

void CsModelDataMng::Delete()
{
	std::map< DWORD, CsModelData* >::iterator it = m_mapModelData.begin();
	std::map< DWORD, CsModelData* >::iterator itEnd = m_mapModelData.end();
	for( ; it!=itEnd; ++it )
	{
		it->second->Delete();
		NiDelete it->second;
	}
	m_mapModelData.clear();
}

void CsModelDataMng::Init()
{
	if( CsFPS::CsFPSystem::IsExistOnlyPack(0, "Data\\Model.dat") )
	{
		_LoadByFilePack();
	}
	else
	{
		_LoadByFile();
	}
}

void CsModelDataMng::_LoadByFilePack()
{
	assert_cs( m_mapModelData.empty() == true );
	
	if( !CsFPS::CsFPSystem::SeekLock(0) )
		return;
	
	int nHandle = CsFPS::CsFPSystem::GetFileHandle( 0, "Data\\Model.dat" );

	int nCount;
	_read( nHandle, &nCount, sizeof( int ) );

	for( int i=0; i<nCount; ++i )
	{
		CsModelData* pMD = NiNew CsModelData;

		_read( nHandle, pMD->GetInfo(), sizeof( CsModelData::sINFO ) );

		std::map<DWORD, CsModelData *>::iterator it = m_mapModelData.find(pMD->GetInfo()->s_dwID);
		if (it != m_mapModelData.end())
		{
			assert_cs(false);
			NiDelete pMD;
			continue;
		}		

		int nSequenceCount = pMD->GetInfo()->s_nSequenceCount;

		for( int nSeq=0; nSeq<nSequenceCount; ++nSeq )
		{
			CsModelData::sSEQUENCE* pSeq = NiNew CsModelData::sSEQUENCE;
			_read( nHandle, &pSeq->s_SequenceInfo, sizeof( CsModelData::sSEQUENCE::sSEQUENCE_INFO ) );

			int nEventCount = pSeq->s_SequenceInfo.s_nEventCount;
			for( int i=0; i<nEventCount; ++i )
			{
				CsModelData::sEVENT* pEvent = NiNew CsModelData::sEVENT;
				_read( nHandle, pEvent, sizeof( CsModelData::sEVENT ) );
				pSeq->InsertEvent( pEvent );				
			}
			int nShaderCount = pSeq->s_SequenceInfo.s_nShaderCnt;
			for( int i=0; i<nShaderCount; ++i )
			{
				CsModelData::sSHADER* pShader = NiNew CsModelData::sSHADER;
				_read( nHandle, pShader, sizeof( CsModelData::sSHADER ) );
				pSeq->InsertShader( pShader );
			}
			pMD->InsertSequence( pSeq->s_SequenceInfo.s_dwSequenceID, pSeq );
		}
		m_mapModelData.insert(std::make_pair(pMD->GetInfo()->s_dwID, pMD));
	}

	CsFPS::CsFPSystem::SeekUnLock( 0 );
}

void CsModelDataMng::_LoadByFile()
{
	assert_cs( m_mapModelData.empty() == true );

	if( _taccess_s( MODELDATA_MNG_FILENAME, 0 ) != 0 )
		return;

	FILE* fp = NULL;

	_tfopen_s( &fp, MODELDATA_MNG_FILENAME, _T( "rb" ) );
	assert_cs( fp != NULL );

	int nCount;
	fread( &nCount, sizeof( int ), 1, fp );

	for( int i=0; i<nCount; ++i )
	{
		CsModelData* pMD = NiNew CsModelData;
		fread( pMD->GetInfo(), sizeof( CsModelData::sINFO ), 1, fp );

		std::map<DWORD, CsModelData *>::iterator it = m_mapModelData.find(pMD->GetInfo()->s_dwID);
		if (it != m_mapModelData.end())
		{
			assert_cs(false);
			NiDelete pMD;
			continue;
		}

		int nSequenceCount = pMD->GetInfo()->s_nSequenceCount;
		for( int nSeq=0; nSeq<nSequenceCount; ++nSeq )
		{
			CsModelData::sSEQUENCE* pSeq = NiNew CsModelData::sSEQUENCE;
			fread( &pSeq->s_SequenceInfo, sizeof( CsModelData::sSEQUENCE::sSEQUENCE_INFO ), 1, fp );

			int nEventCount = pSeq->s_SequenceInfo.s_nEventCount;
			for( int i=0; i<nEventCount; ++i )
			{
				CsModelData::sEVENT* pEvent = NiNew CsModelData::sEVENT;
				fread( pEvent, sizeof( CsModelData::sEVENT ), 1, fp );
				pSeq->InsertEvent( pEvent );
			}
			int nShaderCount = pSeq->s_SequenceInfo.s_nShaderCnt;
			for( int i=0; i<nShaderCount; ++i )
			{
				CsModelData::sSHADER* pShader = NiNew CsModelData::sSHADER;
				fread( pShader, sizeof( CsModelData::sSHADER ), 1, fp );
				pSeq->InsertShader( pShader );
			}

			pMD->InsertSequence( pSeq->s_SequenceInfo.s_dwSequenceID, pSeq );
		}
		m_mapModelData.insert(std::make_pair(pMD->GetInfo()->s_dwID, pMD));
	}

	fclose( fp );
}

void CsModelDataMng::Save()
{
	// 이미 파일이 존재 한다면 백업 시키다
	if( _taccess_s( MODELDATA_MNG_FILENAME, 0 ) == 0 )
	{
		// 폴더가 존재 하지 않는다면 폴더 생성
		nsCSFILE::CreateDirectory( _T( "GameData\\ModelData_Backup" ) );

		TCHAR szNewFile[ MAX_PATH ];
		SYSTEMTIME st;
		GetLocalTime( &st );
		//st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond;
		_stprintf_s( szNewFile, MAX_PATH, _T( "GameData\\ModelData_Backup\\Model_%.2d%.2d%.2d_%.2d%.2d%.2d.dat" ), st.wYear%100, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond );
		// 이미 파일이 존재 한다면
		if( _taccess_s( szNewFile, 0 ) == 0 )
			DeleteFile( szNewFile );
		_trename( MODELDATA_MNG_FILENAME, szNewFile );
	}

	FILE* fp = NULL;
	_tfopen_s( &fp, MODELDATA_MNG_FILENAME, _T( "wb" ) );

	int nCount = (int)m_mapModelData.size();
	fwrite( &nCount, sizeof( int ), 1, fp );

	std::map< DWORD, CsModelData* >::iterator it = m_mapModelData.begin();
	std::map< DWORD, CsModelData* >::iterator itEnd = m_mapModelData.end();

	for( ; it!=itEnd; ++it )
	{
		fwrite( it->second->GetInfo(), sizeof( CsModelData::sINFO ), 1, fp );

		// 시퀀스 정보 기록
		std::map< DWORD, CsModelData::sSEQUENCE* >* mapSequence = it->second->GetSequenceMap();
		std::map< DWORD, CsModelData::sSEQUENCE* >::iterator itSeq = mapSequence->begin();
		std::map< DWORD, CsModelData::sSEQUENCE* >::iterator itSeqEnd = mapSequence->end();
		assert_cs( mapSequence->size() == it->second->GetInfo()->s_nSequenceCount );
		for( ; itSeq!=itSeqEnd; ++itSeq )
		{
			fwrite( &itSeq->second->s_SequenceInfo, sizeof( CsModelData::sSEQUENCE::sSEQUENCE_INFO ), 1, fp );

			std::list< CsModelData::sEVENT* > listEvent;
			std::list< CsModelData::sEVENT* >::iterator itEvent = listEvent.begin();
			std::list< CsModelData::sEVENT* >::iterator itEventEnd = listEvent.end();

			// 이벤트 시간에 따른 정렬
			int nEventCount = itSeq->second->s_SequenceInfo.s_nEventCount;
			for( int i=0; i<nEventCount; ++i )
			{
				CsModelData::sEVENT* pEvent = itSeq->second->GetEvent( i );
				bool bInsert = false;				
				
				itEvent = listEvent.begin();
				for( ; itEvent!=itEventEnd; ++itEvent )
				{
					if( pEvent->s_fEventTime < (*itEvent)->s_fEventTime )
					{
						listEvent.insert( itEvent, pEvent );
						bInsert = true;
						break;
					}
				}
				if( bInsert == false )
				{
					listEvent.push_back( pEvent );
				}			
			}
			assert_cs( listEvent.size() == nEventCount );

			// 정렬 했으니 저장하자
			itEvent = listEvent.begin();
			for( ; itEvent!=itEventEnd; ++itEvent )
			{
				if( (*itEvent)->s_fEffectScale == 0.0f )
					(*itEvent)->s_fEffectScale = 1.0f;
				fwrite( *itEvent, sizeof( CsModelData::sEVENT ), 1, fp );
			}
			listEvent.clear();

			// 쉐이더 저장
			int nShaderCnt = itSeq->second->s_SequenceInfo.s_nShaderCnt;
			for( int i=0; i<nShaderCnt; ++i )
			{
				CsModelData::sSHADER* pShader = itSeq->second->GetShader( i );
				fwrite( pShader, sizeof( CsModelData::sSHADER ), 1, fp );
			}
		}
	}
	fclose( fp );
}

bool CsModelDataMng::IsData( DWORD const& dwID ) const
{ 
	std::map< DWORD, CsModelData* >::const_iterator it = m_mapModelData.find( dwID );
	if( it == m_mapModelData.end() )
		return false;

	return true;
}

CsModelData* CsModelDataMng::GetData( DWORD const& dwID )
{ 
	std::map< DWORD, CsModelData* >::iterator it = m_mapModelData.find( dwID );
	if( it == m_mapModelData.end() )
		return NULL;

	return it->second;
}

std::string CsModelDataMng::GetKfmPath( DWORD const& dwID ) const
{
	std::map< DWORD, CsModelData* >::const_iterator it = m_mapModelData.find( dwID );
	if( it == m_mapModelData.end() )
		return "";

	SAFE_POINTER_RETVAL( it->second, "" );

	CsModelData::sINFO const* pInfo = it->second->GetInfo();
	SAFE_POINTER_RETVAL( pInfo, "" );
	return pInfo->s_cKfmPath;
}

void CsModelDataMng::InsertData( CsModelData::sINFO* pModelInfo )
{
	SAFE_POINTER_RET( pModelInfo );
	std::map< DWORD, CsModelData* >::iterator it = m_mapModelData.find( pModelInfo->s_dwID );
	if( it != m_mapModelData.end() )
		return;

	CsModelData* pMD = NiNew CsModelData;
	memcpy( pMD->GetInfo(), pModelInfo, sizeof( CsModelData::sINFO) );
	m_mapModelData.insert( std::make_pair( pMD->GetInfo()->s_dwID, pMD ) );
}

void CsModelDataMng::DeleteData( DWORD dwID )
{ 
	std::map< DWORD, CsModelData* >::iterator it = m_mapModelData.find( dwID );
	if( it != m_mapModelData.end() )
	{
		SAFE_NIDELETE( it->second );
		m_mapModelData.erase( it );
	}
}

std::map< DWORD, CsModelData* >* CsModelDataMng::GetDataMap()
{
	return &m_mapModelData;
}

bool CsModelDataMng::_IsSaversCharacter(int nModelTamerIdx) const
{
	switch(nModelTamerIdx)
	{
	case CODE_MASARU:
	case CODE_TOHMA:
	case CODE_YOSHINO:
	case CODE_IKUTO:
		return true;
	}
	return false;
}

bool CsModelDataMng::_IsAdventureCharacter(int nModelTamerIdx) const
{
	switch(nModelTamerIdx)
	{
	case CODE_TAICHI:
	case CODE_MIMI:
	case CODE_MATT:
	case CODE_TAKERU:
	case CODE_HIKARI:
	case CODE_SORA:
 	case CODE_JANG:
 	case CODE_JUNG:
		return true;
	}
	return false;
}

bool CsModelDataMng::_IsMaleCharacter(int nModelTamerIdx) const
{
	switch(nModelTamerIdx)
	{
	case CODE_MASARU:	/* 세이버즈 Boy */
	case CODE_TOHMA:
	case CODE_IKUTO:
	case CODE_TAICHI:	/* 어드벤쳐 Boy */
	case CODE_MATT:
	case CODE_TAKERU:
	case CODE_JANG:
	case CODE_JUNG:
		return true;
	}
	return false;
}

bool CsModelDataMng::_IsFemaleCharacter(int nModelTamerIdx) const
{
	switch(nModelTamerIdx)
	{
	case CODE_YOSHINO:	/* 세이버즈 Girl */
	case CODE_MIMI:		/* 어드벤쳐 Girl */
	case CODE_HIKARI:	/* 어드벤쳐 신나리 */
	case CODE_SORA:		/* 어드벤쳐 한소라 */
		return true;
	}
	return false;
}

bool CsModelDataMng::CostumeCategory( int nTamerModelIdx, int nValue ) const
{	
	bool bFlag = false;
	switch(nValue)
	{
	case COSTUME_ID_EMPTY:	bFlag = false;											break;
	case COSTUME_ID_2601:	bFlag = true;											break;	// 2601 > 공통
	case COSTUME_ID_2602:	if( _IsSaversCharacter(nTamerModelIdx) )bFlag = true;	break;	// 2602 > 세이버즈 전용
	case COSTUME_ID_2603:	if( _IsAdventureCharacter(nTamerModelIdx) )bFlag = true;break;	// 2603 > 어드벤쳐 전용
	case COSTUME_ID_2604:	if( _IsMaleCharacter(nTamerModelIdx) )bFlag = true;		break;	// 2604 > 남성 전용
	case COSTUME_ID_2605:	if( _IsFemaleCharacter(nTamerModelIdx) )bFlag = true;	break;	// 2605 > 여성 전용
	case COSTUME_ID_2606: bFlag = CODE_MASARU == nTamerModelIdx ? true : false;		break;	// 최건우 전용
	case COSTUME_ID_2607: bFlag = CODE_TOHMA == nTamerModelIdx ? true : false;		break;	// 토마 전용
	case COSTUME_ID_2608: bFlag = CODE_YOSHINO == nTamerModelIdx ? true : false;	break;	// 유진 전용
	case COSTUME_ID_2609: bFlag = CODE_IKUTO == nTamerModelIdx ? true : false;		break;	// 한지호 전용
	case COSTUME_ID_2610: bFlag = CODE_TAICHI == nTamerModelIdx ? true : false;		break;	// 최건우 전용
	case COSTUME_ID_2611: bFlag = CODE_MIMI == nTamerModelIdx ? true : false;		break;	// 이미나 전용
	case COSTUME_ID_2612: bFlag = CODE_MATT == nTamerModelIdx ? true : false;		break;	// 매튜 전용
	case COSTUME_ID_2613: bFlag = CODE_TAKERU == nTamerModelIdx ? true : false;		break;	// 리키 전용
 	case COSTUME_ID_2614: bFlag = CODE_HIKARI == nTamerModelIdx ? true : false;		break;	// 신나리 전용
 	case COSTUME_ID_2615: bFlag = CODE_SORA  == nTamerModelIdx ? true : false;		break;	// 한소라 전용
 	case COSTUME_ID_2616: bFlag = CODE_JANG == nTamerModelIdx ? true : false;	break;		// 장한솔
 	case COSTUME_ID_2617: bFlag = CODE_JUNG == nTamerModelIdx ? true : false;	break;		// 정석
	}

	return bFlag;
}

bool CsModelDataMng::CostumeCategory_DProject( int nTamerModelIdx, int nValue ) const
{
	// 예외 : 어드벤쳐 캐릭터가 세이버즈 전용 코스튬 사용이 가능 (추후 기획 파트의 요청이 있으면 변경합니다.) !< 리소스는 적용 안함 >
	if(	nValue == COSTUME_ID_2602	&&				// 세이버즈 전용이지만,
		_IsAdventureCharacter(nTamerModelIdx))		// 어드벤쳐 캐릭터가 착용하려고 할 때 예외를 허용합니다.
	{
		return true;
	}

	return CostumeCategory(nTamerModelIdx, nValue);
}

DWORD CsModelDataMng::CostumeToTamerCode( int nValue ) const
{
	switch(nValue)
	{
	case COSTUME_ID_2606: return CODE_MASARU;	// 최건우
	case COSTUME_ID_2607: return CODE_TOHMA;	// 토마
	case COSTUME_ID_2608: return CODE_YOSHINO;	// 유진
	case COSTUME_ID_2609: return CODE_IKUTO;	// 한지호
	case COSTUME_ID_2610: return CODE_TAICHI;	// 최건우
	case COSTUME_ID_2611: return CODE_MIMI;		// 이미나
	case COSTUME_ID_2612: return CODE_MATT;		// 매튜
	case COSTUME_ID_2613: return CODE_TAKERU;	// 리키
 	case COSTUME_ID_2614: return CODE_HIKARI;	// 신나리
 	case COSTUME_ID_2615: return CODE_SORA;		// 한소라
 	case COSTUME_ID_2616: return CODE_JANG;		// 장한솔
 	case COSTUME_ID_2617: return CODE_JUNG;		// 정석
	default:
		assert_csm1(false, "테이머 전용 코스튬에 없는 소분류 입니다. 테이머 추가 시 클라에게 문의 ID=%d", nValue);
		break;
	}

	return 0;
}

std::string CsModelDataMng::GetBigModelIconFile( DWORD const& dwID )
{
	std::string imgFile = GetKfmPath( dwID );
	if( imgFile.empty() )
		imgFile = "Data\\Dummy\\dummy";

	char szDir[_MAX_DIR] = {0,};
	char szFName[_MAX_FNAME] = {0,};
	char szExt[_MAX_EXT] = {0,};
	_splitpath_s(imgFile.c_str(), NULL, 0, szDir, _MAX_DIR, szFName, _MAX_FNAME, szExt, _MAX_EXT);

	imgFile = szDir;
	imgFile += szFName;
	imgFile += "XL.tga";
	return imgFile;
}

std::string CsModelDataMng::GetBossModelIconFile( DWORD const& dwID )
{
	std::string imgFile = GetKfmPath( dwID );
	if( imgFile.empty() )
		imgFile = "Data\\Dummy\\dummy";

	char szDir[_MAX_DIR] = {0,};
	char szFName[_MAX_FNAME] = {0,};
	char szExt[_MAX_EXT] = {0,};
	_splitpath_s(imgFile.c_str(), NULL, 0, szDir, _MAX_DIR, szFName, _MAX_FNAME, szExt, _MAX_EXT);

	imgFile = szDir;
	imgFile += szFName;
	imgFile += "B.tga";
	return imgFile;
}

std::string CsModelDataMng::GetEvoModelIconFile( DWORD const& dwID )
{
	std::string imgFile = GetKfmPath( dwID );
	if( imgFile.empty() )
		imgFile = "Data\\Dummy\\dummy";

	char szDir[_MAX_DIR] = {0,};
	char szFName[_MAX_FNAME] = {0,};
	char szExt[_MAX_EXT] = {0,};
	_splitpath_s(imgFile.c_str(), NULL, 0, szDir, _MAX_DIR, szFName, _MAX_FNAME, szExt, _MAX_EXT);

	imgFile = szDir;
	imgFile += szFName;
	imgFile += "_evo.tga";
	return imgFile;
}

std::string CsModelDataMng::GetLargeModelIconFile( DWORD const& dwID )
{
	std::string imgFile = GetKfmPath( dwID );
	if( imgFile.empty() )
		imgFile = "Data\\Dummy\\dummy";

	char szDir[_MAX_DIR] = {0,};
	char szFName[_MAX_FNAME] = {0,};
	char szExt[_MAX_EXT] = {0,};
	_splitpath_s(imgFile.c_str(), NULL, 0, szDir, _MAX_DIR, szFName, _MAX_FNAME, szExt, _MAX_EXT);

	imgFile = szDir;
	imgFile += szFName;
	imgFile += "L.tga";
	return imgFile;
}

std::string	CsModelDataMng::GetSmallModelIconFile( DWORD const& dwID )
{
	std::string imgFile = GetKfmPath( dwID );
	if( imgFile.empty() )
		imgFile = "Data\\Dummy\\dummy";

	char szDir[_MAX_DIR] = {0,};
	char szFName[_MAX_FNAME] = {0,};
	char szExt[_MAX_EXT] = {0,};
	_splitpath_s(imgFile.c_str(), NULL, 0, szDir, _MAX_DIR, szFName, _MAX_FNAME, szExt, _MAX_EXT);

	imgFile = szDir;
	imgFile += szFName;
	imgFile += "S.tga";
	return imgFile;

}

int	CsModelDataMng::CreateDataFromAnother(int SrcID, int DestID, std::string NewKfmPath, bool ForceSave)
{
	std::map< DWORD, CsModelData* >::iterator srcIter = m_mapModelData.find(SrcID);
	if(srcIter == m_mapModelData.end())
	{
		return 1;
	}
	std::map< DWORD, CsModelData* >::iterator destIter = m_mapModelData.find(DestID);
	if(destIter != m_mapModelData.end())
	{
		if(ForceSave == false)	
			return 2;
	}
	CsModelData* pkCopiedModelData = NiNew CsModelData();
	memcpy(pkCopiedModelData, srcIter->second, sizeof( CsModelData) );
	pkCopiedModelData->GetInfo()->s_dwID = DestID;
	strcpy_s( pkCopiedModelData->GetInfo()->s_cKfmPath, OBJECT_PATH_LEN, NewKfmPath.c_str());// "Data\\Digimon\\DorumonX\\0075102_Dorumon.kfm" );
	m_mapModelData.insert(  std::map< DWORD, CsModelData* >::value_type( DestID, pkCopiedModelData ));
	
	return 0;

}