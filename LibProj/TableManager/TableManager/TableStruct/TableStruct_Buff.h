#pragma once
BEGIN_NAME_TABLE

struct sBuffInfo
{
	sBuffInfo():s_dwID(0), s_nBuffIcon(0), s_nBuffType(0), s_nBuffLifeType(0), s_nBuffTimeType(0)
				, s_nMinLv(0), s_nBuffClass(0), s_dwSkillCode(0), s_dwDigimonSkillCode(0), s_bDelete(false)
				, s_nConditionLv(0)
	{}

	DWORD			s_dwID;

	USHORT			s_nBuffIcon;
	USHORT			s_nBuffType;
	USHORT			s_nBuffLifeType;
	USHORT			s_nBuffTimeType;

	USHORT			s_nMinLv;
	USHORT			s_nBuffClass;
	DWORD			s_dwSkillCode;
	DWORD			s_dwDigimonSkillCode;
	bool			s_bDelete;
	std::string		s_szEffectFile;
	USHORT			s_nConditionLv;

	std::wstring	s_szName;
	std::wstring	s_szComment;
};

END_NAME_TABLE
