
#include "NiSystemPCH.h"
#include "NiFile_Mem.h"

NiMemFile::NiMemFile(void* pvBuffer, OpenMode eMode, unsigned int uiBytes)
: NiFile()
{
	m_pBuffer = (char*)pvBuffer;
	m_uiBufferReadSize = uiBytes;
	m_uiBufferAllocSize = uiBytes;
	m_uiSize = uiBytes;
	m_uiPos = 0;
	m_eMode = eMode;
	m_bGood = true;

	SetEndianSwap(false);
}

NiMemFile::~NiMemFile(void)
{

}

char* NiMemFile::GetBuffer()
{
	return m_pBuffer;
}

void NiMemFile::Seek(int iOffset, int iWhence)
{
	if(iWhence == NiFile::ms_iSeekSet)
	{
		m_uiPos = iOffset;
	}
	else if(iWhence == NiFile::ms_iSeekCur)
	{
		m_uiPos += iOffset;
	}
	else 
	{
		NIASSERT(!"NiMemFile::Seek has limited functionality");
	}
}

unsigned int NiMemFile::GetFileSize() const
{
	return m_uiSize;
}


