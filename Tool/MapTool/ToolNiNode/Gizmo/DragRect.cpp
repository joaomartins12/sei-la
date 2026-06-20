#include "StdAfx.h"
#include "DragRect.h"
using namespace GIZMO;

DragRect::DragRect(void)
:bDragStart(false)
,m_FirstMouseClick(NiPoint3::ZERO)
,m_CurrMouse(NiPoint3::ZERO)
,MinLength(3.0f)
{
}

DragRect::~DragRect(void)
{
	m_spMesh =0;
}
bool DragRect::Initizlize(void)
{
	int ui=  0;
	NiPoint3* pVTpos = NiNew NiPoint3[ 4 ];
	pVTpos[LB] = NiPoint3(5.0f, 5.0f, 0.0f);//0
	pVTpos[LT] = NiPoint3(5.0f, 100.0f, 0.0f);//1
	pVTpos[RB] = NiPoint3(100.0f, 5.0f, 0.0f);//2
	pVTpos[RT] = NiPoint3(100.0f, 100.0f, 0.0f);//3

	unsigned short* kIndices = NiAlloc( unsigned short, 6 );

	int vertexidx = 0;
	kIndices[vertexidx++]	= 0;
	kIndices[vertexidx++]	= 2;
	kIndices[vertexidx++]	= 3;

	kIndices[vertexidx++]	= 0;
	kIndices[vertexidx++]	= 3;
	kIndices[vertexidx++]	= 1;

	m_spMesh = NiNew NiTriShape( 4, pVTpos, NULL, NULL, NULL, 0, NiGeometryData::NBT_METHOD_NONE, 2, kIndices );
	NiMaterialPropertyPtr pMapProp = NiNew NiMaterialProperty;
	pMapProp->SetAlpha( 0.5f );
	pMapProp->SetEmittance( NiColor::BLACK);
	m_spMesh->AttachProperty( pMapProp );	
	NiAlphaPropertyPtr pAlphaProp = NiNew NiAlphaProperty;
	pAlphaProp->SetAlphaBlending( true );
	pAlphaProp->SetAlphaTesting( false );
	m_spMesh->AttachProperty( pAlphaProp );
	NiStencilPropertyPtr pStencilProp = NiNew NiStencilProperty;
	pStencilProp->SetDrawMode( NiStencilProperty::DRAW_BOTH );
	m_spMesh->AttachProperty( pStencilProp );
	NiWireframePropertyPtr pWireProp = NiNew NiWireframeProperty;
	pWireProp->SetWireframe( true );
	m_spMesh->AttachProperty( pWireProp );
	m_spMesh->UpdateProperties();
	m_spMesh->Update( 0.0f );

	return true;

}
void DragRect::Update(const float& fDeltaTime)
{
	if(m_spMesh)
		m_spMesh->Update(fDeltaTime);
}
void DragRect::Render(void)
{
	if(m_spMesh != NULL && 	m_spMesh->GetAppCulled() == false)
		NiDrawScene( CAMERA_ST.GetCameraObj(), m_spMesh, *g_pCuller );

}
void DragRect::Terminate(void)
{
	m_spModel = 0;
}

float	DragRect::GetLength(const CPoint& kFirst, const CPoint& kSecond)
{
	float Length = (kSecond.x - kFirst.x) * (kSecond.x - kFirst.x) + (kSecond.y - kFirst.y) * ( kSecond.y - kFirst.y);
	if(Length != 0)
	{
		return sqrt(Length);
	}
	return 0;
}

