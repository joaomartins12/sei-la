
#pragma once 

class CsGBObjPropFactor : public NiMemObject
{
	CSGBMEMPOOL_H( CsGBObjPropFactor )

public:
	CsGBObjPropFactor();
	~CsGBObjPropFactor();

protected:
	std::map< DWORD, CsGBObjProp* >		m_mapProp;
public:
	void			Delete();

	CsGBObjProp*	InsertProp( CsGBObjProp::ePROP_TYPE eType, CsGBObject* pParentObject );
	CsGBObjProp*	InsertProp( CsGBObjProp::ePROP_TYPE eType, NiNode* pParentNiNode );

	void			Update( float fElapsedTime, float fAniTime );

	CsGBObjProp*	GetProp( CsGBObjProp::ePROP_TYPE eType );
	bool			IsProp( CsGBObjProp::ePROP_TYPE eType );

};

#include "CsGBObjPropFactor.inl"

