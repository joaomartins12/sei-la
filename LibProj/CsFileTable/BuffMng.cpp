#include "stdafx.h"
#include "BuffMng.h"

void CsBuffMng::Delete()
{
	CsBuff::MAP_IT it = m_mapBuff.begin();
	CsBuff::MAP_IT itEnd = m_mapBuff.end();

	for (; it != itEnd; ++it)
	{
		if (it->second)
		{
			it->second->Delete();
			delete it->second;
		}
	}

	m_mapBuff.clear();
}

bool CsBuffMng::Init(char* cPath)
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
		_LoadFilePack(cPath);
		break;

	default:
		assert_cs(false);
		return false;
	}

	return true;
}

CsBuff* CsBuffMng::GetBuff(USHORT nID)
{
	CsBuff::MAP_IT it = m_mapBuff.find(nID);

	if (it == m_mapBuff.end())
	{
		// IMPORTANTE:
		// Não usar assert aqui.
		//
		// Em alguns servers privados, o server envia buff IDs que não existem
		// no Buff.bin do client. Exemplo visto no crash.log: buff 40211.
		//
		// Se usarmos assert, o client fecha durante loading/entrada no mundo.
		return NULL;
	}

	return it->second;
}

void CsBuffMng::_LoadExcel()
{
#define SHEET_NAME					"Buff_Code"
#define IL_COL_ID					1
#define IL_COL_NAME_KOR				IL_COL_ID + 1
#define IL_COL_NAME_ENG				IL_COL_ID + 2
#define IL_COL_COMMENT				IL_COL_NAME_ENG + 1
#define IL_COL_ICON					5

	char cBuffer[CEXCLE_CELL_LEN];
	wchar_t wBuffer[CEXCLE_CELL_LEN];
	wchar_t wBuffer_Temp[CEXCLE_CELL_LEN];

	cExcelReader excel;
	bool bSuccess = excel.Open(nsCsFileTable::GetPath_DM_Buff(), SHEET_NAME);
	assert_cs(bSuccess == true);

	// 2줄 são descartadas.
	bSuccess = excel.MoveFirst();
	assert_cs(bSuccess == true);

	bSuccess = excel.MoveNext();
	assert_cs(bSuccess == true);

	CsBuff::sINFO info;

	while (excel.MoveNext() == true)
	{
		memset(&info, 0, sizeof(CsBuff::sINFO));

		// ID
		info.s_dwID = atoi(excel.GetData(IL_COL_ID, cBuffer));
		if (info.s_dwID == 0)
			continue;

		// Evita duplicados.
		if (m_mapBuff.find(info.s_dwID) != m_mapBuff.end())
			continue;

		memset(info.s_szName, 0, SKILL_NAME_LEN);
		memset(info.s_szComment, 0, BUFF_COMMENT_LEN);

		switch (nsCsFileTable::g_eLanguage)
		{
		case nsCsFileTable::KOREA_ORIGINAL:
		case nsCsFileTable::KOREA_TRANSLATION:
			M2W(info.s_szName, excel.GetData(IL_COL_NAME_KOR, cBuffer), SKILL_NAME_LEN);
			break;

		case nsCsFileTable::ENGLISH:
		case nsCsFileTable::ENGLISH_A:
			M2W(info.s_szName, excel.GetData(IL_COL_NAME_ENG, cBuffer), BUFF_NAME_LEN);
			break;

		case nsCsFileTable::THAILAND:
		case nsCsFileTable::TAIWAN:
		case nsCsFileTable::HONGKONG:
			wcscpy_s(info.s_szName, SKILL_NAME_LEN, excel.GetData(IL_COL_NAME_ENG, wBuffer));
			break;

		default:
			assert_cs(false);
			break;
		}

		memset(info.s_szComment, 0, BUFF_COMMENT_LEN);

		switch (nsCsFileTable::g_eLanguage)
		{
		case nsCsFileTable::KOREA_ORIGINAL:
		case nsCsFileTable::KOREA_TRANSLATION:
		case nsCsFileTable::ENGLISH:
		case nsCsFileTable::ENGLISH_A:
			M2W(info.s_szComment, excel.GetData(IL_COL_COMMENT, cBuffer), BUFF_COMMENT_LEN);
			break;

		case nsCsFileTable::THAILAND:
		case nsCsFileTable::TAIWAN:
		case nsCsFileTable::HONGKONG:
#pragma warning( disable : 4996 )
			wcscpy(wBuffer_Temp, excel.GetData(IL_COL_COMMENT, wBuffer));
#pragma warning( default : 4996 )
			wcscpy_s(info.s_szComment, _tcslen(wBuffer_Temp) + 1, wBuffer_Temp);
			break;

		default:
			assert_cs(false);
			break;
		}

		__CsStringChange::NewLineCheck(info.s_szComment);

		int nCol = IL_COL_ICON;

		// Icon
		info.s_nBuffIcon = atoi(excel.GetData(nCol++, cBuffer));

		// BuffType
		info.s_nBuffType = atoi(excel.GetData(nCol++, cBuffer));
		nCol++;

		// LifeType
		info.s_nBuffLifeType = atoi(excel.GetData(nCol++, cBuffer));
		nCol++;

		// TimeType
		info.s_nBuffTimeType = atoi(excel.GetData(nCol++, cBuffer));
		nCol++;

		// MinLV
		info.s_nMinLv = atoi(excel.GetData(nCol++, cBuffer));

		// Class
		info.s_nBuffClass = atoi(excel.GetData(nCol++, cBuffer));

		// Item Skill Code
		info.s_dwSkillCode = atoi(excel.GetData(nCol++, cBuffer));

		// Digimon Skill Code
		info.s_dwDigimonSkillCode = atoi(excel.GetData(nCol++, cBuffer));

		// delete
		nCol += 1;
		info.s_bDelete = atoi(excel.GetData(nCol++, cBuffer)) != 0;

		// EffectFile
		strcpy_s(info.s_szEffectFile, BUFF_EFFECT_LEN, excel.GetData(nCol++, cBuffer));

		// 상태이상 레벨
		nCol += 4;
		info.s_nConditionLv = atoi(excel.GetData(nCol++, cBuffer));

		CsBuff* pBuff = csnew CsBuff;
		if (!pBuff)
			continue;

		pBuff->Init(&info);

		m_mapBuff[info.s_dwID] = pBuff;
	}

	excel.Close();
}

