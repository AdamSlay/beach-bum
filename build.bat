@echo on

REM Check if vcpkg is installed
if not exist "vcpkg" (
    REM Clone vcpkg repository
    git clone https://github.com/microsoft/vcpkg.git
    REM Bootstrap vcpkg
    .\vcpkg\bootstrap-vcpkg.bat
)

REM Install dependencies
.\vcpkg\vcpkg install sdl2
.\vcpkg\vcpkg install sdl2-image
.\vcpkg\vcpkg install nlohmann-json

REM Configure and build project
mkdir build
cmake -DCMAKE_TOOLCHAIN_FILE=.\vcpkg\scripts\buildsystems\vcpkg.cmake -B build
cmake --build build