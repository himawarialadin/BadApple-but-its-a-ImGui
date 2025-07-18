. ".\SelectFolder.ps1"

# 変数定義
$Dialog_Title              = "Select FMOD Folder (.../FMOD SoundSystem/FMOD Studio API Windows/api/core)"
$Dialog_InitialDirectory   = "C:\Program Files (x86)\FMOD SoundSystem\FMOD Studio API Windows\api\core"
$IncPath                   = "..\ThirdParty\FMOD\Inc\FMOD\"
$LibPath                   = "..\ThirdParty\FMOD\Lib\x64\"

# コピーリスト定義（Src は相対パス）
$ExportLists = @(
    @{ Src = "\inc\*.*";      Dest = $IncPath },
    @{ Src = "\lib\x64\*.*";  Dest = $LibPath }
)

# フォルダ選択
$DirPath = Select-FolderUsingFileDialog -Title $Dialog_Title -InitialDirectory $Dialog_InitialDirectory

if (-not $DirPath) {
    Write-Host "フォルダ選択がキャンセルされました。スクリプトを終了します。"
    exit 1
}

# コピー処理
foreach ($entry in $ExportLists) {
    $srcPath = Join-Path $DirPath $entry.Src
    $destPath = $entry.Dest

    # 出力先ディレクトリが存在しない場合は作成
    if (-not (Test-Path $destPath)) {
        New-Item -ItemType Directory -Path $destPath -Force | Out-Null
    }

    Write-Host "`n[コピー中] $srcPath → $destPath"
    try {
        Copy-Item -Path $srcPath -Destination $destPath -Recurse -Force
    }
    catch {
        Write-Warning "コピー失敗: $srcPath → $destPath"
        Write-Warning $_.Exception.Message
    }
}

Write-Host "すべてのファイルコピーが完了しました。"
