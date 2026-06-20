
#pragma once 

#include "PreviewObj.h"

class cPreviewObj_TObj : public cPreviewObj
{
public:
	cPreviewObj_TObj(){
		m_eType = POT_TERRAIN_OBJ;
	}

public:
	void				Delete();
	void				Init();

	void				Update( CsGBSubCamera* pCamera );
	void				Render( CsGBSubCamera* pCamera );

	// ===== ¸ðµ¨ ºä
protected:
	NiNodePtr			m_pCurTObj;
	float				m_fAniTime;

public:
	void				SetModelView( TCHAR* szPath, TCHAR* szName );
	void				ReleaseModelView();

	virtual bool		IsRender(){ return ( m_pCurTObj != NULL ); }
};

