#include "StdAfx.h"
#include "ScaleGizmo.h"

GIZMO::Scale::Scale(void)
:m_bEdited(false)
{
}

GIZMO::Scale::~Scale(void)
{
	Terminate();
}

bool GIZMO::Scale::Initizlize(void)
{
	m_NifPath = ".\\ToolData\\scale.nif";
	NiStream kStream;
	bool bLoad = kStream.Load( m_NifPath.c_str() );
	if(bLoad == true)
	{
		m_spModel = (NiNode*)kStream.GetObjectAt( 0 );
		m_spModel->UpdateProperties();
		m_spModel->Update(0.0f);
		kStream.RemoveAllObjects();
		return true;
	}
	return false;
}

void GIZMO::Scale::Update(const float& fDeltaTime)
{
	if(m_spModel != NULL)
	{
		UpdateSize();
		m_spModel->Update(fDeltaTime);
	}
}
void GIZMO::Scale::Render(void)
{
	NiDrawScene( CAMERA_ST.GetCameraObj(), m_spModel, *g_pCuller );
}
void GIZMO::Scale::Terminate(void)
{
	m_spModel = 0;
}
void GIZMO::Scale::OnCheckPicked(const CPoint& kMousePt)
{
	if(m_spModel != NULL)
	{
		NiPoint3 origin, dir;
		CAMERA_ST.WindowPointToRay( kMousePt.x, kMousePt.y, origin, dir );
		CsGBPick pick;
		//if( pick.Pick_IsBound( m_spModel, origin, dir ) == false )
		//	return;	
		pick.Pick(m_spModel, origin, dir, false );
		NiPick::Record* pRecord = pick.GetRecord();
		if( pRecord != NULL )
		{
			NiFixedString SelectedName =  pRecord->GetAVObject()->GetName();
			if(SelectedName == "Box")
			{
				if(pRecord->GetAVObject()->GetParent() != NULL)
				{
					SelectedName =  pRecord->GetAVObject()->GetParent()->GetName();
				}
			}
			std::string kSelectedName =SelectedName; 
			if(m_SelectedAxisName != SelectedName)
			{
				ResetAxisColor();
				m_SelectedAxisName = SelectedName;
				ChangeAxisColor(m_SelectedAxisName);
			}
			kSelectedName+= "\n";
			//OutputDebugStringA(kSelectedName.c_str());
			return;
		}
	}
	if(m_SelectedAxisName != "")
	{
		ResetAxisColor();
		m_SelectedAxisName = "";
	}
}
void GIZMO::Scale::ChangeAxisColor(const NiFixedString& AxisName)
{
	NiAVObject* pkAxisColor = GetAxisProperty(AxisName);
	if(pkAxisColor != NULL)
	{
		NiMaterialProperty* pkMp =  (NiMaterialProperty*)pkAxisColor->GetProperty(NiProperty::MATERIAL);
		if(pkMp != NULL)
		{
			pkMp->SetAmbientColor(SelectedAxisColor);
			pkMp->SetDiffuseColor(SelectedAxisColor);
			pkMp->SetEmittance(SelectedAxisColor);
			pkAxisColor->UpdateProperties();
		}
	}
}
void GIZMO::Scale::ResetAxisColor(void)
{
	if(m_SelectedAxisName != "")
	{
		NiAVObject* pkAxisColor = GetAxisProperty(m_SelectedAxisName);
		if(pkAxisColor != NULL)
		{
			NiMaterialProperty* pkMp =  (NiMaterialProperty*)pkAxisColor->GetProperty(NiProperty::MATERIAL);
			if(pkMp != NULL)
			{
				NiColor ResetColor = GetAxisOriginColor(pkAxisColor->GetName());
				pkMp->SetAmbientColor(ResetColor);
				pkMp->SetDiffuseColor(ResetColor);
				pkMp->SetEmittance(ResetColor);
				pkAxisColor->UpdateProperties();
			}
		}
	}
}
const NiColor GIZMO::Scale::GetAxisOriginColor(NiFixedString Name )
{
	if(Name == "X_axis")
		return OriginAxisXColor;
	else if(Name == "Y_axis")
		return OriginAxisYColor;
	else if(Name == "Z_axis")
		return OriginAxisZColor;

	return NiColor::WHITE;
}
NiAVObject* GIZMO::Scale::GetAxisProperty(NiString kAxisName)//X_axis_pick
{
	if(m_spModel != NULL)
	{
		kAxisName.Replace("_pick", "");
		NiAVObject* pkSelectedAxis = m_spModel->GetObjectByName((NiFixedString)kAxisName);
		if(pkSelectedAxis != NULL)
		{
			if(pkSelectedAxis->GetParent() != NULL)
			{
				NiFixedString kName = pkSelectedAxis->GetName();
				if(kName == "X_axis" || kName == "Y_axis" || kName == "Z_axis")
					return pkSelectedAxis;
			}
		}
	}
	return NULL;
}
void GIZMO::Scale::UpdateSize(void)
{
	Billboard(m_spModel, 0.16f);
	NiPoint3 ToCam = CAMERA_ST.GetWorldLocation() - m_spModel->GetWorldTranslate();
	float fDist = NiPoint3::VectorLength( ToCam );
	float fScale = sqrt( fDist*GIZMOSIZE );
	m_spModel->SetScale(fScale);

}
void GIZMO::Scale::OnMouseMove(int iGizmoSensor, const CPoint& kMousePt, GIZMO::EditAble* pkEditAble)
{
	if(m_SelectedAxisName != "" && m_bEdited == true)
	{
		if(m_bTakeSnapshot == false)
		{
			LastCUID = pkEditAble->TakeUndoSnapshot(2);
			m_bTakeSnapshot =true;
		}
		MoveGizmo(kMousePt, pkEditAble);
	}
	else
	{
		OnCheckPicked(kMousePt);
	}
}
void GIZMO::Scale::OnLButtonUp(const CPoint& kMousePt, EditAble* pkEditAble)
{
	if(m_bTakeSnapshot == true)
	{
		pkEditAble->TakeRedoSnapshot(LastCUID);
	}
	m_StartMousePt = 0;
	m_bEdited = false;
	m_bTakeSnapshot = false;
	m_SelectedAxisName="";
}
bool GIZMO::Scale::OnLButtonDown(const CPoint& kMousePt, EditAble* pkEditAble)
{
	NiPoint3 origin, dir;
	CAMERA_ST.WindowPointToRay( kMousePt.x, kMousePt.y, origin, dir );
	CsGBPick pick;
	//if( pick.Pick_IsBound( m_spModel, origin, dir ) == false )
	//	return;	
	pick.Pick(m_spModel, origin, dir, false );
	NiPick::Record* pRecord = pick.GetRecord();
	if( pRecord != NULL )
	{
		m_bEdited = true;
		return true;
	}
	m_bEdited = false;
	m_StartMousePt = kMousePt;
	return false;

}
void GIZMO::Scale::MoveGizmo(const CPoint& kMousePt, GIZMO::EditAble* pkEditAble)
{
	if(kMousePt == m_StartMousePt)
		return;
	CPoint MoveAmount = kMousePt - m_StartMousePt;
	NiPoint3 StartPos = m_spModel->GetTranslate();
	NiPoint3 kAxisDir = GetSelectedAxisDir();
	if(kAxisDir == NiPoint3::ZERO)
		return;

	NiPoint3 EndPos = StartPos + kAxisDir;

	NiPoint3 StartScreenPos, EndScreenPos;
	StartScreenPos.z =0.0f;
	EndScreenPos.z = 0.0f;
	CsGBCamera::GetSingleton().GetCameraObj()->WorldPtToScreenPt(StartPos, StartScreenPos.x, StartScreenPos.y);
	CsGBCamera::GetSingleton().GetCameraObj()->WorldPtToScreenPt(EndPos, EndScreenPos.x, EndScreenPos.y);

	NiPoint3 vScreenObjDir = (EndScreenPos - StartScreenPos);
	NiPoint3::UnitizeVector(vScreenObjDir);

	NiPoint3 vMouseDir = NiPoint3(MoveAmount.x, MoveAmount.y,0.0f);
	vMouseDir.y *= -1.0f;
	NiPoint3::UnitizeVector(vMouseDir);

	float Acc = vMouseDir.Dot(vScreenObjDir);

	//m_spModel->SetTranslate(m_spModel->GetTranslate()+ kAxisDir * Acc * fSpeed);
	//CString kLog;
	//kLog.Format(_T(" Dot : [%f] \n"), Acc);
	//OutputDebugStringW(kLog);

	m_StartMousePt = kMousePt;
	pkEditAble->SetScaleByGizmo(kAxisDir, Acc);
}
NiPoint3 GIZMO::Scale::GetSelectedAxisDir()
{
	NiPoint3 SelectedAxis = NiPoint3::ZERO;
	if(m_SelectedAxisName == "X_axis_pick")
		SelectedAxis = GetWorldUpVector() * -1.0f;
	else if(m_SelectedAxisName == "Y_axis_pick")
		SelectedAxis = GetWorldDirection();
	else if(m_SelectedAxisName == "Z_axis_pick")
		SelectedAxis = GetWorldRightVector();
	else if(m_SelectedAxisName == "XZ_axis_pick")
		SelectedAxis = GetWorldRightVector() - GetWorldUpVector();
	else if(m_SelectedAxisName == "YZ_axis_pick")
		SelectedAxis = GetWorldRightVector() + GetWorldDirection();
	else if(m_SelectedAxisName == "XY_axis_pick")
		SelectedAxis = GetWorldDirection() - GetWorldUpVector();
	else
		SelectedAxis = NiPoint3::ZERO;

	NiPoint3::UnitizeVector(SelectedAxis);
	return SelectedAxis;
}
NiPoint3 GIZMO::Scale::GetWorldDirection()
{
	NiPoint3 kDVector;
	m_spModel->GetWorldTransform().m_Rotate.GetCol(0, kDVector);
	return kDVector;
}
//---------------------------------------------------------------------------
NiPoint3 GIZMO::Scale::GetWorldUpVector()
{
	NiPoint3 kUVector;
	m_spModel->GetWorldTransform().m_Rotate.GetCol(1, kUVector);
	return kUVector;
}
//---------------------------------------------------------------------------
NiPoint3 GIZMO::Scale::GetWorldRightVector()
{
	NiPoint3 kRVector;
	m_spModel->GetWorldTransform().m_Rotate.GetCol(2, kRVector);
	return kRVector;
}

