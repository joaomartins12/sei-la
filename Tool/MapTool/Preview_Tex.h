
#pragma once 

#include "PreviewObj.h"

class cPreviewObj_Tex : public cPreviewObj
{
public:
	cPreviewObj_Tex(){
		m_eType = POT_TEXTURE;
	}

public:
	void				Delete();
	void				Init();

	void				Update( CsGBSubCamera* pCamera );
	void				Render( CsGBSubCamera* pCamera );

	// ===== ¸ðµ¨ ºä
protected:
	bool				m_bTexRender;
	NiNodePtr			m_pTexObj;
public:
	void				SetModelView( TCHAR* szPath, TCHAR* szName );
	void				ReleaseModelView();

	virtual bool		IsRender(){ return m_bTexRender; }
};

