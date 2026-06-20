#include "StdAfx.h"
#include "EventDataMng.h"

CEventDataMng*		g_pEventDataMng = NULL;

#define EVENTDATA_FILENAME		_T( "Data\\EventData.dat" )

CEventDataMng::CEventDataMng(void)
{
}

CEventDataMng::~CEventDataMng(void)
{
}

void CEventDataMng::Delete()
{	
	std::map< UINT, CEventSceneData* >::iterator it = m_mapEventSceneData.begin();
	std::map< UINT, CEventSceneData* >::iterator itEnd = m_mapEventSceneData.end();
	for( ; it != itEnd; it++ )
	{
		it->second->Delete();
		SAFE_NIDELETE( it->second );
	}
	m_mapEventSceneData.clear();	
}

void CEventDataMng::DeleteData( UINT nIdx )
{
	CEventSceneData* pData = LoadData( nIdx );
	if( pData != NULL )
	{
		pData->Delete();
		SAFE_NIDELETE( pData );

		m_mapEventSceneData.erase( nIdx );
	}	
}

void CEventDataMng::Init()
{	
	if( CsFPS::CsFPSystem::IsExistOnlyPack( 0, "Data\\EventData.dat" ) )
	{
		LoadFilePack();
	}
	else
	{
		LoadFile();
	}
}

void CEventDataMng::LoadFilePack()
{
	//파일 로드	
	assert_cs( m_mapEventSceneData.empty() == true );
	
	if( !CsFPS::CsFPSystem::SeekLock(0) )
		return;

	int nHandle = CsFPS::CsFPSystem::GetFileHandle( 0, "Data\\EventData.dat" );
	int nCount;
	_read( nHandle, &nCount, sizeof( int ) );

	for( int i=0; i<nCount; ++i )
	{
		int nNum;
		_read( nHandle, &nNum, sizeof( int ) );	

		UINT SceneNum;			
		_read( nHandle, &SceneNum , sizeof( UINT ) );		
		assert_cs( m_mapEventSceneData.find( SceneNum ) == m_mapEventSceneData.end() );

		CEventSceneData* pSceneData = NiNew CEventSceneData;
		m_mapEventSceneData[ SceneNum ] = pSceneData;		

		CEventSceneData::sEvent_Condition	Info;
		_read( nHandle, &Info , sizeof( CEventSceneData::sEvent_Condition ) );					
		pSceneData->Init( SceneNum, &Info );

		for( int i = 0; i < nNum; i++ )
		{												
			LoadEventPack( nHandle, SceneNum );				
		}			
	}

	CsFPS::CsFPSystem::SeekUnLock(0);	
}

void CEventDataMng::LoadEventPack( int nHandle, UINT SceneNum ) 
{
	int nNum;
	_read( nHandle, &nNum, sizeof( int ) );	

	CEventData* pEventData = NiNew CEventData;

	CEventData::sEvent_Info eInfo;
	CEventData::sEventObject_Info oInfo;

	_read( nHandle, &eInfo , sizeof( CEventData::sEvent_Info ) );
	_read( nHandle, &oInfo , sizeof( CEventData::sEventObject_Info ) );	

	pEventData->Init( &eInfo, &oInfo );		

	for( int j = 0 ; j < nNum; j++ )			
	{
		CEventData::sBase_Info* pData = NiNew CEventData::sBase_Info;
		_read( nHandle, pData, sizeof( CEventData::sBase_Info ) );			
		int nsize = sizeof( CEventData::sBase_Info );

		switch( pData->GetType() )
		{
		case CEventData::ET_Ani:
			{					
				_lseeki64( nHandle, -nsize, SEEK_CUR );
				CEventData::sAnimation_Info* pAni = NiNew CEventData::sAnimation_Info;
				_read( nHandle, pAni, sizeof( CEventData::sAnimation_Info ) );
				pEventData->InsertEventData( pAni );
			}
			break;
		case CEventData::ET_Move:
			{
				_lseeki64( nHandle, -nsize, SEEK_CUR );
				CEventData::sMovePos_Info* pMove = NiNew CEventData::sMovePos_Info;
				_read( nHandle, pMove, sizeof( CEventData::sMovePos_Info ) );
				pEventData->InsertEventData( pMove );
			}
			break;
		case CEventData::ET_Talk:
			{
				_lseeki64( nHandle, -nsize, SEEK_CUR );
				CEventData::sEventTalk_Info* pTalk = NiNew CEventData::sEventTalk_Info;
				_read( nHandle, pTalk, sizeof( CEventData::sEventTalk_Info ) );
				pEventData->InsertEventData( pTalk );
			}
			break;
		case CEventData::ET_Sound:
			{
				_lseeki64( nHandle, -nsize, SEEK_CUR );
				CEventData::sEventSound_Info* pSound = NiNew CEventData::sEventSound_Info;
				_read( nHandle, pSound, sizeof( CEventData::sEventSound_Info ) );
				pEventData->InsertEventData( pSound );
			}
			break;
		case CEventData::ET_Effect:
			{
				_lseeki64( nHandle, -nsize, SEEK_CUR );
				CEventData::sEventEffect_Info* pEffect = NiNew CEventData::sEventEffect_Info;
				_read( nHandle, pEffect, sizeof( CEventData::sEventEffect_Info ) );
				pEventData->InsertEventData( pEffect );
			}
			break;
		default:
			assert_cs( false );
			break;
		}
		SAFE_NIDELETE( pData );			
	}	
	m_mapEventSceneData[ SceneNum ]->InsertData( pEventData );
}


