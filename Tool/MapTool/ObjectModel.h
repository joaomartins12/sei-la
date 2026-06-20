
#pragma once 

class cObjectModel
{
public:
	cObjectModel();
protected:
	CsGBObject::sINFO	m_ObjectInfo;
	NiNodePtr			m_pNiNode;

	float				m_fAniTime;	
	NiTransform			m_trOrigin;
	
	NiPoint3			m_ptGroupOffset;
	NiPoint3			m_ptGroupPos;
	NiMatrix3			m_matRotation;
	bool				m_bEditedLock;
	//NiColor			m_Origin;
	//NiColor			m_Locking;
public:	
	void			Delete();
	void			DeleteSelect();
	void			CancelSelect();
	void			Create( CsGBObject::sINFO* pObjectInfo, bool bRender, float fAniTime, float fOpacity );
	void			Create( float fOpacity, CsGBObject::sINFO* pObjectInfo, bool bRender);


public:
	void			Update();
	void			Render();

	CsGBObject::sINFO*	GetObjectInfo();
	NiNode*				GetNiNode(){ return m_pNiNode; }	
	void			SetLockObject(bool TOF){m_bEditedLock = TOF;}
	inline bool			isLocked(void){return m_bEditedLock;}

	//====== Render()
protected:
	bool			m_bRender;
public:
	void			SetRender( bool bRender );

	//===== state
public:
	float			GetAniTime(){ return m_fAniTime; }

	//====== Transform
public:
	void			SetTransform( NiTransform tr );

	void			SetGroupTranslate( NiPoint3 pos, bool bOffSnap );
	void			SetGroupRotation( NiMatrix3 mat );
	void			SetGroupRotationZ( float fAngle );
	void			SetGroupOffsetPos( NiPoint3 ptOffset );	

	void			SetTranslateByGizmo(const NiPoint3& kPos);
	void			SetSanpByGizmo(const NiPoint3& kSnapPos);
	void			SetRotateByGizmo(const NiPoint3& kCenter, const NiMatrix3& kRot);
	void			SetRotateByGizmo(const NiPoint3& kCenterPos, int iCoord, NiFixedString kAxis, float fAngle);
	void			SetScaleByGizmo(const float fScale);

	void			ResetTransformSelectObject();
	void			AttachTerrain();

	//====== Resist
public:
	CsGBObject*		ResistObject();
	bool				ResistObject(NiTransform kTr);
	CsGBObject*		ResistBuffRegion(const NiPoint3& kPos, const float fScale);

	//====== Object State
public:
	enum eOBJECT_STATE
	{
		OS_DISABLE, OS_READY, OS_RESIST,
	};

protected:
	eOBJECT_STATE	m_eObjectState;
public:
	void			SetObjectState( eOBJECT_STATE os ){ m_eObjectState = os; }
	eOBJECT_STATE	GetObjectState(){ return m_eObjectState; }
	

	//====== Child
protected:
	std::map< DWORD, cObjectModel* >		m_mapChild;
	int				m_nNiChildIndex;
	int				m_nChildSlotID;

public:
	void			SetNiChildIndex( int nChildIndex ){ m_nNiChildIndex = nChildIndex; }

	void			SetChildSlotID( int nSlotID ){ m_nChildSlotID = nSlotID; }
	int				GetChildSlotID(){ return m_nChildSlotID; }

	bool			ResetChildPos( cObjectModel* pParent );

	cObjectModel*	GetChild( int nSlotID );
};