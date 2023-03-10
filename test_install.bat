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

set FIND_PACKAGE_TEST_DIR=%REPOSITORY_PARENT_PATH%\zotero_to_file_tree_find_package_test
set FIND_PACKAGE_TEST_BUILD_DIR=%FIND_PACKAGE_TEST_DIR%\zotero_to_file_tree_build
set FIND_PACKAGE_TEST_SOURCE_DIR=.\test\test_install\template_find_package_install_test
set FIND_PACKAGE_TEST_INSTALL_DIR=%FIND_PACKAGE_TEST_DIR%\zotero_to_file_tree_install

echo "FIND_PACKAGE_TEST_INSTALL_DIR=%FIND_PACKAGE_TEST_INSTALL_DIR%"

rmdir /S /Q %FIND_PACKAGE_TEST_DIR%
mkdir %FIND_PACKAGE_TEST_DIR%

cmake   -B %FIND_PACKAGE_TEST_BUILD_DIR% ^
        -S %FIND_PACKAGE_TEST_SOURCE_DIR% ^
        -G %CMAKE_GENERATOR% -A x64 ^
        --preset %PRESET% ^
        -DBUILD_SHARED_LIBS=ON ^
        -DCMAKE_CXX_STANDARD=20 ^
        -DCMAKE_PREFIX_PATH=%INSTALL_DIR%

cmake --build %FIND_PACKAGE_TEST_BUILD_DIR% --config Debug --parallel 24

cmake --install %FIND_PACKAGE_TEST_BUILD_DIR% --config Debug --prefix %FIND_PACKAGE_TEST_INSTALL_DIR%

%FIND_PACKAGE_TEST_BUILD_DIR%\bin\Debug\use_installdird.exe
%FIND_PACKAGE_TEST_INSTALL_DIR%\bin\use_installdird.exe