void DragRect::OnMouseMove(int iGizmoSensor, const CPoint& kMousePt, GIZMO::EditAble* pkEditable)
{
	float Length = GetLength(FirstMousePt, kMousePt);
	if(Length < 5.0f)
	{
		return;
	}
	NiPick::Record* pkRecord = nsCsGBTerrain::g_pCurRoot->Pick_T_TB_TBP_ByTool( CAMERA_ST.GetCameraObj(), kMousePt.x, kMousePt.y );		
	if(pkRecord != NULL )
	{
		m_CurrMouse = pkRecord->GetIntersection();
		if(m_FirstMouseClick != NiPoint3::ZERO &&  m_FirstMouseClick != m_CurrMouse)
		{
			NiPoint3 kCenter = (m_FirstMouseClick+ m_CurrMouse)*0.5f;
			float HalfWidht = abs(m_CurrMouse.x - m_FirstMouseClick.x) * 0.5f;
			float HalfHeight = abs(m_CurrMouse.y - m_FirstMouseClick.y) * 0.5f;

			NiPoint3* pkVT = m_spMesh->GetVertices();
			pkVT[LB] = NiPoint3( - HalfWidht,  - HalfHeight, 0.0f);
			pkVT[LT] = NiPoint3( - HalfWidht,   HalfHeight, 0.0f);
			pkVT[RB] = NiPoint3(  HalfWidht,  - HalfHeight, 0.0f);
			pkVT[RT] = NiPoint3(  HalfWidht,  + HalfHeight, 0.0f);
			m_spMesh->GetModelData()->MarkAsChanged(NiGeometryData::VERTEX_MASK );
			m_spMesh->SetTranslate(kCenter);
			m_spMesh->Update(0.0f);
			m_spMesh->UpdateWorldData();
			bDragStart= true;
		}


	}

}
void DragRect::OnLButtonUp(const CPoint& kMousePt, EditAble* pkEditAble)
{
	if(bDragStart == true)
	{
		NiPoint2 kNiLT,kNiRB;
		CPoint kLT, kRB;

		NiPoint3* pkVT = m_spMesh->GetVertices();
		CAMERA_ST.GetSingleton().GetCameraObj()->WorldPtToScreenPt(pkVT[LT] + m_spMesh->GetTranslate(),
			kNiLT.x, kNiLT.y);
		CAMERA_ST.GetSingleton().GetCameraObj()->WorldPtToScreenPt(pkVT[RB] + m_spMesh->GetTranslate(),
			kNiRB.x, kNiRB.y);

		kLT.x = kNiLT.x * 800.0f;
		kLT.y = (1.0f - kNiLT.y) * 600.0f;
		kRB.x = kNiRB.x* 800.0f;
		kRB.y = (1.0f - kNiRB.y) * 600.0f;
		CRect kDragRect;
		kDragRect.SetRect(kLT, kRB);
		g_ObjectMng.SelectByDrag(pkVT[LT] + m_spMesh->GetTranslate(), pkVT[RB] + m_spMesh->GetTranslate());
	}
	m_spMesh->SetAppCulled(true);
	m_CurrMouse = NiPoint3::ZERO;
	m_FirstMouseClick = NiPoint3::ZERO;
	bDragStart = false;


}
bool DragRect::OnLButtonDown(const CPoint& kMousePt, EditAble* pkEditAble)
{
	m_spMesh->SetAppCulled(false);
	NiPick::Record* pkRecord = nsCsGBTerrain::g_pCurRoot->Pick_T_TB_TBP_ByTool( CAMERA_ST.GetCameraObj(), kMousePt.x, kMousePt.y );		
	if(pkRecord != NULL)
	{
		FirstMousePt = kMousePt;
		m_CurrMouse = m_FirstMouseClick = pkRecord->GetIntersection();
		NiPoint3* pkVT = m_spMesh->GetVertices();
		pkVT[LB] = NiPoint3(m_FirstMouseClick.x, m_FirstMouseClick.y, 0.0f);
		pkVT[LT] = NiPoint3(m_FirstMouseClick.x, m_CurrMouse.y, 0.0f);
		pkVT[RB] = NiPoint3(m_CurrMouse.x, m_FirstMouseClick.y, 0.0f);
		pkVT[RT] = NiPoint3(m_CurrMouse.x, m_CurrMouse.y, 0.0f);
		m_spMesh->GetModelData()->MarkAsChanged(NiGeometryData::VERTEX_MASK );
		m_spMesh->Update(0.0f);
	}
	return true;
}


void DragRect::OnCheckPicked(const CPoint& kMousePt)
{

}
void DragRect::OnObjectSelected(const NiPoint3& kMousePt)
{

}
void DragRect::OnShowGizmo(const NiPoint3& kWorldPos)
{

}
void DragRect::OnHideGizmo()
{
	m_spModel->SetAppCulled(true);
	m_spModel->SetTranslate(NiPoint3::ZERO);
	m_spModel->Update(0.0f);
}
