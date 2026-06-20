


#pragma once 

class CsGBObjProp_AvTbp : public CsGBObjProp
{
	CSGBMEMPOOL_H( CsGBObjProp_AvTbp );

public:
	CsGBObjProp_AvTbp();
	~CsGBObjProp_AvTbp();

public:
	static CsGBObjProp*		GetInstance( CsGBObject* pParentObj );
	static CsGBObjProp*		GetInstance( NiNode* pParentNiNode );

protected:
	NiAVObject*				m_pNiAvObject;

public:
	virtual ePROP_TYPE		GetType(){ return PROP_AV_TBP; }

	virtual void			Delete();
	virtual void			Init( CsGBObject* pParentObj );
	virtual void			Init( NiNode* pParentNiNode );

public:
			void			Pick( CsGBPick* pCsPick, NiPoint3 origin, NiPoint3 dir, bool bAppend );
};

#include "CsGBObjProp_AvTbp.inl"

