
#pragma once 

#include "PreviewObj.h"

class cPreviewObj_Char : public cPreviewObj
{
public:
	cPreviewObj_Char(){
		m_eType = POT_CHAR;
	}

public:
	void				Delete();
	void				Init();

	void				Update( CsGBSubCamera* pCamera );
	void				Render( CsGBSubCamera* pCamera );

	// ===== ¸ðµ¨ ºä
protected:
	cModel_Res*			m_pCurModelView;
public:
	bool				SetModelView( DWORD dwModelID );
	void				ReleaseModelView();

	virtual bool		IsRender(){ return ( m_pCurModelView != NULL ); }


	//====== ¸ðµ¨ ¸®¼Ò½º
public:	
	typedef std::map< DWORD, cModel_Res* >				MAP_MODELRES;
	typedef std::map< DWORD, cModel_Res* >::iterator	MAP_MODELRES_IT;
	CsMemPool< cModel_Res >	m_mempoolModelRes;
protected:	
	MAP_MODELRES			m_mapModelRes;

public:	
	cModel_Res*				GetModelRes( DWORD dwModelKey );
};

