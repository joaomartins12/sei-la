#include "StdAfx.h"
#include "AdaptGotchaMachine.h"

#include "../../ContentsSystem/ContentsSystemDef.h"
#include "../00.Contents/BattleSystemContent.h"

AdaptGotchaMachine::AdaptGotchaMachine(void) : m_pContents(NULL)
{

}

AdaptGotchaMachine::~AdaptGotchaMachine(void)
{

}

bool AdaptGotchaMachine::SetContents(IUIContentBase* pContents)
{
	SAFE_POINTER_RETVAL( pContents, false );

	if( E_CT_GOTCHAMACHINE_CONTENTS != pContents->GetContentsIdentity() )
		return false;

	m_pContents = static_cast< cGotchaMachineContents* >( pContents );
	return true;
}

bool AdaptGotchaMachine::IsGotchaPlaying() const
{
	SAFE_POINTER_RETVAL( m_pContents, false );
	return m_pContents->IsGotchaPlaying();
}
