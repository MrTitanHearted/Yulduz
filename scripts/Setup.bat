@echo off
for /F "delims=" %%I in ('git rev-parse --show-toplevel') do set "GIT_ROOT=%%I"
cd /D "%GIT_ROOT%"

rem wgpu-cmake

set DEST_DIR=%GIT_ROOT%\wgpu-cmake
set ZIP_FILE=wgpu-native.zip

mkdir "%DEST_DIR%\include\wgpu"
mkdir "%DEST_DIR%\lib\Debug"
mkdir "%DEST_DIR%\lib\Release"

set ZIP_URL=https://github.com/gfx-rs/wgpu-native/releases/download/v0.19.4.1/wgpu-windows-x86_64-debug.zip
call curl -o %ZIP_FILE% -L %ZIP_URL%
powershell -nologo -noprofile -command "Expand-Archive -Path .\%ZIP_FILE% -DestinationPath %DEST_DIR%"

del %ZIP_FILE%
del "%DEST_DIR%\commit-sha"
del "%DEST_DIR%\*.dll"
del "%DEST_DIR%\*.dll.lib"

move "%DEST_DIR%\*.h" "%DEST_DIR%\include\wgpu"
move "%DEST_DIR%\*.lib" "%DEST_DIR%\lib\Debug"
move "%DEST_DIR%\*.pdb" "%DEST_DIR%\lib\Debug"

set ZIP_URL=https://github.com/gfx-rs/wgpu-native/releases/download/v0.19.4.1/wgpu-windows-x86_64-release.zip
call curl -o %ZIP_FILE% -L %ZIP_URL%
powershell -nologo -noprofile -command "Expand-Archive -Path .\%ZIP_FILE% -DestinationPath %DEST_DIR%"

del %ZIP_FILE%
del "%DEST_DIR%\commit-sha"
del "%DEST_DIR%\*.h"
del "%DEST_DIR%\*.dll"
del "%DEST_DIR%\*.dll.lib"

move "%DEST_DIR%\*.lib" "%DEST_DIR%\lib\Release"
move "%DEST_DIR%\*.pdb" "%DEST_DIR%\lib\Release"

copy "%GIT_ROOT%\scripts\windows.cmake" "%DEST_DIR%\wgpuConfig.cmake"

rem wgpu-cmake

rem dependencies

call vcpkg install glfw3 glm stb assimp EnTT --triplet=x64-windows

rem dependencies