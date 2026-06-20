
#pragma once 

class cTacticsAni : public cScriptUI
{
public:
	cTacticsAni(){}

	enum eAniProcess { eAni_None = 0, eAni_Start, eAni_BG, eAni_Play, eAni_End };	

	struct sTacticsAni : public NiMemObject
	{
	public:
		sTacticsAni() { s_fDigimonAlpha = 0.0f; s_pDigimon = NULL; }
		
		float				s_fDigimonAlpha;
		NiPoint3			s_vStartPos;	
		NiPoint3			s_vEndPos;	

		float				s_fStartRot;
		float				s_fEndRot;
		int					s_nUpdateSpeed;

		cTacticsObject*		s_pDigimon;
	};	

public:
	static void				GlobalInit();
	static void				GlobalShotDown();

public:	
	void					Delete();
	void					Init();
	
	void					End();
	void					Start( int nValue = 0, CsC_AvObject* pTarget = NULL );
	void					Skip( int nValue, CsC_AvObject* pPickObject);

	void					Update();	
	void					Render();
	void					PostRender();
	void					ResetDevice();
	void					SetDigitama( int nDigitama );

	bool					IsPlay() { return ( m_eProcessType != eAni_None ) ? true : false; }

	void					Set_bAniProcess(bool bAni) { m_bAniProcess = bAni; }
	
protected:
	cTacticsObject*			_AddDigimon( DWORD dwModelID, NiPoint3 vPos, float fRot );

	void					_UpdateStart();
	void					_UpdateBG();
	void					_UpdatePlay();	
	void					_EndAni();
	void					_SetBGAlpha();
	void					_SetText( TCHAR* szText, CFont::eFACE_SIZE eSize );

	// 메모리잡힌 컨트롤
protected:
	cSprite*		m_pBG;
	cSprite*		m_pBGAlpha;
	cSprite*		m_pSprite1;
	cSprite*		m_pSprite2;

	// 포인터만 있는 컨트롤
protected:
	CsC_AvObject*	m_pTarget; // 부화기

	// 필요 변수
protected:	
	eAniProcess		m_eProcessType;

	cSprite*		m_Sprite;
	float			m_fBright;	
	float			m_fCameraDist;
	int				m_nDeltaValue;	

	float			m_fAccumTime;
	float			m_fCurAlpha;
	float			m_fConvert;

	int				m_nDigimonID;
	int				m_nCount;	

	sTacticsAni*	m_sDigimon[ nLimit::EvoStep - 1];
	cStringList		m_StringList;
	cButton*		m_pSkip;

	bool			m_bAniProcess;	// Tactics 애니 시작 위치 변경을 위한 bool 변수
	CSound*			m_pHatchingSound;

	// 초월 연출용
private:
	cSprite*		m_pTranscendBG;
	cSprite*		m_pTranscendFrame;
	cSprite*		m_pTranscendDigimon;
	cSprite*		m_pTranscendCanopy;
	cSpriteAni*		m_pTranscendAni;
	cText*			m_pTranscendText;
	bool			m_bTranscend;

public:
	void			SetTranscendAni( bool bAni, std::string strFile, std::wstring strName );
};

extern cTacticsAni*		g_pTacticsAni;
