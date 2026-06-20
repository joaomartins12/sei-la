rem %1 targetFilePath
rem %2 projectDir
ECHO "" > postBuildLog.txt
ECHO %1 >> postBuildLog.txt
ECHO %2 >> postBuildLog.txt

copy "%1" "%2..\..\..\MayaPlugin\PostBuild"