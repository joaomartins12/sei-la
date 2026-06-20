
#pragma once 


#include "Preview_Char.h"
#include "Preview_TObj.h"
#include "Preview_Tex.h"

class cPreview
{

public:
	void				Delete();
	void				Init();

	void				PreViewReset();
	void				PostViewReset();

	void				Update();
	void				Render();
	void				OnMainViewerSizeChange(int const& x, int const& y);

	void				SetPreview( cPreviewObj::ePREVIEW_OBJ_TYPE eType );

protected:
	//CsGBCamera				m_PreviewCamera;
	NiRenderTargetGroupPtr	m_pRenderTarget;
	NiScreenElementsPtr		m_pScreenElement;

protected:
	cPreviewObj*			m_pCurPreview;

protected:
	cPreviewObj_Char		m_Char;
	cPreviewObj_TObj		m_TObj;
	cPreviewObj_Tex			m_Texture;
public:
	cPreviewObj_Char*		GetChar(){ return &m_Char; }
	cPreviewObj_TObj*		GetTObj(){ return &m_TObj; }
	cPreviewObj_Tex*		GetTexture(){ return &m_Texture; }
};


extern cPreview	g_Preview;