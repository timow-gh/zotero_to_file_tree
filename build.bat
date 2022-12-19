set REL_REPOSITORY_PATH=.\
set REPOSITORY_PARENT_PATH=

pushd %REL_REPOSITORY_PATH%
cd ..\
set REPOSITORY_PARENT_PATH=%CD%
popd

echo "REL_REPOSITORY_PATH=%REL_REPOSITORY_PATH%"
echo "REPOSITORY_PARENT_PATH=%REPOSITORY_PARENT_PATH%"

set BUILD_DIR=%REPOSITORY_PARENT_PATH%\cpp_proj_template_build
set SOURCE_DIR=.\
set INSTALL_DIR=%REPOSITORY_PARENT_PATH%\cpp_proj_template_install

mkdir %BUILD_DIR%

set CMAKE_GENERATOR="Visual Studio 17 2022"
set PRESET="windows-msvc-release-developer-mode"

cmake   -B %BUILD_DIR% -S %SOURCE_DIR% -G %CMAKE_GENERATOR% -A x64 ^
        --preset %PRESET% ^
        -DBUILD_SHARED_LIBS=ON ^
        -DCMAKE_CXX_STANDARD=20 ^
        -Dcpp_proj_template_BUILD_TESTS=ON ^
        -Dcpp_proj_template_BUILD_BENCMARKS=ON ^
        -Dcpp_proj_template_WARNINGS_AS_ERRORS=ON ^
        -Dcpp_proj_template_BUILD_PACKAGE=ON

cmake --build %BUILD_DIR% --config Release --parallel 24

pushd %REL_REPOSITORY_PATH%

cd %BUILD_DIR%
ctest -C Release
cpack -C Release

popd

mkdir %INSTALL_DIR%
cmake --install %BUILD_DIR% --config Release --prefix %INSTALL_DIR%
