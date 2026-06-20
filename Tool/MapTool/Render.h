
#pragma once 

class cRender
{
protected:
	static NiWireframePropertyPtr		m_pPropWire;
	static NiZBufferPropertyPtr			m_pPropZ;
	static NiVertexColorPropertyPtr		m_pPropVertColor;
	static NiAlphaPropertyPtr			m_pPropAlpha;
	
public:
	static void		Init();
	static void		Delete();

	static void		RenderWireFrame( NiNode* pNode );


	//==== 텍스쳐에 랜더링
public:
	enum eR2T{
		R2T_DETAILMAP, R2T_BASEMAP,
	};
	static void					RenderToTexture( eR2T ert );
};