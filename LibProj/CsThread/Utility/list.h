/********************************************************************
	created:	2006/03/31
	created:	31:3:2006   11:14
	filename: 	d:\work\Kuf-cod\Network\Lsp\Iocp\Utility\list.h
	file path:	d:\work\Kuf-cod\Network\Lsp\Iocp\Utility
	file base:	list
	file ext:	h
	author:		celes
	
	Next와 Prev를 가지고 있는 Node의 List.
	erase가 일어났을때 Node를 지우지 않고 FreeNode List에 넣는다.
	insert 시도를 하면 FreeNode List에 Node가 있는지를 보고 사용하며
	없을때만 new가 발생한다.
*********************************************************************/

#pragma once

template <typename T>
class CList
{
public:
	CList();
	virtual ~CList();

	struct Node
	{
		Node* prev;
		Node* next;
		T value;
	};

	DWORD GetCount(){ return m_dwCount; }
	BOOL push_front( const T& value );
	BOOL pop_front( T& value );
	BOOL push_back( const T& value );
	BOOL pop_back( T& value );
	
	BOOL get_first_node( T& value );
	BOOL get_next_node( T& value );
	
protected:
	Node* AllocateNode( const T& value );
	void FreeNode( Node* pNode );

protected:
	DWORD m_dwCount;
	Node m_cnMaster;
	Node m_cnFree;
	Node* m_pCurNode;
};

template <typename T>
CList<T>::CList()
{
	m_cnMaster.next = m_cnMaster.prev = &m_cnMaster;
	m_cnFree.next = m_cnFree.prev = &m_cnFree;
	m_dwCount = 0;
}

template <typename T>
CList<T>::~CList()
{
	Node* pNode = m_cnMaster.next;
	while( pNode != &m_cnMaster )
	{
		Node* pDeleteNode = pNode;
		pNode = pNode->next;
		delete pDeleteNode;
	}

	pNode = m_cnFree.next;
	while( pNode != &m_cnFree )
	{
		Node* pDeleteNode = pNode;
		pNode = pNode->next;
		delete pDeleteNode;
	}
}

template <typename T>
BOOL CList<T>::push_front( const T& value )
{
	Node* pNewNode = AllocateNode( value );
	if( !pNewNode )
		return FALSE;

	pNewNode->prev = &m_cnMaster;
	pNewNode->next = m_cnMaster.next;
	m_cnMaster.next->prev = pNewNode;
	m_cnMaster.next = pNewNode;

	m_dwCount++;
	return TRUE;
}

template <typename T>
BOOL CList<T>::pop_front( T& value )
{
	if( &m_cnMaster == m_cnMaster.next )
		return FALSE;

	Node* pPopNode = m_cnMaster.next;
	m_cnMaster.next = pPopNode->next;
	pPopNode->next->prev = &m_cnMaster;
	value = pPopNode->value;
	FreeNode( pPopNode );

	m_dwCount--;
	return TRUE;
}

template <typename T>
BOOL CList<T>::push_back( const T& value )
{
	Node* pNewNode = AllocateNode( value );
	if( !pNewNode )
		return FALSE;

	pNewNode->prev = m_cnMaster.prev;
	pNewNode->next = &m_cnMaster;
	m_cnMaster.prev->next = pNewNode;
	m_cnMaster.prev = pNewNode;

	m_dwCount++;
	return TRUE;
}

template <typename T>
BOOL CList<T>::pop_back( T& value )
{
	if( &m_cnMaster == m_cnMaster.prev )
		return FALSE;

	Node* pPopNode = m_cnMaster.prev;
	m_cnMaster.prev = pPopNode->prev;
	pPopNode->prev->next = &m_cnMaster;
	value = pPopNode->value;
	FreeNode( pPopNode );

	m_dwCount--;
	return TRUE;
}
template <typename T>
BOOL CList<T>::get_first_node( T& value )
{
	m_pCurNode = m_cnMaster.next;
	if( m_pCurNode == &m_cnMaster )
		return FALSE;

	value = m_pCurNode->value;
	m_pCurNode = m_pCurNode->next;

	return TRUE;
}

template <typename T>
BOOL CList<T>::get_next_node( T& value )
{
	if( m_pCurNode == &m_cnMaster )
		return FALSE;

	value = m_pCurNode->value;
	m_pCurNode = m_pCurNode->next;

	return TRUE;
}

template <typename T>
typename CList<T>::Node* CList<T>::AllocateNode( const T& value )
{
	Node* pNewNode = m_cnFree.next;
	if( pNewNode != &m_cnFree )
	{
		m_cnFree.next = pNewNode->next;
		pNewNode->next->prev = &m_cnFree;
	}
	else{
		pNewNode = new Node;
	}

	pNewNode->value = value;
	pNewNode->next = NULL;
	pNewNode->prev = NULL;

	return pNewNode;
}

template <typename T>
void CList<T>::FreeNode( Node* pNode )
{
	pNode->next = &m_cnFree;
	pNode->prev = m_cnFree.prev;
	m_cnFree.prev->next = pNode;
	m_cnFree.prev = pNode;
}