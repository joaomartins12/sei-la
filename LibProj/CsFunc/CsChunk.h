
#pragma once 

#define CHUNK_BUF_SIZE		1024
#define CHUNK_ADD_SIZE		1024

class CsChunk
{
public:
	CsChunk(CsChunk* pTail);
	~CsChunk();

protected:
	CsChunk*		m_pPrev;
	CsChunk*		m_pNext;

	int				m_lSize;
	int				m_CurFp;

	BYTE*			m_pData;

public:
	BYTE*			Write(BYTE* pData, long size);	

	CsChunk*		GetHead();
	CsChunk*		GetTail();
	CsChunk*		FindIndex(int index);
	int				GetNodeCount();
	long			GetTotalSize();
	void			DeleteChild();

	CsChunk*		Prev(){return m_pPrev;}
	CsChunk*		Next(){return m_pNext;}
	BYTE*			Buffer(){return m_pData;}
	void			SetPrev(CsChunk* pChunk){m_pPrev = pChunk;}
	void			SetNext(CsChunk* pChunk){m_pNext = pChunk;}
	int				GetBufSize(){return m_lSize;}
	int				CurSize(){return m_CurFp;}
protected:
	void			_ResizeBuf();
};