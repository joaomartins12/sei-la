
#pragma once 

class CsGBObjProp_Alpha : public CsGBObjProp
{
	CSGBMEMPOOL_H( CsGBObjProp_Alpha );

public:
	CsGBObjProp_Alpha();
	~CsGBObjProp_Alpha();

public:
	enum eALPHA_TYPE{ AT_NONE, AT_ALPHA_INCREASE, AT_ALPHA_DECREASE };
	
	// ======= CsProperty
protected:
	nsCSGBFUNC::CsBackupProperty	m_CsBackupProp;

	eALPHA_TYPE				m_eAlphaType;
	float					m_fCurAlphaValue;	

public:
	static float			m_fDestAlphaValue;
	static float			m_fDeltaAlphaValue;
	static float			m_fDestAlphaTime;

	static CsGBObjProp*		GetInstance( CsGBObject* pParentObj );
	static CsGBObjProp*		GetInstance( NiNode* pParentNiNode );
public:
	virtual ePROP_TYPE		GetType(){ return PROP_ALPHA; }

	virtual void			Delete();
	virtual void			Init( CsGBObject* pParentObj );
	virtual void			Init( NiNode* pParentNiNode );

	virtual void			Update( float fElapsedTime, float fAniTime );
public:
	inline void				SetAlphaBlend( bool bAlpha );
	float					GetAlphaValue(){ return m_fCurAlphaValue; }
};

#include "CsGBObjProp_Alpha.inl"

