@echo off
setlocal

:: 引数の確認
if "%~1"=="" (
    echo 使用方法: %~nx0 ProjectDir
    exit /b 1
)

:: 引数と変数の設定
set "ProjectDir=%~1"
set "SolutionDIr=%~2"
set "FMOD_DllPath=%SolutionDir%\ThirdParty\FMOD\Lib\x64\*.dll"
set "OpenCV_DllPath=%SolutionDir%\ThirdParty\OpenCV\Lib\*.dll"
set "Plugins_Path=%ProjectDir%\Plugins"

:: Pluginsディレクトリが存在しない場合は作成
if not exist "%Plugins_Path%" (
    echo Pluginsディレクトリが存在しないため作成中: %Plugins_Path%
    mkdir "%Plugins_Path%"
)

:: DLLのコピー処理
echo FMOD DLLをコピー中...
copy /Y "%FMOD_DllPath%" "%Plugins_Path%"

echo OpenCV DLLをコピー中...
copy /Y "%OpenCV_DllPath%" "%Plugins_Path%"

echo すべてのDLLをコピーしました。
endlocal
