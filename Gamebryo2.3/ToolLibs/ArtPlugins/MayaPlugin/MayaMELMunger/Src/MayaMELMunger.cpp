// EMERGENT GAME TECHNOLOGIES PROPRIETARY INFORMATION
//
// This software is supplied under the terms of a license agreement or
// nondisclosure agreement with Emergent Game Technologies and may not 
// be copied or disclosed except in accordance with the terms of that 
// agreement.
//
//      Copyright (c) 1996-2007 Emergent Game Technologies.
//      All Rights Reserved.
//
// Emergent Game Technologies, Calabasas, CA 91302
// http://www.emergent.net
#include "MayaMelMungerPCH.h"

#if defined(_UNICODE)
#	define remove_file(value) _wremove(value)
#	define rename_file(value1, value2) _wrename(value1, value2)
#	define __strcpy(value1, value2) wcscpy(value1, value2)
#	define __strstri(value1, value2) _wcsicmp(value1, value2)
#	define __strlen(value) wcslen(value)
#	define __substr(value) wcslen(value)
#else
#	define remove_file(value) remove(value)
#	define rename_file(value1, value2) rename(value1, value2)
#	define __strcpy(value1, value2) strcpy(value1, value2)
#	define __strstri(value1, value2) stricmp(value1, value2)
#	define __strlen(value) strlen(value)
#	define __substr(value) strlen(value)
#endif

typedef std::basic_string<TCHAR> tstring;
typedef std::basic_ifstream<TCHAR> tifstream;
typedef std::basic_ofstream<TCHAR> tofstream;

using namespace std;

tstring g_DefaultScriptPath;
tstring g_OthersScriptPath;
tstring g_BackupPath;

tifstream g_FileStream;
tofstream g_LogFile;

bool g_bReverse = false;
tstring g_kLogFilename;
tstring g_kMayaInstall;

const tstring PREFIX = _T("_gb_");
#define REVISION_FILE _T("./revision.dat")

//just a guess
const int MAX_LINE_LENGTH = 500;
const int MAX_FILES_TO_DUPLICATE = 20;

