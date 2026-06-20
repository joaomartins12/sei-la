

#include "stdafx.h"
#include "ScenePreview.h"

cScenePreview::cScenePreview()
{	
}

cScenePreview::~cScenePreview()
{
}

//==========================================================================================
//
//		Delete
//
//==========================================================================================
void cScenePreview::Delete()
{
}

//==========================================================================================
//
//		Init
//
//==========================================================================================
void cScenePreview::Init()
{
	m_ePreviewMode = PM_WORLD;
}



void cScenePreview::Reset()
{
	g_Actor.ReleaseVisible();
	m_ePreviewMode = PM_WORLD;
}

//==========================================================================================
//
//		Update
//
//==========================================================================================

void cScenePreview::Update()
{
	if( m_ePreviewMode == PM_CHAR )
		g_Actor.Update();
}

//==========================================================================================
//
//		Render
//
//==========================================================================================

void cScenePreview::Render()
{
	if( m_ePreviewMode == PM_CHAR )
		g_Actor.Render();

	g_MapMng.RenderTerrain();
	g_MapMng.RenderObject();
}

//==========================================================================================
//
//		Mouse
//
//==========================================================================================
void cScenePreview::OnLButtonDown( CPoint pt )
{
	switch( m_ePreviewMode )
	{
	case PM_WORLD:
		{
			NiPoint3 origin, dir;
			CAMERA_ST.WindowPointToRay( pt.x, pt.y, origin, dir );
			NiPick::Record* pRecord = nsCsGBTerrain::g_pCurRoot->Pick_T_TB_TBP_ByTool( origin, dir );
			if( pRecord == NULL )
				return;

			m_ePreviewMode = PM_CHAR;
			g_Actor.ResetVisible( pRecord->GetIntersection() );
		}
		break;
	case PM_CHAR:
		{
			NiPoint3 origin, dir;
			CAMERA_ST.WindowPointToRay( pt.x, pt.y, origin, dir );
			NiPick::Record* pRecord = nsCsGBTerrain::g_pCurRoot->Pick_T_TB_TBP_ByTool( origin, dir );
			if( pRecord == NULL )
				return;

			g_Actor.SetMoveDest( pRecord->GetIntersection() );
		}
		break;
	}	
}

void cScenePreview::OnLButtonUp( CPoint pt )
{	
}

void cScenePreview::OnRButtonDown( CPoint pt )
{
}

void cScenePreview::OnRButtonUp( CPoint pt )
{
}

void cScenePreview::OnMouseMove( CPoint pt )
{
}

void cScenePreview::OnMouseWheel( short zDelta )
{
}


//==========================================================================================
//
//		Key
//
//==========================================================================================

void cScenePreview::KeyEscape()
{
	if( m_ePreviewMode == PM_CHAR )
	{
		Reset();
	}
}