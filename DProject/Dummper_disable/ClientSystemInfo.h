#pragma once

class ClientSystemInfo
{

protected:
	LPDIRECTDRAW7		m_pkDirectDraw7;

public:
	void Write(char* szPath);		// PC의 정보를 dump이름과 동일하게 txt로 생성합니다.

protected:
	void _WriteGraphicInfo(FILE* pkFile);
	void _Write_OS_CPUInfo(FILE* pkFile);
	

protected:
	std::string _GetCPUModel();			//!< CPU Model, Core 정보를 문자열로 반환합니다.
	std::string _GetWindowsVersion();	//!< OS 이름을 문자열로 반환합니다.
	std::string _GetWindowsPack();		//!< OS의 버전을 문자열로 반환합니다.
	std::string _GetMemoryInfo();		//!< PC의 RAM 정보를 문자열로 반환합니다.
	std::string _GetBit();				//!< Pc의 Bit 정보를 문자열로 반환합니다.
public:
	ClientSystemInfo(void);
	~ClientSystemInfo(void);
};
