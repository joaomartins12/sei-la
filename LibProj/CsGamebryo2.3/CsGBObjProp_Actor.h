
#pragma once 

#pragma once 

class CsGBObjProp_Actor : public CsGBObjProp
{
	CSGBMEMPOOL_H( CsGBObjProp_Actor );

public:
	CsGBObjProp_Actor();
	~CsGBObjProp_Actor();

public:
	static CsGBObjProp*		GetInstance( CsGBObject* pParentObj );
	static CsGBObjProp*		GetInstance( NiNode* pParentNiNode );

public:
	virtual ePROP_TYPE		GetType(){ return PROP_ACTOR; }

	virtual void			Delete();
	virtual void			Init( CsGBObject* pParentObj );
	virtual void			Init( NiNode* pParentNiNode );

	virtual void			Update( float fElapsedTime, float fAniTime );


public:
	void					InitActor( char* cNifPath, sUDD_ACTOR* pUDD );
	char*					GetKey(){ return m_cKey; }
	void					SetAnimation( DWORD nID ){ m_pActorMng->SetTargetAnimation( nID ); }
	// 인던 문 오브젝트 위해 추가 chu8820
	DWORD					GetAnimation(){ return m_pActorMng->GetTargetAnimation(); }

	enum eACT_ANIMATION
	{
		ACT_ANIMATION1	=	1,	// 애니 1 ( Door : 닫힘 Idle )
		ACT_ANIMATION2	=	2,	// 애니 2 ( Door : 열림 )
		ACT_ANIMATION3	=	3,	// 애니 3 ( Door : 열림 Idle )
	};
protected:
	NiActorManagerPtr		m_pActorMng;
	char					m_cKey[ 32 ];
};