//-----------------------------------------------------------------------------
void Init(const tstring& mayaInstallPath)
{
    g_DefaultScriptPath = mayaInstallPath + _T("\\scripts\\AETemplates\\");

    g_OthersScriptPath = mayaInstallPath + _T("\\scripts\\others\\");
    
    g_BackupPath =  mayaInstallPath + _T("\\scripts\\GamebryoBackup\\");
}
//-----------------------------------------------------------------------------
bool FileExists(const tstring& filePath)
{
    g_FileStream.clear();
    bool returnValue;
    
    g_FileStream.open(filePath.c_str());

    if(g_FileStream.fail())
    {
        returnValue = false;
    }
    else
    {
        returnValue = true;
    }

    g_FileStream.close();
    return returnValue;
}
//-----------------------------------------------------------------------------
bool IsOverridenFile(const tstring& fileName)
{
    tstring otherPath = g_OthersScriptPath + fileName + _T(".mel");
    //if the override file exists don't use the default
    return FileExists(otherPath);
}
//-----------------------------------------------------------------------------
bool TestForPriorAlias(const tstring& fileName)
{
    tstring otherPath = g_OthersScriptPath + PREFIX + fileName + _T(".mel");

    return FileExists(otherPath);
}
//-----------------------------------------------------------------------------
void MakeAliasedFile(const tstring& fileName, 
                     const tstring& fileToCopy, 
                     const tstring& fileToMake)
{
    if(FileExists(fileToCopy))
    {
        tifstream oldFile(fileToCopy.c_str());
        tofstream aliasedFile(fileToMake.c_str());

        tstring mainFunctionName = _T("global proc ") + fileName;
        tstring newMainFunctionName = _T("global proc ") + PREFIX + fileName;

        while(!oldFile.eof())
        {
            _TCHAR lineBuffer[MAX_LINE_LENGTH];
            oldFile.getline(lineBuffer, MAX_LINE_LENGTH);
            tstring line = lineBuffer;

            if(line.compare(0, mainFunctionName.length(), mainFunctionName) 
                == 0)
            {
                line.replace(0, mainFunctionName.length(), 
                    newMainFunctionName);
            }
        
            aliasedFile << line << "\n";
        }

        oldFile.close();
        aliasedFile.close();
    }
}
//-----------------------------------------------------------------------------
bool ShouldMakeBackup(const tstring& fileName)
{
    g_FileStream.clear();
    //if there is already a backup don't make another one
    tstring filePath = g_OthersScriptPath + fileName + _T(".mel");
    g_FileStream.open(filePath.c_str());

    //awesome magic line length
    _TCHAR lineBuffer[MAX_LINE_LENGTH];
    g_FileStream.getline(lineBuffer, MAX_LINE_LENGTH);

    g_FileStream.close();

    tstring firstLine(lineBuffer);

    if(firstLine.find(_T("Gamebryo Forwarding File")) == tstring::npos)
    {
        //couldn't find the "Gamebryo Forwarding File"
        return true;
    }
    else
    {
        //is a "Gamebryo Forwarding File" don't not make a copy
        return false;
    }


}
//-----------------------------------------------------------------------------
void GetBackupPath(const tstring& fileName, tstring& output)
{
    output = g_BackupPath + fileName + _T(".mel");
}
//-----------------------------------------------------------------------------
void MakeBackup(const tstring& filePath, const tstring& backupPath)
{
    //copy file to backup
    g_FileStream.clear();
    
    g_FileStream.open(filePath.c_str());
    g_LogFile << _T("File To Backup ") << filePath.c_str() << endl;
    
    tofstream backupFileStream(backupPath.c_str());    
    if(!g_FileStream.fail())
    {
        while(!g_FileStream.eof())
        {
            _TCHAR lineBuffer[500];
            g_FileStream.getline(lineBuffer, 500);
            tstring line = lineBuffer;
            backupFileStream << line << "\n";
        }
    }
    else
    {
        g_LogFile << _T("Failed to Load ") << filePath.c_str() << endl;
    }

    g_FileStream.close();
}
//-----------------------------------------------------------------------------
bool BackupExists(const tstring& fileName)
{
    tstring backupPath;
    GetBackupPath(fileName, backupPath);
    return FileExists(backupPath);
}
//-----------------------------------------------------------------------------
void OverrideFile(const tstring& mayaInstallPath, const tstring& fileName)
{
    tstring fileToCopy;
    tstring fileToMake;

    if(TestForPriorAlias(fileName))
    {
        //we have already done all the duplication and backup so do not repeat
        //TODO: Make sure the overridden file has not be modified
        //if it is just warn the user that the file is going to be overridden
        //if it has not been modified
        //break out we don't have do anything except copy over the gamebryo 
        //files
        return;
    }

    //see if there is an overridden file already, i.e. gamebryo is installed 
    //already
    if(!IsOverridenFile(fileName))
    {
        //if there is not copy the default maya one
        //and append _ on it and the main function
        fileToCopy = g_DefaultScriptPath + fileName + _T(".mel");
        fileToMake = g_OthersScriptPath + PREFIX + fileName + _T(".mel");
        MakeAliasedFile(fileName, fileToCopy, fileToMake);
    }
    else
    {
        //if there is make a backup of it
        if(ShouldMakeBackup(fileName))
        {
            tstring backupPath;
            GetBackupPath(fileName, backupPath);

            tstring filePath = g_OthersScriptPath + fileName + _T(".mel");
            MakeBackup(filePath, backupPath);
            fileToCopy = backupPath;
        }
        else
        {
            //This a weird case. There is an overriden file, but it is the 
            //Gamebryo Forwarding File. So either for some reason the aliased
            //file was removed.
            //regenerate the aliased file from either the backup copy
            //or if there is not a backup use the default file.
            if(BackupExists(fileName))
            {
                GetBackupPath(fileName, fileToCopy);
            }
            else
            {
                fileToCopy = g_DefaultScriptPath + fileName + _T(".mel");
            }
        }

        //and PREFIX on it and the main function
        fileToMake = g_OthersScriptPath + PREFIX + fileName + _T(".mel");

        MakeAliasedFile(fileName, fileToCopy, fileToMake);
    }
}
//-----------------------------------------------------------------------------
bool ReplaceInFile(tstring kFilename, tstring Search, 
				   tstring Replace)
{	
	tstring kBuffer;			
	tstring kTempFile(REVISION_FILE);

	tifstream kMelFile(kFilename.c_str(), 
		ios::in|ios::out);

	tofstream kUpdatedFile(kTempFile.c_str(),
		ofstream::out|ofstream::beg|ofstream::trunc);

	if ( !kMelFile.is_open() )
		return(false);

	while(!kMelFile.eof())
	{				
		getline(kMelFile, kBuffer);		
		size_t spot = kBuffer.find(Search.c_str());
		if(spot != string::npos)
        {			
			tstring tmpString = kBuffer.substr(0, spot);
			
			if ( Replace.length() != NULL )
				tmpString += Replace;
			
			tmpString += kBuffer.substr(spot + Search.length(), kBuffer.length());
			kBuffer = tmpString;			
		}

		kUpdatedFile << kBuffer.c_str() << std::endl;
	}
	
	kUpdatedFile.flush();
	kUpdatedFile.close();

	kMelFile.close();	
	return(true);
}
//-----------------------------------------------------------------------------
void RevertFile(tstring& kFilename)
{		
	tstring kReplaceValue = _T(" ");
	
	tstring kFullpath = g_OthersScriptPath;	
	kFullpath.append(_T("\\"));
	kFullpath.append(PREFIX);
	kFullpath.append(kFilename);
	kFullpath.append(_T(".mel"));

	tstring kNewpath = g_OthersScriptPath;	
	kNewpath.append(_T("\\"));	
	kNewpath.append(kFilename);
	kNewpath.append(_T(".mel"));
	
	if ( ReplaceInFile(kFullpath, PREFIX, kReplaceValue) )
	{
		unsigned int iResult;

		remove_file(kNewpath.c_str());
		iResult = rename_file(REVISION_FILE, kNewpath.c_str());
		if ( iResult == 0 )
		{
			iResult = remove_file(kFullpath.c_str());
			iResult = remove_file(REVISION_FILE);
		}
	}
}
//-----------------------------------------------------------------------------
void substr(const _TCHAR* source, 
			_TCHAR* destination, 
			const size_t index, 
			const size_t size)
{
	_TCHAR* pTemp = new _TCHAR[254];
	size_t iIndex = index;

	for ( size_t idx = 0 ; iIndex <= size ; iIndex++, idx++)
	{
		pTemp[idx] = source[iIndex];
	}

	__strcpy(destination, pTemp);
}
//-----------------------------------------------------------------------------
void ParseArgs(int argc, _TCHAR* argv[])
{
	if ( argc <= 1 )
		return;

	for ( int idx = 0 ; argv[idx] != NULL ; idx++ )
	{		
		_TCHAR* argument = new _TCHAR[254];
		_TCHAR* value = new _TCHAR[254];
		
		__strcpy(argument, argv[idx]);		
		if ( argument[0] == '/' || argument[0] == '-' )
		{			
			// copy the argument string to the argument string
			//  minus the first character.
			__substr(argument, argument, 1, __strlen(argument));

			if ( __strstri(argument, _T("unregister")) == 0 )
			{
				g_bReverse = true;
			}
			else if ( __strstri(argument, _T("log")) == 0 )
			{
				__strcpy(value, argv[++idx]);
				g_kLogFilename = tstring(value);				
			}
			else if ( __strstri(argument, _T("maya")) == 0 )
			{
				__strcpy(value, argv[++idx]);
				g_kMayaInstall = tstring(value);
			}
		}
	}
}
//-----------------------------------------------------------------------------
LPSTR ConvertLPTSTRToLPSTR (LPTSTR pszString)
{	 
	USES_CONVERSION;

	LPSTR pszCharString = new char[_tcslen (pszString) + 1];
	memset (pszCharString, 0x00, _tcslen (pszString) + 1);
	strcpy (pszCharString, T2A (pszString));

	return pszCharString;
}

