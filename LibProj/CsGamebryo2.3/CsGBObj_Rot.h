

#pragma once 

class CsGBObj_Rot : public CsGBObject
{
	CSGBMEMPOOL_H( CsGBObj_Rot );

protected:
	bool			m_bLimit;
	bool			m_bPlus;

	float			m_fCurRot;

	sUDD_Rotation*	m_pUDDRotation;

public:
	CsGBObj_Rot();
	~CsGBObj_Rot();

	virtual eOBJ_TYPE	GetType(){ return OT_ROTATION; }
	virtual void		Init( NiNode* pNiNode, int nGetID, sINFO* pObjectInfo, sUSER_DEFINE* pUserDefine );
public:
	virtual void		Update( float fElapsedTime, bool bCulling = true );	

protected:
	void			_UpdateRotation( float fElapsedTime );
	void			_UpdateRotationLimit( float fElapsedTime );
};

