
#pragma once 

#include "AxisRotation.h"
#include "Region.h"
#include "ToolNiNode/GizmoManager.h"
#include "ToolNiNode/Editable.h"
#include "ToolNiNode/Gizmo/DragRect.h"
class cToolNiNodeMng
{
public:
	cAxisRotation		m_AxisRotation;
	cRegion				m_Region;
	GIZMO::Manager*	m_pkGizmoManager;
	GIZMO::DragRect*			m_pkDragRect;
public:
	void		Delete();
	void		Init();

	void		Update();
	void		Render();
	void		RenderClearZBuffer();

public:
	bool		LButtonDown( CPoint pt , GIZMO::EditAble* pkEditable);
	void		LButtonUp( CPoint pt , GIZMO::EditAble* pkEditable);	
	void		MouseMove( CPoint pt, GIZMO::EditAble* pkEditable);
	void		OnKeyUp(const DWORD kMsg, GIZMO::EditAble* pkEditable);
	void		OnObjectSelected(const NiPoint3& kSelectedPos, GIZMO::EditAble* pkEditable);
	void		UpdateGizmoPos(GIZMO::EditAble* pkEditable);
	void		OnStartObjectEdit(int iCoord);
	void		OnStartObjectEdit(void);
	void		OnEndObjectEdit(void);
	void		OnStartPathEdit(void);
	void		OnResetGizmo(void);
	void		OnChangeGizmoEditSense(int CurrValue);
	void		OnChangeGizmoCoord(int iCoord, GIZMO::EditAble* pkEditable);
public:
	cAxisRotation*		GetAxisRotation(){ return &m_AxisRotation; }
	cRegion*			GetRegion(){ return &m_Region; }

	cToolNiNodeMng();
	~cToolNiNodeMng();
};

extern cToolNiNodeMng g_ToolNiNodeMng;