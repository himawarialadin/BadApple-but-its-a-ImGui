function Select-FolderUsingFileDialog {
    param (
        [string]$Title = "フォルダを選択してください",
        [string]$InitialDirectory = [Environment]::GetFolderPath("Desktop")
    )

    Add-Type -AssemblyName System.Windows.Forms

    $dialog = New-Object System.Windows.Forms.OpenFileDialog
    $dialog.Title = $Title
    $dialog.InitialDirectory = $InitialDirectory
    $dialog.Filter = "フォルダを選択|."
    $dialog.FileName = "このフォルダを選択"  # 擬似的にファイル名を設定

    $dialog.CheckFileExists = $false  # 存在しないファイル名でもOKにする

    if ($dialog.ShowDialog() -eq [System.Windows.Forms.DialogResult]::OK) {
        return [System.IO.Path]::GetDirectoryName($dialog.FileName)
    } else {
        return $null
    }
}