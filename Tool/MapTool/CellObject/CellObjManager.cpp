#include "StdAfx.h"
#include "CellObjManager.h"
#include "../ToolNiNode/UndoManager.h"
CELLOBJ::Manager::Manager(void)
{
}
CELLOBJ::Manager::~Manager(void)
{
}
void CELLOBJ::Manager::Init(void)
{
}
void CELLOBJ::Manager::Update(const float& fDeltatime)
{
}
void CELLOBJ::Manager::Render(void)
{
}
void CELLOBJ::Manager::InitAddCellObj(CsGBCell::sINFO* pkCellInfo)
{
	//int CurrSize  = m_AllCellObjs.size();
	//AllCellObjIter kIter = m_AllCellObjs.find(CurrSize);
	//if(kIter == m_AllCellObjs.end())
	//{
	//	m_AllCellObjs.insert(AllCellObj::value_type(CurrSize, pkCellInfo));
	//}
}

void CELLOBJ::Manager::CreateAddCellObj(NiAVObject* pkAVObject)
{
}
void CELLOBJ::Manager::ResetAllPaths(void)
{
	m_AllCellObjs.clear();
}
void CELLOBJ::Manager::ResetSelected(void)
{
	m_SelectedCellObjs.clear();
}
void CELLOBJ::Manager::AddSelected(int iKindIdx, CELLOBJ::INFO kInfo)
{
	AllCellObjIter kIter = m_SelectedCellObjs.find(iKindIdx);
	if(kIter == m_SelectedCellObjs.end())
	{
		m_SelectedCellObjs.insert(AllCellObj::value_type(iKindIdx, kInfo));

		NiNode* pBaseObject = nsCsGBTerrain::g_pCurRoot->GetSpeedCell()->GetBaseObject( kInfo.Kind )->s_Node.m_pNiNode;
		CsGBCell* pCell = nsCsGBTerrain::g_pCurRoot->GetSpeedCell()->GetCell( kInfo.CellIdx );
		CsGBCell::sINFO* pCellInfo = pCell->GetInfo( kInfo.Kind, kInfo.ObjectIdx );

		pBaseObject->SetLocalTransform( pCellInfo->s_trParent );
		pBaseObject->Update(0.0f);
	}
}
int CELLOBJ::Manager::GetSelectedCnt(void)
{
	return m_SelectedCellObjs.size();
}
const NiPoint3 CELLOBJ::Manager::GetSelectedCenter(void)
{
	NiPoint3 kCenter = GetSelectedModelCenter();
	return kCenter;// m_SelectedCellObjs.size();
}
const NiPoint3  CELLOBJ::Manager::GetSelectedModelCenter(void)
{
	NiPoint3 kCenter = NiPoint3::ZERO;
	AllCellObjIter kIter = m_SelectedCellObjs.begin();
	for(;kIter != m_SelectedCellObjs.end(); ++kIter)
	{
		NiNode* pBaseObject = nsCsGBTerrain::g_pCurRoot->GetSpeedCell()->GetBaseObject( kIter->second.Kind )->s_Node.m_pNiNode;
		if(pBaseObject){
			CsGBCell* pCell = nsCsGBTerrain::g_pCurRoot->GetSpeedCell()->GetCell( kIter->second.CellIdx );
			if(pCell && pCell->GetSize(kIter->second.Kind) >  0){
				CsGBCell::sINFO* pCellInfo = pCell->GetInfo( kIter->second.Kind, kIter->second.ObjectIdx );
				if(pCellInfo)
					kCenter += pCellInfo->s_trParent.m_Translate;
			}
		}
	}
	kCenter /= m_SelectedCellObjs.size();
	return kCenter;
}
void	CELLOBJ::Manager::SetTranslateByGizmo(const NiPoint3& kPos, bool UpdateObsever, bool isTwoSideTranslate)
{
	AllCellObjIter kIter = m_SelectedCellObjs.begin();
	for(;kIter != m_SelectedCellObjs.end(); ++kIter)
	{
		NiNode* pBaseObject = nsCsGBTerrain::g_pCurRoot->GetSpeedCell()->GetBaseObject( kIter->second.Kind )->s_Node.m_pNiNode;
		if(pBaseObject){
			CsGBCell* pCell = nsCsGBTerrain::g_pCurRoot->GetSpeedCell()->GetCell( kIter->second.CellIdx );
			if(pCell){
				CsGBCell::sINFO* pCellInfo = pCell->GetInfo( kIter->second.Kind, kIter->second.ObjectIdx );
				if(pCellInfo)
				{
					NiTransform kTr = pBaseObject->GetWorldTransform();
					kTr.m_Translate += kPos;
					pBaseObject->SetLocalTransform( kTr );
					pBaseObject->Update( 0.0f, false );

					NiAVObject* pAvObject = pBaseObject;
					while( 1 )
					{
						assert_cs( NiIsKindOf( NiNode, pAvObject ) == true )
							pAvObject = ( (NiNode*)pAvObject )->GetAt( 0 );
						if( NiIsKindOf( NiGeometry, pAvObject ) == true )
							break;
					}
					pCellInfo->s_trParent = kTr;
					pCellInfo->s_trObject = pAvObject->GetWorldTransform();
				}
			}
		}




	}
}
void	CELLOBJ::Manager::SetRotateByGizmo(const NiMatrix3& kRotate, bool UpdateObsever)
{
	NiTransform tr, trDest, trResult;
	tr.MakeIdentity();
	tr.m_Rotate = kRotate;
	NiPoint3 kCurrCenter = GetSelectedCenter();

	AllCellObjIter kIter = m_SelectedCellObjs.begin();
	for(;kIter != m_SelectedCellObjs.end(); ++kIter)
	{
		NiNode* pBaseObject = nsCsGBTerrain::g_pCurRoot->GetSpeedCell()->GetBaseObject( kIter->second.Kind )->s_Node.m_pNiNode;
		if(pBaseObject){
			CsGBCell* pCell = nsCsGBTerrain::g_pCurRoot->GetSpeedCell()->GetCell( kIter->second.CellIdx );
			if(pCell){
				CsGBCell::sINFO* pCellInfo = pCell->GetInfo( kIter->second.Kind, kIter->second.ObjectIdx );
				if(pCellInfo)
				{
					trDest = pBaseObject->GetWorldTransform();
					trDest.m_Translate -= kCurrCenter;//m_pNiNode->GetTranslate();

					trResult = tr*trDest;
					trResult.m_Translate += kCurrCenter;//m_pNiNode->GetTranslate();
					
					pBaseObject->SetLocalTransform( trResult );
					pBaseObject->Update( 0.0f, false );

					NiAVObject* pAvObject = pBaseObject;
					while( 1 )
					{
						assert_cs( NiIsKindOf( NiNode, pAvObject ) == true )
							pAvObject = ( (NiNode*)pAvObject )->GetAt( 0 );
						if( NiIsKindOf( NiGeometry, pAvObject ) == true )
							break;
					}

					pCellInfo->s_trParent = trResult;
					pCellInfo->s_trObject = pAvObject->GetWorldTransform();
				}
			}
		}
	}
}
void	CELLOBJ::Manager::SetRotateByGizmo(const NiPoint3& kCenterPos, int iCoord, NiFixedString kAxis, float fAngle)
{
	NiMatrix3 kRot =  NiMatrix3::IDENTITY;
	NiString kAxisName = kAxis;
	kAxisName.Replace("_pick", "");
	if(kAxisName == "X_axis")
	{
		kRot.MakeYRotation(fAngle);
	}
	else if(kAxisName == "Y_axis")
	{
		kRot.MakeXRotation(fAngle);
	}
	else if(kAxisName == "Z_axis")
	{
		kRot.MakeZRotation(-fAngle);
	}
	SetRotateByGizmo(kRot, false);
}
void	CELLOBJ::Manager::SetScaleByGizmo(const NiPoint3& vDir, const float& fScale)
{
}
void	CELLOBJ::Manager::SetSnapByGizmo(const NiPoint3& kSnapPos)
{
}
NiPick::Record*	CELLOBJ::Manager::SnapPickingObj(const CPoint& kMousePt)
{
	return NULL;
}
void	CELLOBJ::Manager::DeleteModel(void)
{
}
void CELLOBJ::Manager::UpdateViewer()
{
}
int	CELLOBJ::Manager::TakeUndoSnapshot(int eEditType)
{
	if( eEditType == UNDO::CELLOBJ_CREATE)
	{
		return UNDO::Manager::GetSingletonPtr()->AddCommand(UNDO::EDIT_TYPE::CELLOBJ_CREATE, NULL);
	}
	else
	{
		UNDO::Snapshot* pkSnapshot = new UNDO::Snapshot();
		AllCellObjIter kIter = m_SelectedCellObjs.begin();
		for(;kIter != m_SelectedCellObjs.end(); ++kIter)
		{
			NiNode* pBaseObject = nsCsGBTerrain::g_pCurRoot->GetSpeedCell()->GetBaseObject( kIter->second.Kind )->s_Node.m_pNiNode;
			if(pBaseObject){
				CsGBCell* pCell = nsCsGBTerrain::g_pCurRoot->GetSpeedCell()->GetCell( kIter->second.CellIdx);
				if(pCell){
					CsGBCell::sINFO* pCellInfo = pCell->GetInfo( kIter->second.Kind, kIter->second.ObjectIdx );
					if(pCellInfo){
						pkSnapshot->AddSnapshot(kIter->second.Kind, kIter->second.CellIdx, 
							kIter->second.ObjectIdx, pCellInfo->s_trParent );
					}
				}
			}
		}
		return UNDO::Manager::GetSingletonPtr()->AddCommand(UNDO::EDIT_TYPE::CELLOBJ_TRANSLATE, pkSnapshot);
	}

	return -1;

}
void	CELLOBJ::Manager::TakeRedoSnapshot(int CUID)
{
	UNDO::Snapshot* pkSnapshot = new UNDO::Snapshot();
	AllCellObjIter kIter = m_SelectedCellObjs.begin();
	for(;kIter != m_SelectedCellObjs.end(); ++kIter)
	{
		NiNode* pBaseObject = nsCsGBTerrain::g_pCurRoot->GetSpeedCell()->GetBaseObject( kIter->second.Kind )->s_Node.m_pNiNode;
		if(pBaseObject){
			CsGBCell* pCell = nsCsGBTerrain::g_pCurRoot->GetSpeedCell()->GetCell( kIter->second.CellIdx);
			if(pCell){
				CsGBCell::sINFO* pCellInfo = pCell->GetInfo( kIter->second.Kind, kIter->second.ObjectIdx );
				if(pCellInfo){
					pkSnapshot->AddSnapshot(kIter->second.Kind, kIter->second.CellIdx, 
						kIter->second.ObjectIdx, pCellInfo->s_trParent );
				}
			}
		}
	}
	return UNDO::Manager::GetSingletonPtr()->AddRedoSnapshot(CUID, pkSnapshot);
}
void	CELLOBJ::Manager::TakeRedoSnapshotCellRedo(int CUID)
{
	UNDO::Manager::GetSingletonPtr()->AddRedoSnapshot(CUID, NULL);
}
void CELLOBJ::Manager::TranslateByKey(const UNDO::CellObjInfo& kTr)
{
	NiNode* pBaseObject = nsCsGBTerrain::g_pCurRoot->GetSpeedCell()->GetBaseObject( kTr.BaseIdx )->s_Node.m_pNiNode;
	if(pBaseObject){
		CsGBCell* pCell = nsCsGBTerrain::g_pCurRoot->GetSpeedCell()->GetCell( kTr.CellIdx);
		if(pCell){
			CsGBCell::sINFO* pCellInfo = pCell->GetInfo( kTr.BaseIdx, kTr.ObjIdx );
			if(pCellInfo)
			{
				pBaseObject->SetLocalTransform( kTr.mLastTranform );
				pBaseObject->Update( 0.0f, false );

				NiAVObject* pAvObject = pBaseObject;
				while( 1 )
				{
					assert_cs( NiIsKindOf( NiNode, pAvObject ) == true )
						pAvObject = ( (NiNode*)pAvObject )->GetAt( 0 );
					if( NiIsKindOf( NiGeometry, pAvObject ) == true )
						break;
				}
				pCellInfo->s_trParent = kTr.mLastTranform;
				pCellInfo->s_trObject = pAvObject->GetWorldTransform();
			}
		}
	}
}
void	CELLOBJ::Manager::RemovePathObj(NiGeometry* pkGeo)
{
}
void	CELLOBJ::Manager::RemovePathObjNoSnapshot(NiGeometry* pkGeo)
{
}
NiGeometryPtr CELLOBJ::Manager::GetPathObject(int key)
{
	return NULL;
}