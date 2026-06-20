#include "StdAfx.h"
#include "RotateGizmo.h"
#include <float.h>
using namespace GIZMO;
GIZMO::Rotate::Rotate(void)
:m_bEdited(false)
{
	
}

GIZMO::Rotate::~Rotate(void)
{
}
bool GIZMO::Rotate::Initizlize(void)
{
	m_NifPath = ".\\ToolData\\rotation.nif";
	NiStream kStream;
	bool bLoad = kStream.Load( m_NifPath.c_str() );
	if(bLoad == true)
	{
		m_spModel = (NiNode*)kStream.GetObjectAt( 0 );

		NiZBufferPropertyPtr spZBufferPtr = NiNew NiZBufferProperty();
		spZBufferPtr->SetZBufferTest(true);
		spZBufferPtr->SetZBufferWrite(false);
		spZBufferPtr->SetTestFunction(NiZBufferProperty::TEST_LESSEQUAL);

		NiStencilPropertyPtr spStencilProp = NiNew NiStencilProperty();
		spStencilProp->SetStencilFunction(NiStencilProperty::TEST_GREATER);
		spStencilProp->SetStencilFailAction(NiStencilProperty::ACTION_KEEP);
		spStencilProp->SetStencilPassAction(NiStencilProperty::ACTION_INCREMENT);
		spStencilProp->SetDrawMode(NiStencilProperty::DRAW_CCW);

		NiAVObject* pkXAxis = m_spModel->GetObjectByName("X_axis");
		pkXAxis->AttachProperty(spZBufferPtr);
		pkXAxis->AttachProperty(spStencilProp);
		pkXAxis->UpdateProperties();
		NiAVObject* pkYAxis = m_spModel->GetObjectByName("Y_axis");
		pkYAxis->AttachProperty(spZBufferPtr);
		pkYAxis->AttachProperty(spStencilProp);
		pkYAxis->UpdateProperties();
		NiAVObject* pkZAxis = m_spModel->GetObjectByName("Z_axis");
		pkZAxis->AttachProperty(spZBufferPtr);
		pkZAxis->AttachProperty(spStencilProp);
		pkZAxis->UpdateProperties();

		m_spModel->UpdateProperties();
		m_spModel->Update(0.0f);
		kStream.RemoveAllObjects();

		bLoad = kStream.Load( ".\\ToolData\\axisPlane.nif" );
		if( bLoad == false )
		{
			MessageBox( NULL, L".\\ToolData\\axisPlane.nif 가 존재 하지 않습니다. 비정상적으로 종료 될수 있습니다.", NULL, MB_OK );
			return false;
		}
		m_pAxisPlaneNiNode = (NiNode*)kStream.GetObjectAt( 0 );
		m_pAxisPlaneNiNode->UpdateProperties();
		kStream.RemoveAllObjects();
		return true;
	}
	return false;
}
void GIZMO::Rotate::Update(const float& fDeltaTime)
{
	if(m_spModel != NULL)
	{
		UpdateGizmos();
	}
}
void GIZMO::Rotate::Render(void)
{	
	NiDrawScene( CAMERA_ST.GetCameraObj(), m_pAxisPlaneNiNode, *g_pCuller );
	NiDrawScene( CAMERA_ST.GetCameraObj(), m_spModel, *g_pCuller );

}
void GIZMO::Rotate::Terminate(void)
{
	m_pAxisPlaneNiNode=0;
}
void GIZMO::Rotate::OnCheckPicked(const CPoint& kMousePt)
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
			if(SelectedName == "Tube")
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
void GIZMO::Rotate::ChangeAxisColor(const NiFixedString& AxisName)
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
void GIZMO::Rotate::ResetAxisColor(void)
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
const NiColor GIZMO::Rotate::GetAxisOriginColor(NiFixedString Name )
{
	if(Name == "X_axis")
		return OriginAxisXColor;
	else if(Name == "Y_axis")
		return OriginAxisYColor;
	else if(Name == "Z_axis")
		return OriginAxisZColor;

	return NiColor::WHITE;
}
NiAVObject* GIZMO::Rotate::GetAxisProperty(NiString kAxisName)//X_axis_pick
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


