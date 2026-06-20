
#pragma once 

class CCharResMng : public NiMemObject
{
public:
	static	bool		CALLBACK_ThreadDelete( sTCUnit* pUnit );

public:
	CCharResMng();

public:
	void				Destroy();
	void				Init();
	void				Reset();				// 로딩시 호출
	void				PostLoadMap();	

	void				LoadChar( sTCUnit* pUnit );

	void				Update( cGlobalInput::eMOUSE_INPUT mi );
	void				Update_Tutorial( cGlobalInput::eMOUSE_INPUT mi );

	void				Render_PostTR();
	void				Render();
	//void				Render_Tutorial();
	void				Render_PostChar();

	void				ResetDevice();

	//=====================================================================================
	//
	//	케릭터 제거
	//
	//=====================================================================================
protected:
	std::queue< CsC_AvObject* >		m_queueDeleteReady;

public:
	void				DeleteChar( CsC_AvObject* pObject );
	void				ThreadDeleteChar( CsC_AvObject* pObject );
	void				InsertDeleteReady( CsC_AvObject* pObject );
protected:
	void				_UpdateDeleteReady();

	//=====================================================================================
	//
	//	마우스 온 오브젝트
	//
	//=====================================================================================
protected:
	CsC_AvObject*		m_pMouseOnLastObject;
	CsC_AvObject*		m_pMouseOnObject;
	float				m_fMouseOnObjectAlpha;
	float				m_fMouseOnObjectDimmer;

protected:
	void				_UpdateMouseOnObject( cGlobalInput::eMOUSE_INPUT mi );
	void				_UpdateMouseOnObject_Tutorial( cGlobalInput::eMOUSE_INPUT mi );

	void				_RenderMouseOnObject();
	void				_EndRenderMouseOnObject();
	void				_ReleaseMouseOnObject();

	//=====================================================================================
	//
	//	케릭터 이미지 로드
	//
	//=====================================================================================
protected:
	std::map< DWORD, sCHAR_IMAGE* >		m_mapCharImage;
	CsTimeSeq							m_CharImageRefCheckTimeSeq;

protected:
	void				_DeleteCharImage();
	void				_InitCharImage();
	void				_UpdateCharImage();
	void				_ResetDeviceCharImage();
	
public:
	void				CharImageResDelete( sCHAR_IMAGE** ppCharImage );
	sCHAR_IMAGE*		CharImageResLoad( const DWORD nModelID );	

	//=====================================================================================
	//
	//	디폴트 라이트 - 지형이 없을 경우에 쓰는 라이트
	//
	//=====================================================================================
protected:
	NiDirectionalLightPtr	m_pDefaultLight;

protected:
	void				_CreateDefaultLight();
	void				_DeleteDefaultLight();

public:
	NiDirectionalLight*		GetDefaultLight(){ return m_pDefaultLight; }

	//=====================================================================================
	//
	//	무브 포인트
	//
	//=====================================================================================
protected:
	CsNodeObj		m_MovePoint;
	bool			m_bMovePointRender;
	bool			m_bEnableMovePoint;
	NiPoint3		m_vMovePoint;
protected:
	void			_CreateMovePoint();
	void			_DeleteMovePoint();
	void			_RenderMovePoint();
public:
	void			SetMovePoint( NiPoint3 vMovePoint );
	void			ReleaseMovePoint();

	void			SetEnableMovePoint( bool bEnable );
	bool			IsEnableMovePoint();

	//=====================================================================================
	//
	//	타겟팅
	//
	//=====================================================================================
protected:
	enum eTARGET_MARK_TYPE{ TMT_NONE, TMT_RED, TMT_GREEN, };
protected:
	CsNodeObj			m_TargetMarkG;
	CsNodeObj			m_TargetMarkR;
	CsNodeObj			m_TargetMarkB;
	CsC_AvObject*		m_pTargetObject;
	eTARGET_MARK_TYPE	m_eTargetType;
	float				m_fKeepCenterHeight;

protected:
	void			_CreateTargetMark();
	void			_DeleteTargetMark();

