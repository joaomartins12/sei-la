
#pragma once

//=====================================================================================================
//
//		생성자 소멸자 호출 안됨
//
//=====================================================================================================


template <typename T>
class CsVectorLink
{
public:
	struct iterator
	{
		iterator(){ Delete(); }
		void Delete(){ s_pPrev = s_pNext = NULL; s_nKey = 0; s_bHaveElement = false; }

		T				s_Element;
		iterator*		s_pPrev;		
		iterator*		s_pNext;
		unsigned short	s_nKey;
		bool			s_bHaveElement;
	};

protected:
	int			m_nAllocCount;
	int			m_nElementCount;
	DWORD		m_AllocPlag;

	iterator*	m_DataArray;
	iterator	m_DataBegin;
	iterator	m_DataEnd;

public:
	CsVectorLink();
	CsVectorLink( int nCount );
	~CsVectorLink();	

public:
	void		Destroy();
	iterator*	DeleteElement( int nIndex )
	{
		assert_cs( m_nElementCount > 0 );
		iterator* pData = &m_DataArray[ nIndex ];
		// 데이터가 존재 하는건지 확인
		assert_cs( pData->s_bHaveElement == true );

		iterator* itReturn = pData->s_pPrev;
		// 내 앞을 뒤와 연결
		pData->s_pPrev->s_pNext = pData->s_pNext;
		// 내 뒤를 앞과 연결
		pData->s_pNext->s_pPrev = pData->s_pPrev;

		// 요소 제거
		pData->Delete();

		--m_nElementCount;
		return itReturn;
	}
	void		Init( int nAllocCount );
	void		Reset();
	void		SetData( int nIndexArray, T Element );

	int			Size(){ return m_nElementCount; }
	int			GetAllocCount(){ return m_nAllocCount; }

	iterator*	Begin(){ return m_DataBegin.s_pNext; }
	iterator*	End(){ return &m_DataEnd; }
	iterator*	Get( int nIndexArray ){ assert_cs( nIndexArray < m_nAllocCount ); return &m_DataArray[ nIndexArray ]; }
	T			GetData( int nIndexArray ){ assert_cs( nIndexArray < m_nAllocCount ); return m_DataArray[ nIndexArray ].s_Element; }
	bool		IsElement( int nIndexArray ){ if( nIndexArray >= m_nAllocCount ){ return false; } return m_DataArray[ nIndexArray ].s_bHaveElement; }
	
public:
	T&			operator[](int nIndex){ assert_cs( nIndex < m_nAllocCount ); return (T&)m_DataArray[ nIndex ].s_Element; }	

	template <typename _Pro>
	static void Search(iterator const* itBe, iterator const* itEd, _Pro& pCmp)
	{
		for( ; itBe != itEd; itBe = itBe->s_pNext )
			pCmp( itBe->s_Element );
	}
};

#include "VectorLink.inl"