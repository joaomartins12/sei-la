#pragma once

class IUIContentBase;

#include "../00.Contents/TutorialContents.h"

class AdaptTutorialQuest
{
public:
	explicit AdaptTutorialQuest(void);
	virtual ~AdaptTutorialQuest(void);

	bool			SetContents(IUIContentBase* pContents);  
	bool			IsTutorialPlaying() const;

	bool			IsTutorialPlaying_ClientMode() const;
	
private:
	CTutorialContents* m_pContents;
};