	void			_UpdateTargetMark();
	void			_RenderTargetMark();

	void			_CheckTargetMark( CsC_Object* pObject );

public:
	void			SetTargetMark( CsC_AvObject* pObject, bool bOnEvent = true );
	void			ReleaseTargetMark( bool bAbsoluteRelease );

	CsC_AvObject*	GetTargetMark(){ return m_pTargetObject; }

	//=====================================================================================
	//
	//	Npc 마크
	//
	//=====================================================================================
protected:
	struct sNPC_MARK : public NiMemObject
	{
		CSGBMEMPOOL_H( CCharResMng::sNPC_MARK );

		int										s_nNpcIDX;		
		NiPoint3								s_vPos;		
		bool									s_bExistOwner;
		cData_QuestOwner::sNpcOwner::eREV_TYPE	s_eDiplayType;
	};
	CsVectorPB< sNPC_MARK* >	m_vpNpcMark;

	CsNodeObj		m_NpcMarkTalk;

	CsNodeObj		m_NpcMarkRecv_Main;
	CsNodeObj		m_NpcMarkRecv_Sub;
	CsNodeObj		m_NpcMarkRecv_Repeat;
	CsNodeObj		m_NpcMarkRecv_EventRepeat;
	CsNodeObj		m_NpcMarkLevelSoon;

	CsNodeObj		m_NpcMarkQustionG;
	CsNodeObj		m_NpcMarkQustionR;

	CsNodeObj		m_NpcMarkTrain;
	CsNodeObj		m_NpcMarkPortal;
protected:
	void			_DeleteNpcMark();
	void			_CreateNpcMark();

	void			_RenderNpcMark();

public:
	void			NpcMarkInsert( int nNpcIDX, NiPoint3 vPos, cData_QuestOwner::sNpcOwner::eREV_TYPE eDispType );

	//=====================================================================================
	//
	//	몬스터 생성 원
	//
	//=====================================================================================
protected:
	CsNodeObj		m_MonsterCreateScene;
	float			m_fMonsterCreateSceneAniLength;

	struct sMONSTER_CREATE_SCENE_INFO : public NiMemObject
	{
		NiPoint3	s_vPos;
		float		s_fAniTime;
		float		s_fScale;
	};
	std::stack< sMONSTER_CREATE_SCENE_INFO* >	m_stackMonsterCreateScenePool;
	std::list< sMONSTER_CREATE_SCENE_INFO* >	m_listMonsterCreateSceneInfo;

protected:
	void			_DeleteMonsterCreateScene();
	void			_CreateMonsterCreateScene();
	void			_ResetMonsterCreateScene();

	void			_RenderMonsterCreateScene();

public:
	void			MonsterCreateSceneInsert( NiPoint3 vPos, float fScale, CsC_AvObject* pTarget );

	//=====================================================================================
	//
	//	피규어 이미지
	//
	//=====================================================================================
public:
	struct sFIGURE_INFO : public NiMemObject
	{
		enum eFIGURETYPE
		{
			FT_TAMER, FT_DIGIMON, FT_EMPLOYMENT,FT_PAT,
		};

		int			s_nType;
		NiPoint3	s_vPos;
		float		s_fRot;
		float		s_fAniTime;
	};

protected:
	CsNodeObj		m_FigureTamerNode;
	CsNodeObj		m_FigureDigimonNode;
	CsNodeObj		m_FigureEmployment;
	CsNodeObj		m_FigurePatNode;
	std::stack< sFIGURE_INFO* >		m_stackFigurePool;
	CsVectorPB< sFIGURE_INFO* >		m_vpFigure;

protected:
	void			_DeleteFigure();
	void			_CreateFigure();
	void			_ResetFigure();
	void			_PostLoadMapFigure();

	void			_RenderFigure();

public:
	void			FigureInsert( NiPoint3 vPos, float fRot, float fAniTime, sFIGURE_INFO::eFIGURETYPE eFigureType );
};

extern CCharResMng*	g_pCharResMng;