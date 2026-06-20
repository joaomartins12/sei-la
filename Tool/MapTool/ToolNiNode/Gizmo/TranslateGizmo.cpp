#include "StdAfx.h"
#include "TranslateGizmo.h"

GIZMO::Translate::Translate(void)
:m_bEdited(false)
{

}

GIZMO::Translate::~Translate(void)
{
}

bool GIZMO::Translate::Initizlize(void)
{
	m_NifPath = ".\\ToolData\\translate.nif";
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

void GIZMO::Translate::Update(const float& fDeltaTime)
{
	if(m_spModel != NULL)
	{
		UpdateCoord();
		UpdateSize();
		m_spModel->Update(fDeltaTime);
	}
}
void GIZMO::Translate::Render(void)
{
	NiDrawScene( CAMERA_ST.GetCameraObj(), m_spModel, *g_pCuller );
}
void GIZMO::Translate::Terminate(void)
{
	m_spModel = 0;
}


void GIZMO::Translate::OnCheckPicked(const CPoint& kMousePt)
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
			if(SelectedName == "Editable Poly")
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
			m_LastIntersection = pRecord->GetIntersection();
			m_LastGizmoTrans = m_spModel->GetTranslate();
			return;
		}
	}
	if(m_SelectedAxisName != "")
	{
		ResetAxisColor();
		m_SelectedAxisName = "";
	}
}
void GIZMO::Translate::ChangeAxisColor(const NiFixedString& AxisName)
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
void GIZMO::Translate::ResetAxisColor(void)
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
const NiColor GIZMO::Translate::GetAxisOriginColor(NiFixedString Name )
{
	if(Name == "X_axis")
		return OriginAxisXColor;
	else if(Name == "Y_axis")
		return OriginAxisYColor;
	else if(Name == "Z_axis")
		return OriginAxisZColor;

	return NiColor::WHITE;
}
NiAVObject* GIZMO::Translate::GetAxisProperty(NiString kAxisName)//X_axis_pick
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
void GIZMO::Translate::UpdateCoord(void)
{
	NiPoint3 toTarget = CAMERA_ST.GetWorldLocation() - m_spModel->GetWorldTranslate();
	switch(mGizmoCoord)
	{
	case WORLD:
		LookAt(m_spModel, NiPoint3(1.0f,0.0f,0.0f));
		break;
	case MODEL:
		if(m_pkCurrEditable != NULL)
		{
			NiPoint3 kSelectedFront = m_pkCurrEditable->GetSelectedFront();
			LookAt(m_spModel, kSelectedFront);
		}

		break;
	case CAM:
		toTarget = m_spModel->GetWorldTranslate() - CAMERA_ST.GetWorldLocation();
		LookAt(m_spModel, CAMERA_ST.GetCameraObj()->GetWorldUpVector());
		break;
	}
}
void GIZMO::Translate::UpdateSize(void)
{
	float fDist = NiPoint3::VectorLength( CAMERA_ST.GetWorldLocation() - m_spModel->GetWorldTranslate() );
	float fScale = sqrt( fDist*GIZMOSIZE );
	m_spModel->SetScale(fScale);
}

