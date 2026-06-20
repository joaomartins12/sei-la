
#include "stdafx.h"
#include "CsGBObjOpt.h"

CsGBObjOpt::CsGBObjOpt()
{
	m_pParentObj = NULL;
	m_pParentUserDefine = NULL;
}

CsGBObjOpt::~CsGBObjOpt()
{
}

void CsGBObjOpt::DeleteOption( CsGBObjOpt* pDeleteOpt )
{
	switch( pDeleteOpt->GetType() )
	{
	case OPT_DARKMAP:
		pDeleteOpt->Delete();
		CsGBObjOpt_DarkMap::DeleteInstance( (CsGBObjOpt_DarkMap*)pDeleteOpt );
		return;
	}
	assert_cs( false );
}

void CsGBObjOpt::Delete()
{
	m_pParentObj = NULL;
	m_pParentUserDefine = NULL;
}

void CsGBObjOpt::Init( CsGBObject* pParentObj )
{
	assert_cs( m_pParentObj == NULL );
	m_pParentObj = pParentObj;
	m_pParentUserDefine = m_pParentObj->GetUserDefine();
}