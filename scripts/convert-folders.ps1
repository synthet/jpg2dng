param(
    [string]$SourceRoot = 'D:\Photos\Camera JPEG',
    [string]$DestinationRoot = 'D:\Photos\DNG',
    [string[]]$Days = @('2026-10-03', '2026-10-04'),
    [string]$ExifTool = "$env:LOCALAPPDATA\Programs\ExifTool\ExifTool.exe",
    [switch]$Execute,
    [switch]$Display,
    [int]$Limit = 0
)

$ErrorActionPreference = 'Stop'
$repo = Split-Path $PSScriptRoot -Parent
$converter = Join-Path $repo 'build\jpg2dng.exe'
$validator = Join-Path $repo 'build\dng_validate.exe'
foreach ($path in @($converter, $validator, $ExifTool)) {
    if (-not (Test-Path -LiteralPath $path)) { throw "Missing tool: $path" }
}

$jobs = @(
    foreach ($day in $Days) {
        $dir = Join-Path $SourceRoot $day
        if (-not (Test-Path -LiteralPath $dir)) { throw "Missing source folder: $dir" }
        Get-ChildItem -LiteralPath $dir -File |
            Where-Object { $_.Extension -in @('.jpg', '.jpeg') } |
            Sort-Object Name |
            ForEach-Object {
                [pscustomobject]@{
                    Source = $_.FullName
                    Destination = Join-Path (Join-Path $DestinationRoot $day) ($_.BaseName + '.dng')
                }
            }
    }
)
if ($Limit -gt 0) { $jobs = @($jobs | Select-Object -First $Limit) }
Write-Host "JPEG files selected: $($jobs.Count)"
if (-not $Execute) {
    $jobs | Select-Object -First 5 | ForEach-Object { Write-Host "$($_.Source) -> $($_.Destination)" }
    Write-Host 'Dry run. Pass -Execute to convert.'
    return
}

$expectedSoftware = if ($Display) { 'jpg2dng Adobe SDK 1.7.1 display v5' } else { 'jpg2dng Adobe SDK 1.7.1' }

$converted = 0
$skipped = 0
foreach ($job in $jobs) {
    $src = $job.Source
    $dest = $job.Destination
    $dir = Split-Path $dest -Parent
    New-Item -ItemType Directory -Path $dir -Force | Out-Null
    if (Test-Path -LiteralPath $dest) {
        $software = & $ExifTool -s3 -Software $dest 2>$null
        if ($software -eq $expectedSoftware) {
            $skipped++
            continue
        }
    }
    $temporary = Join-Path $dir ('.' + [IO.Path]::GetFileNameWithoutExtension($dest) + '.building.dng')
    $backup = "$dest.previous"
    if ((Test-Path -LiteralPath $temporary) -or (Test-Path -LiteralPath $backup)) {
        throw "An unfinished conversion exists for $dest; inspect it before retrying."
    }
    $converterArgs = if ($Display) { @('--display', $src, $temporary) } else { @($src, $temporary) }
    & $converter @converterArgs
    if ($LASTEXITCODE -ne 0) { throw "Conversion failed: $src" }
    & $ExifTool -overwrite_original '-TagsFromFile' $src '-Make' '-Model' '-DateTimeOriginal' '-CreateDate' '-ModifyDate' '-SubSecTimeOriginal' '-OffsetTimeOriginal' '-ExposureTime' '-FNumber' '-ISO' '-FocalLength' '-LensModel' '-GPS:all' '-Orientation' $temporary | Out-Null
    if ($LASTEXITCODE -ne 0) { throw "EXIF copy failed: $src" }
    & $validator $temporary *> $null
    if ($LASTEXITCODE -ne 0) { throw "Adobe DNG validation failed: $temporary" }
    $hadPrevious = Test-Path -LiteralPath $dest
    if ($hadPrevious) { Move-Item -LiteralPath $dest -Destination $backup }
    try {
        Move-Item -LiteralPath $temporary -Destination $dest
    } catch {
        if ($hadPrevious) { Move-Item -LiteralPath $backup -Destination $dest }
        throw
    }
    if ($hadPrevious) { Remove-Item -LiteralPath $backup }
    $converted++
    Write-Host "[$converted/$($jobs.Count)] $dest"
}
Write-Host "DONE converted=$converted skipped=$skipped selected=$($jobs.Count)"
