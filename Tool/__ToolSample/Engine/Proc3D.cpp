

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
	m_Helper.ResetDevice();
}

void cProc3D::Delete()
{
	m_Helper.Delete();
}

void cProc3D::Init()
{
	m_Helper.Init();
}

void cProc3D::Update( float fDeltaTime )
{
	m_Helper.Update();
}

void cProc3D::Render()
{
	m_Helper.Render();
}