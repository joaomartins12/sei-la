
#pragma once 

class CNpc		:	public	CsC_AvObject
{
	THREAD_MEMPOOL_H( CNpc )
	CsCRTTI_H

public:
	CNpc();

public:
	enum eNPC_RENDER_TYPE
	{
		NRT_NORMAL, NRT_DIGIVICE, NRT_TEMP,
	};

public:
	virtual void		Delete();
	virtual void		PreDelete();
	virtual void		Init( uint nUID, uint nFileTableID, sCREATEINFO* pProp );
	virtual void		Init_NoFT( uint nUID, sCREATEINFO* pProp, eNPC_RENDER_TYPE nrt );
	virtual void		Init_NoMapFT( uint nUID, uint nFileTableID, sCREATEINFO* pProp, eNPC_RENDER_TYPE nrt );	

	virtual void		Update( float fDeltaTime );	

	virtual void		Render();
	virtual void		Render_PostChar();		
	bool				IsDisplayType( DWORD const& dwDisplayType ) const;

	//====================================================================================================
	// 파일 테이블
	//====================================================================================================
protected:
	CsMapNpc*		m_pFT_MapNpc;
	CsNpc*			m_pFT_Npc;
public:
	CsNpc*			GetFTNpc(){ return m_pFT_Npc; }

	//====================================================================================
	//		CallBack
	//====================================================================================
public:
	virtual bool		CallBack_EndofSequence( DWORD dwAniID );

	//====================================================================================================
	//	부가 정보
	//====================================================================================================
public:
	virtual UINT			GetClass(){ return nClass::Npc; }
	virtual UINT			GetUniqID(){ cType type; type.m_nClass = GetClass(); type.m_nIDX = GetIDX(); return type.m_nUID; }
	virtual NiColor			GetNameColor(){ return NiColor( 1.0f, 0.878f, 0.153f ); }
	virtual UINT			GetModelID() const;
			CCharOption*	GetCharOption(){ return &m_Option; }
			sCHAR_IMAGE*	GetCharImage(){ return m_pCharImage; }

	virtual TCHAR*			GetName(){ return m_Option.GetName(); }
	virtual bool			IsRenderName(){ return g_pResist->m_Global.s_bNpcRenderName; }

	//virtual ANI::eTYPE		GetRandomIdle();
	virtual float			GetAlphaHideDist(){ return 9000.0f; }

	virtual bool			IsPickEnable(){ return CsC_AvObject::IsPickEnable()&&m_bEnableUse; }
	eNPC_RENDER_TYPE		GetNpcRenderType(){ return m_eNpcRenderType; }

protected:
	sCHAR_IMAGE*				m_pCharImage;
	CCharOption					m_Option;
	eNPC_RENDER_TYPE			m_eNpcRenderType;


	//====================================================================================
	//		Sound
	//====================================================================================
public:
	virtual void*		PlaySound( char* cSoundPath );

	//====================================================================================
	//		엔피씨 기능
	//====================================================================================
protected:
	bool				m_bEnableUse;

public:
	void				CheckQuestExtra();
	bool				CheckTamerDist(float Dist = IF_TARGET_TAMER_MAX_DIST);
	void				Try_Talk();
	void				Try_Yes();
	void				Try_No();
	void				SetOpenAnimation();
	void				SetCloseAnimation();


	void				SetEnableUse(bool bEnable);
	bool				IsEnableUse()const{ return m_bEnableUse; }
	

	//====================================================================================================
	//	퀘스트
	//====================================================================================================
public:
	enum eQUEST_VIEWTYPE{ QV_NONE, QV_NPCTALK, };
protected:
	eQUEST_VIEWTYPE						m_eQuestViewType;
	cData_QuestOwner::sNpcOwner*		m_pQuestOwner;

public:
	void							SetQuestOwner( cData_QuestOwner::sNpcOwner* pQuestOwner ){ m_pQuestOwner = pQuestOwner; }
	cData_QuestOwner::sNpcOwner*	GetQuestOwner(){ return m_pQuestOwner; }

	void							SetQuestViewType( eQUEST_VIEWTYPE qv ){ m_eQuestViewType = qv; }
	eQUEST_VIEWTYPE					GetQuestViewType(){ return m_eQuestViewType; }

public:
	nsCsNpcTable::eNPC_TYPE			GetNPCType();

};