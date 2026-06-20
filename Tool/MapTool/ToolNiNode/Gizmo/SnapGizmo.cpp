#include "StdAfx.h"
#include "SnapGizmo.h"
#include "../../Path/PathManager.h"
GIZMO::Snap::Snap(void)
:m_pkLastPickNode(NULL)
{
}

GIZMO::Snap::~Snap(void)
{
	Terminate();
}

bool GIZMO::Snap::Initizlize(void)
{
	m_NifPath = ".\\ToolData\\snap.nif";
	NiStream kStream;
	bool bLoad = kStream.Load( m_NifPath.c_str() );
	if(bLoad == true)
	{
		m_spModel = (NiNode*)kStream.GetObjectAt( 0 );
		m_spModel->UpdateProperties();
		m_spModel->Update(0.0f);
		kStream.RemoveAllObjects();

		m_NifPath = ".\\ToolData\\snap.nif";
		bool bLoad = kStream.Load( m_NifPath.c_str() );
		if(bLoad == true)
		{
			m_spLastPT = (NiNode*)kStream.GetObjectAt( 0 );
			m_spLastPT->UpdateProperties();
			m_spLastPT->Update(0.0f);
			kStream.RemoveAllObjects();
			return true;
		}
	}


	return false;
}
void GIZMO::Snap::Update(const float& fDeltaTime)
{
	if(m_spModel != NULL || m_spLastPT != NULL)
	{
		UpdateSize(fDeltaTime);
		m_spModel->Update(fDeltaTime);
		m_spLastPT->Update(fDeltaTime);

	}
}
void GIZMO::Snap::UpdateSize(const float& fDeltaTime)
{
	NiPoint3 ToCam = CAMERA_ST.GetWorldLocation() - m_spModel->GetWorldTranslate();
	float fDist = NiPoint3::VectorLength( ToCam );
	float fScale = sqrt( fDist*GIZMOSIZE );
	m_spModel->SetScale(fScale);
	m_spLastPT->SetScale(fScale);
}

void GIZMO::Snap::Render(void)
{
	NiDrawScene( CAMERA_ST.GetCameraObj(), m_spModel, *g_pCuller );
	NiDrawScene( CAMERA_ST.GetCameraObj(), m_spLastPT, *g_pCuller );
}
void GIZMO::Snap::Terminate(void)
{
m_spLastPT = 0;
}