void GIZMO::Scale::OnObjectSelected(const NiPoint3& kSelectedPos)
{
	if(m_spModel != NULL)
	{
		m_spModel->SetTranslate(kSelectedPos);
		m_spModel->Update(0.0f);
	}
}

void GIZMO::Scale::OnShowGizmo(const NiPoint3& kWorldPos)
{
	m_spModel->SetAppCulled(false);
	m_spModel->SetTranslate(kWorldPos);
	m_spModel->Update(0.0f);
}
void GIZMO::Scale::OnHideGizmo(void)
{
	m_spModel->SetAppCulled(true);
	m_spModel->SetTranslate(NiPoint3::ZERO);
	m_spModel->Update(0.0f);
}

void GIZMO::Scale::Billboard( NiAVObject* pNode, float fAniTime )
{
	NiPoint3 ModelFront = GetWorldDirection();
	NiPoint3 CameraFront = CAMERA_ST.GetWorldDirection() * 1.0f;
	ModelFront.z  =0.0f;
	CameraFront.z = 0.0f;

	float ModelDegree = FindDegree(ModelFront.x, ModelFront.y) ;
	float CamDegree = FindDegree(CameraFront.x, CameraFront.y) ;

	float ModelRad =(ModelDegree * NI_PI ) / 180.0f;
	float CamRad =(CamDegree * NI_PI ) / 180.0f;

	NiMatrix3 SelectedAxis = NiMatrix3::IDENTITY;
	int iRad = (int)((CamRad - ModelRad) / (NI_PI *0.5f));
	float FinalRad =  (NI_PI *0.5f) * (float)iRad;

	SelectedAxis.MakeZRotation(FinalRad);
	///
	NiTransform tr, trDest, trResult;
	tr.MakeIdentity();
	tr.m_Rotate = SelectedAxis;

	trDest = m_spModel->GetWorldTransform();
	trDest.m_Translate -= m_spModel->GetTranslate();
	trResult = tr*trDest;
	trResult.m_Translate += m_spModel->GetTranslate();

	m_spModel->SetLocalTransform(trResult);
	m_spModel->Update(0.0f);

}


float GIZMO::Scale::FindDegree(float x, float y)
{
	float value = (( atan2(x, y) / NI_PI) * 180.f);
	if (value < 0) value += 360.f;
	return value;
}
