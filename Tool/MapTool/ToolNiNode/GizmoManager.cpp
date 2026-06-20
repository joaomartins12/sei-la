#include "StdAfx.h"
#include "GizmoManager.h"

GIZMO::Manager::Manager(void)
:m_pCurrGizmo(NULL),
m_eCurrGizmo(GIZMO::END)
,mCurrCoord(GIZMO::WORLD)
,m_bRender(true)
,m_bUse(true)
,m_GizmoSense(50)
{
}

GIZMO::Manager::~Manager(void)
{
	Terminate();
}

void GIZMO::Manager::Initizlize(void)
{
	mGizmos.insert(GIZMO::Gizmos::value_type(GIZMO::TRANSLATE, new GIZMO::Translate()));
	mGizmos.insert(GIZMO::Gizmos::value_type(GIZMO::ROTATE, new GIZMO::Rotate()));
	mGizmos.insert(GIZMO::Gizmos::value_type(GIZMO::SCALE, new GIZMO::Scale()));
	mGizmos.insert(GIZMO::Gizmos::value_type(GIZMO::SNAP, new GIZMO::Snap()));

	GizmoIter kIter = mGizmos.begin();
	for(;kIter != mGizmos.end(); ++kIter){		
		kIter->second->Initizlize();
	}
	ChangeGizmo(GIZMO::TRANSLATE, NULL);
}
void GIZMO::Manager::Reset(void)
{
	ChangeGizmo(GIZMO::TRANSLATE, NULL);
}
void GIZMO::Manager::Update(const float& fDeltaTime)
{
	if(m_bUse == false)
		return;
	if(m_bRender == false)
		return;
	if(m_pCurrGizmo != NULL)
		m_pCurrGizmo->Update(fDeltaTime);
}
void GIZMO::Manager::Render(void)
{
	if(m_bUse == false)
		return;
	if(m_bRender == false)
		return;
	if(m_pCurrGizmo != NULL)
		m_pCurrGizmo->Render();
}
void GIZMO::Manager::Terminate(void)
{
	GizmoIter kIter = mGizmos.begin();
	for(;kIter != mGizmos.end();){		
		kIter->second->Terminate();
		SAFE_DELETE((kIter->second));
		kIter = mGizmos.erase(kIter);
	}
	mGizmos.clear();
}

bool GIZMO::Manager::ChangeGizmo(GIZMO::TYPE eType , EditAble* pkEditAble)
{
	if(m_bUse == false)
		return false;

	NiPoint3 kPRevPos = NiPoint3::ZERO;
	if(m_pCurrGizmo)
	{
		m_pCurrGizmo->OnHideGizmo();
		m_pCurrGizmo = NULL;
		m_eCurrGizmo = GIZMO::END;
		
	}
	GizmoIter kIter = mGizmos.find(eType);
	if(kIter != mGizmos.end())
	{
		
		m_eCurrGizmo = eType;
		m_pCurrGizmo = kIter->second;
		m_pCurrGizmo->OnChangeGizmoCoord(mCurrCoord, pkEditAble);
		return true;
	}
	return false;
}
void GIZMO::Manager::UpdateGizmoPos(EditAble* pkEditAble)
{
	//int SelectedObjCnt = g_ObjectMng.GetSelectModelCnt();
	int SelectedObjCnt = pkEditAble->GetSelectedCnt();
	if(SelectedObjCnt <= 0)
	{
		m_bRender = false;
		m_pCurrGizmo->OnHideGizmo();
	}
	if(SelectedObjCnt > 0 && 
		m_bUse == true && m_pCurrGizmo != NULL)
	{
		//m_pCurrGizmo->OnShowGizmo(	g_ObjectMng.GetCenterSelectedObj());
		m_pCurrGizmo->OnShowGizmo(	pkEditAble->GetSelectedCenter());
	}


}
bool GIZMO::Manager::OnMouseMove(const CPoint& pt, GIZMO::EditAble* pkEditable)
{
	if(m_bUse == true && m_pCurrGizmo != NULL)
	{
		m_pCurrGizmo->OnMouseMove(m_GizmoSense, pt, pkEditable);
	}
	return true;
}
bool GIZMO::Manager::OnLButtonDown(const CPoint& pt, GIZMO::EditAble* pkEditable)
{
	if(m_bUse == true && m_pCurrGizmo != NULL)
	{
		return m_pCurrGizmo->OnLButtonDown(pt, pkEditable);
	}
	return false;
}
void GIZMO::Manager::OnLButtonUp(const CPoint& pt, EditAble* pkEditAbl)
{
	if(m_bUse == true && m_pCurrGizmo != NULL)
	{
		m_pCurrGizmo->OnLButtonUp(pt,pkEditAbl);
	}
}
void GIZMO::Manager::OnObjectSelected(const NiPoint3& kPos)
{
	if(m_bUse == true && m_pCurrGizmo != NULL)
	{
		m_bRender = true;
		m_pCurrGizmo->OnObjectSelected(kPos);
	}
}

void GIZMO::Manager::SetGizmoSense(int Sense)
{
	m_GizmoSense = Sense;
}

void GIZMO::Manager::SetGizmoCoord(GIZMO::COORD eCoord, GIZMO::EditAble* pkEditable)
{
	mCurrCoord = eCoord;
	if(m_pCurrGizmo)
		m_pCurrGizmo->OnChangeGizmoCoord(eCoord, pkEditable);
}

void GIZMO::Manager::SetGizmoCoord(GIZMO::COORD iCoord)
{
	mCurrCoord = iCoord;
}