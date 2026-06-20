#include "StdAfx.h"
#include "PathManager.h"
#include "../ToolNiNode/UndoManager.h"
PATH::Manager::Manager(void)
{
}

PATH::Manager::~Manager(void)
{
}

void PATH::Manager::Init(void)
{

}
void PATH::Manager::Update(const float& fDeltatime)
{

}
void PATH::Manager::Render(void)
{

}
void PATH::Manager::InitAddPaths(NiAVObject* pkAVObject)
{
	int CurrSize  = m_AllPaths.size();
	AllPathIter kIter = m_AllPaths.find(CurrSize);
	if(kIter == m_AllPaths.end())
	{
		m_AllPaths.insert(AllPath::value_type(CurrSize, pkAVObject));
	}

}

void PATH::Manager::CreateAddPaths(NiAVObject* pkAVObject)
{
	UNDO::Snapshot* pkSnapshot = new UNDO::Snapshot();
	UNDO::Snapshot* pkRedoSnapshot = new UNDO::Snapshot();

	int CurrSize  = m_AllPaths.size();
	AllPathIter kIter = m_AllPaths.find(CurrSize);
	if(kIter == m_AllPaths.end())
	{
		m_AllPaths.insert(AllPath::value_type(CurrSize, pkAVObject));
		NiGeometry* pkGeo = (NiGeometry*)pkAVObject;
		if(pkGeo)
		{			
			pkSnapshot->AddSnapshot(CurrSize,pkGeo->GetVertices(), pkGeo->GetVertexCount());
			pkRedoSnapshot->AddSnapshot(CurrSize,pkGeo->GetVertices(), pkGeo->GetVertexCount());
		}
	}
	int CUID =UNDO::Manager::GetSingletonPtr()->AddCommand(UNDO::PATH_CREATE, pkSnapshot);
	UNDO::Manager::GetSingletonPtr()->AddRedoSnapshot(CUID, pkRedoSnapshot);


}
void PATH::Manager::ResetAllPaths(void)
{
	m_AllPaths.clear();
}

void PATH::Manager::ResetSelected(void)
{
	m_SelectedPaths.clear();
}
void PATH::Manager::AddSelected(int Key, NiAVObject* pkAVObject)
{
	AllPathIter kIter = m_SelectedPaths.find(Key);
	if(kIter == m_SelectedPaths.end())
	{
		m_SelectedPaths.insert(AllPath::value_type(Key, pkAVObject));
	}

}
int PATH::Manager::GetSelectedCnt(void)
{
	return m_SelectedPaths.size();
}
const NiPoint3 PATH::Manager::GetSelectedCenter(void)
{
	NiPoint3 kCenter = GetSelectedModelCenter();
	kCenter.z = 0.0f;
	return kCenter;// m_SelectedPaths.size();
}

const NiPoint3  PATH::Manager::GetSelectedModelCenter(void)
{
	NiPoint3 kCenter = NiPoint3::ZERO;
	AllPathIter kIter = m_SelectedPaths.begin();
	for(;kIter != m_SelectedPaths.end(); ++kIter)
	{
		NiPoint3* pPos = ((NiGeometry*)kIter->second)->GetVertices();
		int VtCnt = ((NiGeometry*)kIter->second)->GetVertexCount(); 
		for( int i=0; i<VtCnt; ++i )
		{
			kCenter += pPos[i];
		}
		kCenter /= VtCnt;

	}
	return kCenter;
}

