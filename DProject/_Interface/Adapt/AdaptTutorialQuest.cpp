#include "StdAfx.h"
#include "AdaptTutorialQuest.h"

#include "../../ContentsSystem/ContentsSystemDef.h"

AdaptTutorialQuest::AdaptTutorialQuest(void):m_pContents(NULL)
{
}

AdaptTutorialQuest::~AdaptTutorialQuest(void)
{
}

bool AdaptTutorialQuest::SetContents(IUIContentBase* pContents)
{
	SAFE_POINTER_RETVAL( pContents, NULL );
	if( E_CT_TUTORIAL_CONTENTS != pContents->GetContentsIdentity() )
		return false;

	m_pContents = static_cast< CTutorialContents* >( pContents );

	return true;
}

bool AdaptTutorialQuest::IsTutorialPlaying() const
{
	SAFE_POINTER_RETVAL( m_pContents, false );
	return m_pContents->IsTutorialPlaying();
}

bool AdaptTutorialQuest::IsTutorialPlaying_ClientMode() const
{
	SAFE_POINTER_RETVAL( m_pContents, false );
	return m_pContents->IsTutorialPlaying_ClientMode();
}
