#pragma once
#include "Gizmo.h"

namespace GIZMO
{
class Rotate : public GIZMO::cGizmo
{

public:
	bool Initizlize(void);
	void Update(const float& fDeltaTime);
	void Render(void);
	void Terminate(void);

	void OnShowGizmo(const NiPoint3& kWorldPos);
	void OnHideGizmo();

	void OnMouseMove(int iGizmoSensor, const CPoint& kMousePt, GIZMO::EditAble* pkEditAble);
	void OnLButtonUp(const CPoint& kMousePt, EditAble* pkEditAble);
	bool OnLButtonDown(const CPoint& kMousePt, EditAble* pkEditAble);

	void OnCheckPicked(const CPoint& kMousePt);
	void OnObjectSelected(const NiPoint3& kMousePt);
	void UpdateGizmos(void);
	NiPoint3 GetSelectedAxisDir(void);
	NiMatrix3 GetSelectedAxisRotate(const float fAngle);
	NiPoint3 GetWorldDirection(void);
	NiPoint3 GetWorldUpVector(void);
	NiPoint3 GetWorldRightVector(void);
	void ResetAxisColor(void);
	const NiColor GetAxisOriginColor(NiFixedString Name );
	NiAVObject* GetAxisProperty(NiString kAxisName);//X_axis_pick
	void ChangeAxisColor(const NiFixedString& AxisName);
	void RotateGizmo(const CPoint& kMousePt, GIZMO::EditAble* pkEditAble);
	float GetInnerRadian(NiPoint3 kRv, NiPoint3 kLv);
	float FindDegree(float x, float y);
	void LookAt(NiAVObject* pkFrom, const NiMatrix3& kTargetDir);
	NiNodePtr		m_pAxisPlaneNiNode;

	NiMatrix3			m_Rotation;
	bool				m_bEdited;
public:
	Rotate(void);
	~Rotate(void);
};
}
