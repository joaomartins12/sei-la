
#pragma once 

class cToolObjectMng
{
public:
	void				Delete();
	void				Init();

	void				PreViewDelete();
	void				PreViewReset();
	void				PostViewReset();

	void				Update();
	void				Render();

	void				LButtonDown( CPoint pt );
	void				MouseMove( CPoint pt );
};

extern cToolObjectMng	g_ToolObjMng;