void CEventDataMng::LoadFile()
{	
	//파일 로드	
	if( _taccess_s( EVENTDATA_FILENAME, 0 ) != 0 )
	{
		//CsMessageBox( MB_OK, _T( "EventData.dat파일 없음" ) );
	}		
	else
	{
		FILE* fp = NULL;

		_tfopen_s( &fp, EVENTDATA_FILENAME, _T( "rb" ) );
		assert_cs( fp != NULL );

		int nCount;
		fread( &nCount, sizeof( int ), 1, fp );	

		for( int i = 0; i < nCount; i++ )
		{
			int nNum;
			fread( &nNum, sizeof( int ), 1, fp );	
			
			UINT SceneNum;			
			fread( &SceneNum , sizeof( UINT ), 1, fp );		
			assert_cs( m_mapEventSceneData.find( SceneNum ) == m_mapEventSceneData.end() );

			CEventSceneData* pSceneData = NiNew CEventSceneData;
			m_mapEventSceneData[ SceneNum ] = pSceneData;		

			CEventSceneData::sEvent_Condition	Info;
			fread( &Info , sizeof( CEventSceneData::sEvent_Condition ), 1, fp );					
			pSceneData->Init( SceneNum, &Info );

			for( int i = 0; i < nNum; i++ )
			{												
				LoadEvent( fp, SceneNum );				
			}	
		}

		fclose( fp );
	}

}

void CEventDataMng::LoadEvent( FILE* fp, UINT SceneNum )
{
	int nNum;
	fread( &nNum, sizeof( int ), 1, fp );	

	CEventData* pEventData = NiNew CEventData;

	CEventData::sEvent_Info eInfo;
	CEventData::sEventObject_Info oInfo;

	fread( &eInfo , sizeof( CEventData::sEvent_Info ), 1, fp );
	fread( &oInfo , sizeof( CEventData::sEventObject_Info ), 1, fp );	

	pEventData->Init( &eInfo, &oInfo );		

	for( int j = 0 ; j < nNum; j++ )			
	{
		CEventData::sBase_Info* pData = NiNew CEventData::sBase_Info;
		fread( pData, sizeof( CEventData::sBase_Info ), 1, fp );			
		int nsize = sizeof( CEventData::sBase_Info );

		switch( pData->GetType() )
		{
		case CEventData::ET_Ani:
			{					
				fseek( fp, -nsize, SEEK_CUR );
				CEventData::sAnimation_Info* pAni = NiNew CEventData::sAnimation_Info;
				fread( pAni, sizeof( CEventData::sAnimation_Info ), 1, fp );
				pEventData->InsertEventData( pAni );
			}
			break;
		case CEventData::ET_Move:
			{
				fseek( fp, -nsize, SEEK_CUR );
				CEventData::sMovePos_Info* pMove = NiNew CEventData::sMovePos_Info;
				fread( pMove, sizeof( CEventData::sMovePos_Info ), 1, fp );
				pEventData->InsertEventData( pMove );
			}
			break;
		case CEventData::ET_Talk:
			{
				fseek( fp, -nsize, SEEK_CUR );
				CEventData::sEventTalk_Info* pTalk = NiNew CEventData::sEventTalk_Info;
				fread( pTalk, sizeof( CEventData::sEventTalk_Info ), 1, fp );
				pEventData->InsertEventData( pTalk );
			}
			break;
		case CEventData::ET_Sound:
			{
				fseek( fp, -nsize, SEEK_CUR );
				CEventData::sEventSound_Info* pSound = NiNew CEventData::sEventSound_Info;
				fread( pSound, sizeof( CEventData::sEventSound_Info ), 1, fp );
				pEventData->InsertEventData( pSound );
			}
			break;
		case CEventData::ET_Effect:
			{
				fseek( fp, -nsize, SEEK_CUR );
				CEventData::sEventEffect_Info* pEffect = NiNew CEventData::sEventEffect_Info;
				fread( pEffect, sizeof( CEventData::sEventEffect_Info ), 1, fp );
				pEventData->InsertEventData( pEffect );
			}
			break;
		default:
			assert_cs( false );
			break;
		}
		SAFE_NIDELETE( pData );			
	}	
	m_mapEventSceneData[ SceneNum ]->InsertData( pEventData );
}

