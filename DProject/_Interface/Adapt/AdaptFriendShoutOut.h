#pragma once

class IUIContentBase;
class cFriendListContents;

class AdaptFriendShoutOut
{
public:
	explicit AdaptFriendShoutOut(void);
	virtual ~AdaptFriendShoutOut(void);

	bool	SetContents(IUIContentBase* pContents);

	bool	IsShoutOutUser( std::wstring const& name ) const;
	bool	IsMyFriend( std::wstring const& wsTamerName ) const;
	void	GetFriendList( std::list<std::wstring>& friendlist ) const;

private:
	cFriendListContents* m_pContents;
};