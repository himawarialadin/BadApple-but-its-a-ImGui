. ".\SelectFolder.ps1"

# 変数定義
$Dialog_Title              = "Select OpenCV Path (.../opencv/build)"
$Dialog_InitialDirectory   = "C:\"
$IncPath                   = "..\ThirdParty\OpenCV\Inc\"
$LibPath                   = "..\ThirdParty\OpenCV\Lib"

# コピーリスト定義（Src は相対パス）
$ExportLists = @(
    @{ Src = "\include\opencv2\";    Dest = $IncPath },
    @{ Src = "\x64\vc16\lib\*.lib";  Dest = $LibPath },
    @{ Src = "\x64\vc16\bin\*.dll";  Dest = $LibPath },
    @{ Src = "\bin\*.*";             Dest = $LibPath }
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
