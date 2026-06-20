#include "StdAfx.h"
#include "AdaptFriendShoutOut.h"

#include "../../ContentsSystem/ContentsSystemDef.h"
#include "../00.Contents/FriendListContents.h"

AdaptFriendShoutOut::AdaptFriendShoutOut(void):m_pContents(NULL)
{
}

AdaptFriendShoutOut::~AdaptFriendShoutOut(void)
{
}

bool AdaptFriendShoutOut::SetContents(IUIContentBase* pContents)
{
	SAFE_POINTER_RETVAL( pContents, NULL );
	if( E_CT_FRIENDLIST != pContents->GetContentsIdentity() )
		return false;

	m_pContents = static_cast< cFriendListContents* >( pContents );

	return true;
}

bool AdaptFriendShoutOut::IsShoutOutUser( std::wstring const& name ) const
{
	SAFE_POINTER_RETVAL(m_pContents, true);

	return m_pContents->IsShutout( &name );
}

void AdaptFriendShoutOut::GetFriendList( std::list<std::wstring>& friendlist ) const
{
	SAFE_POINTER_RET(m_pContents);
	const cFriendListContents::FRIENDLIST& info = m_pContents->GetFriendList();
	cFriendListContents::CFITR it = info.begin();
	for( ; it != info.end(); ++it )
		friendlist.push_back( (*it).s_szName );
}

bool AdaptFriendShoutOut::IsMyFriend( std::wstring const& wsTamerName ) const
{
	SAFE_POINTER_RETVAL(m_pContents, false);
	return m_pContents->IsMyFriend( wsTamerName );
}