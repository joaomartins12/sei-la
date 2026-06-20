#pragma once
#include "Gizmo.h"
#include "../Editable.h"
namespace GIZMO
{
class Translate : public GIZMO::cGizmo
{
	bool Initizlize(void);
	void Update(const float& fDeltaTime);
	void Render(void);
	void Terminate(void);
	void OnMouseMove(int iGizmoSensor, const CPoint& kMousePt, GIZMO::EditAble* pkEditAble);
	//void UpdateAxis(void);
	void UpdateCoord(void);
	void UpdateSize(void);
	void ChangeAxisColor(const NiFixedString& AxisName);
	void ResetAxisColor(void);
	void OnLButtonUp(const CPoint& kMousePt, EditAble* pkEditAble);
	bool OnLButtonDown(const CPoint& kMousePt, EditAble* pkEditAble);
	void OnCheckPicked(const CPoint& kMousePt);
	void OnObjectSelected(const NiPoint3& kSelectedPos);
	void OnShowGizmo(const NiPoint3& kWorldPos);
	void OnHideGizmo();
		float FindDegree(float x, float y);

	void MoveGizmo(int iSense, const CPoint& kMousePt, GIZMO::EditAble* pkEditAble);
	const NiColor GetAxisOriginColor(NiFixedString Name );
	NiAVObject* GetAxisProperty(NiString kAxisName);

	void LookAt( NiAVObject* pkFrom, const NiPoint3& kTargetDir);

	const NiPoint3 RotateVector(const NiPoint3& kFrom, const NiPoint3& kTo);

	NiPoint3 GetSelectedAxisDir(CPoint kMousePT, GIZMO::EditAble* pkEditAble);
	NiPoint3 GetWorldDirection(NiAVObject* pkAVObject);
	NiPoint3 GetWorldUpVector(NiAVObject* pkAVObject);
	NiPoint3 GetWorldRightVector(NiAVObject* pkAVObject);

	NiPoint3			m_LastIntersection;
	NiPoint3			m_LastGizmoTrans;
	NiPoint3			m_PrevGizmoTrans;
	bool				m_bEdited;
public:
	Translate(void);
	~Translate(void);
};
}