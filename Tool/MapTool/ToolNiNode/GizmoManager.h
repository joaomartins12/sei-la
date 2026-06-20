#pragma once
#include "Gizmo/Gizmo.h"
#include "Gizmo/RotateGizmo.h"
#include "Gizmo/TranslateGizmo.h"
#include "Gizmo/ScaleGizmo.h"
#include "Gizmo/SnapGizmo.h"
#include "Gizmo/DragRect.h"
#include "Editable.h"
namespace GIZMO
{
enum TYPE
{
	TRANSLATE,
	ROTATE,
	SCALE,
	SNAP,
	END,
};
typedef map<GIZMO::TYPE, GIZMO::cGizmo*> Gizmos;
typedef map<GIZMO::TYPE, GIZMO::cGizmo*>::iterator GizmoIter;



class Manager
{
public:
	void Initizlize(void);
	void Update(const float& fDeltaTime);
	void Render(void);
	void Terminate(void);
	void Reset(void);

public:
	bool ChangeGizmo(GIZMO::TYPE eType , EditAble* pkEditAble);
	void UpdateGizmoPos(EditAble* pkEditAble);
	bool OnMouseMove(const CPoint& pt, EditAble* pkEditAble);
	bool OnLButtonDown(const CPoint& Pt, EditAble* pkEditAbl);
	void OnLButtonUp(const CPoint& Pt, EditAble* pkEditAbl);
	void OnObjectSelected(const NiPoint3& kPos);
	void SetUse(bool TOP){m_bUse = TOP;}
	void SetRender(bool TOP){m_bRender = TOP;}
	void SetGizmoSense(int Sense);
	void SetGizmoCoord(GIZMO::COORD iCoord, GIZMO::EditAble* pkEditable);
	void SetGizmoCoord(GIZMO::COORD iCoord);
private:
	Gizmos mGizmos;
	GIZMO::cGizmo* m_pCurrGizmo;
	GIZMO::TYPE m_eCurrGizmo;
	GIZMO::COORD mCurrCoord;
	int m_GizmoSense;
	bool m_bUse;
	bool m_bRender;
public:
	Manager(void);
	~Manager(void);

};
}
