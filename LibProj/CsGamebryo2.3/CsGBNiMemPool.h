


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

#pragma once 


//*************************************************************************
//
// 선언
//
//*************************************************************************
template < typename T >
class CsGBMemPool : public NiMemObject
{
public:
	CsGBMemPool( int nTCount );	
	~CsGBMemPool(){ _ReleaseAll(); }

protected:

	int					m_nTCount;			// 버퍼의 갯수
	std::stack< T* >	m_FreeStack;		// 프리 메모리 스택
	std::stack< T* >	m_DelStack;			// 메모리 제거를 위한 스택


public:

	bool				Init( int nTCount );
	void				Push( T* pNode );


	T*					Pop();

private:

	void				_ReleaseAll();
	void				_Addallot();
};

#define CSGBMEMPOOL_H( classname )		\
	public:\
		static CsGBMemPool< classname >*		m_pMemPool;\
		static void			DeleteInstance( ##classname* pInst ){ m_pMemPool->Push( pInst ); }\
		static classname*	NewInstance(){ return m_pMemPool->Pop(); }



#define CSGBMEMPOOL_CPP( classname )		CsGBMemPool< classname >*	classname::m_pMemPool = NULL

#define CSGBMEMPOOL_INIT( classname, count )	\
	assert_cs( classname::m_pMemPool == NULL );\
	classname::m_pMemPool = NiNew CsGBMemPool< classname >( count )

#define CSGBMEMPOOL_DELETE( classname )			SAFE_NIDELETE( classname::m_pMemPool )



//*************************************************************************
//
// 정의
//
//*************************************************************************

//========================================================================
// 생성자
//========================================================================
template < typename T >
CsGBMemPool<T>::CsGBMemPool( int nTCount )
{
	m_nTCount	=	nTCount;
	_Addallot();
}
//========================================================================
// 모든 노드 제거
//========================================================================
template < typename T >
void CsGBMemPool<T>::_ReleaseAll()
{
	// 제거 스택을 배열로 제거
	T* pNode;
	while( !m_DelStack.empty() )
	{
		pNode = m_DelStack.top();
		m_DelStack.pop();
		NiDelete[] pNode;
	}
}

//========================================================================
// 인자 없는 생성자 호출시 초기화 
//========================================================================
template < typename T >
bool CsGBMemPool<T>::Init( int nTCount )
{
	// 이미 노드를 가지고 잇다면 리턴
	if( m_nTCount )
		return false;

	m_nTCount	=	nTCount;
	_Addallot();
	return true;
}


//========================================================================
// 모든 프리노드 소모..  추가 할당
//========================================================================
template < typename T >
void CsGBMemPool<T>::_Addallot()
{
	// 노드들을 스택에 집어 넣는다
	T* pNode = NiNew T[ m_nTCount ];
	for( int i=0; i<m_nTCount; ++i )
		m_FreeStack.push( &pNode[i] );
	// 가장 첫 포인터를 제거 스택에 집어 넣는다	
	m_DelStack.push( pNode );

	m_nTCount += m_nTCount;
}

//========================================================================
// 프리노드 꺼내온다
//========================================================================
template < typename T >
T* CsGBMemPool<T>::Pop()
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
void CsGBMemPool<T>::Push( T* pNode )
{
	m_FreeStack.push( pNode );
}


