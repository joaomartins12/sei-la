
#include "stdafx.h"
#include "AutoUpgradeDlg.h"

// 쓰레드 상에서 호출됨
void CAutoUpgradeDlg::SetDownState( int nState, DWORD nFileSize )
{
	_SYNCHRONIZE_THREAD_( &m_CSTimer );

	KillTimer( ID_TIMER_URL_STATE );

	sURL_INFO* pInfo = new sURL_INFO;
	pInfo->s_nState = nState;
	pInfo->s_nFileSize = nFileSize;
	m_queueState.Put( pInfo );

	SetTimer( ID_TIMER_URL_STATE, ID_TIMER_URL_STATE_TICK, NULL );	
}

// 타이머 상에서 호출됨
void CAutoUpgradeDlg::ProcDownState()
{
	_SYNCHRONIZE_THREAD_( &m_CSTimer );

	KillTimer( ID_TIMER_URL_STATE );

	sURL_INFO* pInfo = NULL;
	while( m_queueState.Get( pInfo ) )
	{
		switch( pInfo->s_nState )
		{
		case US_VERSION_ERROR:
			{
				MessageBox( STR_FAIL_ONLINE, STR_FAIL, MB_OK );
				PostMessage( WM_QUIT );
			}break;
		case US_FILE_ERROR:
			{
				CThread_Data::sINFO info;
				g_pThread->Get()->GetInfo( &info );
				if( SetError( info.s_nVersion ) == true )
				{
					NextFile();
				}
				else
				{
					if( m_nServerVersion > m_nDownVersion  )
					{
						IncreaseDownCompFileCount();
						ResetError();
						NextFile();
						return;
					}
					else
					{
						DisplayError();
						PostMessage( WM_QUIT );
					}
				}
			}
			break;
		case US_VERSION_INFO:
			{
				SetState( eSTATE_VERSION_DOWN );
				Set_FileInfo( pInfo->s_nFileSize );
			}break;
		case US_VERSION_START:
			{
				SetTimer( ID_TIMER_DOWNLOAD, ID_TIMER_DOWNLOAD_TICK, NULL );
				Proc_MainState();
			}break;
		case US_VERSION_END:
			{
				assert( g_pThread->Get()->GetVersion() == URL_SUCCESS );
				KillTimer( ID_TIMER_DOWNLOAD );
				SetState( eSTATE_VERSION_COMP );
				if( VersionCheck() == true )
					NextFile();
			}break;
		case US_FILE_INFO:
			{
				Set_FileInfo( pInfo->s_nFileSize );
			}break;
		case US_FILE_START:
			{
				SetTimer( ID_TIMER_DOWNLOAD, ID_TIMER_DOWNLOAD_TICK, NULL );
			}break;
		case US_FILE_END:
			{
				assert( g_pThread->Get()->GetVersion() == URL_SUCCESS );
				KillTimer( ID_TIMER_DOWNLOAD );
				UnZip_File();
			}break;
		default:
			assert( false );
			break;
		}

		delete pInfo;
	}	
}