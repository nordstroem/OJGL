<#
.SYNOPSIS
  Renders a Clinkster song (already converted from Renoise via RenoiseConvert.py) to a normal
  .wav file, so you can send it to people who don't have the OJGL build set up.

.DESCRIPTION
  Clinkster never plays through Renoise/the VST at runtime -- it synthesizes the whole song
  into a raw PCM buffer up front (see src/music/ClinksterPlayer.cpp), and clinkster.h ships a
  ready-made WAV header for exactly this purpose. This script assembles clinkster.asm together
  with your song.asm and a tiny main.cpp that dumps that buffer straight to disk.

  Must be run from an x86 Visual Studio developer PowerShell (same requirement as the main OJGL
  build -- clinkster.asm assembles to win32/x86 object code) with an assembler (yasm/vsyasm/nasm)
  on PATH.

.PARAMETER SongAsm
  Path to the converted song.asm (RenoiseConvert.py's output).

.PARAMETER OutWav
  Path to write the rendered .wav to. Defaults to <SongAsm's name>.wav next to it.

.EXAMPLE
  .\tools\clinkster_wav\build.ps1 -SongAsm .\productions\edison2026\music\song.asm -OutWav .\mysong.wav
#>
param(
    [Parameter(Mandatory = $true)]
    [string]$SongAsm,

    [string]$OutWav
)

$ErrorActionPreference = "Stop"

$scriptDir = Split-Path -Parent $MyInvocation.MyCommand.Path
$clinksterDir = Resolve-Path (Join-Path $scriptDir "..\..\src\thirdparty\clinkster")
$outDir = Join-Path $scriptDir "out"
New-Item -ItemType Directory -Force -Path $outDir | Out-Null

$songAsmResolved = Resolve-Path $SongAsm
$songDir = Split-Path -Parent $songAsmResolved

if (-not $OutWav) {
    $OutWav = Join-Path (Get-Location) ((Get-Item $songAsmResolved).BaseName + ".wav")
}

$asm = Get-Command vsyasm, yasm, nasm, nasmw -ErrorAction SilentlyContinue | Select-Object -First 1
if (-not $asm) {
    throw "No assembler found on PATH (need vsyasm, yasm, nasm, or nasmw) -- install one, e.g. https://yasm.tortall.net/"
}
if (-not (Get-Command cl -ErrorAction SilentlyContinue)) {
    throw "cl.exe not found -- run this from an x86 Visual Studio developer PowerShell (see CLAUDE.md)."
}

Write-Host "Assembling clinkster engine + $($songAsmResolved.Path) ..."
& $asm.Source -f win32 -I "$songDir\" -o "$outDir\clinkster.obj" "$clinksterDir\clinkster.asm"
if ($LASTEXITCODE -ne 0) { throw "Assembling clinkster.asm failed" }

Write-Host "Compiling + linking renderer ..."
& cl /nologo /EHsc /Fo"$outDir\main.obj" /Fe"$outDir\render.exe" `
    /I "$clinksterDir" "$scriptDir\main.cpp" "$outDir\clinkster.obj" `
    /link /SAFESEH:NO winmm.lib
if ($LASTEXITCODE -ne 0) { throw "Compiling/linking render.exe failed" }

Write-Host "Rendering to $OutWav ..."
& "$outDir\render.exe" $OutWav
if ($LASTEXITCODE -ne 0) { throw "render.exe failed" }

Write-Host "Done: $OutWav"