void GIZMO::Translate::OnMouseMove(int iGizmoSensor, const CPoint& kMousePt, GIZMO::EditAble* pkEditAble)
{
	if(m_SelectedAxisName != "" && m_bEdited == true)
	{
		if(m_bTakeSnapshot == false)
		{
			LastCUID = pkEditAble->TakeUndoSnapshot(0);
			m_bTakeSnapshot =true;
		}
		MoveGizmo(iGizmoSensor, kMousePt, pkEditAble);
	}
	else
	{
		OnCheckPicked(kMousePt);
	}
}
void GIZMO::Translate::OnLButtonUp(const CPoint& kMousePt, EditAble* pkEditAble)
{
	if(m_bTakeSnapshot == true)
	{
		pkEditAble->TakeRedoSnapshot(LastCUID);
	}

	m_StartMousePt = 0;
	m_bEdited = false;
	m_bTakeSnapshot = false;
	ResetAxisColor();
	m_SelectedAxisName = "";

	m_LastIntersection = NiPoint3::ZERO;
	m_LastGizmoTrans = NiPoint3::ZERO;
}
bool GIZMO::Translate::OnLButtonDown(const CPoint& kMousePt, EditAble* pkEditAble)
{
	m_StartMousePt = kMousePt;

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

	return false;
}
void GIZMO::Translate::MoveGizmo(int iSense, const CPoint& kMousePt, GIZMO::EditAble* pkEditAble)
{
	if(kMousePt == m_StartMousePt)
		return;
	CPoint MoveAmount = kMousePt - m_StartMousePt;
	NiPoint3 vMouseDir = NiPoint3(MoveAmount.x, MoveAmount.y,0.0f);
	vMouseDir.y *= -1.0f;
	NiPoint3::UnitizeVector(vMouseDir);

	NiPoint3 StartPos = m_spModel->GetTranslate();
	NiPoint3 kAxisDir = GetSelectedAxisDir(kMousePt, pkEditAble);
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

	float Acc = vMouseDir.Dot(vScreenObjDir);
	const float fSpeed = iSense;

	//CString fRotateAngle;
	//fRotateAngle.Format(_T("MouseDir.x[%f] MouseDir.x[%f] ACC[%f] vScreenObjDir.x[%f] vScreenObjDir.y[%f]\n"), 
	//	vMouseDir.x, vMouseDir.y, Acc,vScreenObjDir.x,vScreenObjDir.y );
	//OutputDebugString(fRotateAngle);

	m_spModel->SetTranslate(m_spModel->GetTranslate()+ kAxisDir * Acc * fSpeed);
	m_spModel->Update(0.0f);
	m_StartMousePt = kMousePt;
	pkEditAble->SetTranslateByGizmo(kAxisDir * Acc * fSpeed);
}
NiPoint3 GIZMO::Translate::GetSelectedAxisDir(CPoint kMousePT, GIZMO::EditAble* pkEditAble)
{
	NiPoint3 SelectedAxis = NiPoint3::ZERO;
	if(m_SelectedAxisName == "X_axis_pick")
		SelectedAxis = GetWorldUpVector(m_spModel);
	else if(m_SelectedAxisName == "Y_axis_pick")
		SelectedAxis = GetWorldDirection(m_spModel);
	else if(m_SelectedAxisName == "Z_axis_pick")
		SelectedAxis = GetWorldRightVector(m_spModel);
	else
	{
		CsGBPick kPick;
		NiPoint3 origin, dir;
		CAMERA_ST.WindowPointToRay( kMousePT.x, kMousePT.y, origin, dir );
		kPick.Pick(m_spModel, origin, dir, false );
		NiPick::Record* pRecord = kPick.GetRecord();
		if( pRecord != NULL )
		{
			m_PrevGizmoTrans = m_spModel->GetTranslate();
			NiPoint3 Gap = (pRecord->GetIntersection() - m_LastIntersection);
			NiPoint3 FinalPos= m_LastGizmoTrans;
			if(m_SelectedAxisName == "XZ_axis_pick")
			{
				FinalPos.y = m_LastGizmoTrans.y + Gap.y;
				FinalPos.z = m_LastGizmoTrans.z + Gap.z;
				Gap.x = 0.0f;

			}
			else if(m_SelectedAxisName == "YZ_axis_pick")
			{
				FinalPos.x = m_LastGizmoTrans.x + Gap.x;
				FinalPos.z = m_LastGizmoTrans.z + Gap.z;
				Gap.y = 0.0f;
			}
			else if(m_SelectedAxisName == "XY_axis_pick")
			{
				FinalPos.x = m_LastGizmoTrans.x + Gap.x;
				FinalPos.y = m_LastGizmoTrans.y + Gap.y;
				Gap.z = 0.0f;
			}
			NiPoint3 kPrevGap = FinalPos - m_PrevGizmoTrans;
			m_spModel->SetTranslate(FinalPos);
			m_spModel->Update(0.0f);
			pkEditAble->SetTranslateByGizmo(kPrevGap, true, true); 

			//CString fRotateAngle;
			//fRotateAngle.Format(_T("CurrPos.x[%f].y[%f] Gap.x[%f].y[%f] Final.x[%f].y[%f]\n"), 
			//	m_PrevGizmoTrans.x, m_PrevGizmoTrans.y, Gap.x,Gap.y,FinalPos.x,FinalPos.y );
			//OutputDebugString(fRotateAngle);
		}
	}
	NiPoint3::UnitizeVector(SelectedAxis);
	return SelectedAxis;
}
NiPoint3 GIZMO::Translate::GetWorldDirection(NiAVObject* pkAVObject)
{
	NiPoint3 kDVector;
	pkAVObject->GetWorldTransform().m_Rotate.GetCol(0, kDVector);
	return kDVector;
}
//---------------------------------------------------------------------------
NiPoint3 GIZMO::Translate::GetWorldUpVector(NiAVObject* pkAVObject)
{
	NiPoint3 kUVector;
	pkAVObject->GetWorldTransform().m_Rotate.GetCol(1, kUVector);
	return kUVector;
}
//---------------------------------------------------------------------------
NiPoint3 GIZMO::Translate::GetWorldRightVector(NiAVObject* pkAVObject)
{
	NiPoint3 kRVector;
	pkAVObject->GetWorldTransform().m_Rotate.GetCol(2, kRVector);
	return kRVector;
}

