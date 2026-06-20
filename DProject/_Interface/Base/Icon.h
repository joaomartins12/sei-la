
#pragma once



class cIcon : public NiMemObject
{
public:
	cIcon(){ m_pSpr = NULL;	}
	~cIcon(){ Delete(); }

protected:
	static cText*		g_pTextNumber[ 10 ];
public:
	static void			GlobalShotdown();
	static void			GlobalInit();
	static void			GlobalResetDevice();
	static void			GlobalUpdate();

	static float		m_flickerTime;			// 퀘스트 관련 아이콘 깜박임 효과
	static bool			bFlicker;				// 깜박임 상태.
	static bool			FlickerMode(int nIndex);

public:
	void		Delete();
	void		Init( CsPoint patch, CsPoint size , char* cPath );
	void		ResetDevice();

	//====================================================================================
	//
	//		이미지
	//
	//====================================================================================
protected:
	cSprite*		m_pSpr;
	CsPoint			m_ptPatchSize;
	CsPoint			m_ptIndexCount;
	NiPoint2		m_vPatchTex;

public:
	void			Render( int nIconIdx, CsPoint pos );
	void			Render( int nCol, int nRow, CsPoint pos );
	void			Render( int nCol, int nRow, CsPoint pos, CsPoint size );
	void			Render( int nIconIdx, CsPoint pos, CsPoint size );
	void			Render( int nIconIdx, CsPoint pos, CsPoint size, float fAlpha );
	cSprite*		GetSprite(){ return m_pSpr; }
	CsPoint			GetIndexCount(){ return m_ptIndexCount; }
	CsPoint		GetPatchSize(){return m_ptPatchSize;}
	//====================================================================================
	//
	//		카운트
	//
	//====================================================================================
public:
	void			RenderCount( int nCount, CsPoint pos );	

	//====================================================================================
	//
	//		아이콘 위에 텍스트 입히기
	//
	//====================================================================================
public:
	void			SetIconText( int nIconIdx, TCHAR const* szText, CsPoint ptDeltaPos );	

protected:	
	struct sIcon_Text : public NiMemObject 
	{
	public:
		sIcon_Text() { s_IconText = NULL; }
		virtual ~sIcon_Text() { SAFE_NIDELETE( s_IconText ); }

		CsPoint		s_DeltaPos;
		cText*		s_IconText;
	};

	std::map< int, sIcon_Text* >		m_MapIconText; 	
	
};

