@echo off
setlocal

:: �����̊m�F
if "%~1"=="" (
    echo �g�p���@: %~nx0 ProjectDir
    exit /b 1
)

:: �����ƕϐ��̐ݒ�
set "ProjectDir=%~1"
set "SolutionDIr=%~2"
set "FMOD_DllPath=%SolutionDir%\ThirdParty\FMOD\Lib\x64\*.dll"
set "OpenCV_DllPath=%SolutionDir%\ThirdParty\OpenCV\Lib\*.dll"
set "Plugins_Path=%ProjectDir%\Plugins"

:: Plugins�f�B���N�g�������݂��Ȃ��ꍇ�͍쐬
if not exist "%Plugins_Path%" (
    echo Plugins�f�B���N�g�������݂��Ȃ����ߍ쐬��: %Plugins_Path%
    mkdir "%Plugins_Path%"
)

:: DLL�̃R�s�[����
echo FMOD DLL���R�s�[��...
copy /Y "%FMOD_DllPath%" "%Plugins_Path%"

echo OpenCV DLL���R�s�[��...
copy /Y "%OpenCV_DllPath%" "%Plugins_Path%"

echo ���ׂĂ�DLL���R�s�[���܂����B
endlocal