void CEventDataMng::SaveFile()
{
	// 이미 파일이 존재 한다면 백업 시키다
	if( _taccess_s( EVENTDATA_FILENAME, 0 ) == 0 )
	{
		if( CsMessageBox( MB_YESNO, _T( "이전파일을 백업할까요?" ) ) == IDYES )
		{
			// 폴더가 존재 하지 않는다면 폴더 생성
			nsCSFILE::CreateDirectory( _T( "ToolData\\EventData" ) );

			TCHAR szNewFile[ MAX_PATH ];
			SYSTEMTIME st;
			GetLocalTime( &st );

			_stprintf_s( szNewFile, MAX_PATH, _T( "ToolData\\EventData\\EventData_%.2d%.2d%.2d_%.2d%.2d%.2d.dat" ), st.wYear%100, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond );
			// 이미 파일이 존재 한다면
			if( _taccess_s( szNewFile, 0 ) == 0 )
				DeleteFile( szNewFile );
			_trename( EVENTDATA_FILENAME, szNewFile );
		}		
	}

	if( m_mapEventSceneData.empty() )
	{
		CsMessageBox( MB_OK, _T( "저장할게 없네요." ) );
		return;			
	}	

	FILE* fp = NULL;
	_tfopen_s( &fp, EVENTDATA_FILENAME, _T( "wb" ) );

	//전체 이벤트씬의수
	int nCount = (int)m_mapEventSceneData.size();
	fwrite( &nCount, sizeof( int ), 1, fp );	

	std::map< UINT, CEventSceneData* >::iterator it = m_mapEventSceneData.begin();
	std::map< UINT, CEventSceneData* >::iterator itEnd = m_mapEventSceneData.end();
	for( ; it != itEnd; it++ )
	{
		// 이벤트의 수
		int nSize = it->second->GetEventSize();
		fwrite( &nSize, sizeof( int ), 1, fp );

		UINT SceneNum = it->second->GetSceneNum();
		assert_cs( SceneNum > 0 && SceneNum < 161 );
		fwrite( &SceneNum, sizeof( UINT ), 1, fp );
		fwrite( it->second->GetCondition(), sizeof( CEventSceneData::sEvent_Condition ), 1, fp );		
		
		for( int i = 0 ; i < nSize; i++ )
		{							
			SaveEvent( fp, it->second->GetEventDatabyIdx( i ) );
		}	
	}	

	fclose( fp );

	CsMessageBox( MB_OK, _T( "저장완료." ) );
}

void CEventDataMng::SaveEvent( FILE* fp, CEventData* pData )
{	
	//오프젝트 명령이벤트 수
	int nNum = (int)pData->GetEventSize();
	fwrite( &nNum, sizeof( int ), 1, fp );

	fwrite( pData->GetEventInfo(), sizeof( CEventData::sEvent_Info ), 1, fp );		
	fwrite( pData->GetObjectInfo(), sizeof( CEventData::sEventObject_Info ), 1, fp );	

	for( int i = 0; i < pData->GetEventSize(); i++ )
	{
		switch( pData->GetEventData( i )->GetType() )
		{
		case CEventData::ET_Ani:
			fwrite( pData->GetEventData( i ), sizeof( CEventData::sAnimation_Info ), 1, fp );
			break;
		case CEventData::ET_Move:
			fwrite( pData->GetEventData( i ), sizeof( CEventData::sMovePos_Info ), 1, fp );
			break;
		case CEventData::ET_Talk:
			fwrite( pData->GetEventData( i ), sizeof( CEventData::sEventTalk_Info ), 1, fp );
			break;
		case CEventData::ET_Sound:
			fwrite( pData->GetEventData( i ), sizeof( CEventData::sEventSound_Info ), 1, fp );
			break;
		case CEventData::ET_Effect:
			fwrite( pData->GetEventData( i ), sizeof( CEventData::sEventEffect_Info ), 1, fp );
			break;
		default:
			assert_cs( false );
		}					
	}		

}


