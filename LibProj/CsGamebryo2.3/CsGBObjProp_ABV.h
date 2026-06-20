
#pragma once 

class CsGBObjProp_ABV : public CsGBObjProp
{
	CSGBMEMPOOL_H( CsGBObjProp_ABV );

public:
	CsGBObjProp_ABV();
	~CsGBObjProp_ABV();

public:
	static CsGBObjProp*		GetInstance( CsGBObject* pParentObj );
	static CsGBObjProp*		GetInstance( NiNode* pParentNiNode );

public:
	virtual ePROP_TYPE		GetType(){ return PROP_ABV; }

	virtual void			Delete();
	virtual void			Init( CsGBObject* pParentObj );
	virtual void			Init( NiNode* pParentNiNode );

protected:
	NiBoundingVolume*		m_pNiBoundingVolume;

public:
	inline NiBoundingVolume* GetABV(){
		if( m_pNiBoundingVolume != NULL )
			return m_pNiBoundingVolume;
		return NiGetCollisionData( m_pParentNiNode )->GetWorldSpaceABV();
	}
	inline bool				TestIntersect( float fElapsedTime, NiBoundingVolume* pNiBoundingVolume )
	{
		return NiBoundingVolume::TestIntersect( fElapsedTime, *GetABV(), NiPoint3::ZERO, *pNiBoundingVolume, NiPoint3::ZERO );
	}
};

