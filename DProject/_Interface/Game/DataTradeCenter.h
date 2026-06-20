#pragma once 

#define	NONE_TRADE_EFFECT	-1
#define END_TRADE_EFFECT	0

class CDataTradeCenter : public NiMemObject, public DataTradeContents::ObserverType
{
public:
	CDataTradeCenter(void);
	~CDataTradeCenter(void);

	enum{
		NONE_PICK=-1,
		LEFT_PICK,
		RIGHT_PICK,
		EXIT_PICK,
	};

	virtual	bool			Construct(void);
	////////////							COMMON				//////////////////
protected:
	virtual	void			Notify(int const& iNotifiedEvt, ContentsStream const& kStream);

public:
	void	Start();
	void	End();

	void	PlaySystemSound( char const* szSound )	{ g_pSoundMgr->PlaySystemSound( szSound ); }
	void	PlayMusic( char const* szMusic )		{ g_pSoundMgr->PlayMusic( szMusic ); }	

public:
	void		Delete();
	void		Init();

	void		Update( float fDeltaTime, cGlobalInput::eMOUSE_INPUT mi );	
	void		Render();	

	void		ResetDevice();
private:
	void		_SetDefaultCamera();
	void		_SetTradeCamera();
	void		_UpdateDigimonViewCamera(float dTime);		// 디지몬 바라보는 카메라
	void		_UpdateObjectDelete(float dTime);			// 삭제될 오브젝트 갱신

	char const* _LoadMusic();
	void		_PickExit();			// 데이터 선택창에서의 마우스 피킹. EXIT만 사용.
	void		_PickUpdate();			// 데이터 교환시 마우스 피킹 업데이트

public:			//	데이터 교환 콘텐츠에서 사용할 변수들.
	bool		_IsPickLeftSlot();
	bool		_IsPickRightSlot();
	bool		_IsModelDeleteTime();	// 교환 이벤트 삭제 시간을 내부에서 확인 가능. 현재 미사용.

	int			_EndTradeEffect();		// 이펙트가 끝났는지 여부 판단한다.

	void		_CreateTradeEffect();
	void		_CreateDigimonLeft( DWORD dwID );
	void		_CreateDigimonRight( DWORD dwID );

	void		_DeleteTradeEffect();
	void		_DeleteDigimonLeft();
	void		_DeleteDigimonRight();

	void		_DeleteEffectDigimonLeft();
	void		_DeleteEffectDigimonRight();


protected:
	// 사용변수들
	CsNodeObj	m_DatsCenter;

	int			m_nPicked;	
	CsNodeObj	m_PickLeftObj;
	CsNodeObj	m_PickRightObj;
	CsNodeObj	m_PickExitObj;

	NiPoint3	m_vPos;
	float		m_fCameraDist;
	float		m_fCameraRot;
	float		m_fCameraPitch;		

	bool			m_bIsSound;
	float			m_fBackupDistMin;
	float			m_fBackupDistMax;
	bool			m_bResult;
	bool			m_bOpenUI;
	int				m_nOldTime;
	int				m_nOver;
	int				m_nSelect;
	CsTimeSeq		m_SelectTime;	

	bool				m_bLeftDelete;
	bool				m_bRightDelete;

	float				m_fLeftDeleteTime;
	float				m_fRightDeleteTime;

	sCREATEINFO			m_prop;
	CsC_AvObject*		m_pTradeEffect;
	CsC_AvObject*		m_pDigimonLeft;
	CsC_AvObject*		m_pDigimonRight;
	CsC_EffectObject*	m_pEffectObject;
	
};