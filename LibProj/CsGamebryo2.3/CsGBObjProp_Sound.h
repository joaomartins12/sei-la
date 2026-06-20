


#pragma once 

class CsGBObjProp_Sound : public CsGBObjProp
{
	CSGBMEMPOOL_H( CsGBObjProp_Sound );

public:
	CsGBObjProp_Sound();
	~CsGBObjProp_Sound();

public:
	static CsGBObjProp*		GetInstance( CsGBObject* pParentObj );
	static CsGBObjProp*		GetInstance( NiNode* pParentNiNode );

public:
	virtual ePROP_TYPE		GetType(){ return PROP_SOUND; }

	virtual void			Delete();
	virtual void			Init( CsGBObject* pParentObj );
	virtual void			Init( NiNode* pParentNiNode );

	virtual void			Update( float fElapsedTime, float fAniTime );

protected:
	char					m_cPath[ MAX_FILENAME ];
	float					m_fAniTime;
	float					m_fLoopTime;
	float					m_fMinDist;
	float					m_fMaxDist;
};

