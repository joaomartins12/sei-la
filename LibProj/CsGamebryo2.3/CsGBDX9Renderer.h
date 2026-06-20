
#pragma once 

class CsGBDX9Renderer : public NiDX9Renderer
{
public:
	//virtual void	Do_RenderTristrips( NiTriStrips* pkTriStrips );

// protected:	
// 	void			_DrawPrimitive(NiGeometry* pkGeometry, NiGeometryData* pkData, 
// 									NiSkinInstance* pkSkinInstance, const NiTransform& kWorld, 
// 									const NiBound& kWorldBound, NiGeometryBufferData* pkBuffData);

public:
	// Renderer creation
	static CsGBDX9Renderer* CsGBCreate(unsigned int uiWidth, unsigned int uiHeight,
		unsigned int uiUseFlags, NiWindowRef kWndDevice, 
		NiWindowRef kWndFocus, unsigned int uiAdapter = D3DADAPTER_DEFAULT, 
		DeviceDesc eDesc = DEVDESC_PURE,
		FrameBufferFormat eFBFormat = FBFMT_UNKNOWN, 
		DepthStencilFormat eDSFormat = DSFMT_UNKNOWN, 
		PresentationInterval ePresentationInterval = PRESENT_INTERVAL_ONE,
		SwapEffect eSwapEffect = SWAPEFFECT_DEFAULT,
		unsigned int uiFBMode = FBMODE_DEFAULT,
		unsigned int uiBackBufferCount = 1, 
		unsigned int uiRefreshRate = REFRESHRATE_DEFAULT);

};

typedef NiPointer<CsGBDX9Renderer> CsGBDX9RendererPtr;