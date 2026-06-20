#pragma once
#include "../Editable.h"
namespace GIZMO {

enum COORD
{
	WORLD,
	MODEL,
	CAM,
};


class cGizmo
{

public:
	virtual bool Initizlize(void){return false;}
	virtual void Update(const float& fDeltaTime){}
	virtual void Render(void){}
	virtual void Terminate(void){}
	
	virtual void OnMouseMove(int iGizmoSensor, const CPoint& kMousePt, GIZMO::EditAble* pkEditable){}
	virtual void OnLButtonUp(const CPoint& kMousePt, EditAble* pkEditAble){}
	virtual bool OnLButtonDown(const CPoint& kMousePt, EditAble* pkEditAble){return false;}
	
	
	virtual void OnCheckPicked(const CPoint& kMousePt){}
	virtual void OnObjectSelected(const NiPoint3& kMousePt){}
	virtual void OnShowGizmo(const NiPoint3& kWorldPos){};
	virtual void OnHideGizmo(){};
	virtual void OnChangeGizmoCoord(GIZMO::COORD iCoord, GIZMO::EditAble* pkEditable)
	{
		mGizmoCoord = iCoord;
		m_pkCurrEditable = pkEditable;
	};
	virtual void OnChangeGizmoCoord(GIZMO::COORD iCoord)
	{
		mGizmoCoord = iCoord;
	};

protected:
	std::string			m_NifPath;
	NiFixedString		m_SelectedAxisName;
	NiNodePtr		m_spModel;
	CPoint				m_StartMousePt;
	NiColor OriginAxisXColor;
	NiColor OriginAxisYColor;
	NiColor OriginAxisZColor;
	NiColor SelectedAxisColor;
	bool				m_bTakeSnapshot;
	const float GIZMOSIZE;
	int					LastCUID;
	GIZMO::COORD mGizmoCoord;
	GIZMO::EditAble* m_pkCurrEditable;
public:
	cGizmo(void)
		:m_NifPath("")
		,m_SelectedAxisName("")
		,m_bTakeSnapshot(false)
		,GIZMOSIZE(0.005f)
		,LastCUID(-1)
		,m_pkCurrEditable(NULL)
	{
		OriginAxisXColor = NiColor(1.0f,0.0f,0.0f);
		OriginAxisYColor = NiColor(0.0f,1.0f,0.0f);
		OriginAxisZColor = NiColor(0.0f,0.0f,1.0f);
		SelectedAxisColor = NiColor(1.0f,1.0f,0.0f);
	};
	~cGizmo(void);
};
}
