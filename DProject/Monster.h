
#pragma once 

class CMonster : public CsC_AvObject
{
	THREAD_MEMPOOL_H( CMonster )
	CsCRTTI_H

public:
	CMonster();

public:
	enum eMONSTER_STATE{
		MONSTER_CREATE, MONSTER_NORMAL, MONSTER_BATTLE, MONSTER_DIE,
	};


public:
	virtual void		Delete();
	virtual void		PreDelete();
	virtual void		Init( uint nUID, uint nFileTableID, nSync::Pos sPos, float fRot, sCREATEINFO* pProp, eMONSTER_STATE MonsterState );
	virtual void		Init( uint nUID, uint nFileTableID, NiPoint3 vPos, float fRot, sCREATEINFO* pProp, eMONSTER_STATE MonsterState );

protected:
	void				_Init( eMONSTER_STATE MonsterState );

public:
	virtual void		Update( float fDeltaTime, bool bAnimation = true );
	virtual void		Render();
	virtual void		Render_PostChar();

	//====================================================================================
	//		제거 대기 상태
	//====================================================================================
public:
	virtual void		SetDeleteState( eDELETE_STATE ds );
	virtual bool		UpdateDelete( float fDeltaTime );

	//====================================================================================
	//	파일 테이블
	//====================================================================================
protected:
	CsMonster*			m_pFT_Monster;
	CsDigimon*			m_pFT_Digimon;

public:
	CsDigimon*			GetDigimonFT();
	CsMonster*			GetMonsterFT();

	//====================================================================================
	//		CallBack
	//====================================================================================
public:
	virtual bool		CallBack_EndofSequence( DWORD dwAniID );

	//=============================================================================================================
	//	상태
	//=============================================================================================================
protected:
	eMONSTER_STATE		m_eMonsterState;
public:
	eMONSTER_STATE		GetMonsterState();
	bool				IsMonsterState( eMONSTER_STATE eMonState );
	virtual void		SetBattle();
	virtual void		SetBattleEnd();
	virtual void		SetDie();
protected:
	bool				m_bIsCasting;
public:
	bool				IsCasting_Monster();
	void				SetCasting_Monster( bool bCasting );

	//====================================================================================================
	//	Path
	//====================================================================================================
public:
	virtual void		DeletePath();

	//====================================================================================================
	//	Property
	//====================================================================================================
protected:
	virtual void		_SetDefault_IdleAnimation_Stand();
	virtual void		_SetDefault_IdleAnimation_Move();

	//====================================================================================
	//	스킬
	//====================================================================================
protected:
	cSkill				m_SkillMng;
public:
	cSkill*				GetSkillMng();

	//====================================================================================
	//	컨디션
	//====================================================================================
protected:
	cCondition			m_Condition;
public:
	cCondition*			GetCondition();
	void				SetCondition( uint nCondition );

	//====================================================================================
	//	버프
	//====================================================================================
protected:
	cBuffData		m_BuffData;

public:
	cBuffData*		GetBuff();

	//====================================================================================
	//	스탯	- 항상 동적으로 구하겠다 - 공식을 변경하고 싶으면 이부분을 바꾸라
	//====================================================================================
public:
	virtual float			GetAttackRange();
	virtual float			GetAttackCharSize();
	//virtual float			GetAttackSpeed();

	//====================================================================================
	//	등장 연출
	//====================================================================================
protected:
	NiPoint3			m_vCreateSceneBackupPos;			// 연출 위치
	float				m_fCreateSceneTime;
protected:
	void				_InitCreateScene();
	void				_UpdateCreateScene( float fDeltaTime );

	//====================================================================================
	//		Sound
	//====================================================================================
public:
	char*				GetSoundDir();
	virtual void*		PlaySound( char* cSoundPath );
	virtual void*		PlayStaticSound( char* cSoundPath );

	//====================================================================================================
	//	부가 정보
	//====================================================================================================
public:
	virtual bool			IsPickEnable();
	virtual UINT			GetClass();
	virtual UINT			GetUniqID();
	virtual NiColor			GetNameColor();
	virtual UINT			GetModelID() const;
			CCharOption*	GetCharOption();
			sCHAR_IMAGE*	GetCharImage();

	virtual TCHAR*			GetName();
	virtual bool			IsRenderName();

	// 애니메이션 한 루프당 이동 가능한 거리
	virtual float		_GetAniTimeMoveDist_Walk();
	virtual float		_GetAniTimeMoveDist_Run();
	virtual float		_GetAniTimeMoveDist_ARun();
	virtual float		CalIdleAniTime();
	virtual ANI::eTYPE	GetRandomIdle();
	virtual int			GetAttackRangeType();


	void				SetOwnerID( uint nUID );
	uint				GetOwnerID();

protected:
	sCHAR_IMAGE*		m_pCharImage;
	CCharOption			m_Option;

	uint				m_nOwnerID;					// 소유자 ID;

	// 시간 되면 소멸 되는 오브젝트
protected:
	n4					m_nExpireTimeTS;				// 소멸 되기 까지 남은 시간
	n4					m_nTotalExpireTime;				// 소멸 되기 까지 셋팅된 원래 시간
public:
	u4					GetExpireTimeTS();
	u4					GetTotalExpireTime();
	void				SetExpireTimeTS( n4 nTimeTS );
	void				SetTotalExpireTime( n4 nTime );

	// 카메라
public:	
	virtual void		AttachCamera_Critical( CsC_AvObject* pTarget );
	bool				CheckTamerDist(float Dist = IF_TARGET_TAMER_MAX_DIST);

protected:
	cText3D*			m_pMonsterIcon;
	DWORD				m_dwIcon;
#ifdef MONSTER_SKILL_GROWTH
	float				m_fOrgScale;	// 버프 받기 전 원래 크기
public:
	void				_SetOrgScale( float fScale );
	float				_GetOrgScale();
#endif
};