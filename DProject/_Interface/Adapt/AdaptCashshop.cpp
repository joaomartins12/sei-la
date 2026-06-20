#include "StdAfx.h"
#include "AdaptCashshop.h"

#include "../../ContentsSystem/ContentsSystemDef.h"
#include "../00.Contents/NewCashshopContents.h"

AdaptCashshop::AdaptCashshop(void):m_pContents(NULL)
{
}

AdaptCashshop::~AdaptCashshop(void)
{
}

bool AdaptCashshop::SetContents(IUIContentBase* pContents)
{
	SAFE_POINTER_RETVAL( pContents, false );
	if( E_CT_NEW_CASHSHOP != pContents->GetContentsIdentity() )
		return false;

	m_pContents = static_cast< NewCashshopContents* >( pContents );

	return true;
}

void AdaptCashshop::GetAllIncludePackageItemsFromProductIDX(DWORD const& dwProductID, std::wstring & wsProductName, LIST_PACKAGE_ITEMS & listData)
{
	SAFE_POINTER_RET( m_pContents );	

	m_pContents->GetAllIncludePackageItemsFromProductIDX( dwProductID, wsProductName, listData );
}