param(
    [string]$SdkRoot = (Join-Path (Split-Path $PSScriptRoot -Parent) '.vendor\dng_sdk_1_7_1'),
    [string]$MsBuild = 'C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\MSBuild\Current\Bin\MSBuild.exe'
)

$ErrorActionPreference = 'Stop'
$repo = Split-Path $PSScriptRoot -Parent
$SdkRoot = (Resolve-Path -LiteralPath $SdkRoot).Path
$win = Join-Path $SdkRoot 'dng_sdk\projects\win'
$sourceProject = Join-Path $win 'dng_validate\dng_validate.vcxproj'
$targetProject = Join-Path $win 'dng_validate\jpg2dng.vcxproj'
$sourceProps = Join-Path $win 'settings\dngsdkValidateTarget.props'
$targetProps = Join-Path $win 'settings\dngsdkJpg2DngTarget.props'

$project = [IO.File]::ReadAllText($sourceProject)
$project = $project.Replace('..\..\..\source\dng_validate.cpp', (Join-Path $repo 'src\jpg2dng.cpp'))
$project = $project.Replace('dngsdkValidateTarget.props', 'dngsdkJpg2DngTarget.props')
[IO.File]::WriteAllText($targetProject, $project)
[IO.File]::WriteAllText($targetProps, ([IO.File]::ReadAllText($sourceProps)).Replace('dng_validate.exe', 'jpg2dng.exe'))
$solution = [IO.File]::ReadAllText((Join-Path $win 'dng_validate.sln'))
$solution = $solution.Replace('"dng_validate", "dng_validate\dng_validate.vcxproj"', '"jpg2dng", "dng_validate\jpg2dng.vcxproj"')
$targetSolution = Join-Path $win 'jpg2dng.sln'
[IO.File]::WriteAllText($targetSolution, $solution)

# Some Windows sessions expose PATH and Path simultaneously. The VS 2022
# MSBuild task treats that as a duplicate key; use a single Path in this process.
$pathValue = $env:PATH
Remove-Item Env:Path -ErrorAction SilentlyContinue
$env:Path = $pathValue
$arguments = @('/p:Configuration=Validate Release', '/p:Platform=x64', '/p:PlatformToolset=v143', '/m:1', '/nr:false', '/v:minimal', '/nologo')
& $MsBuild (Join-Path $win 'dng_validate.sln') @arguments
if ($LASTEXITCODE -ne 0) { throw 'Adobe dng_validate build failed' }
& $MsBuild $targetSolution @arguments
if ($LASTEXITCODE -ne 0) { throw 'jpg2dng build failed' }

$buildDir = Join-Path $repo 'build'
New-Item -ItemType Directory -Path $buildDir -Force | Out-Null
Copy-Item -LiteralPath (Join-Path $SdkRoot 'dng_sdk\targets\win\release64_x64\dng_validate.exe') -Destination $buildDir -Force
Copy-Item -LiteralPath (Join-Path $SdkRoot 'dng_sdk\targets\win\release64_x64\jpg2dng.exe') -Destination $buildDir -Force
Write-Host "Built $buildDir\jpg2dng.exe and dng_validate.exe"
