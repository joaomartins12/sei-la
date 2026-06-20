

#pragma once

#define MAX_ANAL_LEN		8192

// 스트링 분석기
class cStringAnalysis : public NiMemObject
{
public:
	static void		GlobalInit();
	static void		GlobalShotDown();
	static bool		CutLimitWigth( std::list<dm_string_t>& outPut, cText::sTEXTINFO const& pTextInfo, int const& nWidthLimit );
	static bool		CutLimitWigth( std::list<dm_string_t>& outPut, dm_string_t const& surText, 
								   CFont const* pFont, CFont::eFACE_SIZE fontSize, int const& nWidthLimit );

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

	// Per-call working state.  Was previously held on the singleton (one shared
	// set of m_pString / m_listParcing / m_szToken / etc), which crashed when
	// any two callers were active concurrently or sequentially-with-leftover
	// state — the fatal pattern was the network thread (cCliGame::OnExecute →
	// SyncInBuff → ChatContents → ChatMainWindow::Notify → Cut) racing the
	// main UI thread.  One thread's `m_pString = NiNew cString` got stomped by
	// the other thread's `SAFE_NIDELETE(m_pString)` defensive cleanup, the
	// freed slot was reused by the NiAllocator for some other type (the
	// debugger's symbolizer typically named it cData_QuickSlot), and the next
	// `m_pString->GetListSize()` AV'd reading garbage as a vtable.  By
	// stack-allocating this context per call, every Cut*/_MakeToken* invocation
	// gets its own scratch — no shared mutable state to race over.
	struct sCutContext
	{
		NiColor                m_OrgColor;
		int                    m_nOrgBoldLevel;
		cString*               m_pString;
		cStringList*           m_pStringList;
		bool                   m_bNewLine;
		std::list< sPARCING* > m_listParcing;
		cText::sTEXTINFO*      m_pTextInfo;
		TCHAR                  m_szToken[ MAX_ANAL_LEN ];
		int                    m_nTokenIndex;
		int                    m_nTotalWidth;
		sSTR_INFO*             m_pStrInfo;
		int                    m_nLineLimit;
		std::wstring           m_strTempText;

		sCutContext()
			: m_OrgColor()
			, m_nOrgBoldLevel( 0 )
			, m_pString( NULL )
			, m_pStringList( NULL )
			, m_bNewLine( false )
			, m_pTextInfo( NULL )
			, m_nTokenIndex( 0 )
			, m_nTotalWidth( 0 )
			, m_pStrInfo( NULL )
			, m_nLineLimit( -1 )
		{
			memset( m_szToken, 0, sizeof( m_szToken ) );
		}

		~sCutContext()
		{
			// Caller is responsible for transferring m_pString to the
			// stringList (or, for Cut_Parcing_ForCardMaster, leaving it owned
			// by the caller) and nulling it before this dtor runs.  If a
			// method aborts early without doing so, free here so we don't leak.
			if( m_pString != NULL )
			{
				SAFE_NIDELETE( m_pString );
				m_pString = NULL;
			}
			std::list< sPARCING* >::iterator it = m_listParcing.begin();
			std::list< sPARCING* >::iterator itEnd = m_listParcing.end();
			for( ; it != itEnd; ++it )
				sPARCING::DeleteInstance( *it );
			m_listParcing.clear();
		}
	};

protected:
	void			_Parcing( sCutContext& ctx, TCHAR const* str, int nStrLen, int& nIndex );
	int				_ParcingTypeStart( TCHAR const* str, ePARCING_TYPE& pt );
	bool			_ParcingValueEnableCheck( int nStrLen, int nIndex, ePARCING_TYPE& pt );
	int				_ParcingValue( sCutContext& ctx, TCHAR const* str, ePARCING_TYPE& pt );
	int				_ParcingTypeEnd( sCutContext& ctx, TCHAR const* str );
	//void			_ParcingWord( TCHAR const* str, int nStrLen, int& nIndex, int nWidth, cButton::eIMAGE_TYPE Type );

	void		_RemoveParcing( sCutContext& ctx, ePARCING_TYPE pt );

