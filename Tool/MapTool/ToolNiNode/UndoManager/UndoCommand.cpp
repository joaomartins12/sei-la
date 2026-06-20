#include "StdAfx.h"
#include "UndoCommand.h"
#include "../../ObjectMng.h"
#include "../../Path/PathManager.h"
UNDO::Snapshot::Snapshot(void)
{
}

UNDO::Snapshot::~Snapshot(void)
{
	SnapshotIter kIter = m_Snapshots.begin();
	for(; kIter != m_Snapshots.end();)
	{
		kIter = m_Snapshots.erase(kIter);
	}
	m_Snapshots.clear();
}
void UNDO::Snapshot::AddSnapshot(DWORD UID, const NiTransform& kTr)
{
	SnapshotIter kIter = m_Snapshots.find(UID);
	if(kIter == m_Snapshots.end())
	{
		NiTransform kTransform(kTr);
		m_Snapshots.insert(Snapshots::value_type(UID, kTr));
	}
	else
		kIter->second = kTr;

}
void UNDO::Snapshot::AddSnapshot(DWORD UID, NiPoint3* pkPos, int VtCnt)
{
	ModelSnapshotIter kIter = m_ModelSnapshots.find(UID);
	if(kIter == m_ModelSnapshots.end())
	{
		ModelVtInfo kVTInfo;
		for(int i =0; i < VtCnt; ++i)
		{
			kVTInfo.mVTPos.insert(ModelVtPos::value_type(i, pkPos[i]));
		}
		m_ModelSnapshots.insert(ModelSnapshots::value_type(UID, kVTInfo));
	}
}
void UNDO::Snapshot::AddSnapshot(std::string kFullPath, const NiTransform& kTr)
{
	PathInfo kPathInfo;
	kPathInfo.mFullPath = kFullPath;
	kPathInfo.mLastTranform = kTr;

	m_ModelFullPaths.push_back(kPathInfo);
}
void UNDO::Snapshot::AddSnapshot(int iBaseIdx, int iCellIdx, int iObjIdx, const NiTransform& kTr)
{
	CellObjInfo kInfo;
	kInfo.BaseIdx = iBaseIdx;
	kInfo.CellIdx = iCellIdx;
	kInfo.ObjIdx = iObjIdx;
	kInfo.mLastTranform = kTr;

	m_CellObjs.push_back(kInfo);
}

/////////////////////////////////////////////////////
UNDO::Command::Command(void)
:m_pkUndo(NULL),
m_pkRedo(NULL)
{

}
UNDO::Command::~Command(void)
{
	Terminate();
}
void UNDO::Command::Terminate()
{
	SAFE_DELETE(m_pkUndo);
	SAFE_DELETE(m_pkRedo);
}
void UNDO::Command::SetUndo(Snapshot* pkSnapshots)
{
	m_pkUndo = pkSnapshots;
}
void UNDO::Command::SetRedo(Snapshot* pkSnapshot)
{
	m_pkRedo = pkSnapshot;
}

/////////////////////////////////////////////////////

