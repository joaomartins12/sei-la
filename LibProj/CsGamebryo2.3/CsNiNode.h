

#pragma once 

NiSmartPointer(CsNiNode);

class CsGBTerrainNode;
class NIMAIN_ENTRY CsNiNode : public NiNode
{
	NiDeclareRTTI;
	NiDeclareClone(CsNiNode);
	NiDeclareStream;

public:
	CsNiNode();
	virtual ~CsNiNode();

public:
	void				SetParentCsTNode( CsGBTerrainNode* pParentCsTNode );		
	void				UpdateBuffer();
	virtual void		OnVisible(NiCullingProcess& kCuller);

protected:
	CsGBTerrainNode*	m_pParentCsTNode;
};

