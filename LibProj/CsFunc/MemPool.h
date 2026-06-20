


//*************************************************************************
//
// 메모리 풀
//
//  - 반드시 지켜주어야 할 것 -
// 1. 메모리 제거를 사용자가 하여서는 안된다. ( 메모리 제거 하였을경우 소멸자 호출 될시 뻑 )
// 2. 소멸자는 반드시 호출 되어야 한다 ( molloc 으로 선언하여 주면 안된다. )
//		1, 2를 지켜주었을 시 Memory leak 현상은 발생하지 않는다. ( 내부적으로 모든 메모리 제거 )
// 3. 템플릿에 포인터를 넣어 주어서는 안된다. ( 메모리 할당될 객체 자체를 넣어 주어야만 한다 )
//
//	- 지켜주면 좋은 것 -
// 1. Pop 한것은 Push 해서 다시 채워 주는것이 좋다 ( Free 메모리가 부족할 때 마다 메모리 할당 하므로 )
// 2. 증가 갯수에 따라 플래그를 설정해주는 것이 좋다.
// 3. 로그 파일 확인후 초기 갯수를 설정하여 주자 ( 잦은 메모리 할당 또는 너무 넉넉한 할당은 피해 주자 )
//
//*************************************************************************


#ifndef CS_MEM_POOL
#define CS_MEM_POOL

// 메모리 추가 바이트량 계산 플래그
enum eMemPool_Reallot
{
	MPR_DOUBLE		=		0x00000001,			// m_TCount *= 2
	MPR_ADD			=		0x00000002,			// 현상유지
	MPR_PLUSONE		=		0x00000003,			// m_TCount += 1
};

//*************************************************************************
//
// 선언
//
//*************************************************************************
template < typename T >
class CsMemPool
{
public:
	CsMemPool();
	CsMemPool( int nTCount, DWORD plag = MPR_DOUBLE );	
	~CsMemPool(){ _ReleaseAll(); }

protected:

	int					m_nTCount;			// 버퍼의 갯수
	DWORD				m_dwPlag;			// 플래그
	std::stack< T* >	m_FreeStack;		// 프리 메모리 스택
	std::stack< T* >	m_DelStack;			// 메모리 제거를 위한 스택


public:

	bool				Init( int nTCount, DWORD plag = MPR_DOUBLE );
	void				Push( T* pNode );


	T*					Pop();

private:

	void				_ReleaseAll();
	void				_Addallot();
};

//*************************************************************************
//
// 정의
//
//*************************************************************************

//========================================================================
// 생성자
//========================================================================
template < typename T >
CsMemPool<T>::CsMemPool( int nTCount, DWORD plag /*=MPR_DOUBLE*/ )
{
	m_nTCount	=	nTCount;
	m_dwPlag	=	plag;

#ifdef CSMEM_LOG
	_LogPath();
	_LogWriteDate();
#endif CSMEM_LOG

	_Addallot();
}

//========================================================================
// 생성자
//========================================================================
template < typename T >
CsMemPool<T>::CsMemPool()
{
	m_nTCount	=	10;
	m_dwPlag	=	MPR_DOUBLE;
}

//========================================================================
// 모든 노드 제거
//========================================================================
template < typename T >
void CsMemPool<T>::_ReleaseAll()
{
	// 제거 스택을 배열로 제거
	T* pNode;
	while( !m_DelStack.empty() )
	{
		pNode = m_DelStack.top();
		m_DelStack.pop();
		delete[] pNode;
	}
}

//========================================================================
// 인자 없는 생성자 호출시 초기화 
//========================================================================
template < typename T >
bool CsMemPool<T>::Init( int nTCount, DWORD plag = MPR_DOUBLE )
{
	// 이미 노드를 가지고 잇다면 리턴
	if( m_nTCount )
		return false;

	m_nTCount	=	nTCount;
	m_dwPlag	=	plag;

#ifdef CSMEM_LOG
	_LogPath();
	_LogWriteDate();
#endif CSMEM_LOG

	_Addallot();
	return true;
}


//========================================================================
// 모든 프리노드 소모..  추가 할당
//========================================================================
template < typename T >
void CsMemPool<T>::_Addallot()
{
	// 노드들을 스택에 집어 넣는다
	T* pNode = csnew T[ m_nTCount ];
	for( int i=0; i<m_nTCount; ++i )
		m_FreeStack.push( &pNode[i] );
	// 가장 첫 포인터를 제거 스택에 집어 넣는다	
	m_DelStack.push( pNode );

	switch( m_dwPlag&0x000000ff )
	{
	case MPR_DOUBLE:		m_nTCount *= 2;		return;
	case MPR_ADD:								return;
	case MPR_PLUSONE:		++m_nTCount;		return;
	default:				assert_csm( false, _T( "플래그로 알수 없는 값이 들어 왔습니다." ) );
	}
}

//========================================================================
// 프리노드 꺼내온다
//========================================================================
template < typename T >
T* CsMemPool<T>::Pop()
{
	assert_csm( m_nTCount, _T( "초기화가 되어 있지 않습니다 ( m_TCount == 0 )" ) );
	if( m_FreeStack.empty() )
		_Addallot();

	T* pNode = m_FreeStack.top();
	m_FreeStack.pop();
	return pNode;
}


//========================================================================
// 프리노드 복귀
//========================================================================
template < typename T >
void CsMemPool<T>::Push( T* pNode )
{
	assert_cs( pNode != NULL );
	m_FreeStack.push( pNode );
}



#endif /*CS_MEM_POOL*/