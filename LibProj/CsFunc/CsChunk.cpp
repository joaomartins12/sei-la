#include "stdafx.h"
#include "CsChunk.h"

CsChunk::CsChunk(CsChunk* pTail)
{
	if(pTail)
	{
		pTail->SetNext(this);
	}

	m_pPrev			=	pTail;
	m_pNext			=	NULL;

	m_CurFp			=	0;
	m_lSize			=	CHUNK_BUF_SIZE;
	m_pData			=	csnew BYTE[m_lSize];
	memset(m_pData, 0, CHUNK_BUF_SIZE);
}

CsChunk::~CsChunk()
{
	if(m_pPrev) m_pPrev->SetNext(m_pNext);
	if(m_pNext)	m_pNext->SetPrev(m_pPrev);
	SAFE_DELETE_ARRAY(m_pData);
}

BYTE* CsChunk::Write(BYTE* pData, long size)
{
	assert_cs( size > 0 );

	while( m_CurFp+size > m_lSize )
	{
		_ResizeBuf();
	}
	memcpy(&m_pData[m_CurFp], pData, size);

	m_CurFp	+=	size;
	return &m_pData[m_CurFp];
}

void CsChunk::_ResizeBuf()
{
	BYTE* pData	=	csnew BYTE[m_lSize + CHUNK_ADD_SIZE];
	memset(pData, 0 , m_lSize+CHUNK_ADD_SIZE);
	memcpy(pData, m_pData, m_lSize);
	delete[](m_pData);
	m_pData	=	pData;
	m_lSize	+=	CHUNK_ADD_SIZE;
}

CsChunk* CsChunk::GetHead()
{
	CsChunk* pTemp = this;
	while (pTemp->m_pPrev) 
	{
		pTemp = pTemp->m_pPrev;
	}
	return pTemp;
}

CsChunk* CsChunk::GetTail()
{
	CsChunk* pTemp = this;
	while (pTemp->m_pNext) 
	{
		pTemp = pTemp->m_pNext;
	}
	return pTemp;
}

CsChunk* CsChunk::FindIndex(int index)
{
	assert_cs( index >= 0 );

	CsChunk* pTemp = GetHead();
	while (index) 
	{
		pTemp = pTemp->m_pNext;
		--index;
	}
	return pTemp;
}

int CsChunk::GetNodeCount()
{
	int index = 0;
	CsChunk* pTemp = GetHead();
	while(pTemp)
	{
		++index;
		pTemp = pTemp->m_pNext;		
	}
	return index;
}

long CsChunk::GetTotalSize()
{
	long TotalSize = 0;
	CsChunk* pTemp = GetHead();
	while (pTemp) 
	{
		TotalSize	+=	pTemp->CurSize();
		pTemp		=	pTemp->m_pNext;		
	}
	return TotalSize;
}

void CsChunk::DeleteChild()
{
	if(m_pNext)
	{
		m_pNext->DeleteChild();
		SAFE_DELETE(m_pNext);
	}
}

