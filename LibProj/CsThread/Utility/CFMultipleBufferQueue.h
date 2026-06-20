/********************************************************************
	created:	2007/06/01
	created:	1:6:2007   16:25
	filename: 	d:\work\KUF-COD\Network\CFMTLib\Utility\CFDoubleBuffer
	file path:	d:\work\KUF-COD\Network\CFMTLib\Utility
	file base:	CFDoubleBuffer
	file ext:	h
	author:		celes
	
	purpose:	Thread간 통신에 사용되는 Queue. 두 개의 Queue를 Flip 시키면서 쓰기 때문에
				읽기/쓰기간에 race condition이 발생하지 않기 때문에 동기화 병목이 적다.
				Read Thread에서 Flip을 호출하면 Read Buffer과 Write Buffer가 교환됨.
*********************************************************************/

#pragma once

#include<vector>
#include"CFAutoLock.h"

using namespace std;

template <typename T>
class CFDoubleBuffer
{
public:
	enum{
		MAX_BUFFER_COUNT = 10,
		DEFAULT_BUFFER_SIZE = 500,
	};

	CFDoubleBuffer( size_t unBufferCount = 2 );
	virtual ~CFDoubleBuffer(){};

	void Flip( vector<T>& vItemList );
	void InsertItem( T& item );

protected:
	CFCriticalSection m_csBuffer;

	size_t m_unReadIndex;
	size_t m_unWriteIndex;

	vector<T> m_pBuffer[2];
};

template <typename T>
CFDoubleBuffer<T>::CFDoubleBuffer( size_t unBufferCount )
{
	m_unReadIndex = 0;
	m_unWriteIndex = 1;

	m_pBuffer[0].reserve( DEFAULT_BUFFER_SIZE );
	m_pBuffer[1].reserve( DEFAULT_BUFFER_SIZE );
}

template <typename T>
void CFDoubleBuffer<T>::Flip( vector<T>& vecItemList )
{
	vecItemList.assign( m_pBuffer[m_unReadIndex].begin(), m_pBuffer[m_unReadIndex].end() );
	m_pBuffer[m_unReadIndex].clear();

	_SYNCHRONIZE_THREAD_( &m_csBuffer );
	m_unReadIndex = m_unWriteIndex;
	m_unWriteIndex = 1 - m_unReadIndex;
}

template <typename T>
void CFDoubleBuffer<T>::InsertItem( T& item )
{
	_SYNCHRONIZE_THREAD_( &m_csBuffer );
	m_pBuffer[m_unWriteIndex].push_back( item );
}


