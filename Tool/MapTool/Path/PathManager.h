#pragma once
#include "../ToolNiNode/GizmoManager.h"
#include "../ToolNiNode/UndoManager/UndoCommand.h"
namespace PATH
{

	typedef std::map<DWORD, NiAVObject*> AllPath;
	typedef std::map<DWORD, NiAVObject*>::iterator AllPathIter;



	class Manager : public CSingleton< Manager >, public GIZMO::EditAble
	{	
	public:
		void Init(void);
		void Update(const float& fDeltatime);
		void Render(void);

		void InitAddPaths(NiAVObject* pkAVObject);
		void CreateAddPaths(NiAVObject* pkAVObject);
		void ResetAllPaths(void);

		void ResetSelected(void);
		void AddSelected(int Key, NiAVObject* pkAVObject);

		int GetSelectedCnt(void);
		const NiPoint3 GetSelectedCenter(void);
		const NiPoint3 GetSelectedModelCenter(void);
		NiPick::Record*	SnapPickingObj(const CPoint& kMousePt);//{return NULL;}
		
		void	SetTranslateByGizmo(const NiPoint3& kMove, bool UpdateObsever = true,  bool isTwoSideTranslate = false);
		void	SetRotateByGizmo(const NiMatrix3& kRotate, bool UpdateObsever = true);
		void	SetRotateByGizmo(const NiPoint3& kCenterPos, int iCoord, NiFixedString kAxis, float fAngle);
		void	SetScaleByGizmo(const NiPoint3& vDir, const float& fScale);
		void	SetSnapByGizmo(const NiPoint3& kSnapPos);
		void	DeleteModel(void);
		void	UpdateViewer(void);
		int		TakeUndoSnapshot(int eEditType);
		void	TakeRedoSnapshot(int CUID);
		void	RemovePathObj(NiGeometry* pkGeo);
		void	RemovePathObjNoSnapshot(NiGeometry* pkGeo);

		void TranslateByKey(int Key, const UNDO::ModelVtInfo& kTr);
		NiGeometryPtr GetPathObject(int key);

	private:
		AllPath m_AllPaths;
		AllPath m_SelectedPaths;

	public:
		Manager(void);
		~Manager(void);
	};

}


