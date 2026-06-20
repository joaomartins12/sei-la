

#include "stdafx.h"
#include "ToolNiNodeMng.h"

cToolNiNodeMng g_ToolNiNodeMng;

cToolNiNodeMng::cToolNiNodeMng()
:m_pkGizmoManager(NULL),
m_pkDragRect(NULL)
{

}
cToolNiNodeMng::~cToolNiNodeMng()
{

}

void cToolNiNodeMng::Delete()
{
	m_AxisRotation.Delete();
	m_Region.Delete();
	SAFE_DELETE(m_pkGizmoManager);
	SAFE_DELETE(m_pkDragRect);
}

void cToolNiNodeMng::Init()
{
	m_AxisRotation.Init();
	m_Region.Init();
	m_pkGizmoManager = new GIZMO::Manager();
	m_pkGizmoManager->Initizlize();
	m_pkDragRect= new GIZMO::DragRect();
	m_pkDragRect->Initizlize();
}

void cToolNiNodeMng::Update()
{
	m_AxisRotation.Update();
	m_pkGizmoManager->Update(0.0f);
	if(GET_MV_MAINTAB_DLG->_GetDlgObejct()->IsWindowVisible() == TRUE)
		m_pkDragRect->Update(0.0f);
}

void cToolNiNodeMng::Render()
{

}

void cToolNiNodeMng::RenderClearZBuffer()
{
	m_AxisRotation.Render();
	m_pkGizmoManager->Render();
	if(GET_MV_MAINTAB_DLG->_GetDlgObejct()->IsWindowVisible() == TRUE)
		m_pkDragRect->Render();
}
void cToolNiNodeMng::OnKeyUp(const DWORD kMsg,GIZMO::EditAble* pkEditable)
{
	switch(kMsg)
	{
	case 'W':
		m_pkGizmoManager->ChangeGizmo(GIZMO::TRANSLATE, pkEditable);
		break;
	case 'E':
		m_pkGizmoManager->ChangeGizmo(GIZMO::ROTATE, pkEditable);
		break;
	case 'R':
		m_pkGizmoManager->ChangeGizmo(GIZMO::SCALE, pkEditable);
		break;
	case 'S':
		//m_pkGizmoManager->ChangeGizmo(GIZMO::SNAP);
		break;
	case 'L':
		g_ObjectMng.LockSelectedObj();
		break;
	case 'J':
		g_ObjectMng.DeployOnTerrain();
		break;

	default:
		break;
	}
	m_pkGizmoManager->UpdateGizmoPos(pkEditable);

}

bool cToolNiNodeMng::LButtonDown( CPoint pt , GIZMO::EditAble* pkEditable)
{
	if(m_pkGizmoManager != NULL)
	{
		bool Suc= m_pkGizmoManager->OnLButtonDown(pt, pkEditable);
		if(Suc == true)
			return true;
	}
	if(m_AxisRotation.m_bUsingOldRotation == true)
	{
		bool Suc = m_AxisRotation.LButtonDown( pt );
		if(Suc == true)
			return true;
	}

	if(GET_MV_MAINTAB_DLG->_GetDlgObejct()->IsWindowVisible() == TRUE)
		m_pkDragRect->OnLButtonDown(pt, pkEditable);

	return false;
}

void cToolNiNodeMng::LButtonUp( CPoint pt , GIZMO::EditAble* pkEditable)
{
	if(m_pkGizmoManager != NULL)
	{
		m_pkGizmoManager->OnLButtonUp(pt, pkEditable);
	}
	m_AxisRotation.LButtonUp( pt );
	
	if(GET_MV_MAINTAB_DLG->_GetDlgObejct()->IsWindowVisible() == TRUE)
		m_pkDragRect->OnLButtonUp(pt, pkEditable);
}

void cToolNiNodeMng::MouseMove( CPoint pt , GIZMO::EditAble* pkEditable)
{
	m_AxisRotation.MouseMove( pt );
	if(m_pkGizmoManager != NULL)
	{
		m_pkGizmoManager->OnMouseMove(pt, pkEditable);
	}
	if(GET_MV_MAINTAB_DLG->_GetDlgObejct()->IsWindowVisible() == TRUE)
		m_pkDragRect->OnMouseMove(0, pt, pkEditable);
}


void cToolNiNodeMng::OnObjectSelected(const NiPoint3& kSelectedPos, GIZMO::EditAble* pkEditable)
{
	if(m_pkGizmoManager != NULL)
	{
		m_pkGizmoManager->OnObjectSelected(kSelectedPos);
		UpdateGizmoPos(pkEditable);
	}
}
void	cToolNiNodeMng::OnStartObjectEdit(int iCoord)
{
	m_AxisRotation.SetUse(false);
	if(m_pkGizmoManager != NULL)
	{
		m_pkGizmoManager->SetGizmoCoord((GIZMO::COORD)iCoord);
		m_pkGizmoManager->SetUse(true);
	}
}
void	cToolNiNodeMng::OnStartObjectEdit(void)
{
	m_AxisRotation.SetUse(false);
	if(m_pkGizmoManager != NULL)
	{
		m_pkGizmoManager->SetUse(true);
	}
}

void cToolNiNodeMng::OnEndObjectEdit(void)
{
	m_AxisRotation.SetUse(true);
	if(m_pkGizmoManager != NULL)
		m_pkGizmoManager->SetUse(false);
}

void cToolNiNodeMng::OnStartPathEdit(void)
{
	m_AxisRotation.SetUse(false);
	if(m_pkGizmoManager != NULL)
	{
		m_pkGizmoManager->SetUse(true);
	}
}
void cToolNiNodeMng::OnResetGizmo(void)
{
	if(m_pkGizmoManager != NULL)
		m_pkGizmoManager->SetUse(false);
}
void	cToolNiNodeMng::UpdateGizmoPos(GIZMO::EditAble* pkEditable)
{
	if(m_pkGizmoManager != NULL)
		m_pkGizmoManager->UpdateGizmoPos(pkEditable);

}
void	cToolNiNodeMng::OnChangeGizmoEditSense(int CurrValue)
{
	if(m_pkGizmoManager != NULL)
		m_pkGizmoManager->SetGizmoSense(CurrValue);;

}

void	cToolNiNodeMng::OnChangeGizmoCoord(int iCoord, GIZMO::EditAble* pkEditable)
{
	if(m_pkGizmoManager != NULL)
		m_pkGizmoManager->SetGizmoCoord((GIZMO::COORD)iCoord, pkEditable);

}