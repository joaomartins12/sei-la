#pragma once
namespace GIZMO
{
	class DragRect : public GIZMO::cGizmo
	{
		enum DRPOS
		{
			LB,
			LT,
			RB,
			RT,
			END
		};

	public:
		 bool Initizlize(void);
		 void Update(const float& fDeltaTime);
		 void Render(void);
		 void Terminate(void);

		 void OnMouseMove(int iGizmoSensor, const CPoint& kMousePt, GIZMO::EditAble* pkEditable);
		 void OnLButtonUp(const CPoint& kMousePt, EditAble* pkEditAble);
		 bool OnLButtonDown(const CPoint& kMousePt, EditAble* pkEditAble);


		 void OnCheckPicked(const CPoint& kMousePt);
		 void OnObjectSelected(const NiPoint3& kMousePt);
		 void OnShowGizmo(const NiPoint3& kWorldPos);
		 void OnHideGizmo();

		 float	GetLength(const CPoint& kFirst, const CPoint& kSecond);

		 NiTriShapePtr m_spMesh;

		 NiPoint3 m_FirstMouseClick;
		 NiPoint3 m_CurrMouse;
		 CPoint FirstMousePt;
		 bool bDragStart;
		 float MinLength;
	public:
		DragRect(void);
		~DragRect(void);
	};

}
