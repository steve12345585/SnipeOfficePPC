Add-Type -AssemblyName System.Web | Out-Null
$root = Split-Path -Parent $MyInvocation.MyCommand.Path
Set-Location $root
$exts = '.png','.jpg','.jpeg','.gif','.bmp','.svg','.ico','.icns','.tif','.tiff','.wmf','.emf','.xbm','.xpm','.cur','.ani','.dds'
$allFiles = Get-ChildItem -Path $root -Recurse | Where-Object { -not $_.PsIsContainer }
$files = $allFiles | Where-Object { $exts -contains $_.Extension.ToLower() }
$groups = $files | Group-Object { $_.Directory.FullName }
$mdLines = New-Object System.Collections.Generic.List[string]
$mdLines.Add("<!-- Total files scanned: $($allFiles.Count); image files: $($files.Count) -->")
$mdLines.Add('')
$mdLines.Add('# Repository Image Inventory')
$mdLines.Add('')
$htmlLines = New-Object System.Collections.Generic.List[string]
$htmlLines.Add('<!DOCTYPE html>')
$htmlLines.Add('<html lang="en"><head><meta charset="utf-8" />')
$htmlLines.Add('<title>Repository Image Gallery</title>')
$htmlLines.Add('<style>body{font-family:Arial,Helvetica,sans-serif;margin:20px;}h1{margin-top:0;}section{margin-bottom:50px;} .dir-title{margin-top:40px;border-bottom:1px solid #ccc;padding-bottom:5px;} .grid{display:flex;flex-wrap:wrap;gap:12px;} .img-card{width:200px;font-size:12px;} .img-card img{width:200px;height:150px;object-fit:contain;border:1px solid #ccc;background:#fafafa;} .path{margin-top:4px;word-break:break-all;}</style>')
$htmlLines.Add('</head><body>')
$htmlLines.Add("<h1>Repository Image Gallery</h1>")
$htmlLines.Add("<p>Total files scanned: $($allFiles.Count); image files: $($files.Count)</p>")
$bt = [char]0x60
foreach ($g in ($groups | Sort-Object Name)) {
    $relPath = if ($g.Name -eq $root) { '.' } else { $g.Name.Substring($root.Length + 1).Replace('\','/') }
    if ([string]::IsNullOrWhiteSpace($relPath)) { $relPath = '.' }
    $mdLines.Add("## $relPath")
    $htmlLines.Add("<section>")
    $htmlLines.Add("<h2 class='dir-title'>$([System.Web.HttpUtility]::HtmlEncode($relPath))</h2>")
    $htmlLines.Add("<div class='grid'>")
    foreach ($file in ($g.Group | Sort-Object FullName)) {
        $relFile = if ($relPath -eq '.') { $file.Name } else { "$relPath/$($file.Name)" }
        $mdLines.Add("- $bt$relFile$bt")
        $encoded = [System.Web.HttpUtility]::HtmlEncode($relFile)
        $htmlLines.Add("<div class='img-card'><img src='$encoded' alt='$encoded' loading='lazy' /><div class='path'>$encoded</div></div>")
    }
    $mdLines.Add('')
    $htmlLines.Add("</div>")
    $htmlLines.Add("</section>")
}
$htmlLines.Add('</body></html>')
$mdPath = Join-Path $root 'IMAGES.md'
$mdLines | Set-Content -Path $mdPath -Encoding UTF8
$htmlPath = Join-Path $root 'IMAGES.html'
$htmlLines | Set-Content -Path $htmlPath -Encoding UTF8
Write-Host "Wrote $($files.Count) images across $($groups.Count) directories to:"
Write-Host " - $mdPath"
Write-Host " - $htmlPath"