void	PATH::Manager::SetTranslateByGizmo(const NiPoint3& kPos, bool UpdateObsever, bool isTwoSideTranslate)
{
	if(isTwoSideTranslate == false)
	{
		AllPathIter kIter = m_SelectedPaths.begin();
		for(;kIter != m_SelectedPaths.end(); ++kIter)
		{
			NiGeometry* spPathObj = (NiGeometry*)kIter->second;
			if(spPathObj != NULL)
			{
				NiPoint3* pPos = spPathObj->GetVertices();
				for( int i=0; i<spPathObj->GetVertexCount(); ++i )
				{
					pPos[ i ].x += kPos.x;
					pPos[ i ].y += kPos.y;
				}
				spPathObj->GetModelData()->MarkAsChanged( NiGeometryData::VERTEX_MASK );
				spPathObj->Update(0.0f);

			}
		}
	}
}
void	PATH::Manager::SetRotateByGizmo(const NiPoint3& kCenterPos, int iCoord, NiFixedString kAxis, float fAngle)
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
		kRot.MakeZRotation(fAngle);
		SetRotateByGizmo(kRot, false);
	}


}
void	PATH::Manager::SetRotateByGizmo(const NiMatrix3& kRotate, bool UpdateObsever)
{
	AllPathIter kIter = m_SelectedPaths.begin();
	for(;kIter != m_SelectedPaths.end(); ++kIter)
	{
		NiGeometry* spPathObj = (NiGeometry*)kIter->second;
		if(spPathObj != NULL)
		{
			NiPoint3* pPos = spPathObj->GetVertices();
			NiPoint3 kCenter = NiPoint3::ZERO;
			for( int i=0; i<spPathObj->GetVertexCount(); ++i ){
				kCenter += pPos[ i ];
			}
			kCenter /= spPathObj->GetVertexCount();
			for( int i=0; i<spPathObj->GetVertexCount(); ++i )
			{
				NiPoint3 vPos = ( pPos[ i ] - kCenter )*kRotate;
				pPos[ i ] = vPos + kCenter;
			}

			spPathObj->GetModelData()->MarkAsChanged( NiGeometryData::VERTEX_MASK );
			spPathObj->Update(0.0f);

		}
	}
}

void	PATH::Manager::SetScaleByGizmo(const NiPoint3& vDir, const float& fScale)
{
	AllPathIter kIter = m_SelectedPaths.begin();
	for(;kIter != m_SelectedPaths.end(); ++kIter)
	{
		NiGeometry* spPathObj = (NiGeometry*)kIter->second;
		if(spPathObj != NULL)
		{
			NiPoint3* pPos = spPathObj->GetVertices();
			NiPoint3 vCenter = NiPoint3::ZERO;
			for( int i=0; i<spPathObj->GetVertexCount(); ++i ){
				vCenter += pPos[ i ];
			}
			vCenter /= spPathObj->GetVertexCount();
			
			for( int i=0; i<spPathObj->GetVertexCount(); ++i )
			{
				NiPoint3 vPos = pPos[ i ];
				NiPoint3 kToVt =  (vPos  - vCenter) * fScale;
				NiPoint3::UnitizeVector(kToVt );
				NiPoint3 kMove = kToVt * 100.0f;
				kMove.z = 0.0f;
				pPos[ i ] = vPos + kMove;
				//NiPoint3::UnitizeVector(kToVt);
				//float fDot = kToVt.Dot(vDir);
				//if(fDot > 0.0f)
				//{
				//	 pPos[ i ] += vCenter + kToVt * 0.1f;
				//}
			}
			spPathObj->GetModelData()->MarkAsChanged( NiGeometryData::VERTEX_MASK );
			spPathObj->Update(0.0f);
		}
	}
}
void	PATH::Manager::SetSnapByGizmo(const NiPoint3& kSnapPos)
{
	AllPathIter kIter = m_SelectedPaths.begin();
	for(;kIter != m_SelectedPaths.end(); ++kIter)
	{
		NiGeometry* spPathObj = (NiGeometry*)kIter->second;
		if(spPathObj != NULL)
		{
			NiPoint3* pPos = spPathObj->GetVertices();
			NiPoint3 kCenter = NiPoint3::ZERO;
			for( int i=0; i<spPathObj->GetVertexCount(); ++i ){
				kCenter += pPos[ i ];
			}
			kCenter /= spPathObj->GetVertexCount();

			NiPoint3 kFinalSnapPos = kSnapPos;
			NiPoint3 MovePos = (kFinalSnapPos - kCenter);
			for( int i=0; i<spPathObj->GetVertexCount(); ++i )
			{
				pPos[ i ].x += MovePos.x;
				pPos[ i ].y += MovePos.y;
			}
			spPathObj->GetModelData()->MarkAsChanged( NiGeometryData::VERTEX_MASK );
			spPathObj->Update(0.0f);

		}
	}
}

