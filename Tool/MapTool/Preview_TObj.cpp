
#include "stdafx.h"
#include "Preview_TObj.h"

void cPreviewObj_TObj::Delete()
{
	m_pCurTObj = 0;
}

void cPreviewObj_TObj::Init()
{
	m_pCurTObj = NULL;
	m_fAniTime = 0.0f;
}

void cPreviewObj_TObj::Update( CsGBSubCamera* pCamera )
{
	if( m_pCurTObj != NULL )
	{
		NiBound Bound = m_pCurTObj->GetWorldBound();
		pCamera->SetRotation( NI_HALF_PI*0.5f, -NI_HALF_PI*0.5f );
		pCamera->SetTranslate( NiPoint3( 0.0f, 0.0f, Bound.GetCenter().z+Bound.GetRadius()*0.01f ) );
		pCamera->SetDist( Bound.GetRadius()*1.7f + 520.0f );
		pCamera->_UpdateCamera();

		m_fAniTime += g_fElapsedTime;
		m_pCurTObj->Update( m_fAniTime, true );
	}
}

void cPreviewObj_TObj::Render( CsGBSubCamera* pCamera )
{
	if( m_pCurTObj != NULL )
		NiDrawScene( pCamera->GetCameraObj(), m_pCurTObj, *g_pCuller );
}

void cPreviewObj_TObj::SetModelView( TCHAR* szPath, TCHAR* szName )	//파일 클릭시 오브젝트 보여주는 함수
{
	TCHAR szFullPath[ MAX_PATH ] = {0, };
	if( szPath != NULL )
	{
		_stprintf_s( szFullPath, MAX_PATH, L"%s\\%s", szPath, szName );
	}
	else
	{
		_tcscpy_s( szFullPath, MAX_PATH, szName );
	}
	
	if( _taccess( szFullPath, 0 ) != 0 )
	{
		GET_MV_MAINTAB_DLG->_GetDlgObejct()->OnObjectRouteChange(szFullPath);
	}

	m_pCurTObj = 0;
	CsGBObject::sINFO ObjInfo;
	ObjInfo.Reset();
	W2M( ObjInfo.s_cObjectPath, szFullPath, OBJECT_PATH_LEN );

	sUSER_DEFINE* pUserDefine = nsCsGBTerrain::g_ObjectResMgr.GetUserDefine( &ObjInfo );
	if(pUserDefine == NULL)
		return;
	
	m_pCurTObj = nsCsGBTerrain::g_ObjectResMgr.GetNiObject( ObjInfo.s_cObjectPath, pUserDefine );
	CsGBObject::GlobalApplyUserDefine( m_pCurTObj, &ObjInfo, pUserDefine );

	nsCSGBFUNC::SetTextureApplyMode( m_pCurTObj, NiTexturingProperty::APPLY_REPLACE );	//오브젝트 뿌려준다.
	
	m_fAniTime = 0.0f;
	m_pCurTObj->UpdateProperties();
	m_pCurTObj->Update( m_fAniTime );
}

void cPreviewObj_TObj::ReleaseModelView()
{
	m_pCurTObj = 0;
}
