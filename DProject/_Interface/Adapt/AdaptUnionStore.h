#pragma once

class IUIContentBase;
class UnionStoreContents;

class AdaptUnionStore
{
public:
	explicit AdaptUnionStore(void);
	virtual ~AdaptUnionStore(void);

	bool	SetContents(IUIContentBase* pContents);
	bool	IhaveUnionStore() const;

private:
	UnionStoreContents* m_pContents;
};