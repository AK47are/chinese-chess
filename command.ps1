param([string]$Target = "debug")

$BuildDir = "build"
$DebugDir = "$BuildDir/debug"
$ReleaseDir = "$BuildDir/release"

switch ($Target) {
    "debug" {
        if (-not (Test-Path $DebugDir)) {
            New-Item -ItemType Directory -Force -Path $DebugDir | Out-Null
            Set-Location $DebugDir
            cmake -G "MinGW Makefiles" -DCMAKE_BUILD_TYPE=Debug -DCMAKE_EXPORT_COMPILE_COMMANDS=ON ../..
            Copy-Item "compile_commands.json" "../.." -Force
        } else {
            Set-Location $DebugDir
        }
        cmake --build . --config Debug
        Set-Location "../.."
    }
    "release" {
        if (-not (Test-Path $ReleaseDir)) {
            New-Item -ItemType Directory -Force -Path $ReleaseDir | Out-Null
            Set-Location $ReleaseDir
            cmake -G "MinGW Makefiles" -DCMAKE_BUILD_TYPE=Release -DCMAKE_EXPORT_COMPILE_COMMANDS=ON ../..
            Copy-Item "compile_commands.json" "../.." -Force
        } else {
            Set-Location $ReleaseDir
        }
        cmake --build . --config Release
        Set-Location "../.."
    }
    "clean" {
        if (Test-Path $BuildDir) { Remove-Item -Recurse -Force $BuildDir }
        if (Test-Path "compile_commands.json") { Remove-Item -Force compile_commands.json }
    }
    "run-debug" {
        if (Test-Path "$DebugDir/chinese_chess.exe") {
            & "$DebugDir/chinese_chess.exe"
        } else {
            Write-Host "Debug version not built, please run: .\command.ps1 debug"
        }
    }
    "run-release" {
        if (Test-Path "$ReleaseDir/chinese_chess.exe") {
            & "$ReleaseDir/chinese_chess.exe"
        } else {
            Write-Host "Release version not built, please run: .\command.ps1 release"
        }
    }
    "run" {
        if (Test-Path "$DebugDir/chinese_chess.exe") {
            & "$DebugDir/chinese_chess.exe"
        } else {
            Write-Host "Debug version not built, please run: .\command.ps1 debug"
        }
    }
