#include "stdafx.h"
#include "TalkMng.h"

void CsTalkMng::Delete()
{
	_Delete_Digimon();
	_Delete_Event();
	_Delete_Message();
	_Delete_Tip();
	_Delete_LoadingTip();

	Delete_Tutorial();
}

bool CsTalkMng::Init(char* cPath)
{
	switch (nsCsFileTable::g_eFileType)
	{
	case nsCsFileTable::FT_EXCEL:
		_LoadExcel();
		break;

	case nsCsFileTable::FT_BIN:
		if (_LoadBin(cPath) == false)
			return false;
		break;

	case nsCsFileTable::FT_FILEPACK:
		if (_LoadFilePack(cPath) == false)
			return false;
		break;

	default:
		assert_cs(false);
		return false;
	}

	return true;
}

void CsTalkMng::_LoadExcel()
{
	_LoadExcel_Digimon();
	_LoadExcel_Event();
	_LoadExcel_Message();
	_LoadExcel_Tip();
	_LoadExcel_LoadingTip();

#ifdef SIMPLE_TOOLTIP
	_LoadExcel_ToolTip();
#endif
}

#define TALK_BIN "Talk.bin"

void CsTalkMng::SaveBin(char* cPath)
{
	char cName[MAX_PATH];
	sprintf_s(cName, MAX_PATH, "%s\\%s", cPath, TALK_BIN);

	FILE* fp = NULL;
	fopen_s(&fp, cName, "wb");
	assert_cs(fp != NULL);

	if (fp == NULL)
		return;

	_SaveBin_Digimon(fp);
	_SaveBin_Event(fp);
	_SaveBin_Message(fp);
	_SaveBin_Tip(fp);
	_SaveBin_LoadingTip(fp);

#ifdef SIMPLE_TOOLTIP
	_SaveBin_ToolTip(fp);
#endif

	fclose(fp);

	_SaveBin_Tutorial(cPath);
}

bool CsTalkMng::_LoadBin(char* cPath)
{
	char cName[MAX_PATH];
	sprintf_s(cName, MAX_PATH, "%s\\%s", cPath, TALK_BIN);

	FILE* fp = NULL;
	fopen_s(&fp, cName, "rb");

	if (fp == NULL)
	{
		CsMessageBoxA(MB_OK, "%s 파일이 존재 하지 않습니다.", cName);
		return false;
	}

	_LoadBin_Digimon(fp);
	_LoadBin_Event(fp);
	_LoadBin_Message(fp);
	_LoadBin_Tip(fp);
	_LoadBin_LoadingTip(fp);

#ifdef SIMPLE_TOOLTIP
	_LoadBin_ToolTip(fp);
#endif

	fclose(fp);

	return true;
}

bool CsTalkMng::_LoadFilePack(char* cPath)
{
	char cName[MAX_PATH];
	sprintf_s(cName, MAX_PATH, "%s\\%s", cPath, TALK_BIN);

	if (!CsFPS::CsFPSystem::SeekLock(FT_PACKHANDLE))
		return false;

	int nHandle = CsFPS::CsFPSystem::GetFileHandle(FT_PACKHANDLE, cName);

	if (nHandle == 0)
	{
		CsFPS::CsFPSystem::SeekUnLock(FT_PACKHANDLE);
		CsMessageBoxA(MB_OK, "Failed to load Talk.bin from file pack: %s", cName);
		return false;
	}

	_LoadFilePack_Digimon(nHandle);
	_LoadFilePack_Event(nHandle);
	_LoadFilePack_Message(nHandle);
	_LoadFilePack_Tip(nHandle);
	_LoadFilePack_LoadingTip(nHandle);

#ifdef SIMPLE_TOOLTIP
	_LoadFilePack_ToolTip(nHandle);
#endif

	CsFPS::CsFPSystem::SeekUnLock(FT_PACKHANDLE);

	return true;
}