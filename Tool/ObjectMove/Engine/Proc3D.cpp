

#include "stdafx.h"
#include "Proc3D.h"


cProc3D* g_pProc3D = NULL;


void cProc3D::GlobalInit()
{
	assert_cs( g_pProc3D == NULL );
	g_pProc3D = new cProc3D;
	g_pProc3D->Init();
}

void cProc3D::GlobalShotdown()
{
	if( g_pProc3D == NULL )
		return;

	g_pProc3D->Delete();
	SAFE_DELETE( g_pProc3D );
}

void cProc3D::ResetDevice()
{
	m_Map.ResetDevice();
}

void cProc3D::Delete()
{
	m_Map.Delete();
	cMap::GlobalShotDown();
}

void cProc3D::Init()
{
	cMap::GlobalInit();
	m_Map.Init();
}

void cProc3D::Update( float fDeltaTime )
{
	m_Map.Update();
}

void cProc3D::Render()
{
	m_Map.Render();
}