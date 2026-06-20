
#include "stdafx.h"
#include "01_Object.h"

CsCROOTRTTI_CPP( CsC_Object, RTTI_OBJECT )

void CsC_Object::ThreadCallBack_LoadCompleate( int unitType, int nAddRefCount )
{
	switch( unitType )
	{
	case sTCUnit::KFM_File:
	case sTCUnit::NIF_File:
		AddRefLinkObject( nAddRefCount );		
		break;
	//case sTCUnit::CARD_File:
	case sTCUnit::DPart_File:
	case sTCUnit::All_Part_File:
	case sTCUnit::One_Part_File:
	case sTCUnit::Part_Delete:
		AddRefLinkPart( nAddRefCount );
		break;

	default:
		assert_cs( false );
	}

	DecreaseRefOnThreadLoad();
}