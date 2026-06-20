#pragma once
#include "../../ContentsSystem/ContentsSystem.h"

class CNeonDisplayContents
	: public IUIContentSubject< CNeonDisplayContents >
{
public:
	enum eEventType
	{
		eAddNeonDisplay,
	};

	static int const IsContentsIdentity(void);

public:
	CNeonDisplayContents(void);
	virtual ~CNeonDisplayContents(void);

	virtual int const GetContentsIdentity(void) const;
	virtual bool Initialize(void);
	virtual void UnInitialize(void);
	virtual bool IntraConnection(ContentsSystem& System);
	virtual void Update(float const& fElapsedTime);
	virtual void NotifyEvent(int const& iNotifiedEvt);
	virtual void MakeMainActorData(void);
	virtual void ClearMainActorData(void);
	virtual void MakeWorldData(void);
	virtual void ClearWorldData(void);

	std::wstring GetShowMessageAndPop();
	void		 closeDisplayWindow();

private:
	void		NTF_NeonDisplayData(void* pData);

	std::wstring GetItemName(DWORD const& dwItemCode);
	std::wstring GetDigimonType( int const& nType ) const;
	std::wstring GetDigimonTableName( DWORD const& dwDigimonTBID ) const;

private:
	std::queue<std::wstring>	m_messageQue;
};