UNDO::Move::Move(void)
{

}
UNDO::Move::~Move(void)
{
	Terminate();
}
UNDO::Scale::Scale(void)
{

}
UNDO::Scale::~Scale(void)
{
	Terminate();
}
UNDO::Rotate::Rotate(void)
{

}
UNDO::Rotate::~Rotate(void)
{
	Terminate();
}
UNDO::PathTranslate::PathTranslate(void)
{

}
UNDO::PathTranslate::~PathTranslate()
{
	Terminate();

}
UNDO::ObjCreate::ObjCreate(void)
{

}
UNDO::ObjCreate::~ObjCreate()
{
	Terminate();

}
UNDO::ObjDelete::ObjDelete(void)
{

}
UNDO::ObjDelete::~ObjDelete()
{	Terminate();


}
UNDO::PathCreate::PathCreate(void)
{

}
UNDO::PathCreate::~PathCreate()
{	
	Terminate();
}
UNDO::PathDelete::PathDelete(void)
{

}
UNDO::PathDelete::~PathDelete()
{	
	Terminate();
}
UNDO::CellObjTranslate::CellObjTranslate(void)
{

}
UNDO::CellObjTranslate::~CellObjTranslate()
{	
	Terminate();
}
UNDO::CellObjCreate::CellObjCreate(void)
{

}
UNDO::CellObjCreate::~CellObjCreate()
{	
	Terminate();
}
////////////////////////////////////////////////////////////////////////////////////////
void UNDO::Move::Undo(void)
{
	Snapshots::const_iterator kIter = m_pkUndo->GetSnapshots().begin();
	for(;kIter != m_pkUndo->GetSnapshots().end(); ++kIter)
	{
		g_ObjectMng.SetTransformByOUID(kIter->first, kIter->second);
	}
	g_ObjectMng.UpdateViewer();
}
void UNDO::Move::Redo(void)
{
	Snapshots::const_iterator kIter = m_pkRedo->GetSnapshots().begin();
	for(;kIter != m_pkRedo->GetSnapshots().end(); ++kIter)
	{
		g_ObjectMng.SetTransformByOUID(kIter->first, kIter->second);
	}
	g_ObjectMng.UpdateViewer();
}
////////////////////////////////////////////////////////////////////////////////////////
void UNDO::Scale::Undo(void)
{
	Snapshots::const_iterator kIter = m_pkUndo->GetSnapshots().begin();
	for(;kIter != m_pkUndo->GetSnapshots().end(); ++kIter)
	{
		g_ObjectMng.SetTransformByOUID(kIter->first, kIter->second);
	}
	g_ObjectMng.UpdateViewer();
}
void UNDO::Scale::Redo(void)
{
	Snapshots::const_iterator kIter = m_pkRedo->GetSnapshots().begin();
	for(;kIter != m_pkRedo->GetSnapshots().end(); ++kIter)
	{
		g_ObjectMng.SetTransformByOUID(kIter->first, kIter->second);
	}
	g_ObjectMng.UpdateViewer();
}
////////////////////////////////////////////////////////////////////////////////////////
void UNDO::Rotate::Undo(void)
{
	Snapshots::const_iterator kIter = m_pkUndo->GetSnapshots().begin();
	for(;kIter != m_pkUndo->GetSnapshots().end(); ++kIter)
	{
		g_ObjectMng.SetTransformByOUID(kIter->first, kIter->second);
	}
	g_ObjectMng.UpdateViewer();
}
void UNDO::Rotate::Redo(void)
{
	Snapshots::const_iterator kIter = m_pkRedo->GetSnapshots().begin();
	for(;kIter != m_pkRedo->GetSnapshots().end(); ++kIter)
	{
		g_ObjectMng.SetTransformByOUID(kIter->first, kIter->second);
	}
	g_ObjectMng.UpdateViewer();
}

