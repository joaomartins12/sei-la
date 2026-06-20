


#pragma once 

class CsGBObjProp_AvCap : public CsGBObjProp
{
	CSGBMEMPOOL_H( CsGBObjProp_AvCap );

public:
	CsGBObjProp_AvCap();
	~CsGBObjProp_AvCap();

public:
	static CsGBObjProp*		GetInstance( CsGBObject* pParentObj );
	static CsGBObjProp*		GetInstance( NiNode* pParentNiNode );

protected:
	NiAVObject*				m_pNiAvObject;

public:
	virtual ePROP_TYPE		GetType(){ return PROP_AV_CAP; }

	virtual void			Delete();
	virtual void			Init( CsGBObject* pParentObj );
	virtual void			Init( NiNode* pParentNiNode );

public:
			void			Pick( CsGBPick* pCsPick, NiPoint3 origin, NiPoint3 dir, bool bAppend );
};

#include "CsGBObjProp_AvCap.inl"