void GIZMO::Translate::OnObjectSelected(const NiPoint3& kSelectedPos)
{
	if(m_spModel != NULL)
	{
		m_spModel->SetTranslate(kSelectedPos);
		m_spModel->Update(0.0f);
	}
}

void GIZMO::Translate::OnShowGizmo(const NiPoint3& kWorldPos)
{
	//m_LastIntersection = NiPoint3::ZERO;
	m_spModel->SetAppCulled(false);
	m_spModel->SetTranslate(kWorldPos);
	m_spModel->Update(0.0f);
}
void GIZMO::Translate::OnHideGizmo(void)
{
	m_LastIntersection = NiPoint3::ZERO;
	m_spModel->SetAppCulled(true);
	m_spModel->SetTranslate(NiPoint3::ZERO);
	m_spModel->Update(0.0f);
}

void GIZMO::Translate::LookAt(NiAVObject* pkFrom, const NiPoint3& kTargetDir)
{
	NiPoint3 ModelFront = GetWorldDirection(pkFrom);
	NiPoint3 CameraFront = kTargetDir;
	NiPoint3::UnitizeVector(CameraFront);
	ModelFront.z  =0.0f;
	CameraFront.z = 0.0f;

	float ModelDegree = FindDegree(ModelFront.x, ModelFront.y) ;
	float CamDegree = FindDegree(CameraFront.x, CameraFront.y) ;

	float ModelRad =(ModelDegree * NI_PI ) / 180.0f;
	float CamRad =(CamDegree * NI_PI ) / 180.0f;
	float FinalRad =  CamRad - ModelRad;

	NiMatrix3 SelectedAxis = NiMatrix3::IDENTITY;
	SelectedAxis.MakeZRotation(FinalRad);
	///
	NiTransform tr, trDest, trResult;
	tr.MakeIdentity();
	tr.m_Rotate = SelectedAxis;

	trDest = pkFrom->GetWorldTransform();
	trDest.m_Translate -= pkFrom->GetTranslate();
	trResult = tr*trDest;
	trResult.m_Translate += pkFrom->GetTranslate();
	pkFrom->SetLocalTransform(trResult);
	pkFrom->Update(0.0f);

}


const NiPoint3 GIZMO::Translate::RotateVector(const NiPoint3& kFrom, const NiPoint3& kTo)
{
	NiPoint3 CameraFront = kTo;
	NiPoint3 Target = kFrom;
	NiPoint3::UnitizeVector(CameraFront);
	Target.z  =0.0f;
	CameraFront.z = 0.0f;

	float ModelDegree = FindDegree(Target.x, Target.y) ;
	float CamDegree = FindDegree(CameraFront.x, CameraFront.y) ;

	float ModelRad =(ModelDegree * NI_PI ) / 180.0f;
	float CamRad =(CamDegree * NI_PI ) / 180.0f;
	float FinalRad =  CamRad - ModelRad;

	NiMatrix3 SelectedAxis = NiMatrix3::IDENTITY;
	SelectedAxis.MakeZRotation(FinalRad);
	///
	NiTransform tr, trDest, trResult;
	tr.MakeIdentity();
	tr.m_Rotate = SelectedAxis;

	NiPoint3::UnitizeVector(Target);

	NiPoint3 RotatedV = tr *  Target;
	return RotatedV ; 
	
}

float GIZMO::Translate::FindDegree(float x, float y)
{
	float value = (( atan2(x, y) / NI_PI) * 180.f);
	if (value < 0) value += 360.f;
	return value;
}
