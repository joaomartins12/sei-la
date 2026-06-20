#pragma once 

#include "DatsCenterContents.h"

#define MAX_CARD_NUM	14
class CDatsCenter : public NiMemObject, public CDatsCenterContents::ObserverType
{
public:
	CDatsCenter(void);
	~CDatsCenter(void);

	enum eSTATE { eSTATE_NOR = 0, eSTATE_ENCHENT, eSTATE_SELECT_ANI, eSTATE_SELECT, eSTATE_SELECT_END };

public:
	void	Start();
	void	End();

	void	PlaySystemSound( char const* szSound ) { g_pSoundMgr->PlaySystemSound( szSound ); }
	void	PlayMusic( char const* szMusic ) { g_pSoundMgr->PlayMusic( szMusic ); }	

public:
	void		Delete();
	void		Init();

	void		Update( float fDeltaTime, cGlobalInput::eMOUSE_INPUT mi );	
	void		Render();	

	void		ResetDevice();

	void		SetSelectCard( bool bUserSelect );
	void		SetSelectMode();	

	void		StartSelectCard();
	void		EndSelectCard();

	void		EnchantSuccess( int nSelect );
	void		EnchantFail( int nSelect );
	void		EnchantEnd();
	void		WeakSuccess();

	eSTATE		GetState() { return m_eState; }

public:
	virtual	bool	Construct(void);
protected:
	virtual	void	Notify(int const& iNotifiedEvt, ContentsStream const& kStream);

protected:
	void		_SetDefaultCamera();
	void		_SetEnchentCamera();
	char const*	_LoadMusic();

protected:
	// 사용변수들
	CsNodeObj	m_DatsCenter;

	int			m_nPicked;	
	CsNodeObj	m_PickInchantObj;
	CsNodeObj	m_PickExitObj;

	NiPoint3	m_vPos;
	float		m_fCameraDist;
	float		m_fCameraRot;
	float		m_fCameraPitch;		

	cSprite*		m_pSpriteNo;
	cSprite*		m_pSprite1;
	cSprite*		m_pSprite2;	
	//cSprite*		m_pSprTitle;	

	cSprite*		m_pSprCardSuccess;
	cSprite*		m_pSprCardFail;

	cSpriteAni*		m_pAniCard;
	cSpriteAni*		m_pAniCardSuccess;
	cSpriteAni*		m_pAniCardFail;
	cSpriteAni*		m_pAniCardSelect;
	cSpriteAni*		m_pAniSuccess;
	cSpriteAni*		m_pAniFail;

	bool			m_bIsSound;
	float			m_fBackupDistMin;
	float			m_fBackupDistMax;
	bool			m_bResult;
	bool			m_bOpenUI;
	int				m_nOldTime;
	int				m_nOver;
	int				m_nSelect;
	CsTimeSeq		m_SelectTime;	

	CsC_AvObject*	m_pDigimon;
	CsC_AvObject*	m_pDigimonClon;
	eSTATE			m_eState;

	struct sCard
	{
		CsRect			s_rcCard;
		NiPoint2		s_ptPos;		
		bool			s_bSuccess;
	};	

	sCard m_Card[ MAX_CARD_NUM ];
};

extern CDatsCenter* g_pDatsCenter;