#define Buff_BIN		"Buff.bin"

void CsBuffMng::SaveBin(char* cPath)
{
	char cName[MAX_PATH];
	sprintf_s(cName, MAX_PATH, "%s\\%s", cPath, Buff_BIN);

	FILE* fp = NULL;
	fopen_s(&fp, cName, "wb");
	assert_cs(fp != NULL);

	if (fp == NULL)
		return;

	int nCount = (int)m_mapBuff.size();
	fwrite(&nCount, sizeof(int), 1, fp);

	CsBuff::MAP_IT it = m_mapBuff.begin();
	CsBuff::MAP_IT itEnd = m_mapBuff.end();

	for (; it != itEnd; ++it)
	{
		if (it->second)
			fwrite(it->second->GetInfo(), sizeof(CsBuff::sINFO), 1, fp);
	}

	fclose(fp);
}

bool CsBuffMng::_LoadBin(char* cPath)
{
	char cName[MAX_PATH];
	sprintf_s(cName, MAX_PATH, "%s\\%s", cPath, Buff_BIN);

	FILE* fp = NULL;
	fopen_s(&fp, cName, "rb");

	if (fp == NULL)
	{
		CsMessageBoxA(MB_OK, "%s 파일이 존재 하지 않습니다.", cName);
		return false;
	}

	int nCount = 0;
	fread(&nCount, sizeof(int), 1, fp);

	if (nCount <= 0)
	{
		fclose(fp);
		return true;
	}

	CsBuff::sINFO info;

	for (int i = 0; i < nCount; ++i)
	{
		memset(&info, 0, sizeof(CsBuff::sINFO));

		if (fread(&info, sizeof(CsBuff::sINFO), 1, fp) != 1)
			break;

		if (info.s_dwID == 0)
			continue;

		// Evita duplicados.
		if (m_mapBuff.find(info.s_dwID) != m_mapBuff.end())
			continue;

		CsBuff* pObject = csnew CsBuff;
		if (!pObject)
			continue;

		pObject->Init(&info);

		m_mapBuff[info.s_dwID] = pObject;
	}

	fclose(fp);

	return true;
}

void CsBuffMng::_LoadFilePack(char* cPath)
{
	char cName[MAX_PATH];
	sprintf_s(cName, MAX_PATH, "%s\\%s", cPath, Buff_BIN);

	if (!CsFPS::CsFPSystem::SeekLock(FT_PACKHANDLE))
		return;

	int nHandle = CsFPS::CsFPSystem::GetFileHandle(FT_PACKHANDLE, cName);

	if (nHandle == 0)
	{
		CsFPS::CsFPSystem::SeekUnLock(FT_PACKHANDLE);
		return;
	}

	int nCount = 0;
	_read(nHandle, &nCount, sizeof(int));

	if (nCount <= 0)
	{
		CsFPS::CsFPSystem::SeekUnLock(FT_PACKHANDLE);
		return;
	}

	CsBuff::sINFO info;

	for (int i = 0; i < nCount; ++i)
	{
		memset(&info, 0, sizeof(CsBuff::sINFO));

		_read(nHandle, &info, sizeof(CsBuff::sINFO));

		if (info.s_dwID == 0)
			continue;

		// Evita duplicados.
		if (m_mapBuff.find(info.s_dwID) != m_mapBuff.end())
			continue;

		CsBuff* pObject = csnew CsBuff;
		if (!pObject)
			continue;

		pObject->Init(&info);

		m_mapBuff[info.s_dwID] = pObject;
	}

	CsFPS::CsFPSystem::SeekUnLock(FT_PACKHANDLE);
}