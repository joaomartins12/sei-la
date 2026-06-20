#pragma once

class IUIContentBase;
class CDigimonArchiveContents;

class AdaptDigimonArchiveSystem
{
public:
	explicit AdaptDigimonArchiveSystem(void);
	virtual ~AdaptDigimonArchiveSystem(void);

	bool			SetContents(IUIContentBase* pContents);  
	
	const int		GetOpenedArchiveCnt(void);
	const DWORD GetDigimonBaseIDInArchive(int ArchiveSlotIdx);
private:
	CDigimonArchiveContents* m_pDigimonArchiveSystemContents;
};