bool CEventDataMng::InsertData( UINT nSceneNum, CEventSceneData::sEvent_Condition* pCon, std::stack< CEventData* >* pStackEventData )
{
	if( nSceneNum <= 0 || nSceneNum > 160 ) 
	{
		CsMessageBox( MB_OK, _T( "1~160 사이의 이벤트번호만 사용해주세요." ) );
		return false;
	}

	if( pStackEventData->size() < 1 )
	{
		CsMessageBox( MB_OK, _T( "저장할 데이터가 없네요.." ) );
		return false;
	}
	
	if( m_mapEventSceneData.find( nSceneNum ) != m_mapEventSceneData.end() )
	{
		TCHAR szTemp[ 128 ];

		_stprintf_s( szTemp, 128, _T( "(씬번호 - %d)의 이벤트씬이 존재합니다.\n무시하고 저장할까요?" ), 
			nSceneNum );
		
		if( CsMessageBox( MB_YESNO, szTemp ) == IDNO )
			return false;
		else
		{
			DeleteData( nSceneNum );
		}
	}
	

	CEventSceneData* pEventScene = NiNew CEventSceneData;
	pEventScene->Init( nSceneNum, pCon );

	//std::map< UINT, CEventData* >::iterator it = pmapEventData->begin();
	//std::map< UINT, CEventData* >::iterator itEnd = pmapEventData->end();
	//for( ; it != itEnd; it++ )

	while( pStackEventData->size() != 0 )
	{
		CEventData* pEventData = pStackEventData->top();
		pStackEventData->pop();

		CEventData* pData = NiNew CEventData;
		pData->Init( pEventData->GetEventInfo(), pEventData->GetObjectInfo() );

		for( int i = 0; i < pEventData->GetEventSize(); i++ )
		{
			assert_cs( pEventData->GetEventData( i ) != NULL );

			if( i + 1 < pEventData->GetEventSize() )
				assert_cs( pEventData->GetEventData( i )->GetEventStartTime() <= pEventData->GetEventData( i+1 )->GetEventStartTime() );

			switch( pEventData->GetEventData( i )->GetType() )	
			{
			case CEventData::ET_Ani:
				{	
					CEventData::sAnimation_Info* pAni = NiNew CEventData::sAnimation_Info;
					CEventData::sAnimation_Info* pEvent = (CEventData::sAnimation_Info*)pEventData->GetEventData( i );				
					memcpy( pAni, pEvent, sizeof( CEventData::sAnimation_Info ) );					
					pData->InsertEventData( pAni );
				}
				break;
			case CEventData::ET_Move:
				{
					CEventData::sMovePos_Info* pMove = NiNew CEventData::sMovePos_Info;
					CEventData::sMovePos_Info* pEvent = (CEventData::sMovePos_Info*)pEventData->GetEventData( i );				
					memcpy( pMove, pEvent, sizeof( CEventData::sMovePos_Info ) );
					pData->InsertEventData( pMove );
				}
				break;
			case CEventData::ET_Talk:
				{
					CEventData::sEventTalk_Info* pTalk = NiNew CEventData::sEventTalk_Info;
					CEventData::sEventTalk_Info* pEvent = (CEventData::sEventTalk_Info*)pEventData->GetEventData( i );				
					memcpy( pTalk, pEvent, sizeof( CEventData::sEventTalk_Info ) );
					pData->InsertEventData( pTalk );
				}
				break;
			case CEventData::ET_Sound:
				{
					CEventData::sEventSound_Info* pSound = NiNew CEventData::sEventSound_Info;
					CEventData::sEventSound_Info* pEvent = (CEventData::sEventSound_Info*)pEventData->GetEventData( i );				
					memcpy( pSound, pEvent, sizeof( CEventData::sEventSound_Info ) );
					pData->InsertEventData( pSound );
				}
				break;
			case CEventData::ET_Effect:
				{
					CEventData::sEventEffect_Info* pEffect = NiNew CEventData::sEventEffect_Info;
					CEventData::sEventEffect_Info* pEvent = (CEventData::sEventEffect_Info*)pEventData->GetEventData( i );				
					memcpy( pEffect, pEvent, sizeof( CEventData::sEventEffect_Info ) );
					pData->InsertEventData( pEffect );
				}
				break;
			default:
				assert_cs( false );
				break;
			}		
		}		

		pEventScene->InsertData( pData );	
	}	
	m_mapEventSceneData[ nSceneNum ] = pEventScene;
	
	return true;
}




