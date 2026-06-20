
#pragma once 

#include "ShaderHelper.h"
#include "PartChar.h"
#include "NormalChar.h"



class CViewModel
{
public:
	CViewModel();
public:
	static	bool		CALLBACK_ThreadDelete( sTCUnit* pUnit );

protected:	
	CsC_AvObject*	m_pModel;
	CsC_AvObject*	m_pDestModel;	

protected:
	CsGeomObj		m_ClientBox;
	bool			m_bRenderClientBox;

public:
	void			Delete();
	void			DeleteSrcModel();
	void			DeleteDestModel();
	void			Init();

	CsC_AvObject*	SetSrcModel( DWORD dwModelID, bool bABVDraw );
	CsC_AvObject*	SetDestModel( DWORD dwModelID );

	void			Update();
	void			Render();

	void			ApplyScale();	
	void			SetRenderClientBox( bool bRenderClientBox ){ m_bRenderClientBox = bRenderClientBox; }

	void			ChangePart( int nPartIndex, TCHAR* szPart );


public:
	CsC_AvObject*	GetSrcMode(){ return m_pModel; }
	CsC_AvObject*	GetDestMode(){ return m_pDestModel; }

	//=====================================================================================
	//
	//	플랜
	//
	//=====================================================================================
protected:
	CsNodeObj		m_Plan;
	
public:
	void			SetPlan_TwoSide( bool bEnable );
	void			SetPlan_WireFrame( bool bEnable );

	//=====================================================================================
	//
	//	오프셋 메쉬
	//
	//=====================================================================================
protected:
	CsNodeObj		m_OffsetMesh;
	
	//=====================================================================================
	//
	//	더미 오브젝트
	//
	//=====================================================================================
protected:
	CsNodeObj		m_Eff0;
	CsNodeObj		m_Eff1;
	NiPoint3		m_vEff0;
	NiPoint3		m_vEff1;

	bool			m_bRenderSpline;

public:
	void			SetSplinePos( NiPoint3 v1, NiPoint3 v2 ); 
	void			ReleaseSpline();

	//=====================================================================================
	//
	//	쉐이더
	//
	//=====================================================================================
protected:
	ShaderHelper			m_ShaderHelper;

	//=====================================================================================
	//
	//	디폴트 라이트 - 지형이 없을 경우에 쓰는 라이트
	//
	//=====================================================================================
protected:
	NiDirectionalLightPtr	m_pDefaultLight;
protected:
	void				_CreateDefaultLight();
	void				_DeleteDefaultLight();
};

extern CViewModel g_ViewModel;