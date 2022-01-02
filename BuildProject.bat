@ECHO off
ECHO:

REM ECHO ************************************************************
REM ECHO *               	  Beautify Console						*  
REM ECHO ************************************************************
REM ECHO .

TITLE TiXeL Enlistment
COLOR 1F

REM ECHO ************************************************************
REM ECHO *            Resolve Enlistment Folder						*  
REM ECHO ************************************************************
REM ECHO .

SET EnlistmentDir=%~dp0
SET EnlistmentDir=%EnlistmentDir:~0,-1%

REM ECHO ************************************************************
REM ECHO *                  Enforce Visual Console 					*  
REM ECHO ************************************************************
REM ECHO .

SET ToolsDir=%EnlistmentDir%\Tools
FOR /f "delims=" %%I IN ('%ToolsDir%\vswhere.exe -latest -property installationPath') DO SET VisualStudioDir=%%I
CALL "%VisualStudioDir%\Common7\Tools\VsDevCmd.bat"
ECHO .

ECHO ************************************************************
ECHO *            	Update GIT Sub-Modules 						
ECHO ************************************************************
ECHO .

git submodule update --recursive

ECHO ************************************************************
ECHO *            Activate Fine-Grained Build 					
ECHO ************************************************************
ECHO .

CALL %EnlistmentDir%\BuildConfiguration.bat Debug x64
CALL %EnlistmentDir%\BuildConfiguration.bat Debug x86
CALL %EnlistmentDir%\BuildConfiguration.bat Release x64
CALL %EnlistmentDir%\BuildConfiguration.bat Release x86

ECHO ************************************************************
ECHO *             Done. (OK)							
ECHO ************************************************************
ECHO .

REM ECHO ************************************************************
REM ECHO *               	  Beautify Console						*  
REM ECHO ************************************************************
REM ECHO .

TITLE TiXeL Enlistment
COLOR 1F

