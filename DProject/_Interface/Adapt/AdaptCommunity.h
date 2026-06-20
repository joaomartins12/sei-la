#pragma once

class IUIContentBase;
class cCommunityContent;

class AdaptCommunityContesnt
{
public:
	explicit AdaptCommunityContesnt(void);
	virtual ~AdaptCommunityContesnt(void);

	bool	SetContents(IUIContentBase* pContents);

#ifdef GUILD_RENEWAL
	int		GetUsingSkillValue(int nApply) const;
#endif

	bool	IhaveGuild() const;

private:
	cCommunityContent* m_pContents;
};