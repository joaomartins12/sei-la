

#pragma once 

NiSmartPointer(CsNiGeometry);

class NIMAIN_ENTRY CsNiGeometry : public NiGeometry
{
	NiDeclareRTTI;
	/*NiDeclareClone(CsNiGeometry);
	NiDeclareStream;*/

public:	
	virtual void		RenderImmediate(NiRenderer* pkRenderer);
};

