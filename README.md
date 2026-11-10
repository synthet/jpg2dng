# jpg2dng

Convert an sRGB JPEG into a 16-bit, three-channel **LinearRaw DNG** with the Adobe DNG SDK. The image is decoded, inverse sRGB transfer is applied, and the linear RGB samples are stored in a DNG. This does **not** recover Nikon sensor data, create a NEF, or restore clipped highlights.

The output uses the generic `sRGB LinearRaw` camera identity and a D65 linear sRGB color matrix. The original JPEGs should be kept. RAW editors can apply different default tone curves, so their initial render can differ from the JPEG.

## Build on Windows

Requires Visual Studio 2022 Build Tools with the C++ v143 toolset, PowerShell 7, and Adobe DNG SDK 1.7.1. The SDK is a separate download and is not committed here. Extract `dng_sdk_1_7_1_2724_20260908.zip` into `.vendor` so that `.vendor\dng_sdk_1_7_1\dng_sdk` exists. The script generates a project from Adobe's `dng_validate` project, builds its dependencies and the converter, and copies both executables to `build`:

```powershell
.\scripts\build.ps1
```

The generated project and Adobe sources remain in ignored `.vendor`. The source file in this repo is `src/jpg2dng.cpp`.

## Convert and check one file

Default mode writes **linear** light (inverse sRGB) with profile `Linear sRGB D65` and `UniqueCameraModel` `sRGB LinearRaw`:

```powershell
.\build\jpg2dng.exe input.JPG output.dng
.\build\dng_validate.exe output.dng
```

The experimental **`--display`** mode stores the same linear RGB as the default mode, but embeds an identity profile tone curve to bypass the DNG renderer's default tone curve. The earlier `--display` build stored gamma-encoded JPEG samples in a LinearRaw DNG and incorrectly described the curve as an input decoder. DNG `ProfileToneCurve` is a rendering curve; this version keeps the stored samples linear. Lightroom may apply additional rendering, so compare a sample before converting a batch. `Make` and `Model` are read from the JPEG when present (ExifTool in the batch script copies them again with the shooting tags).

For `DSC_0112.JPG`, Adobe's DNG renderer validates the new sample and renders it within about 1.6, 1.6, and 2.7 levels per RGB channel of the source JPEG on average (8-bit scale, downsampled comparison). This checks the Adobe SDK rendering path, not Lightroom's appearance. The sample JPEG is tagged sRGB and has no embedded ICC profile. The converter currently assumes sRGB input; other JPEG color spaces need conversion before use. Nikon Z8 or Adobe raw camera profiles are meant for sensor RAW values and should not be applied to this already-rendered JPEG-derived DNG.

```powershell
.\build\jpg2dng.exe --display input.JPG output-display.dng
```

This conversion cannot recover sensor RAW data or make JPEG-derived DNGs suitable for DxO PureRAW. PureRAW accepts supported camera RAW files and uncompressed RAW-DNGs converted from them. For JPEG-only images, edit the original JPEG in Lightroom or use DxO PhotoLab's standard RGB noise reduction.

For PureRAW, locate the original NEF instead of converting the JPEG. The helper below searches for a same-name NEF and compares its EXIF capture time with the JPEG, since names such as `DSC_0112` can repeat:

```powershell
.\scripts\find-matching-nef.ps1 -JpegPath '<photos>\Camera JPEG\<day>\DSC_0112.JPG' -RawRoots '<card>\DCIM'
```

Use the actual mounted RAW folder in `-RawRoots` (the `E:` drive in this example may not be connected). A matching capture time is evidence that the NEF is the corresponding shot. Check the image itself before treating it as certain. If no NEF exists, a JPEG-derived DNG cannot provide the RAW data PureRAW needs.

The sample from Nikon Z 8 `DSC_0001.JPG` produced a DNG with LinearRaw photometric interpretation, 8256 × 5504 pixels, three 16-bit samples per pixel, and Adobe `dng_validate` returned `Validation complete`.

## Two date folders

`scripts/convert-folders.ps1` selects JPEGs in one or more `<SourceRoot>\<day>` folders and writes matching filenames under `<DestinationRoot>\<day>`. All three parameters are required. It runs in dry-run mode by default:

```powershell
$opts = @{ SourceRoot = '<photos>\Camera JPEG'; DestinationRoot = '<photos>\DNG'; Days = '<day1>', '<day2>' }
.\scripts\convert-folders.ps1 @opts
.\scripts\convert-folders.ps1 @opts -Execute
.\scripts\convert-folders.ps1 @opts -Execute -Display
```

The script writes one DNG to a temporary path, copies `Make`, `Model`, and selected shooting EXIF with ExifTool, validates it with Adobe's tool, and then replaces the old DNG. It leaves a failed temporary file for inspection. It skips files already written in the same mode (linear vs `-Display`). To run a small batch first, pass `-Limit 2 -Execute`.

Lens, exposure, ISO, date, GPS, and orientation are copied where present. The original JPEGs are never changed.
