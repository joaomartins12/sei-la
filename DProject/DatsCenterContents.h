#pragma once

class CDatsCenterContents
	: public IUIContentSubject< CDatsCenterContents >
{
public:
	static int const IsContentsIdentity(void);

public:
	CDatsCenterContents(void);
	virtual ~CDatsCenterContents(void);

	virtual int const GetContentsIdentity(void) const;
	virtual bool Initialize(void);
	virtual void UnInitialize(void);
	virtual bool IntraConnection(ContentsSystem& System);
	virtual void Update(float const& fElapsedTime);
	virtual void NotifyEvent(int const& iNotifiedEvt);
	virtual void MakeMainActorData(void);
	virtual void ClearMainActorData(void);
};
typedef	CDatsCenterContents	DCCON;