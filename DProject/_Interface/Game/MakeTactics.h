#pragma once 
#include "MakeTacticsContents.h"

#define IF_MAKE_TACTICS_REQUITE_LEVEL			3
#define IF_DIGITAMA_MAKE_TACTICS_ITEM_TYPE		93

#define IF_MT_MAX_BAR_LEN			169
#define IF_MT_BAR_COLOR_SRC			NiColorA( 61/255.0f, 255/255.0f, 255/255.0f, 1 )
#define IF_MT_BAR_COLOR_DEST		NiColorA( 1, 0, 0, 1 )
#define IF_MT_BAR_COLOR_END			NiColorA( 0.7f, 0, 0, 1 )
#define IF_MT_ICONSIZE				CsPoint( 32, 32 )

#define	DATACHANGE_BUTTON_POS		CsPoint( 184, 230 )		// 데이터 변경 버튼
#define	DATACHANGE_BUTTON_SIZE		CsPoint( 40, 33 )	
#define	HATCH_NPC_ID				90126					// 휴대용 인큐베이터 NPC ID 값.
#define	PORTABLE_DIST				5000.0f					// 휴대용 인큐베이터 꺼지는 거리.

#ifdef MAKE_TACTICS_UI_UPGRADE
	#define	MINIGAME_BUTTON_POS		CsPoint( 184, 280 )
	#define	BACKUPDISK_BUTTON_POS	CsPoint( 60, 280 )
	#define	AUTOREGIST_BUTTON_POS	CsPoint( 60, 230 )		// 자동등록 버튼 위치
	#define	BACKUPDISK_BUTTON_SIZE	CsPoint( 121, 33 )		// 자동등록과 크기 통일
#else
	#define	MINIGAME_BUTTON_POS		CsPoint( 184, 230 )
	#define	BACKUPDISK_BUTTON_POS	CsPoint( 97, 280 )
	#define	AUTOREGIST_BUTTON_POS	CsPoint( 97, 230 )
	#define	BACKUPDISK_BUTTON_SIZE	CsPoint( 84, 33 )
#endif

class cMakeTactics : public cBaseWindow, public MakeTacticsContents::ObserverType
{
	struct sBAR
	{
		enum eCHANGE{ CN_NONE, CN_INCREASE, CN_KEEP, CN_DECREASE, CN_DESTROY, CN_DESTROY_DECREASE };

		sBAR();

		void		InitLevel( int nLevel );
		void		ChangeLevel( eCHANGE type );
		void		SetChangeColor( NiColorA destColor );
		bool		Update(float const& fDeltaTime);
		void		SetActivateState(bool bActivate);
		bool		GetActivateState()	const;

		int			s_nRenderDelta[ 6 ];
		int			s_eChange;
		int			s_nCurLevel;
		int			s_nDestLevel;

		float		s_fCurPos;
		float		s_fDestPos;
		float		s_fDelayTime;
		int			s_nDeltaPos;

		NiColor		s_colorChange;

		cSprite*	s_pBar;	

		NiColorA	s_colorDelta;
		NiColorA	s_colorSetting;
		NiColorA	s_colorSrc;
		NiColorA	s_colorDest;
		bool		s_bColorCheckPlus;
		int			s_nColorCheckIndex;

		bool		s_bColorUpdate;
		bool		s_bActiveBar;
	};	

public:
	cMakeTactics();
	virtual ~cMakeTactics();

protected:
	virtual	void			Notify(int const& iNotifiedEvt, ContentsStream const& kStream);
	virtual	bool			Construct(void);

public:
	virtual eWINDOW_TYPE	GetWindowType(){ return WT_MAKE_TACTICS; }
	virtual void			Destroy();
	virtual bool			Close( bool bSound = true );
	virtual void			DeleteResource();
	virtual void			PreResetMap();
	
	virtual void			Create( int nValue = 0 );
	virtual void			Update(float const& fDeltaTime);
	virtual eMU_TYPE		Update_ForMouse();	
	virtual void			Render();

	virtual bool			CursorIconLBtnUp( CURSOR_ICON::eTYPE eIconType, int nIconSlot, int nIconCount );

	virtual void			ResetDevice();

	virtual bool			OnEscapeKey();
	virtual void			OnMoveWindow();

	virtual void			PlayOpenSound() {}

	virtual void			UserDie();
	virtual void			RecvDiedAlready();

public:
	enum eRESULT{ NONE, DEL_DIGITAMA, DEL_DATA, SUCCESS, DEL_BACKUP };

protected:
	cButton*				m_pBackupDiskBtn;		//2017-04-11-nova 백업디스크 등록/등록취소	
	cButton*				m_pAutoResistBtn;		//2017-04-11-nova 자동입력 / 데이터 입력
	cButton*				m_pCancelBtn;			//2017-04-11-nova 취소 / 제거

