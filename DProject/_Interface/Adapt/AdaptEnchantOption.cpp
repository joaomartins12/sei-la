#include "StdAfx.h"
#include "AdaptEnchantOption.h"

#include "../../ContentsSystem/ContentsSystemDef.h"

AdaptEnchantOption::AdaptEnchantOption(void) : m_pContents(NULL)
{

}

AdaptEnchantOption::~AdaptEnchantOption(void)
{

}

bool AdaptEnchantOption::SetContents(IUIContentBase* pContents)
{
	SAFE_POINTER_RETVAL( pContents, false );

	if( E_CT_ENCHANT_OPTION_CONTENTS != pContents->GetContentsIdentity() )
		return false;

	m_pContents = static_cast< cEnchantOptionContents* >( pContents );
	return true;
}

bool AdaptEnchantOption::IsProcessingEnchant() const
{
	SAFE_POINTER_RETVAL( m_pContents, false );
	return m_pContents->IsProcessingEnchant();
}
