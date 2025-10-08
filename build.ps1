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
            Write-Host "Debug version not built, please run: .\build.ps1 debug"
        }
    }
    "run-release" {
        if (Test-Path "$ReleaseDir/chinese_chess.exe") {
            & "$ReleaseDir/chinese_chess.exe"
        } else {
            Write-Host "Release version not built, please run: .\build.ps1 release"
        }
    }
    "run" {
        if (Test-Path "$DebugDir/chinese_chess.exe") {
            & "$DebugDir/chinese_chess.exe"
        } else {
            Write-Host "Debug version not built, please run: .\build.ps1 debug"
        }
    }
    "package" {
        if (-not (Test-Path "$ReleaseDir/chinese_chess.exe")) {
            Write-Host "Release version not built, building now..."
            & $MyInvocation.MyCommand.Path "release"
        }
        
        $BuildDistDir = "$BuildDir/dist"
        if (Test-Path $BuildDistDir) { Remove-Item -Recurse -Force $BuildDistDir }
        New-Item -ItemType Directory -Force -Path $BuildDistDir | Out-Null
        
        Copy-Item "$ReleaseDir/chinese_chess.exe" $BuildDistDir
        
        $QtDir = & {"qmake -query QT_INSTALL_PREFIX"} 2>$null
        if (-not $QtDir) {
            $QtDir = [System.IO.Path]::GetFullPath((Get-Command qmake).Path + "/../../..")
        }
        
        & "windeployqt.exe" "$BuildDistDir/chinese_chess.exe"
        
        Write-Host "Package created in build/dist directory"
        Write-Host "This package can run on Windows without Qt installed"
    }
}