void GIZMO::Rotate::UpdateGizmos(void)
{
	NiPoint3 toTarget = CAMERA_ST.GetWorldLocation() - m_spModel->GetWorldTranslate();
	switch(mGizmoCoord)
	{
	case WORLD:
		{
			NiMatrix3 kInit;
			kInit.SetRow(0, 1.0f, 0.0f, 0.0f);
			kInit.SetRow(1, 0.0f, 1.0f, 0.0f);
			kInit.SetRow(2, 0.0f, 0.0f, 1.0f);
			LookAt(m_spModel, kInit);

		}
		break;
	case MODEL:
		if(m_pkCurrEditable != NULL)
		{
			NiMatrix3 kSelectedRot = m_pkCurrEditable->GetSelectedRot();
			LookAt(m_spModel, kSelectedRot);
		}
	}

	float fDist = NiPoint3::VectorLength( CAMERA_ST.GetWorldLocation() - m_spModel->GetWorldTranslate() );
	float fScale = sqrt( fDist*GIZMOSIZE );
	m_spModel->SetScale(fScale);
	m_spModel->Update( 0.0f );
	if(m_pAxisPlaneNiNode != NULL)
	{
		m_pAxisPlaneNiNode->SetScale(fScale);
		m_pAxisPlaneNiNode->SetTranslate(m_spModel->GetWorldTranslate() + CAMERA_ST.GetWorldDirection()*10.0f*fScale );
		m_pAxisPlaneNiNode->Update( 0.0f );	

	}

}
void GIZMO::Rotate::OnMouseMove(int iGizmoSensor, const CPoint& kMousePt, GIZMO::EditAble* pkEditAble)
{
	if(m_SelectedAxisName != "" && m_bEdited == true)
	{
		if(m_bTakeSnapshot == false)
		{
			LastCUID = pkEditAble->TakeUndoSnapshot(1);
			m_bTakeSnapshot =true;
		}
		RotateGizmo(kMousePt, pkEditAble);
	}
	else
	{
		OnCheckPicked(kMousePt);
	}
}
void GIZMO::Rotate::OnLButtonUp(const CPoint& kMousePt, EditAble* pkEditAble)
{
	if(m_bTakeSnapshot == true)
	{
		pkEditAble->TakeRedoSnapshot(LastCUID);
	}
	m_Rotation = NiMatrix3::IDENTITY;
	m_bEdited = false;;
	m_bTakeSnapshot = false;
	ResetAxisColor();
	m_SelectedAxisName ="";
	m_StartMousePt = 0;
}
bool GIZMO::Rotate::OnLButtonDown(const CPoint& kMousePt, EditAble* pkEditAble)
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
		m_StartMousePt = kMousePt;
		return true;
	}
	m_StartMousePt = kMousePt;
	m_bEdited = false;
	return false;
}

NiPoint3 GIZMO::Rotate::GetSelectedAxisDir()
{
	NiPoint3 SelectedAxis = NiPoint3::ZERO;
	if(m_SelectedAxisName == "X_axis")
		SelectedAxis = GetWorldUpVector();
	else if(m_SelectedAxisName == "Y_axis")
		SelectedAxis = GetWorldDirection();
	else if(m_SelectedAxisName == "Z_axis")
		SelectedAxis = GetWorldRightVector();
	else
		SelectedAxis = NiPoint3::ZERO;

	NiPoint3::UnitizeVector(SelectedAxis);
	return SelectedAxis;
}
NiMatrix3 GIZMO::Rotate::GetSelectedAxisRotate(const float fAngle )
{
	NiCamera* pkCamera = CAMERA_ST.GetCameraObj();
	NiPoint3 kCamDir = pkCamera->GetWorldDirection();
	NiString kAxisName = m_SelectedAxisName;
	kAxisName.Replace("_pick", "");
	NiMatrix3 SelectedAxis = NiMatrix3::IDENTITY;
	NiPoint3 kModelDir = NiPoint3::ZERO;
	float fFinalAngle = fAngle;
	if(mGizmoCoord == WORLD)
	{
		if(kAxisName == "X_axis")
		{
			m_spModel->GetWorldTransform().m_Rotate.GetCol(1, kModelDir);
			if(kModelDir.Dot(kCamDir) < 0 )			{
				fFinalAngle *= -1.0f;
			}

			SelectedAxis.MakeYRotation(fFinalAngle);
		}
		else if(kAxisName == "Y_axis")
		{
			m_spModel->GetWorldTransform().m_Rotate.GetCol(0, kModelDir);
			if(kModelDir.Dot(kCamDir) < 0 )			{
				fFinalAngle *= -1.0f;
			}
			SelectedAxis.MakeXRotation(fFinalAngle);
		}
		else if(kAxisName == "Z_axis")
		{
			SelectedAxis.MakeZRotation(-fFinalAngle);
		}
	}
	return SelectedAxis;
 

}
NiPoint3 GIZMO::Rotate::GetWorldDirection()
{
	NiPoint3 kDVector;
	m_spModel->GetWorldTransform().m_Rotate.GetCol(0, kDVector);
	return kDVector;
}
NiPoint3 GIZMO::Rotate::GetWorldUpVector()
{
	NiPoint3 kUVector;
	m_spModel->GetWorldTransform().m_Rotate.GetCol(1, kUVector);
	return kUVector;
}
//---------------------------------------------------------------------------
NiPoint3 GIZMO::Rotate::GetWorldRightVector()
{
	NiPoint3 kRVector;
	m_spModel->GetWorldTransform().m_Rotate.GetCol(2, kRVector);
	return kRVector;
}