	cButton*				m_pCloseBtn;			//2017-04-11-nova 닫기
	cButton*				m_pRequiteBtn;			//2017-04-11-nova 지급받기
	cButton*				m_pDataChangeBtn;		//2017-04-11-nova 변경

	
	bool					bBackDiskBtnStatus;		//2017-04-11-nova true = 등록취소,	 false = 백업디스크등록
	bool					bAutoResistBtnStatus;	//2017-04-11-nova true = 데이터입력, false = 자동입력
	bool					bCancelBtnStatus;		//2017-04-11-nova true = 제거,		 false = 취소

	CsRect					m_rcResiDigitama;
	CsRect					m_rcDataChange;			// 데이터 변경 버튼 툴팁 체크 영역

	// SIMPLE_TOOLTIP
	CsRect					m_rcAutoReg;
	CsRect					m_rcChance;
	CsRect					m_rcBackupDisk;
	CsRect					m_rcBackupReg;
	CsRect					m_rcRecv;
	CsRect					m_rcCancel;	

	sBAR					m_CurBar;
	cRenderTex*				m_pRenderTex;

protected:
	void					PressBackupDiskBtn( void* pSender, void* pData );	//2017-04-11-nova
	void					PressAutoResistBtn( void* pSender, void* pData );	//2017-04-11-nova
	void					PressCancelBtn( void* pSender, void* pData );		//2017-04-11-nova
	
	void					PressCloseBtn( void* pSender, void* pData );		//2017-04-11-nova
	void					PressRequiteBtn( void* pSender, void* pData );		//2017-04-11-nova
	void					PressDataChangeBtn( void* pSender, void* pData );	//2017-04-11-nova
	void					PressMiniGameBtn( void* pSender, void* pData );		//2017-04-11-nova
	
	void					_ResistBackupDiskEnableControl();	
	void					_ResistBackupDisk();
	void					_CancelBackupDisk();

	void					_ResistDigitamaEnableControl();
	void					_ResistDigitamaDisableControl();

	void					_CancelEnableControl();								//2017-04-12-nova 취소
	void					_CancelDisableControl();							//2017-04-12-nova 제거

	void					_ResistDigitama();	
	void					_DeleteBackupDisk();
	void					_CancelDigitama();
	void					_DeleteDigitama();	
	void					_TryLevelUp();

private:
	void					CreateData();	
	void					SetNpcAnimation(DWORD dwAniID);	
	n4						GetNpcID() const;
	void					SendMiniGameCheck();	// 2016.03.22	(eiless84) 이성진
	void					_RefreshGradeString();	// 버튼 ID에 따라 하급~고급 버튼 string 반환

protected:
	bool					m_bPortable;	
	bool					m_bCompleateChangeBar;

	float					m_fBackupMusic;

	cStringList				m_StringBody;			//부화 보정 확률 % 스트링

	eRESULT					m_eResult;

	CsC_AvObject*			m_pTarget;				// 기본 강화시에 보여질  Npc 오브젝트
	CsC_AvObject*			m_pNPCObejct;			// 휴대용 강화시에 보여질 Npc 오브젝트

public:
	void					SetTarget( CsC_AvObject* pTarget, bool	bPortable = false );	// bPortable 휴대용 여부
	CsC_AvObject*			GetTarget();

	void					_ResistBackupDiskDisableControl();
	void					SendServer();
	void					RecvServer();
	void					ResistDigitama( int nInvenIndex );
	void					Resist( int nInvenIndex );
	void					ResistBackupDisk( int nInvenIndex );
	void					RequiteTactics( TCHAR* szName );
	void					DestroyResistDigitama( bool bNetwork );
	void					RecvResult( eRESULT result );
	void					ProcessResult();
	void					EndMiniGame();			// 2016.03.22	(eiless84) 이성진	

	bool					GetAutoResistBtnStatus();
	uint					_GetBackupDiskType();

	cButton*				const GetCurResiBtn() const;

	// [2/24/2016 hyun] 추가. 제거버튼 
	bool					GetDeleteBtnState() const;
	void					SetDeleteBtnState(bool bEnable);	

	bool					getBackDiskBtnStatus()	 { return bBackDiskBtnStatus; }
	bool					getAutoResistBtnStatus() { return bAutoResistBtnStatus; }

	void					GetDeleteBackupFunction() { _DeleteBackupDisk(); }
	cItemInfo*				GetResistItem();

#ifdef	MINIGAME
	void					SetEnableMinigameBt(bool bOn);	// 2016.03.16	(eiless84) 이성진
	void					OnEndMiniGame(void);
	void					OnUpdateMinigameInfo(void);
	void					UpdateMiniGameInfoText(const bool isCompleted, const int& Count, const int& DigitamaLv);
	void					UpdateMinigameInfoStartBt(const bool isCompleted, const int& Count, const int& DigitamaLv);
	cButton*				m_pMiniGameBtn;			//2017-04-11-nova 미니게임
#endif
	int						iPotableIndex;
};