
#pragma once 

#define AXIS_ROTATION_OBJECT_PATH			".\\ToolData\\axis.nif"
#define AXIS_ROTATION_PLANE_OBJECT_PATH		".\\ToolData\\axisPlane.nif"

class cAxisRotation
{
public:
	cAxisRotation();

typedef std::list< NiNode* >			LIST_DEST_NINODE;
typedef std::list< NiNode* >::iterator	LIST_DEST_NINODE_IT;

public:
	enum eAXIS_ROTATION{ AR_Z, AR_Y, AR_X };
	enum eAXIS_TYPE{ AT_OBJECT, AT_CELL, AT_NPC, };

	// Base Func
protected:
	bool			m_bRender;	

public:
	void			Delete();
	void			Init();

	void			Update();
	void			Render();	

	// Tool NiNode
protected:
	eAXIS_ROTATION	m_eAR;
	eAXIS_TYPE		m_eAT;
	bool			m_bClickAxis;
	NiNodePtr		m_pAxisNiNode;
	NiNodePtr		m_pAxisPlaneNiNode;

protected:
	void			_SetDefaultEmittance();
	void			_SetEnumAxisRotation( NiColor emittance );
	NiAVObject*		_PickAxisToRenderAxis( NiAVObject* pPickAxis );
	void			_PickAxis( CPoint pt );
	void			_RotAxis( CPoint pt );

public:
	void			ShowOnlyZAxis();
	void			ShowAllAxis();

	// Dest Node
protected:
	LIST_DEST_NINODE m_listDestNiNode;
public:
	NiTransform		InsertVisible( NiNode* pDestNiNode, eAXIS_TYPE at, cObjectModel* pObjectModel = NULL );
	NiTransform		RemoveVisible( NiNode* pDestNiNode );
	bool			IsVisible(){ return ( m_listDestNiNode.empty() == false ); }
	void			ReleaseVisible();
	
	// Transform Node
protected:
	NiPoint3		m_ptPosition;
	NiMatrix3		m_matRotation;
protected:
	void			_TRApplyDestNiNode();
public:
	void			CalTRNodeTransform();	
	NiPoint3		GetPosition(){ return m_ptPosition; }
	void			Get_RotAxis( CPoint pt) { return _RotAxis( pt ); }

	// Mouse
protected:	
	CPoint			m_ptLButtonDown;
public:
	void			LButtonUp( CPoint pt );
	bool			LButtonDown( CPoint pt );
	void			MouseMove( CPoint pt );

	bool			m_bUsingOldRotation;
	void SetUse(bool TOP){m_bUsingOldRotation = TOP;}

};