////////////////////////////////////////////////////////////////////////////////////////
void UNDO::ObjCreate::Undo(void)
{
	Snapshots::const_iterator kIter = m_pkUndo->GetSnapshots().begin();
	for(;kIter != m_pkUndo->GetSnapshots().end(); ++kIter)
	{
		if( g_ObjectMng.DeleteSelectModel( kIter->first ) == true )
			return;

		nsCsGBTerrain::g_pCurRoot->DeleteCsGBObject( kIter->first );
	}
}
void UNDO::ObjCreate::Redo(void)
{
	SAFE_DELETE(m_pkUndo);
	m_pkUndo = new Snapshot();

	ModelPath::const_iterator kIter = m_pkRedo->GetModelPath().begin();
	for(;kIter != m_pkRedo->GetModelPath().end(); ++kIter)
	{
		PathInfo kPathInfo = (*kIter);
		DWORD OUID = g_ObjectMng.InsertModel( kPathInfo.mFullPath, kPathInfo.mLastTranform);
		g_ObjectMng.DeleteModel();
		m_pkUndo->AddSnapshot(OUID, kPathInfo.mLastTranform);
	}
}
////////////////////////////////////////////////////////////////////////////////////////
void UNDO::ObjDelete::Undo(void)
{
	SAFE_DELETE(m_pkRedo);
	m_pkRedo = new Snapshot();

	ModelPath::const_iterator kIter = m_pkUndo->GetModelPath().begin();
	for(;kIter != m_pkUndo->GetModelPath().end(); ++kIter)
	{
		PathInfo kPathInfo = (*kIter);
		DWORD OUID = g_ObjectMng.InsertModel( kPathInfo.mFullPath, kPathInfo.mLastTranform);
		g_ObjectMng.DeleteModel();
		m_pkRedo->AddSnapshot(OUID, kPathInfo.mLastTranform);
	}
}
void UNDO::ObjDelete::Redo(void)
{
	Snapshots::const_iterator kIter = m_pkRedo->GetSnapshots().begin();
	for(;kIter != m_pkRedo->GetSnapshots().end(); ++kIter)
	{
		if( g_ObjectMng.DeleteSelectModel( kIter->first ) == true )
			return;

		nsCsGBTerrain::g_pCurRoot->DeleteCsGBObject( kIter->first );
	}
}
//////////////////////////////////////////////////////////////////
void UNDO::PathCreate::Undo(void)
{
	ModelSnapshots::const_iterator kIter = m_pkUndo->GetModelSnapshots().begin();
	for(;kIter != m_pkUndo->GetModelSnapshots().end(); ++kIter)
	{
		GET_MV_MAINTAB_DLG->_GetDlgPath()->RemovePathObject(kIter->first);
	}
}
void UNDO::PathCreate::Redo(void)
{
	ModelSnapshots::const_iterator kIter = m_pkUndo->GetModelSnapshots().begin();
	for(;kIter != m_pkUndo->GetModelSnapshots().end(); ++kIter)
	{
		GET_MV_MAINTAB_DLG->_GetDlgPath()->__AddTerrainPath(0, NiPoint3::ZERO,
			kIter->second);
	}
}
////////////////////////////////////////////////////////////////////////////////////////
void UNDO::PathDelete::Undo(void)
{
	ModelSnapshots::const_iterator kIter = m_pkUndo->GetModelSnapshots().begin();
	for(;kIter != m_pkUndo->GetModelSnapshots().end(); ++kIter)
	{
		GET_MV_MAINTAB_DLG->_GetDlgPath()->__AddTerrainPath(0, NiPoint3::ZERO,
			kIter->second);
	}
}
void UNDO::PathDelete::Redo(void)
{
	ModelSnapshots::const_iterator kIter = m_pkUndo->GetModelSnapshots().begin();
	for(;kIter != m_pkUndo->GetModelSnapshots().end(); ++kIter)
	{
		GET_MV_MAINTAB_DLG->_GetDlgPath()->RemovePathObject(kIter->first);
	}
}
////////////////////////////////////////////////////////////////////////////////////////
void UNDO::PathTranslate::Undo(void)
{
	ModelSnapshots::const_iterator kIter = m_pkUndo->GetModelSnapshots().begin();
	for(;kIter != m_pkUndo->GetModelSnapshots().end(); ++kIter)
	{
		PATH::Manager::GetSingletonPtr()->TranslateByKey(kIter->first, kIter->second);
	}
}
void UNDO::PathTranslate::Redo(void)
{
	ModelSnapshots::const_iterator kIter = m_pkRedo->GetModelSnapshots().begin();
	for(;kIter != m_pkRedo->GetModelSnapshots().end(); ++kIter)
	{
		PATH::Manager::GetSingletonPtr()->TranslateByKey(kIter->first, kIter->second);
	}
}
////////////////////////////////////////////////////////////////////////////////////////
void UNDO::CellObjTranslate::Undo(void)
{
	CellObjs::const_iterator kIter = m_pkUndo->GetCellObj().begin();
	for(;kIter != m_pkUndo->GetCellObj().end(); ++kIter)
	{
		CELLOBJ::Manager::GetSingletonPtr()->TranslateByKey((*kIter));
	}
}
void UNDO::CellObjTranslate::Redo(void)
{
	CellObjs::const_iterator kIter = m_pkRedo->GetCellObj().begin();
	for(;kIter != m_pkRedo->GetCellObj().end(); ++kIter)
	{
		CELLOBJ::Manager::GetSingletonPtr()->TranslateByKey((*kIter));
	}
	//ModelSnapshots::const_iterator kIter = m_pkRedo->GetModelSnapshots().begin();
	//for(;kIter != m_pkRedo->GetModelSnapshots().end(); ++kIter)
	//{
	//	CELLOBJ::Manager::GetSingletonPtr()->TranslateByKey(kIter->first, kIter->second);
	//}
}
////////////////////////////////////////////////////////////////////////////////////////
void UNDO::CellObjCreate::Undo(void)
{
	g_RollBack.UnDo();
}
void UNDO::CellObjCreate::Redo(void)
{
	g_RollBack.ReDo();
	//ModelSnapshots::const_iterator kIter = m_pkRedo->GetModelSnapshots().begin();
	//for(;kIter != m_pkRedo->GetModelSnapshots().end(); ++kIter)
	//{
	//	CELLOBJ::Manager::GetSingletonPtr()->TranslateByKey(kIter->first, kIter->second);
	//}
}
