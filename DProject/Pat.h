
#pragma once 

class CPat : public CsC_AvObject
{
	THREAD_MEMPOOL_H( CPat )
	CsCRTTI_H

public:
	CPat::CPat();


public:
	virtual void		Delete();
	virtual void		PreDelete();
	virtual void		Init( CsC_AvObject* pParent, uint nUID, uint nFileTableID, NiPoint3 vPos, float fRot, sCREATEINFO* pProp );

protected:
	void				_Init();
	void				_UpdatePos();

public:
	virtual void		Update( float fDeltaTime, bool bAnimation = true );
	virtual void		Render();
	virtual void		Render_PostChar();
	
protected:
	CPat_AI_Mng::sAI_Element*	m_pAI;

protected:
	void			_SetAI( CPat_AI_Mng::sAI_Element* pAI );

	//====================================================================================
	//	시간
	//====================================================================================
protected:
	CsTimeSeq		m_TimeSeq;
protected:
	void			_Set_AI_Time( CPat_AI_Mng::sAI_Element* pAI );
	void			_Update_AI_Time( float fDeltaTime );

	//====================================================================================
	//	애니메이션
	//====================================================================================
protected:
	int				m_nAniLoopCount;
protected:
	void			_Set_AI_Animation( CPat_AI_Mng::sAI_Element* pAI );
	void			_Check_AI_Animation( bool& bResetAni );

	//====================================================================================
	//	이동
	//====================================================================================
protected:
	struct sDEST_POS
	{		
		NiPoint3	s_vSrcPos;
		NiPoint3	s_vDestPos;
		NiPoint3	s_vBegin;			// 실제 가지는 않지만 커브를 틀기 위해 필요한 값
		NiPoint3	s_vEnd;				// 실제 가지는 않지만 커브를 틀기 위해 필요한 값

		float		s_fSpeed;
		float		s_fCurTime;
		float		s_fEndTime;

		void		SetStartPos( float fSpeed, NiPoint3 vBegin, NiPoint3 vSrc, NiPoint3 vDest );
		void		SetNextPos( float fSpeed, sDEST_POS* pPrev, NiPoint3 vDest, NiPoint3* pEnd = NULL );
	};
	CsVectorPB< sDEST_POS >		m_vpDestPos;
	sDEST_POS*					m_pCurPos;
	int							m_nDestPosArrayIndex;

protected:
	void			_Set_AI_Move( CPat_AI_Mng::sAI_Element* pAI );	
	void			_Update_AI_Move( float fDeltaTime );
	bool			_NextMovePoint();
	void			_SetAI_Move_Tamer_Forward( CPat_AI_Mng::sAI_Element* pAI );


	//====================================================================================
	//	부모
	//====================================================================================
protected:
	CsC_AvObject*		m_pParent;

	//====================================================================================
	//	파일 테이블
	//====================================================================================
protected:
	CsDigimon*			m_pFT_Digimon;

public:
	CsDigimon*			GetDigimonFT(){ return m_pFT_Digimon; }

	//====================================================================================
	//		CallBack
	//====================================================================================
public:
	virtual bool		CallBack_EndofSequence( DWORD dwAniID );

	//====================================================================================================
	//	Path
	//====================================================================================================
public:
	virtual void		DeletePath();
	virtual void		ArriveTarget();

	//====================================================================================================
	//	Property
	//====================================================================================================
protected:
	virtual void		_SetDefault_IdleAnimation_Stand();
	virtual void		_SetDefault_IdleAnimation_Move();

	//====================================================================================
	//		Sound
	//====================================================================================
public:
	virtual void*		PlaySound( char* cSoundPath );

	//====================================================================================================
	//	부가 정보
	//====================================================================================================
public:
	virtual bool			IsPickEnable();
	virtual UINT			GetModelID() const;

	// 애니메이션 한 루프당 이동 가능한 거리
	virtual float			_GetAniTimeMoveDist_Walk();
	virtual float			_GetAniTimeMoveDist_Run();
	virtual float			_GetAniTimeMoveDist_ARun();
	virtual float			CalIdleAniTime();
	virtual ANI::eTYPE		GetRandomIdle();

protected:
	NiPoint3		_Cal_Parent_Right( float fDist );
	NiPoint3		_Cal_Parent_Left( float fDist );
	NiPoint3		_Cal_Parent_Forward( float fDist );
	NiPoint3		_Cal_Parent_Back( float fDist );
	NiPoint3		_Cal_Parent_RightForward( float fDist );
};