
#include "stdafx.h"
#include "CsNiNode.h"

NiImplementRTTI(CsNiNode, NiNode);
NiImplementCreateClone(CsNiNode);
//---------------------------------------------------------------------------

CsNiNode::CsNiNode():m_pParentCsTNode(NULL)
{
}

CsNiNode::~CsNiNode()
{
	m_pParentCsTNode = NULL;
}

void CsNiNode::UpdateBuffer()
{
	if( m_pParentCsTNode )
		m_pParentCsTNode->UpdateBuffer();
}

void CsNiNode::SetParentCsTNode( CsGBTerrainNode* pParentCsTNode )
{ 
	m_pParentCsTNode = pParentCsTNode; 
}

void CsNiNode::OnVisible(NiCullingProcess& kCuller)
{
	NiNode::OnVisible(kCuller);
}

void CsNiNode::CopyMembers(CsNiNode* pkDest, NiCloningProcess& kCloning)
{
	NiNode::CopyMembers(pkDest, kCloning);
}

//---------------------------------------------------------------------------
// streaming
//---------------------------------------------------------------------------
NiImplementCreateObject(CsNiNode);
//---------------------------------------------------------------------------
void CsNiNode::LoadBinary(NiStream& kStream)
{
	NiNode::LoadBinary(kStream);
}
//---------------------------------------------------------------------------
void CsNiNode::LinkObject(NiStream& kStream)
{
	NiNode::LinkObject(kStream);
}
//---------------------------------------------------------------------------
bool CsNiNode::RegisterStreamables(NiStream& kStream)
{
	if (!NiNode::RegisterStreamables(kStream))
	{
		return false;
	}
	return true;
}
//---------------------------------------------------------------------------
void CsNiNode::SaveBinary(NiStream& kStream)
{
	NiNode::SaveBinary(kStream);
}
//---------------------------------------------------------------------------
bool CsNiNode::IsEqual(NiObject* pkObject)
{
	if (!NiNode::IsEqual(pkObject))
		return false;

	return true;
}