#pragma once
namespace GIZMO {
class Snap: public cGizmo
{

public:
	bool Initizlize(void);
	void Update(const float& fDeltaTime);
	void Render(void);
	void Terminate(void);

	void OnMouseMove(int iGizmoSensor, const CPoint& kMousePt, EditAble* pkEditAble);
	void OnLButtonUp(const CPoint& kMousePt, EditAble* pkEditAble);
	bool OnLButtonDown(const CPoint& kMousePt, EditAble* pkEditAble);

	void OnCheckPicked(const CPoint& kMousePt);
	void OnObjectSelected(const NiPoint3& kMousePt);

	void OnShowGizmo(const NiPoint3& kWorldPos);
	void OnHideGizmo();

	void UpdateSize(const float& fDeltaTime);

	NiNode* m_pkLastPickNode;
	NiPoint3 LastNearPt;
	NiPoint3 LastPos;
	NiPoint3 LastLocalPos;


	NiNodePtr		m_spLastPT;
	

public:
	Snap(void);
	~Snap(void);
};
}