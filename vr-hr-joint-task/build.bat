@echo off
setlocal EnableDelayedExpansion

set VCPKG_ROOT=%VCPKG_ROOT%
set PROJECT_ROOT=%CD%
set DEPS_DIR=%PROJECT_ROOT%\deps

:: Check if the environment variable is set
IF NOT DEFINED VCPKG_ROOT (
    echo ERROR: The environment variable VCPKG_ROOT is not set.
    echo Download and install VCPKG from https://github.com/microsoft/vcpkg#quick-start-windows.
    echo Create an environment variable VCPKG_ROOT that points to the installation directory.
	pause
    exit /b 1
)

:: Every vcpkg dependency, imgui-platform-kit included, is declared in vcpkg.json and
:: installed by CMake on the first configure, at the versions of the vcpkg commit pinned
:: there as builtin-baseline. vcpkg reads those versions from the files it has checked
:: out, so VCPKG_ROOT has to be a full clone at that commit or a newer one.
set "BASELINE="
for /f "tokens=2 delims=:, " %%B in ('findstr /c:"builtin-baseline" "%PROJECT_ROOT%\vcpkg.json"') do set "BASELINE=%%~B"
if not defined BASELINE ( echo ERROR: no builtin-baseline found in vcpkg.json. & pause & exit /b 1 )

set "SHALLOW="
for /f %%S in ('git -C "%VCPKG_ROOT%" rev-parse --is-shallow-repository 2^>nul') do set "SHALLOW=%%S"
if "%SHALLOW%"=="true" (
    echo vcpkg at %VCPKG_ROOT% is a shallow clone, fetching its full history...
    git -C "%VCPKG_ROOT%" fetch --unshallow origin
    if errorlevel 1 (
        echo ERROR: could not fetch the history of %VCPKG_ROOT%. VCPKG_ROOT must be a clone you can write to.
        pause
        exit /b 1
    )
)
git -C "%VCPKG_ROOT%" merge-base --is-ancestor %BASELINE% HEAD 2>nul
if errorlevel 1 (
    echo ERROR: vcpkg at %VCPKG_ROOT% is older than the pinned baseline %BASELINE%, or is not a git clone.
    echo VCPKG_ROOT must be a git clone of https://github.com/microsoft/vcpkg at that commit or newer.
    echo To update it:
    echo   git -C "%VCPKG_ROOT%" pull ^&^& "%VCPKG_ROOT%\bootstrap-vcpkg.bat" -disableMetrics
    pause
    exit /b 1
)

:: Using MSBuild may require elevation
"%VCPKG_ROOT%\vcpkg.exe" integrate install

:: dynamic-neural-field-composer is built from source at the commit in dnfc-ref.txt and
:: against this project's vcpkg.json (VCPKG_MANIFEST_DIR), so it links the same
:: dependency versions. CMakeLists.txt finds it in deps\dnfc-install. The install is
:: stamped with the ref and a hash of the manifest, so changing either one rebuilds it.
set DNFC_SRC=%DEPS_DIR%\dynamic-neural-field-composer
set DNFC_INSTALL=%DEPS_DIR%\dnfc-install
set "DNFC_REF="
for /f "usebackq delims=" %%R in ("%PROJECT_ROOT%\dnfc-ref.txt") do set "DNFC_REF=%%R"
set "MANIFEST_HASH="
for /f %%H in ('type "%PROJECT_ROOT%\vcpkg.json" "%PROJECT_ROOT%\vcpkg-configuration.json" "%PROJECT_ROOT%\ports\imgui-platform-kit\*" 2^>nul ^| git hash-object --stdin') do set "MANIFEST_HASH=%%H"
set "DNFC_STAMP=%DNFC_REF% %MANIFEST_HASH%"

if exist "%DNFC_INSTALL%" (
    set "REVISION="
    if exist "%DNFC_INSTALL%\.revision" set /p REVISION=<"%DNFC_INSTALL%\.revision"
    if not "!REVISION!"=="%DNFC_STAMP%" (
        echo dnfc-install was built from a different revision or dependency set, rebuilding.
        rmdir /s /q "%DNFC_INSTALL%"
    )
)

if not exist "%DNFC_INSTALL%" (
    if not exist "%DNFC_SRC%" (
        git clone https://github.com/Jgocunha/dynamic-neural-field-composer.git "%DNFC_SRC%"
        if errorlevel 1 ( echo ERROR: Failed to clone dynamic-neural-field-composer. & pause & exit /b 1 )
    )
    git -C "%DNFC_SRC%" cat-file -e %DNFC_REF% 2>nul || git -C "%DNFC_SRC%" fetch origin
    git -C "%DNFC_SRC%" checkout %DNFC_REF%
    if errorlevel 1 ( echo ERROR: Failed to checkout %DNFC_REF%. & pause & exit /b 1 )

    if exist "%DNFC_SRC%\build" rmdir /s /q "%DNFC_SRC%\build"
    cmake -G "Visual Studio 17 2022" -A x64 -S "%DNFC_SRC%\dynamic-neural-field-composer" -B "%DNFC_SRC%\build" -DCMAKE_TOOLCHAIN_FILE="%VCPKG_ROOT%/scripts/buildsystems/vcpkg.cmake" -DVCPKG_MANIFEST_DIR="%PROJECT_ROOT%" -DCMAKE_INSTALL_PREFIX="%DNFC_INSTALL%"
    if errorlevel 1 ( echo ERROR: dynamic-neural-field-composer configure failed. & pause & exit /b 1 )
    cmake --build "%DNFC_SRC%\build" --config Release
    if errorlevel 1 ( echo ERROR: dynamic-neural-field-composer build failed. & pause & exit /b 1 )
    cmake --install "%DNFC_SRC%\build" --config Release
    if errorlevel 1 ( echo ERROR: dynamic-neural-field-composer install failed. & pause & exit /b 1 )
    >"%DNFC_INSTALL%\.revision" echo %DNFC_STAMP%
)

:: Create build folders
mkdir %PROJECT_ROOT%\build\x64-release
mkdir %PROJECT_ROOT%\build\x64-debug

:: Run CMake
cmake -G "Visual Studio 17 2022" -A x64 -S "%PROJECT_ROOT%" -B "%PROJECT_ROOT%\build\x64-release" -DCMAKE_TOOLCHAIN_FILE="%VCPKG_ROOT%/scripts/buildsystems/vcpkg.cmake" -DCMAKE_BUILD_TYPE=Release

:: Build your project using the generated build files
cmake --build "%PROJECT_ROOT%\build\x64-release" --config Release

:: Run CMake
:: cmake -G "Visual Studio 17 2022" -A x64 -S "%PROJECT_ROOT%" -B "%PROJECT_ROOT%\build\x64-debug" -DCMAKE_TOOLCHAIN_FILE="%VCPKG_ROOT%/scripts/buildsystems/vcpkg.cmake" -DCMAKE_BUILD_TYPE=Debug

:: Build your project using the generated build files
:: cmake --build "%PROJECT_ROOT%\build\x64-debug" --config Debug


:: Optionally, you can run your executable here if applicable

pause
exit /b 1
