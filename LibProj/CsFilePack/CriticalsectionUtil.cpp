#include "StdAfx.h"
#include "CriticalsectionUtil.h"

//---------------------------------------------------------------------------------
CriticalsectionUtil::CriticalsectionUtil(int p_iSpinCount)
{
//#if (_WIN32_WINNT >= 0x0403)
	if ( FALSE == InitializeCriticalSectionAndSpinCount( &m_CS, (DWORD)p_iSpinCount ) )
		InitializeCriticalSection( &m_CS );
// #else
// 	InitializeCriticalSection( &m_CS );
// #endif
	m_iSpinCount = p_iSpinCount;
}
//---------------------------------------------------------------------------------
CriticalsectionUtil::~CriticalsectionUtil(void)
{
	DeleteCriticalSection(&m_CS);
}
//---------------------------------------------------------------------------------
BOOL CriticalsectionUtil::TryEnter()
{
//#if (_WIN32_WINNT >= 0x0400)
	return TryEnterCriticalSection(&m_CS );
// #else
// 	Enter();
// 	return TRUE;
// #endif

}
//---------------------------------------------------------------------------------
void CriticalsectionUtil::Enter()
{
	EnterCriticalSection(&m_CS );
}
//---------------------------------------------------------------------------------
void CriticalsectionUtil::Leave()
{
	LeaveCriticalSection(&m_CS);	
}
//---------------------------------------------------------------------------------
//---------------------------------------------------------------------------------
//---------------------------------------------------------------------------------
//---------------------------------------------------------------------------------
//---------------------------------------------------------------------------------

CSSpinLock::CSSpinLock( int p_iSpinCount ):CriticalsectionUtil(p_iSpinCount)
{
}
//---------------------------------------------------------------------------------
CSSpinLock::~CSSpinLock()
{
}
//---------------------------------------------------------------------------------
void CSSpinLock::Enter()
{	
	long	tryCount = 0;
	long	errorCount = 0;
	if( GetSpinCount() > 0 )
	{
		while ( TryEnter() == FALSE )
		{
			++tryCount;
			Sleep(1);

			if ( tryCount > GetSpinCount() )
			{
				tryCount = 0;
				errorCount++;

				if ( errorCount == 1 ) 
				{
					// 에러 발생시 다른 스레드에 CPU TIme 을 주고 다시 해보기
					Sleep(1);

					//ECLOG("TryEnter Fail %d times \n",tryCount);
				}
				else 
					Sleep(1); // 낮은 우선순위 스레드에는 CPU TIme 을 주지 않아!

				if ( errorCount == GetSpinCount() )
				{
					// SpinCount 이상 에러 발생시 EnterCriticalsection 해보기
					errorCount = 0;
					CriticalsectionUtil::Enter();
					break;
				}
			}
		}
	}
	else
	{
		CriticalsectionUtil::Enter();
	}
}

//---------------------------------------------------------------------------------
//---------------------------------------------------------------------------------
//---------------------------------------------------------------------------------
//---------------------------------------------------------------------------------
//---------------------------------------------------------------------------------

CSAutoLocker::CSAutoLocker( CriticalsectionUtil &p_refCsUtil ): m_bLock(FALSE)
{
	m_pCsUtil = &p_refCsUtil;

	Enter();
}
//---------------------------------------------------------------------------------
CSAutoLocker::CSAutoLocker( CriticalsectionUtil *p_pCsUtil ): m_bLock(FALSE)
{
	m_pCsUtil = p_pCsUtil;

	Enter();
}
//---------------------------------------------------------------------------------
CSAutoLocker::~CSAutoLocker()
{
	Leave();
}
//---------------------------------------------------------------------------------
void CSAutoLocker::Enter()
{
	if( !m_pCsUtil )
	{
		//ECERROR("m_pCsUtil is Null \n");
		return ;
	}

	long	tryCount = 0;
	long	errorCount = 0;

	if( m_pCsUtil->GetSpinCount() > 0 )
	{
		while ( m_pCsUtil->TryEnter() == FALSE )
		{
			++tryCount;
			Sleep(1);

			if ( tryCount > m_pCsUtil->GetSpinCount() )
			{
				tryCount = 0;
				errorCount++;

				if ( errorCount == 1 ) 
				{
					// 에러 발생시 다른 스레드에 CPU TIme 을 주고 다시 해보기
					Sleep(1);

					//ECLOG("TryEnter Fail %d times \n",tryCount);
				}
				else 
					Sleep(1); // 낮은 우선순위 스레드에는 CPU TIme 을 주지 않아!

				if ( errorCount == m_pCsUtil->GetSpinCount() )
				{
					// SpinCount 이상 에러 발생시 EnterCriticalsection 해보기
					errorCount = 0;
					m_pCsUtil->Enter();
					break;
				}
			}
		}
	}
	else
	{
		m_pCsUtil->Enter();
	}
}
//---------------------------------------------------------------------------------
void CSAutoLocker::Leave()
{
	if( !m_pCsUtil )
	{
		//ECERROR("m_pCsUtil is Null\n");
		return ;
	}

	m_pCsUtil->Leave();
}