void GIZMO::Rotate::OnObjectSelected(const NiPoint3& kSelectedPos)
{
	if(m_spModel != NULL)
	{
		m_spModel->SetTranslate(kSelectedPos);
		m_spModel->Update(0.0f);
	}
}
void GIZMO::Rotate::RotateGizmo(const CPoint& kMousePt, GIZMO::EditAble* pkEditAble)
{
	NiPoint3 StartModelTranslate= m_spModel->GetTranslate();
	NiPoint3 kRotateCenter = NiPoint3::ZERO;
	CsGBCamera::GetSingleton().GetCameraObj()->WorldPtToScreenPt(StartModelTranslate, kRotateCenter.x, kRotateCenter.y);
	kRotateCenter.x = kRotateCenter.x * VIEWER_WIDTH;
	kRotateCenter.y = (1.0f - kRotateCenter.y) * VIEWER_HEIGHT;
	NiPoint3 kStartMousePt = NiPoint3(m_StartMousePt.x, m_StartMousePt.y, 0.0f);
	NiPoint3 kCurrMousePt = NiPoint3(kMousePt.x, kMousePt.y, 0.0f);
	NiPoint3 vStart = kStartMousePt - kRotateCenter; 
	NiPoint3 vCurr = kCurrMousePt - kRotateCenter;
	float StartDegree = FindDegree(vStart.x, vStart.y) ;
	float CurrDegree = FindDegree(vCurr.x, vCurr.y) ;

	StartDegree =(StartDegree * NI_PI ) / 180.0f;
	CurrDegree =(CurrDegree * NI_PI ) / 180.0f;

	if( mGizmoCoord == GIZMO::WORLD)
	{
		NiMatrix3 kRot =	GetSelectedAxisRotate(CurrDegree - StartDegree);
		pkEditAble->SetRotateByGizmo(kRot);
	}
	else if( mGizmoCoord == GIZMO::MODEL )
	{
		pkEditAble->SetRotateByGizmo(pkEditAble->GetSelectedCenter(), mGizmoCoord, m_SelectedAxisName, CurrDegree - StartDegree );
	}
	m_StartMousePt = kMousePt;
}

void GIZMO::Rotate::OnShowGizmo(const NiPoint3& kWorldPos)
{
	m_Rotation = NiMatrix3::IDENTITY;
	m_spModel->SetAppCulled(false);
	m_spModel->SetTranslate(kWorldPos);
	m_spModel->Update(0.0f);
}
void GIZMO::Rotate::OnHideGizmo(void)
{
	m_Rotation = NiMatrix3::IDENTITY;
	m_spModel->SetAppCulled(true);
	m_spModel->SetTranslate(NiPoint3::ZERO);
	m_spModel->Update(0.0f);
}
float GIZMO::Rotate::GetInnerRadian(NiPoint3 v1, NiPoint3 v2)
{
	NiPoint3 vN1 = v1;
	NiPoint3 vN2 = v2;
	NiPoint3::UnitizeVector(vN1);
	NiPoint3::UnitizeVector(vN2);
	if(vN1 == vN2)
		return 0.0f;
	float Radian = acosf(vN1.Dot(vN2));
	if( _isnan(Radian) == true)
		return 0.0f;

	return Radian;
}

float GIZMO::Rotate::FindDegree(float x, float y)
{
	float value = (( atan2(x, y) / NI_PI) * 180.f);
	if (value < 0) value += 360.f;
	return value;
}
void GIZMO::Rotate::LookAt(NiAVObject* pNode, const NiMatrix3& kFront)
{
	////CString fRotateAngle;
	////fRotateAngle.Format(_T("Angle [%f] \n"), fAngle);
	////OutputDebugString(fRotateAngle);

	//NiPoint3 TargetDir = kFront;
	//NiPoint3::UnitizeVector(TargetDir);
	//NiPoint3 TargetProjX = TargetDir;
	//TargetProjX.z = 0.0f;
	//NiPoint3::UnitizeVector(TargetProjX);

	//NiMatrix3 kZRot;
	//kZRot.MakeZRotation(NI_PI * 0.5f);
	//NiPoint3 TargetRight = kZRot * TargetProjX;
	//NiPoint3::UnitizeVector(TargetRight);

	//NiPoint3 TargetUp =TargetDir.Cross(TargetRight) * -1.0f;
	//NiPoint3::UnitizeVector(TargetUp);
	//TargetRight = TargetUp.Cross(TargetDir);
	//NiPoint3::UnitizeVector(TargetRight);
	//// get parent world transforms
	//NiNode* pkParent = pNode->GetParent();
	//NiCamera* pkCamera = CAMERA_ST.GetCameraObj();

	NiMatrix3 kFaceMat = kFront;//NiMatrix3::IDENTITY;
	//kFaceMat.SetCol(0, TargetDir);
	//kFaceMat.SetCol(1, TargetRight);
	//kFaceMat.SetCol(2, TargetUp);
/////////////////////////////////
	NiTransform tr, trDest, trResult;
	tr.MakeIdentity();
	tr.m_Rotate = kFaceMat;
	
	trDest = pNode->GetWorldTransform();
	trDest.m_Rotate = kFaceMat;//pkCamera->GetWorldTransform().m_Rotate;
	trDest.m_Translate -= pNode->GetTranslate();
	trResult = tr;//*trDest;
	trResult.m_Translate += pNode->GetTranslate();
	trResult.m_fScale = trDest.m_fScale;
	pNode->SetLocalTransform(trResult);
	pNode->Update(0.0f);
}