void GIZMO::Snap::OnMouseMove(int iGizmoSensor, const CPoint& kMousePt, EditAble* pkEditAble)
{
	if(m_pkLastPickNode != NULL)
	{
		NiPick::Record* pkCurrRecord = pkEditAble->SnapPickingObj(kMousePt);
		if(pkCurrRecord != NULL)
		{
			if(m_bTakeSnapshot == false)
			{
				LastCUID = pkEditAble->TakeUndoSnapshot(1);
				m_bTakeSnapshot =true;
			}

			NiNode* pkCurrParent = NULL;
			if(pkEditAble == PATH::Manager::GetSingletonPtr())
				pkCurrParent = (NiNode*)pkCurrRecord->GetAVObject();
			else
				pkCurrParent = nsCSGBFUNC::GetRootNiNode( pkCurrRecord->GetAVObject() );

			if(m_pkLastPickNode == pkCurrParent)
				return;

			NiTriStrips* pTestNode = (NiTriStrips*)pkCurrRecord->GetAVObject();
			if(pTestNode != NULL)
			{
				NiPoint3* pkVertices = pTestNode->GetVertices();
				unsigned short v1, v2, v3;
				pkCurrRecord->GetVertexIndices(v1, v2, v3);

				NiPoint3 kModelCenter = pTestNode->GetWorldTranslate();
				NiPoint3 kVPos1 = pkVertices[v1];
				NiPoint3 kVPos2= pkVertices[v2];
				NiPoint3 kVPos3 = pkVertices[v3];
				NiCamera* pkNiCam = CAMERA_ST.GetCameraObj();
				if(pkNiCam != NULL)
				{		
					float fv1X, fv1Y, fv2X, fv2Y, fv3X, fv3Y;
					float CurrPtX = kMousePt.x / 800.0f;
					float CurrPtY = (1.0f - kMousePt.y / 600.0f);
					pkNiCam->WorldPtToScreenPt(kVPos1+ kModelCenter, fv1X, fv1Y);
					pkNiCam->WorldPtToScreenPt(kVPos2+ kModelCenter, fv2X, fv2Y);
					pkNiCam->WorldPtToScreenPt(kVPos3+ kModelCenter, fv3X, fv3Y);

					float fDist1 = (CurrPtX-fv1X) * (CurrPtX-fv1X) +  (CurrPtY-fv1Y) * (CurrPtY-fv1Y);
					float fDist2 = (CurrPtX-fv2X) * (CurrPtX-fv2X) +  (CurrPtY-fv2Y) * (CurrPtY-fv2Y);
					float fDist3 = (CurrPtX-fv3X) * (CurrPtX-fv3X) +  (CurrPtY-fv3Y) * (CurrPtY-fv3Y);
					NiPoint3 CurrNearPt = NiPoint3::ZERO;
					if( fDist1 < fDist2 && fDist1 < fDist3)
						CurrNearPt = kVPos1;
					else if( fDist2 < fDist1 && fDist2 < fDist3)
						CurrNearPt = kVPos2;
					else if( fDist3 < fDist1 && fDist3 < fDist2)
						CurrNearPt = kVPos3;
					m_spLastPT->SetTranslate(kModelCenter+CurrNearPt);
					m_spLastPT->Update(0.0f);

					if(pkEditAble == PATH::Manager::GetSingletonPtr())
					{
						NiPoint3 kGap = (CurrNearPt - LastNearPt);
						NiPoint3 kGap2 = (CurrNearPt+ kModelCenter) - (LastNearPt + LastLocalPos);
						if(m_pkLastPickNode != NULL)
						{
							NiPoint3 MovedModelCenter = LastPos + kGap;
							pkEditAble->SetSnapByGizmo(MovedModelCenter);
						}
					}
					else
					{
						NiPoint3 kGap = (CurrNearPt+ kModelCenter) - (LastNearPt + LastLocalPos);
						if(m_pkLastPickNode != NULL)
						{
							NiPoint3 FinalMoveAmount = LastPos + kGap;
							pkEditAble->SetSnapByGizmo(FinalMoveAmount);
						}
					}
				}
			}
		}
	}

}
void GIZMO::Snap::OnLButtonUp(const CPoint& kMousePt, EditAble* pkEditAble)
{
	if(m_bTakeSnapshot == true)
	{
		pkEditAble->TakeRedoSnapshot(LastCUID);
	}

	m_bTakeSnapshot = false;
	m_pkLastPickNode = NULL;
	LastNearPt = NiPoint3::ZERO;
	LastPos = NiPoint3::ZERO;
	LastLocalPos = NiPoint3::ZERO;
	pkEditAble->DeleteModel();
}
bool GIZMO::Snap::OnLButtonDown(const CPoint& kMousePt, EditAble* pkEditAble)
{
	NiPick::Record* pkRecord  = pkEditAble->SnapPickingObj(kMousePt);
	if(pkRecord != NULL)
	{
		NiTriStrips* pTestNode = (NiTriStrips*)pkRecord->GetAVObject();
		if(pTestNode != NULL)
		{
			NiPoint3* pkVertices = pTestNode->GetVertices();
			unsigned short v1, v2, v3;
			pkRecord->GetVertexIndices(v1, v2, v3);

			NiPoint3 kModelCenter = pTestNode->GetWorldTranslate();
			LastLocalPos = pTestNode->GetWorldTranslate();
			NiPoint3 kVPos1 = pkVertices[v1];
			NiPoint3 kVPos2= pkVertices[v2];
			NiPoint3 kVPos3 = pkVertices[v3];
			NiCamera* pkNiCam = CAMERA_ST.GetCameraObj();
			if(pkNiCam != NULL)
			{		
				float fv1X, fv1Y, fv2X, fv2Y, fv3X, fv3Y;
				float CurrPtX = kMousePt.x / 800.0f;
				float CurrPtY = (1.0f - kMousePt.y / 600.0f);
				pkNiCam->WorldPtToScreenPt(kVPos1+ kModelCenter, fv1X, fv1Y);
				pkNiCam->WorldPtToScreenPt(kVPos2+ kModelCenter, fv2X, fv2Y);
				pkNiCam->WorldPtToScreenPt(kVPos3+ kModelCenter, fv3X, fv3Y);

				float fDist1 = (CurrPtX-fv1X) * (CurrPtX-fv1X) +  (CurrPtY-fv1Y) * (CurrPtY-fv1Y);
				float fDist2 = (CurrPtX-fv2X) * (CurrPtX-fv2X) +  (CurrPtY-fv2Y) * (CurrPtY-fv2Y);
				float fDist3 = (CurrPtX-fv3X) * (CurrPtX-fv3X) +  (CurrPtY-fv3Y) * (CurrPtY-fv3Y);
				LastNearPt = NiPoint3::ZERO;
				if( fDist1 < fDist2 && fDist1 < fDist3)
					LastNearPt = kVPos1;
				else if( fDist2 < fDist1 && fDist2 < fDist3)
					LastNearPt = kVPos2;
				else if( fDist3 < fDist1 && fDist3 < fDist2)
					LastNearPt = kVPos3;


				if(pkEditAble == PATH::Manager::GetSingletonPtr())
				{
					m_pkLastPickNode = (NiNode*)pkRecord->GetAVObject();
					LastPos = PATH::Manager::GetSingletonPtr()->GetSelectedModelCenter();
				}
				else
				{
					m_pkLastPickNode = nsCSGBFUNC::GetRootNiNode( pkRecord->GetAVObject() );
					LastPos = m_pkLastPickNode->GetTranslate();
				}


				m_spModel->SetTranslate(LastNearPt+ kModelCenter);
				m_spModel->Update(0.0f);
				return false;
			}
		}
	}
	return false;
}
void GIZMO::Snap::OnCheckPicked(const CPoint& kMousePt)
{
}
void GIZMO::Snap::OnObjectSelected(const NiPoint3& kMousePt)
{

}
void GIZMO::Snap::OnShowGizmo(const NiPoint3& kWorldPos)
{

}
void GIZMO::Snap::OnHideGizmo()
{
	m_spModel->SetAppCulled(true);
	m_spModel->SetTranslate(NiPoint3::ZERO);
	m_spModel->Update(0.0f);
}