//-----------------------------------------------------------------------------
int _tmain(int argc, _TCHAR* argv[])
{
    tstring FILES_TO_REPLACE[MAX_FILES_TO_DUPLICATE] = {
        _T("AEanisotropicTemplate"), 
        _T("AEblinnTemplate"), 
        _T("AEdirectionalLightTemplate"),
        _T("AEfileTemplate"), 
        _T("AEjointTemplate"), 
        _T("AElambertTemplate"), 
        _T("AElayeredShaderTemplate"), 
        _T("AElightTemplate"), 
        _T("AElodGroupTemplate"),
        _T("AEparticleTemplate"), 
        _T("AEphongETemplate"), 
        _T("AEphongTemplate"),
        _T("AEpointEmitterTemplate"), 
        _T("AEpointLightTemplate"), 
        _T("AErampShaderTemplate"),
        _T("AEshadingMapTemplate"), 
        _T("AEspotLightTemplate"), 
        _T("AEsurfaceShaderTemplate"),
        _T("AEtransformTemplate"), 
        _T("AEuseBackgroundTemplate")};
    
#if defined(_DEBUG)    
	for(size_t idx = 0 ; idx < argc ; idx++ )
	{				
		std::cout << "Argument[" << idx << "] " 
			<< ConvertLPTSTRToLPSTR(argv[idx]) << std::endl;
	}
#endif

    int returnValue = 0;

	// -- if we don't have the right number of aruments, lets quit
    if(argc > 1)
		return(returnValue);

	ParseArgs(argc, argv);
	tstring mayaInstallPath;

	if ( g_kMayaInstall.empty() )
	{
		mayaInstallPath = argv[1];
	}
	else
	{
		mayaInstallPath = g_kMayaInstall;
	}
        
	Init(mayaInstallPath);

	if ( g_bReverse ) 
	{
		for(size_t fileIndex = 0; fileIndex < MAX_FILES_TO_DUPLICATE; fileIndex++)
		{
			RevertFile(FILES_TO_REPLACE[fileIndex]);
		}			
	}
	else
	{        
		tstring logPath;
		if ( g_kLogFilename.empty() )
		{
			if ( argc > 2 )
			{
				logPath = argv[2];
			}
			else
			{
				logPath = _T("./MayaInstall");
			}
		}
		else
		{
			logPath = g_kLogFilename;
		}

		logPath = logPath + _T(".log");
	    g_LogFile.open(logPath.c_str());
        g_LogFile << _T("Maya Install Path ") << mayaInstallPath << endl;
   
		for(size_t fileIndex = 0; fileIndex < MAX_FILES_TO_DUPLICATE; fileIndex++)
	    {
			OverrideFile(mayaInstallPath, FILES_TO_REPLACE[fileIndex]);
		}
	}
	g_LogFile.close(); 

    return returnValue;
}

