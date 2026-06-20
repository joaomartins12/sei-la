#pragma once
#include "Gizmo.h"
namespace GIZMO
{
class Scale : public GIZMO::cGizmo
{
	bool Initizlize(void);
	void Update(const float& fDeltaTime);
	void Render(void);
	void Terminate(void);
	void OnMouseMove(int iGizmoSensor, const CPoint& kMousePt, GIZMO::EditAble* pkEditAble);
	//void UpdateAxis(void);
	void UpdateSize(void);
	void ChangeAxisColor(const NiFixedString& AxisName);
	void ResetAxisColor(void);
	void OnLButtonUp(const CPoint& kMousePt, EditAble* pkEditAble);
	bool OnLButtonDown(const CPoint& kMousePt, EditAble* pkEditAble);
	void OnCheckPicked(const CPoint& kMousePt);
	void OnObjectSelected(const NiPoint3& kSelectedPos);
	void OnShowGizmo(const NiPoint3& kWorldPos);
	void OnHideGizmo();
	void Billboard( NiAVObject* pNode, float fAniTime );
	float FindDegree(float x, float y);

	void MoveGizmo(const CPoint& kMousePt, GIZMO::EditAble* pkEditAble);
	const NiColor GetAxisOriginColor(NiFixedString Name );
	NiAVObject* GetAxisProperty(NiString kAxisName);

	NiPoint3 GetSelectedAxisDir();
	NiPoint3 GetWorldDirection();
	NiPoint3 GetWorldUpVector();
	NiPoint3 GetWorldRightVector();
	NiPoint3			m_Translate;
	bool				m_bEdited;
public:
	Scale(void);
	~Scale(void);
};
}