
#pragma once 

class CsC_AvObject;
class CsC_Property : public NiMemObject
{
public:
	enum eTYPE
	{
		PATH		=	0,
		ANIMATION,
		ALPHA,
		ATTACK,
		EFFECT,
		MOVE_CONTROL,
		MAX_COUNT,
	};

public:
	CsC_Property();
	virtual ~CsC_Property();

	//=========================================================================================
	//		Instance 
	//=========================================================================================
public:
	static void					ShotDown();

	static CsC_Property*		NewProp( eTYPE type );
	static void					PushProp( CsC_Property* pProp );	

	//=========================================================================================
	//		Parent
	//=========================================================================================
protected:
	CsC_AvObject*				m_pParent;

	//=========================================================================================
	//		Type
	//=========================================================================================
protected:
	eTYPE		m_eType;
public:
	eTYPE		GetType(){ return m_eType; }


	//=========================================================================================
	//		Base Func
	//=========================================================================================
public:
	virtual void	Delete(){ m_pParent = NULL; }
	virtual void	PreDelete(){}

	virtual void	ChangeKFM(){}
	virtual void	Init( CsC_AvObject* pParent ){ m_pParent = pParent; }
	virtual void	CheckModelComp(){}	

	virtual void	PreUpdate( float fDeltaTime ){}
	virtual void	Update( float fDeltaTime ){}
	virtual void	Render(){}	
};