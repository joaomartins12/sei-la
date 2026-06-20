
#include "stdafx.h"
#include "ModelRes.h"

void cModel_Res::Init( NiActorManager* pAM, DWORD dwModelKey )
{
	m_pAM = pAM;
	cModel_Base::_Init( dwModelKey );

	NiNode* pNode = NiDynamicCast( NiNode, m_pAM->GetNIFRoot() );
	nsCSGBFUNC::InitAnimation( pNode, NiTimeController::APP_TIME, NiTimeController::LOOP );
	nsCSGBFUNC::SetTextureApplyMode( pNode, NiTexturingProperty::APPLY_REPLACE );
	pNode->UpdateProperties();
	pNode->Update( 0.0f );
}