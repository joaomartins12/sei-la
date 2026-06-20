#pragma once

namespace GIZMO
{
	class EditAble
	{

	public:
		virtual int GetSelectedCnt(){return 0;}
		virtual const NiPoint3 GetSelectedCenter(){return NiPoint3::ZERO;}
		virtual const NiPoint3 GetSelectedFront(){return NiPoint3::ZERO;}
		virtual const NiMatrix3 GetSelectedRot(){return NiMatrix3::IDENTITY;}
		virtual void SetTranslateByGizmo(const NiPoint3& kMove, bool UpdateObsever = true, bool isTwoSideTranslate = false){return;}
		virtual void SetRotateByGizmo(const NiMatrix3& kRotate, bool UpdateObsever = true){return;}
		virtual	void SetRotateByGizmo(const NiPoint3& kCenterPos, int iCoord, NiFixedString kAxis, float fAngle){;}
		virtual void SetScaleByGizmo(const NiPoint3& vDir, const float& fScale){return;}
		virtual void SetSnapByGizmo(const NiPoint3& kSnapPos){return;}
		virtual int TakeUndoSnapshot(int eEditType){return -1;}
		virtual void TakeRedoSnapshot(int CUID){}
		virtual NiPick::Record*	SnapPickingObj(const CPoint& kMousePt){return NULL;}
		virtual void DeleteModel(void){}
	};
}
