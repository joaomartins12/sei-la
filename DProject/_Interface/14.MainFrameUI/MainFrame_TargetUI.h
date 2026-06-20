#pragma once

#include "../00.Contents/MainFrame_Contents.h"

class cTargetUI : public cBaseWindow, public CMainFrameContents::ObserverType
{
public:
	cTargetUI();
	~cTargetUI();

private:
	virtual	bool			Construct(void);
	virtual	void			Notify(int const& iNotifiedEvt, ContentsStream const& kStream);

public:
	virtual eWINDOW_TYPE	GetWindowType();
	virtual void			Destroy();
	virtual void			DeleteResource();
	virtual bool			Close( bool bSound /* = true  */ );

	virtual void			Create( int nValue = 0 );
	virtual void			Update(float const& fDeltaTime);
	virtual eMU_TYPE		Update_ForMouse();
	virtual void			Render();
	virtual void			ResetDevice();

	virtual bool			OnEscapeKey();

private:
	struct sBaseUI
	{
		sBaseUI() : pTargetBg(NULL), pTargetMark(NULL), pTargetLevel(NULL), pTargetImg(NULL), 
			pNameTxt(NULL), pPenNameTxt(NULL), pLevelTxt(NULL), pHpPb(NULL) {}
		~sBaseUI() {}

		void				SetBaseInfo( std::string const& strImgPath, std::wstring const& wsName, std::wstring const& wsPenName,
										int const& nLevel, float const& fHpRate );
		void				SetClassInfo( USHORT nClassType );
		void				SetOwner( bool bOwner );
		void				SetHpRate( float fHpRate );
		void				SetVisible( bool bVisible );

		bool				Update_ForMouse();

		cSprite*			pTargetBg;
		cSprite*			pTargetMark;
		cSprite*			pTargetLevel;
		cSprite*			pTargetImg;
		cText*				pNameTxt;
		cText*				pPenNameTxt;
		cText*				pLevelTxt;
		cUI_ProgressBar*	pHpPb;
	};

	struct sAttributeUI
	{
		sAttributeUI() : pNormalAttri(NULL), pNormalUpper(NULL), pNormalLower(NULL), pNormalEqual(NULL),
			pNatureAttri(NULL), pNatureComp(NULL), pBattleAttri(NULL) {}
		~sAttributeUI() {}

		void			SetBattleAttribute( int const& nAttriIdx );
		void			SetNormalAttribute( int const& nAttriIdx, int const& nCompType );
		void			SetNatureAttribute( int const& nAttriIdx, int const& nCompType );
		void			SetVisibleNormalAttribute( bool bVisible );
		void			SetVisibleNatureAttribute( bool bVisible );
		void			SetVisible( bool bVisible );

		bool			Update_ForMouse();

		cImage*			pNormalAttri;
		cSprite*		pNormalUpper;
		cSprite*		pNormalLower;
		cSprite*		pNormalEqual;

		cImage*			pNatureAttri;
		cSprite*		pNatureComp;

		cImage*			pBattleAttri;
	};

	struct sTimerUI
	{
		sTimerUI() : pTimerImg(NULL), pTimerPb(NULL) {}
		~sTimerUI() {}

		void			SetTimer( UINT const& nExpireTS, UINT const& nTotalExpireTS );
		void			SetVisible( bool bVisible );

		cSpriteAni*				pTimerImg;
		cUI_TimerProgressBar*	pTimerPb;
	};

	sBaseUI				m_sBaseUI;
	sAttributeUI		m_sAttriUI;
	sTimerUI			m_sTimerUI;

private:
	void				_SetBaseUI( USHORT const& nClassType, bool const& bOwner );
	void				_SetTargetUI();
	void				_SetTargetMonsterUI(UINT const& nDigimonID, bool const& bOwner);
	void				_UpdateTargetUI();

	void				_OnRClickTargetWindow( void* pSender, void* pData );
};