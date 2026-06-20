
#ifndef NIBUFFER_READER_MEM_H
#define NIBUFFER_READER_MEM_H

#include "NiFile.h"

template<class TReader>
class NiBufferReader : public NiFile
{
public : 
	NiBufferReader() {}
	virtual ~NiBufferReader() { Clear(); }

public :
	// 지정한 ReaderClass와 파일 형식이 다르면 NULL를 리턴한다.
	NiPixelData* ReadBuffer(void* pvBuffer, unsigned int uiBytes)  
	{
		if(!pvBuffer) return NULL;

		NiPixelData* pPixelData = NULL;

		SetBuffer(pvBuffer, uiBytes);

		TReader	Reader;
		pPixelData = Reader.ReadFile(*this, NULL);

		Clear();
		return pPixelData;
	}

private :
	unsigned int SetBuffer(void* pvBuffer, unsigned int uiBytes)
	{
		SetEndianSwap(false);

		m_eMode = NiFile::READ_ONLY;
		m_bGood = true;			// 상당히 중요한다 -_-;

		m_pBuffer = (char*)pvBuffer;
		m_uiBufferAllocSize = uiBytes;
		m_uiPos = m_uiBufferReadSize = 0;
		m_uiBufferReadSize = uiBytes;

		return 0;
	}
	void Clear()
	{
		m_pBuffer = NULL;
		m_uiBufferAllocSize = 0;
		m_uiPos = m_uiBufferReadSize = 0;
	}
};

#endif
