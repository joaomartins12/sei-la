
#pragma once

#define IF_BG_SPRITE_ANI_CNT			2
#define IF_BG_SPRITE_CNT			5
#define IF_BG_IMAGE_CNT				1
#define IF_BG_BUTTON_CNT			2
#define DAILY_MAX_SIZE				115

class cBGSprite : public cBaseWindow
{
public:
	cBGSprite();

public:
	enum ePOINTER_TYPE{ POINTER_IMAGE,
						POINTER_CAPSULE_COMP,
						POINTER_ATTENDANCE_EVENT,
						POINTER_PCBANG,

						POINTER_FRIEND_EVENT,				//친구추천 이벤트 버튼
#ifdef MINIGAME
						POINTER_MINIGAME_COUNT,					//MINIGAME 미니게임 카운트다운 추가 chu8820
#endif
#ifdef TRICK_ORTREAT
						POINTER_TIME_ITEM,
#endif
						POINTER_RAID_REWARD,
						POINTER_DAILY_BAR,
#ifdef CROSSWARS_SYSTEM
						POINTTER_CROSSBUTTON,
#endif
	};
	enum eVALUE_TYPE{	SPRITE_MAKE_TACTICE_UP,
						CAPSULE_COMP,
						ATTENDANCE_EVENT,
						PCBANG,
						FRIEND_EVENT,						//친구추천 이벤트
#ifdef MINIGAME
						MINIGAME_COUNT,					//MINIGAME 미니게임 카운트다운 추가 chu8820
#endif
#ifdef TRICK_ORTREAT
						TIME_ITEM,
#endif
						RAID_REWARD,
						DAILY_BAR,
#ifdef CROSSWARS_SYSTEM
						CROSSBUTTON,
#endif
	};
	enum eLIFE{ LIFE_ALWAYS, LIFE_TIME, };

public:
	virtual eWINDOW_TYPE	GetWindowType(){ return WT_BGSPRITE; }
	virtual void			Destroy();
	virtual void			DeleteResource();
	virtual void			Create( int nValue = 0 );

	virtual bool			Close( bool bSound = true );

	virtual void			Update(float const& fDeltaTime);
	virtual eMU_TYPE		Update_ForMouse();
	virtual void			Render();

	virtual void			ResetDevice();
	virtual void			ResetMap(){}
	virtual bool			OnEscapeKey(){ return false; }

	u4						m_fStart, m_fMinute, m_fSecond, m_fTime;

	float					m_fAccumTime;
	float					m_fAlpha;
	bool					m_bIncreaseAlpha;

protected:
	void					_EndTime();

	//===============================================================================================================
	//
	//	공용
	//
	//===============================================================================================================
protected:
	eLIFE			m_eLifeType;
	CsTimeSeq		m_LifeTimeSeq;

	eVALUE_TYPE		m_eValueType;

	int				m_nValue1;
	int				m_nValue2;
	int				m_nValue3;

	float			m_fValue1;
	bool			m_bValue1;

	float			m_fValue2;
	std::vector< int >	m_vValue1;
	CsPoint			m_InitPos;

	cSpriteAni*		m_pSpriteAniArray[ IF_BG_SPRITE_ANI_CNT ];

	float		m_fCurPos;//현재 바 위치
	float		m_fDestPos;//채울 목표 위치
	int			m_nRenderStart;
	int			m_nRenderDest;

	cStringList		m_strlist;
	cSprite*		m_pSpriteArray[ IF_BG_SPRITE_CNT ];
	cImage*			m_pImageArray[ IF_BG_IMAGE_CNT ];
	cButton*		m_pButtonArray[ IF_BG_BUTTON_CNT ];

public:
	void					SetTime( DWORD dwTime );
	void					SetValue1( int nValue );
	void					SetValue2( int nValue );
	void					SetValue3( int nValue );
	void					SetfValue1(const float& fValue);


	int						GetValue1() const;
	int						GetValue2() const;
	int						GetValue3() const;

	void					PushValue1( int nValue );
	//===============================================================================================================
	//
	//	선물 지급
	//
	//===============================================================================================================
public:
	//void					MakeDailyEvent();	

	//===============================================================================================================
	//
	//	출석 체크
	//
	//===============================================================================================================
protected:
	void					MakeAttendance();
public:
	void					PassTime( int nSec );

	//===============================================================================================================
	//
	//	PC방 혜택
	//
	//===============================================================================================================
protected:
	cButton*		m_pCancelButton;
};
