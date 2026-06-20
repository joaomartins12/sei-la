

#pragma once

#define ALIME_MAX_SIZE				310
#define ALIME_CUT_SIZE				280

class cAlime : public NiMemObject
{
public:
	enum eTYPE{ NONE, MAIL, PCBANG, EVENT };
	
public:
	cAlime();
	virtual ~cAlime(){ Delete(); }

	struct sALIME : public NiMemObject
	{
		sALIME() { s_bSet = false; s_bDir = false; }

		bool		s_bSet;	
		bool		s_bDir;
		NiPoint2	s_ptRenderPos;
		CsPoint		s_ptMaxSize;
		cString*	s_pString; // Æ÷ÀÎÅÍ¸¸..
	};

protected:
	cPopUp*			m_pPopupWindow;	
	cStringList		m_StringList;
	
	cSprite*		m_pPost;	
	cSprite*		m_pPcMark;

	std::queue< sALIME > m_QueueAlime; 
	float			m_DelayTime;
	bool			m_bUp;

protected:
	eTYPE			m_eType;
	int				m_nMainID;
	int				m_nSubID_1;
	int				m_nSubID_2;
	int				m_nSubID_3;
	void*			m_pData;			
	
	TCHAR			m_szText[ MAX_PATH ];

public:
	void			Delete();
	void			Init();	
	void			Render();	
	void			ResetDevice();
	
	void			Release();
	void			ResetMap();
	void			SetAlime( int nMaxSizeX, eTYPE type, int nMainID, int nSubID_1 = 0, int nSubID_2 = 0, int nSubID_3 = 0, void* pData = NULL );				

protected:
	void			_Update( sALIME* pAlime );

protected:
	cString*			_MakeAlime_Mail();
	cString*			_MakeAlime_PCbang();
	cString*			_MakeAlime_Event();
};

