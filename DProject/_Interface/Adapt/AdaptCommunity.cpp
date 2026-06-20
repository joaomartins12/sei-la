#include "StdAfx.h"
#include "AdaptCommunity.h"

#include "../../ContentsSystem/ContentsSystemDef.h"
#include "../00.Contents/CommunityContent.h"

AdaptCommunityContesnt::AdaptCommunityContesnt(void):m_pContents(NULL)
{
}

AdaptCommunityContesnt::~AdaptCommunityContesnt(void)
{
}

bool AdaptCommunityContesnt::SetContents(IUIContentBase* pContents)
{
	SAFE_POINTER_RETVAL( pContents, false );
	if( E_CT_COMMUNITY != pContents->GetContentsIdentity() )
		return false;

	m_pContents = static_cast< cCommunityContent* >( pContents );

	return true;
}
#ifdef GUILD_RENEWAL
int AdaptCommunityContesnt::GetUsingSkillValue(int nApply) const
{
	SAFE_POINTER_RETVAL(m_pContents, false);
	return m_pContents->GetUsingSkillValue(nApply);
}
#endif

bool AdaptCommunityContesnt::IhaveGuild() const
{
	SAFE_POINTER_RETVAL(m_pContents, false);
	return m_pContents->IsHaveMyInfo();
}