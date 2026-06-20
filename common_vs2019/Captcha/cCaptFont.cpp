#include <stdio.h>
#include <memory.h>
#include <stdlib.h>
#include "cCaptFont.h"


//-------------------------------------------------------------------------------------------------
cFontFile::cFontFile()
	: m_pGlyphBuf(0)
	, m_pCodeBuf(0)
{
	memset(&m_fileHead, 0, sizeof(Head));
}

//-------------------------------------------------------------------------------------------------
cFontFile::~cFontFile()
{
	Release();
}

//--------------------------------------------------------------------------
void cFontFile::Release(void)
{
	if(m_pCodeBuf)
	{
		delete[] m_pCodeBuf;
		m_pCodeBuf = 0;
	}
	if(m_pGlyphBuf)
	{
		delete[] m_pGlyphBuf;
		m_pGlyphBuf = 0;
	}

	memset(&m_fileHead, 0, sizeof(Head));
}

//--------------------------------------------------------------------------
void cFontFile::_pointRuntime(void)
{
	for(unsigned int i=0; i<m_fileHead.nCodeCounts; i++)
	{
		Code& code = m_pCodeBuf[i];
		if(code.wCode==0 || code.nGlyphCounts==0) continue;  //예외처리

		CodeGlyph* codeGlyph = (CodeGlyph*)(m_pGlyphBuf+code.nFirstOffset);
		while(codeGlyph->nNextOffset !=0 )
		{
			codeGlyph->pNext = (CodeGlyph*)(m_pGlyphBuf + codeGlyph->nNextOffset);
			codeGlyph = codeGlyph->pNext;
		}

		code.pFirst = (CodeGlyph*)(m_pGlyphBuf+code.nFirstOffset);
	}
}

//--------------------------------------------------------------------------
bool cFontFile::_readStrem(unsigned char* dest, unsigned int size, const unsigned char*& pStream, const unsigned char* pStreamEnd)
{
	if((unsigned int)(pStreamEnd-pStream)<size) return false;
	memcpy(dest, pStream, size);
	pStream += size;
	return true;
}

//--------------------------------------------------------------------------
bool cFontFile::LoadFromDataStream(const unsigned char* pStream, unsigned int streamSize)
{
	if(pStream==0 || streamSize<=sizeof(Head)) return false;	//크기 예외처리

	//열려있을것을 대비해서 릴리즈하는데 실제로는 필요없을듯
	Release();

	const unsigned char* p = pStream;
	const unsigned char* pStreamEnd = pStream+streamSize;
	//시작과 끝을 구한다
	
	if(!_readStrem((unsigned char*)&m_fileHead, sizeof(m_fileHead), p, pStreamEnd))		//헤더만큼읽어온다 , 사실상 fread임
	{
		return false;
	}
	if(m_fileHead.dwMagicCode != MAGIC_CODE)	//헤더가맞는지 검사한다 , 이거도 제외
	{
		return false;
	}
	
	int codeCounts = GetCodeCounts();		//코드 갯수,
	m_pCodeBuf = new Code[codeCounts];		//코드갯수만큼 만듬 , 이게 코드마다 이미지 정보인듯
	if(!_readStrem((unsigned char*)m_pCodeBuf, sizeof(Code)*codeCounts, p, pStreamEnd))
	{
		return false;
	}
	
	m_pGlyphBuf = new unsigned char[m_fileHead.nGlyphSize];
	if(!_readStrem(m_pGlyphBuf, m_fileHead.nGlyphSize, p, pStreamEnd))
	{
		return false;
	}

	_pointRuntime();

	return true;
}

//--------------------------------------------------------------------------
unsigned short cFontFile::GetCodeFromIndex(unsigned int index) const
{
	if(index<0 || index>=GetCodeCounts()) return 0;
	return m_pCodeBuf[index].wCode;
}

//--------------------------------------------------------------------------
char* cFontFile::GetCodeGlyphFromIndex(unsigned int index) const
{
	if(index<0 || index>=GetCodeCounts()) return 0;

	const Code& codeInfo = m_pCodeBuf[index];
	if(codeInfo.nGlyphCounts <= 0) return 0;
	int glyphIndex=0;
	if(codeInfo.nGlyphCounts > 1)
	{
		glyphIndex = rand()%(codeInfo.nGlyphCounts);
	}

	CodeGlyph* glyph = codeInfo.pFirst;
	for(int i=0; i<glyphIndex; i++) glyph=glyph->pNext;

	return (char*)glyph->data;
}

