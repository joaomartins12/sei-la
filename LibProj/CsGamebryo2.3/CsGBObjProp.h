
#pragma once 

class CsGBObjProp : public NiMemObject
{
public:
	CsGBObjProp();
	virtual ~CsGBObjProp();

public:
	enum ePROP_TYPE
	{
		PROP_ALPHA	=	1,
		PROP_ABV,
		PROP_AV_CAP,
		PROP_AV_TBP,
		PROP_SOUND,
		PROP_ACTOR,
	};
protected:
	CsGBObject*			m_pParentObj;
	NiNode*				m_pParentNiNode;

public:
	static void			DeleteProp( CsGBObjProp* pDeleteProp );

public:
	virtual ePROP_TYPE	GetType() = 0;

	virtual void		Delete();
	virtual void		Init( CsGBObject* pParentObj );
	virtual void		Init( NiNode* pParentNiNode );

	virtual void		Update( float fElapsedTime, float fAniTime ){}

public:
	CsGBObject*			GetParent(){ return m_pParentObj; }	
};