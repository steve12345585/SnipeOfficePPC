$root = Split-Path -Parent $MyInvocation.MyCommand.Path
$new = Join-Path $root 'filter'
$old = Join-Path $root 'filter old/filter'

$newFiles = Get-ChildItem -Path $new -Recurse -File | ForEach-Object { $_.FullName.Substring($new.Length + 1) }
$oldFiles = Get-ChildItem -Path $old -Recurse -File | ForEach-Object { $_.FullName.Substring($old.Length + 1) }

$diff = Compare-Object -ReferenceObject $oldFiles -DifferenceObject $newFiles
$added   = @($diff | Where-Object { $_.SideIndicator -eq '=>' } | Select-Object -ExpandProperty InputObject)
$removed = @($diff | Where-Object { $_.SideIndicator -eq '<=' } | Select-Object -ExpandProperty InputObject)

$addedPath   = Join-Path $root 'filter_added.txt'
$removedPath = Join-Path $root 'filter_removed.txt'

$added   | Sort-Object | Set-Content -Path $addedPath
$removed | Sort-Object | Set-Content -Path $removedPath

Write-Host \"New-only files: $($added.Count) -> $addedPath\"
Write-Host \"Old-only files: $($removed.Count) -> $removedPath\"