	//============================================================================================
	//		문자열 자르기
	//============================================================================================
protected:
	bool	_MakeToken_Parcing_ForCardMaster( sCutContext& ctx, CsPoint pos );
	bool	_MakeToken_Parcing( sCutContext& ctx, bool bNewString );
	bool	_MakeToken( sCutContext& ctx, bool bNewString );
	bool	_MakeToken_OnlyNewLine( sCutContext& ctx, bool bNewString );
	void	_AddText( sCutContext& ctx, CsPoint ptDeltaPos );

public:	
	void	Cut_Parcing_ForCardMaster( cString* pString, int width, CsPoint	Pos, TCHAR const* str, cText::sTEXTINFO* pTextInfo );	// 색깔 구분 해서 문자열 자르기

	void	Cut_LinkParcing( cStringList* pStringList, int width, TCHAR const* str, cText::sTEXTINFO* pTextInfo, cButton::eIMAGE_TYPE Type );	// 색깔 구분, 단어링크, 문자열 자르기
	void	Cut_Parcing( cStringList* pStringList, int width, TCHAR const* str, cText::sTEXTINFO* pTextInfo );	// 색깔 구분 해서 문자열 자르기
	void	Cut( cStringList* pStringList, int width, TCHAR const* str, cText::sTEXTINFO* pTextInfo, int nLineLimit = -1 );			// 색깔 구분 해서 문자열 자르기
	void	Cut_NoMultyLine( cStringList* pStringList, TCHAR const* str, cText::sTEXTINFO* pTextInfo, int nLineLimit = -1 );			// 색깔 구분 해서 문자열 자르기
	void	AddCut( cStringList* pStringList, int width, TCHAR const* str, cText::sTEXTINFO* pTextInfo );
	void	Cut_MailParcing( cStringList* pStringList, int width, TCHAR const* str, cText::sTEXTINFO* pTextInfo );			// 색깔 구분 해서 문자열 자르기
	void	Cut_LimitWigth( cStringList* pStringList, int width, TCHAR * str, cText::sTEXTINFO* pTextInfo, int nLineLimit = -1 );			// 색깔 구분 해서 문자열 자르기

	TCHAR*	StringTransfer( TCHAR* szOut, int nOutSize, TCHAR const* szIn );										// 문자열 대체

	void	MaxLine_Cut(TCHAR const* word, cStringList* pStringList, int width, TCHAR const* str, cText::sTEXTINFO* pTextInfo, int nLineLimit = -1);	// 문자열을 길이를 넘어서면 word문자열 입력후 자름.

	TCHAR*		Quest_Parcing( TCHAR * szDest, const int nDestSize, TCHAR const* szSrc, CsQuestCompRequireGroup* pApplyGroup );

	void		ItemComment_Parcing( cStringList* pList, cItemInfo* pEquipItem );
	void		ItemComment_Parcing( cStringList* pList, int nEquipType );

	void		Equip_EItemComment_Parcing( cStringList* pList, cString* pBeginString, cItemInfo* pEquip, int nSocketIndex, bool bCashEndItem = false );	
	
// [4/14/2016 hyun] 추가
	void		SetParcingColor(const NiColor color);
	NiColor		GetParcingColor() const;
	void		CompleteParcingColor();

	NiColor		m_ParcingColor;
	bool		m_bChangeColor;
// @@ 여기까지

	std::wstring				m_strTempText;
	std::vector<std::wstring> 	m_vecStringTotalText;


	//TCHAR*		FindToken( TCHAR* szMsg );
	//static int StartSpaceDiv( std::list< CString* >* pList, CFont* pFont, NiColor color, int x, int y, int nFontSize, int width, TCHAR* str );			// 색깔 구분 해서 문자열 자르기,공백 단위로 자름
	
	//static int StartNoColorSpaceDiv( std::list< CString* >* pList, CFont* pFont, NiColor color, int x, int y, int nFontSize, int width, TCHAR* str );	// 색깔 구분 안하고 문자열 자르기,공백 단위로 자름
	//static NiColor GetColor(TCHAR *);

	//static int GetWordLen(CFont* pFont, int nFontSize, TCHAR* str);			// 다음 단어 길이 가져오기
	//static void CutWord(CFont* pFont, int nFontSize, TCHAR* str,TCHAR* cut);	// 다음 단어 길이 가져오기
	//static void StrFilter(TCHAR* str);
};

extern cStringAnalysis* g_pStringAnalysis;
