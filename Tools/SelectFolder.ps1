function Select-FolderUsingFileDialog {
    param (
        [string]$Title = "�t�H���_��I�����Ă�������",
        [string]$InitialDirectory = [Environment]::GetFolderPath("Desktop")
    )

    Add-Type -AssemblyName System.Windows.Forms

    $dialog = New-Object System.Windows.Forms.OpenFileDialog
    $dialog.Title = $Title
    $dialog.InitialDirectory = $InitialDirectory
    $dialog.Filter = "�t�H���_��I��|."
    $dialog.FileName = "���̃t�H���_��I��"  # �[���I�Ƀt�@�C������ݒ�

    $dialog.CheckFileExists = $false  # ���݂��Ȃ��t�@�C�����ł�OK�ɂ���

    if ($dialog.ShowDialog() -eq [System.Windows.Forms.DialogResult]::OK) {
        return [System.IO.Path]::GetDirectoryName($dialog.FileName)
    } else {
        return $null
    }
}