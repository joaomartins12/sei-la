
#pragma once

class CsGBObj_Portal : public CsGBObject
{
	CSGBMEMPOOL_H( CsGBObj_Portal );

public:
	CsGBObj_Portal();
	~CsGBObj_Portal();

public:
	struct sTARGET
	{
		sTARGET(){ Reset(); }
		void Reset(){ s_ptCenterPos = NiPoint2::ZERO; s_fRadius = 500.0f; }
		NiPoint2		s_ptCenterPos;
		float			s_fRadius;
	};
protected:
	DWORD			m_dwPortalType;
	DWORD			m_dwPortalTypeID;

	DWORD			m_dwTargetMapID;
	sTARGET			m_SrcTarget;
	sTARGET			m_DestTarget;
	NiPoint2		m_vViewTarget;

public:
	virtual eOBJ_TYPE	GetType(){ return OT_PORTAL; }

	virtual void		Delete();
	virtual void		Init( NiNode* pNiNode, int nGetID, sINFO* pObjectInfo, sUSER_DEFINE* pUserDefine );

	// ==== Portal Data
public:
	void				SetPortalID( DWORD dwPortalID ){ m_dwPortalTypeID = dwPortalID; }
	void				SetTargetMapID( DWORD dwTargetMapID ){ m_dwTargetMapID = dwTargetMapID; }
	DWORD				GetPortalID(){ return m_dwPortalTypeID; }
	DWORD				GetTargetMapID(){ return m_dwTargetMapID; }
	void				SetPortalType( DWORD dwPortalType ){ m_dwPortalType = dwPortalType; }
	DWORD				GetPortalType(){ return m_dwPortalType; }
	void				CalSrcTarget();
	sTARGET*			GetSrcTarget(){ return &m_SrcTarget; }
	sTARGET*			GetDestTarget(){ return &m_DestTarget; }

	NiPoint2			GetViewTarget(){ return m_vViewTarget; }
	void				SetViewTarget( NiPoint2 vView ){ m_vViewTarget = vView; }

	// ==== Extra Data
public:
	static UINT			SelfSaveSize();
	virtual UINT		GetSaveSize();
	virtual bool		SaveExtraData( char* pData, UINT& uiOffset );
	virtual void		LoadExtraData( char* pData, UINT& uiOffset, DWORD dwCheckPlag );
};

