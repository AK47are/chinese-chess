param([string]$Target = "debug")

$BuildDir = "build"
$DebugDir = "$BuildDir/debug"
$ReleaseDir = "$BuildDir/release"

switch ($Target) {
    "debug" {
        if (Test-Path $DebugDir) { Remove-Item -Recurse -Force $DebugDir }
        New-Item -ItemType Directory -Force -Path $DebugDir | Out-Null
        Set-Location $DebugDir
        cmake -G "MinGW Makefiles" -DCMAKE_BUILD_TYPE=Debug -DCMAKE_EXPORT_COMPILE_COMMANDS=ON ../..
        cmake --build . --config Debug
        if (Test-Path "compile_commands.json") {
            Copy-Item "compile_commands.json" "../.." -Force
        }
        Set-Location "../.."
    }
    "release" {
        if (Test-Path $ReleaseDir) { Remove-Item -Recurse -Force $ReleaseDir }
        New-Item -ItemType Directory -Force -Path $ReleaseDir | Out-Null
        Set-Location $ReleaseDir
        cmake -G "MinGW Makefiles" -DCMAKE_BUILD_TYPE=Release -DCMAKE_EXPORT_COMPILE_COMMANDS=ON ../..
        cmake --build . --config Release
        if (Test-Path "compile_commands.json") {
            Copy-Item "compile_commands.json" "../.." -Force
        }
        Set-Location "../.."
    }
    "clean" {
        if (Test-Path $BuildDir) { Remove-Item -Recurse -Force $BuildDir }
    }
    "run-debug" {
        & "$DebugDir/chinese_chess.exe"
    }
    "run-release" {
        & "$ReleaseDir/chinese_chess.exe"
    }
    "run" {
        & "$DebugDir/chinese_chess.exe"
    }
}
