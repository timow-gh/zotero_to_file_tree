set REL_REPOSITORY_PATH=.\
set REPOSITORY_PARENT_PATH=

pushd %REL_REPOSITORY_PATH%
cd ..\
set REPOSITORY_PARENT_PATH=%CD%
popd

echo "REL_REPOSITORY_PATH=%REL_REPOSITORY_PATH%"
echo "REPOSITORY_PARENT_PATH=%REPOSITORY_PARENT_PATH%"

set BUILD_DIR=%REPOSITORY_PARENT_PATH%\zotero_to_file_tree_build
set SOURCE_DIR=.\
set INSTALL_DIR=%REPOSITORY_PARENT_PATH%\zotero_to_file_tree_install

mkdir %BUILD_DIR%

set CMAKE_GENERATOR=Ninja

cmake   -B %BUILD_DIR% -S %SOURCE_DIR% -G %CMAKE_GENERATOR% ^
        -DCMAKE_BUILD_TYPE=Release ^
        --preset "windows-msvc-release-user-mode" ^
        -DCMAKE_CXX_STANDARD=20 ^
        -DBUILD_SHARED_LIBS=OFF ^
        -DCMAKE_CXX_COMPILER=clang++ ^
        -DCMAKE_C_COMPILER=clang

cmake --build %BUILD_DIR% --config Release --parallel

mkdir %INSTALL_DIR%
cmake --install %BUILD_DIR% --config Release --prefix %INSTALL_DIR%
