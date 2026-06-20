#include "StdAfx.h"
#include "AdaptMacroProtectSystem.h"

#include "../../ContentsSystem/ContentsSystemDef.h"
#include "../00.Contents/MacroProtectContents.h"

AdaptMacroProtect::AdaptMacroProtect(void):m_pContents(NULL)
{
}

AdaptMacroProtect::~AdaptMacroProtect(void)
{
}

bool AdaptMacroProtect::SetContents(IUIContentBase* pContents)
{
	SAFE_POINTER_RETVAL( pContents, false );
	if( E_CT_MACROPROTECT != pContents->GetContentsIdentity() )
		return false;

	m_pContents = static_cast< CMacroProtectContents* >( pContents );

	return true;
}

bool AdaptMacroProtect::IsMacroProtectUsing() const
{
	SAFE_POINTER_RETVAL(m_pContents, false);
	return m_pContents->IsMacroProtectUsing();
}