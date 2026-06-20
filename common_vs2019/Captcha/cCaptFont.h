#pragma once


class cFontFile
{
public:
	// 폰트파일
	enum { MAGIC_CODE = 0x46504143, };
	struct Head	
	{
		unsigned int dwMagicCode;		//'CAPF'
		unsigned int nCodeOffset;
		unsigned int nCodeCounts;	
		unsigned int nGlyphOffset;	
		unsigned int nGlyphCounts;	
		unsigned int nGlyphSize;	
	};

	struct CodeGlyph
	{
		union
		{
			unsigned long long	nNextOffset;	
			CodeGlyph*			pNext;			
		};
		char				data[1];
	};
		struct Code
	{
		unsigned short		wCode;			//!< 유니코드 표에서의 문자의 위치이다
		unsigned short		nGlyphCounts;	//!< 해당 문자열이 몇개의 폰트를 가지고있는지에대한 정보이다
		union
		{
			unsigned long long	nFirstOffset;	
			CodeGlyph*			pFirst;			
		};
	};	
	enum { GLPYH_EOL=-128, GLPYH_EOF=-127 };

public:
	void Release(void);
	bool LoadFromDataStream(const unsigned char* pStream, unsigned int streamSize);
	unsigned int GetCodeCounts(void) const { return m_fileHead.nCodeCounts; }
	unsigned short GetCodeFromIndex(unsigned int index) const;
	char* GetCodeGlyphFromIndex(unsigned int index) const;

protected:
	void _pointRuntime(void);

private:
	bool _readStrem(unsigned char* dest, unsigned int size, const unsigned char*& pStream, const unsigned char* pStreamEnd);

protected:
	Head m_fileHead;
	Code* m_pCodeBuf;
	unsigned char* m_pGlyphBuf;

public:
	cFontFile();
	virtual ~cFontFile();
};



