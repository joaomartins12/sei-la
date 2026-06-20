#pragma once
#include "../ToolNiNode/GizmoManager.h"
#include "../ToolNiNode/UndoManager/UndoCommand.h"
namespace CELLOBJ
{
	struct INFO
	{
		int Kind;
		int CellIdx;
		int ObjectIdx;
	};


	typedef std::map<int, INFO> AllCellObj;
	typedef AllCellObj::iterator AllCellObjIter;



	class Manager : public CSingleton< Manager >, public GIZMO::EditAble
	{	
	public:
		void Init(void);
		void Update(const float& fDeltatime);
		void Render(void);

		void InitAddCellObj(CsGBCell::sINFO* pkCellInfo);
		void CreateAddCellObj(NiAVObject* pkAVObject);
		void ResetAllPaths(void);

		void ResetSelected(void);
		void AddSelected(int iKindIdx, CELLOBJ::INFO kInfo);

		int GetSelectedCnt(void);
		const NiPoint3 GetSelectedCenter(void);
		const NiPoint3 GetSelectedModelCenter(void);
		NiPick::Record*	SnapPickingObj(const CPoint& kMousePt);//{return NULL;}
		
		void	SetTranslateByGizmo(const NiPoint3& kMove, bool UpdateObsever = true, bool isTwoSideTranslate = false);
		void	SetRotateByGizmo(const NiPoint3& kCenterPos, int iCoord, NiFixedString kAxis, float fAngle);
		void	SetRotateByGizmo(const NiMatrix3& kRotate, bool UpdateObsever = true);
		void	SetScaleByGizmo(const NiPoint3& vDir, const float& fScale);
		void	SetSnapByGizmo(const NiPoint3& kSnapPos);
		void	DeleteModel(void);
		void	UpdateViewer(void);
		int		TakeUndoSnapshot(int eEditType);
		void	TakeRedoSnapshot(int CUID);
		void	TakeRedoSnapshotCellRedo(int CUID);
		void	RemovePathObj(NiGeometry* pkGeo);
		void	RemovePathObjNoSnapshot(NiGeometry* pkGeo);

		void TranslateByKey(const UNDO::CellObjInfo& kTr);
		NiGeometryPtr GetPathObject(int key);

	private:
		AllCellObj m_AllCellObjs;
		AllCellObj m_SelectedCellObjs;

	public:
		Manager(void);
		~Manager(void);
	};

}


