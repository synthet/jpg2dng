param(
    [Parameter(Mandatory)]
    [string]$JpegPath,
    [Parameter(Mandatory)]
    [string[]]$RawRoots,
    [string]$ExifTool = "$env:LOCALAPPDATA\Programs\ExifTool\ExifTool.exe"
)

$ErrorActionPreference = 'Stop'
$jpeg = (Resolve-Path -LiteralPath $JpegPath).Path
if (-not (Test-Path -LiteralPath $jpeg -PathType Leaf)) { throw "Missing JPEG: $jpeg" }
if (-not (Test-Path -LiteralPath $ExifTool -PathType Leaf)) { throw "Missing ExifTool: $ExifTool" }

function Get-CaptureTime([string]$path) {
    $value = & $ExifTool -s3 -DateTimeOriginal $path
    if ($LASTEXITCODE -ne 0) { throw "Could not read capture time: $path" }
    return ([string]$value).Trim()
}

$jpegTime = Get-CaptureTime $jpeg
$stem = [IO.Path]::GetFileNameWithoutExtension($jpeg)
$candidates = @(
    foreach ($root in $RawRoots) {
        if (-not (Test-Path -LiteralPath $root -PathType Container)) {
            throw "Missing RAW folder: $root"
        }
        $resolvedRoot = (Resolve-Path -LiteralPath $root).Path
        Get-ChildItem -LiteralPath $resolvedRoot -Recurse -File -Filter "$stem.nef" |
            ForEach-Object { $_.FullName }
    }
) | Sort-Object -Unique

if ($candidates.Count -eq 0) {
    Write-Host "No same-name NEF found for $jpeg"
    return
}

$matches = @(
    foreach ($candidate in $candidates) {
        $rawTime = Get-CaptureTime $candidate
        [pscustomobject]@{
            Match = if ($jpegTime -and $rawTime -eq $jpegTime) { 'capture time matches' } else { 'check manually' }
            JpegCaptureTime = $jpegTime
            RawCaptureTime = $rawTime
            RawPath = $candidate
        }
    }
)
$matches | Sort-Object @{ Expression = { if ($_.Match -eq 'capture time matches') { 0 } else { 1 } } }, RawPath
