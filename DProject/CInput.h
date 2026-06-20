#ifndef __INPUT_H__
#define __INPUT_H__

class CInput : public NiMemObject
{
public:
	CInput()
	{
		m_bExclusiveMouse = false;
	}

	virtual ~CInput()
	{
		m_spInputSystem = NULL;
	}

public:
	inline NiInputSystem* GetInputSystem()
	{
		return m_spInputSystem;
	}


	NiInputSystem::CreateParams* GetInputSystemCreateParams();
	bool CreateInputSystem();

	void UpdateInput();

public:
	NiInputSystemPtr m_spInputSystem;
	bool m_bExclusiveMouse;
};

#endif