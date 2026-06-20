#pragma once

class CardEventContents: public IUIContentSubject< CardEventContents >
{
public:
	CardEventContents(void);
	virtual ~CardEventContents(void);	

	static int const IsContentsIdentity(void);

	enum CARDEVENT_EVENT
	{
		E_OPEN_WINDOW,
	};

public:
	virtual int const GetContentsIdentity(void) const;
	virtual bool Initialize(void);
	virtual void UnInitialize(void);
	virtual bool IntraConnection(ContentsSystem& System);
	virtual void Update(float const& fElapsedTime);
	virtual void NotifyEvent(int const& iNotifiedEvt);
	virtual void NotifyEventStream(int const& iNotifiedEvt, ContentsStream const& kStream);
	virtual void MakeMainActorData(void);
	virtual void ClearMainActorData(void);
	virtual void MakeWorldData(void);
	virtual void ClearWorldData(void);

private:
	void			InitData();
	void			openCardEventUI(void* pData);
	void			recvCardEventResult(void* pData);
	void			recvCardEventEnd(void* pData);

public:
	DWORD			getNpcID() const;
	DWORD			getResultItemCode() const;
	int				getItemCount() const;
	void			SendCardEventEnd();


private:
	DWORD			m_dwNpcID;
	DWORD			m_dwResultItemCode;
	int				m_nItemCount;
};
