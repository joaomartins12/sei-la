
#include "stdafx.h"
#include "ToolObjectMng.h"

cToolObjectMng	g_ToolObjMng;


void cToolObjectMng::Delete()
{
	g_ObjectMng.Delete();
	g_ModelMng.Delete();

	g_Preview.Delete();
}

void cToolObjectMng::Init()
{
	g_ModelMng.Init();
	g_ObjectMng.Init();

	g_Preview.Init();
}

void cToolObjectMng::PreViewDelete()
{
	g_ModelMng.PreViewDelete();
}

void cToolObjectMng::PreViewReset()
{	
	g_ObjectMng.Delete();

	g_Preview.PreViewReset();
}

void cToolObjectMng::PostViewReset()
{
	g_ObjectMng.Init();	
	g_ObjectMng.PosetViewReset();	

	g_Preview.PostViewReset();
	g_ModelMng.PostViewReset();
}

void cToolObjectMng::Update()
{
	g_ObjectMng.Update();
	g_ModelMng.Update();

	g_Preview.Update();
}

void cToolObjectMng::Render()
{
	g_ModelMng.Render();

	g_Sorting.Render( CAMERA_ST.GetCameraObj() );

	g_ObjectMng.Render();

	g_Preview.Render();
}

void cToolObjectMng::LButtonDown( CPoint pt )
{
	g_ModelMng.LButtonDown( pt );
}

void cToolObjectMng::MouseMove( CPoint pt )
{
	g_ModelMng.MouseMove( pt );
}