NiPick::Record*	PATH::Manager::SnapPickingObj(const CPoint& kMousePt)
{
	NiPoint3 origin, dir;
	CAMERA_ST.WindowPointToRay( kMousePt.x, kMousePt.y, origin, dir );

	CsGBPick pick( false );
	AllPathIter kIter = m_AllPaths.begin();
	for(; kIter != m_AllPaths.end(); ++kIter)
	{
		pick.Pick( kIter->second, origin, dir, true );
	}
	if(pick.GetRecord() != NULL)
	{
		return pick.GetRecord();;
		if(pick.GetRecord()->GetIntersection().z > 0.0f)
		{

		}
	}
	return NULL;

}
void	PATH::Manager::DeleteModel(void)
{
}
void PATH::Manager::UpdateViewer()
{

}
int	PATH::Manager::TakeUndoSnapshot(int eEditType)
{
	UNDO::Snapshot* pkSnapshot = new UNDO::Snapshot();
	AllPathIter kIter = m_SelectedPaths.begin();
	for(; kIter != m_SelectedPaths.end(); ++kIter)
	{
		NiGeometry* spPathObj = (NiGeometry*)kIter->second;
		pkSnapshot->AddSnapshot( kIter->first, spPathObj->GetVertices(), 
			spPathObj->GetVertexCount());

	}
	return UNDO::Manager::GetSingletonPtr()->AddCommand( UNDO::PATH_TRANSLATE, pkSnapshot);
}
void	PATH::Manager::TakeRedoSnapshot(int CUID)
{
	UNDO::Snapshot* pkSnapshot = new UNDO::Snapshot();
	AllPathIter kIter = m_SelectedPaths.begin();
	for(; kIter != m_SelectedPaths.end(); ++kIter)
	{
		NiGeometry* spPathObj = (NiGeometry*)kIter->second;
		pkSnapshot->AddSnapshot( kIter->first, spPathObj->GetVertices(), 
			spPathObj->GetVertexCount());

	}
	return UNDO::Manager::GetSingletonPtr()->AddRedoSnapshot(CUID, pkSnapshot);

}

void PATH::Manager::TranslateByKey(int Key, const UNDO::ModelVtInfo& kTr)
{
	AllPathIter kIter = m_AllPaths.find(Key);
	if(kIter != m_AllPaths.end())
	{
		NiGeometry* spPathObj = (NiGeometry*)kIter->second;
		if(spPathObj != NULL)
		{
			NiPoint3* pPos = spPathObj->GetVertices();
			UNDO::ModelVtPos::const_iterator kIter = kTr.mVTPos.begin();
			for(; kIter != kTr.mVTPos.end(); ++kIter)
			{
				pPos[ kIter->first ]= kIter->second;

			}
			spPathObj->GetModelData()->MarkAsChanged( NiGeometryData::VERTEX_MASK );
			spPathObj->Update(0.0f);
		}
	}
}

void	PATH::Manager::RemovePathObj(NiGeometry* pkGeo)
{
	m_SelectedPaths.clear();
	UNDO::Snapshot* pkSnapshot = new UNDO::Snapshot();
	UNDO::Snapshot* pkRedoSnapshot = new UNDO::Snapshot();

	AllPathIter kIter = m_AllPaths.begin();
	for(; kIter != m_AllPaths.end(); ++kIter)
	{
		if(kIter->second == pkGeo)
		{
			pkRedoSnapshot->AddSnapshot(kIter->first,pkGeo->GetVertices(), pkGeo->GetVertexCount());
			pkSnapshot->AddSnapshot(kIter->first,pkGeo->GetVertices(), pkGeo->GetVertexCount());
			kIter = m_AllPaths.erase(kIter);
			break;
		}
	}
	int CUID =UNDO::Manager::GetSingletonPtr()->AddCommand(UNDO::PATH_DELETE, pkSnapshot);
	UNDO::Manager::GetSingletonPtr()->AddRedoSnapshot(CUID, pkRedoSnapshot);
}
void	PATH::Manager::RemovePathObjNoSnapshot(NiGeometry* pkGeo)
{
	m_SelectedPaths.clear();
	AllPathIter kIter = m_AllPaths.begin();
	for(; kIter != m_AllPaths.end(); ++kIter)
	{
		if(kIter->second == pkGeo)
		{
			kIter = m_AllPaths.erase(kIter);
			break;
		}
	}
}


NiGeometryPtr PATH::Manager::GetPathObject(int key)
{
	AllPathIter kIter = m_AllPaths.find(key);
	if(kIter != m_AllPaths.end())
	{
		return (NiGeometry*)kIter->second;
	}
	return NULL;
}