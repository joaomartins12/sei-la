

#pragma once

#define MAX_ANAL_LEN		512

// 스트링 분석기
class cStringAnalysis : public NiMemObject
{
public:
	static void		GlobalInit();
	static void		GlobalShotDown();

public:
	void			Init();
	void			Delete();


	//============================================================================================
	//		파싱
	//============================================================================================
protected:
	enum ePARCING_TYPE{
		PT_NONE,
		PT_TEXT_COLOR,
		PT_TEXT_BOLD,
	};
	struct sPARCING : public NiMemObject
	{
		CSGBMEMPOOL_H( cStringAnalysis::sPARCING );

		ePARCING_TYPE	s_eType;
		DWORD			s_Value;
	};

	NiColor					m_OrgColor;
	int						m_nOrgBoldLevel;
	cString*				m_pString;
	cStringList*			m_pStringList;

	std::list< sPARCING* >	m_listParcing;

	cText::sTEXTINFO*		m_pTextInfo;

	TCHAR					m_szToken[ MAX_ANAL_LEN ];
	int						m_nTokenIndex;
	int						m_nTotalWidth;

protected:
	void			_Parcing( TCHAR* str, int nStrLen, int& nIndex );
	int				_ParcingTypeStart( TCHAR* str, ePARCING_TYPE& pt );
	bool			_ParcingValueEnableCheck( int nStrLen, int nIndex, ePARCING_TYPE& pt );
	int				_ParcingValue( TCHAR* str, ePARCING_TYPE& pt );
	int				_ParcingTypeEnd( TCHAR* str );


	void		_RemoveParcing( ePARCING_TYPE pt );

	//============================================================================================
	//		문자열 자르기
	//============================================================================================
protected:
	void	_MakeToken_Parcing( bool bNewString );
	void	_MakeToken( bool bNewString );
public:	
	void	Cut_Parcing( cStringList* pStringList, int width, TCHAR* str, cText::sTEXTINFO* pTextInfo );	// 색깔 구분 해서 문자열 자르기
	void	Cut( cStringList* pStringList, int width, TCHAR* str, cText::sTEXTINFO* pTextInfo );			// 색깔 구분 해서 문자열 자르기
	//TCHAR*	StringTransfer( TCHAR* szOut, int nOutSize, TCHAR* szIn );										// 문자열 대체


	//TCHAR*		Quest_Parcing( TCHAR* szDest, const int nDestSize, TCHAR* szSrc, CsQuestCompRequireGroup* pApplyGroup );
	//void		Equip_EItemComment_Parcing( cStringList* pList, cString* pBeginString, cItemInfo* pEquip, int nSocketIndex );
	//void		EItemComment_Parcing( cStringList* pList, cItemInfo* pEItem );
	//void		EItemComment_Parcing( cStringList* pList, int nEItemType );
	TCHAR*		FindToken( TCHAR* szMsg );
	





	//static int StartSpaceDiv( std::list< CString* >* pList, CFont* pFont, NiColor color, int x, int y, int nFontSize, int width, TCHAR* str );			// 색깔 구분 해서 문자열 자르기,공백 단위로 자름
	
	//static int StartNoColorSpaceDiv( std::list< CString* >* pList, CFont* pFont, NiColor color, int x, int y, int nFontSize, int width, TCHAR* str );	// 색깔 구분 안하고 문자열 자르기,공백 단위로 자름
	//static NiColor GetColor(TCHAR *);

	//static int GetWordLen(CFont* pFont, int nFontSize, TCHAR* str);			// 다음 단어 길이 가져오기
	//static void CutWord(CFont* pFont, int nFontSize, TCHAR* str,TCHAR* cut);	// 다음 단어 길이 가져오기
	//static void StrFilter(TCHAR* str);
};

extern cStringAnalysis* g_pStringAnalysis;
