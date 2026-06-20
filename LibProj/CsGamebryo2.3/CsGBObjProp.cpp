
#include "stdafx.h"
#include "CsGBObjProp.h"

CsGBObjProp::CsGBObjProp()
{
	m_pParentObj = NULL;
	m_pParentNiNode = NULL;
}

CsGBObjProp::~CsGBObjProp()
{
	m_pParentObj = NULL;
	m_pParentNiNode = NULL;
}

void CsGBObjProp::DeleteProp( CsGBObjProp* pDeleteProp )
{
	pDeleteProp->Delete();

	switch( pDeleteProp->GetType() )
	{
	case PROP_ALPHA:
		CsGBObjProp_Alpha::DeleteInstance( (CsGBObjProp_Alpha*)pDeleteProp );
		return;
	case PROP_ABV:
		CsGBObjProp_ABV::DeleteInstance( (CsGBObjProp_ABV*)pDeleteProp );
		return;
	case PROP_AV_CAP:
		CsGBObjProp_AvCap::DeleteInstance( (CsGBObjProp_AvCap*)pDeleteProp );
		return;
	case PROP_AV_TBP:
		CsGBObjProp_AvTbp::DeleteInstance( (CsGBObjProp_AvTbp*)pDeleteProp );
		return;
	case PROP_SOUND:
		CsGBObjProp_Sound::DeleteInstance( (CsGBObjProp_Sound*)pDeleteProp );
		return;
	case PROP_ACTOR:
		CsGBObjProp_Actor::DeleteInstance( (CsGBObjProp_Actor*)pDeleteProp );
		return;
	}
	assert_cs( false );
}

void CsGBObjProp::Delete()
{
	m_pParentObj = NULL;
	m_pParentNiNode = NULL;
}

void CsGBObjProp::Init( CsGBObject* pParentObj )
{
	assert_cs( m_pParentObj == NULL );
	assert_cs( m_pParentNiNode == NULL );
	m_pParentObj = pParentObj;
	m_pParentNiNode = m_pParentObj->GetNiNode();
}

void CsGBObjProp::Init( NiNode* pParentNiNode )
{
	assert_cs( m_pParentObj == NULL );
	assert_cs( m_pParentNiNode == NULL );
	m_pParentNiNode = pParentNiNode;
}