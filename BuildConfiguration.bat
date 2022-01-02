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
REM ECHO *              Assign Wrokspace Folders					*  
REM ECHO ************************************************************
REM ECHO .

SET SourcesDir=%EnlistmentDir%\Source
SET BuildDir=%EnlistmentDir%\Build

REM ECHO ************************************************************
REM ECHO *                Assign Build Variables 					*  
REM ECHO ************************************************************
REM ECHO .

SET TIXEL_BUILD_CONFIG=%1
SET TIXEL_BUILD_PLATFORM=%2

REM ECHO ************************************************************
REM ECHO *               	  Beautify Console						*  
REM ECHO ************************************************************
REM ECHO .

TITLE TiXeL Build [%TIXEL_BUILD_CONFIG%^|^%TIXEL_BUILD_PLATFORM%]
COLOR 1F

ECHO ************************************************************
ECHO *    		Activating Build   
ECHO ************************************************************
ECHO .

MSBuild.exe %SourcesDir%\TiXeL.sln /property:Configuration=%TIXEL_BUILD_CONFIG% /property:Platform=%TIXEL_BUILD_PLATFORM% /property:OutputPath=%BuildDir% /t:Clean;Rebuild

REM ECHO ************************************************************
REM ECHO *               	  Beautify Console						*  
REM ECHO ************************************************************
REM ECHO .

TITLE TiXeL Enlistment
COLOR 1F	