ECHO OFF
:: Batch file: csd2101.bat
:: Author: Parminder Singh
:: Date: April 20, 2026
:: Revision: 1.0

::     .d8888b.   .d8888b.  8888888b.          .d8888b.   d888   .d8888b.   .d88     ::
::    d88P  Y88b d88P  Y88b 888  "Y88b        d88P  Y88b d8888  d88P  Y88b d8888     ::
::    888    888 Y88b.      888    888               888   888  888    888   888     ::
::    888         "Y888b.   888    888             .d88P   888  888    888   888     ::
::    888            "Y88b. 888    888         .od888P"    888  888    888   888     ::
::    888    888       "888 888    888 888888 d88P"        888  888    888   888     ::
::    Y88b  d88P Y88b  d88P 888  .d88P        888"         888  Y88b  d88P   888     ::
::     "Y8888P"   "Y8888P"  8888888P"         888888888  8888888 "Y8888P"  8888888   ::


REM Set the source directory to "projects".
SET "projects-dir=projects"

REM Set the test directory to "test-submissions".
SET "test-submission-dir=test-submissions"

REM Set the resource directory meshes.
SET "meshes-resource-dir=meshes"

REM Set the resource directory meshes.
SET "scene-resource-dir=scenes"

REM Set the resource directory images.
SET "images-resource-dir=images"

REM Set the library directory.
SET "lib-dir=lib"

REM Set start code directory for csd-2101.
SET "csd-2100-starter-kit=%lib-dir%\csd-2101"

REM Setting Git directory path
SET "GitDir=%~dp0.git"

REM Setting build directory path
SET "BuildDir=%~dp0build"

REM Setting Git modules path
SET "GitModules=%~dp0.gitmodules"

REM Setting CMake file path
SET "CMakeFile=%~dp0CMakeLists.txt"

CLS
:MENU
	ECHO.
	ECHO .....................................................
	ECHO Select your task, or Esc to EXIT.
	ECHO .....................................................
	ECHO   S   - Setup OpenGL 4.5
	ECHO   D   - Delete OpenGL 4.5 Setup
	ECHO   B   - Build All
	ECHO   C   - Clean and Build All
	ECHO   R   - Refresh Solution
	ECHO   Z   - Create Submission Zip
	ECHO   E   - EXIT
	ECHO   1   - Create Tutorial 1 
	ECHO   2   - Create Tutorial 2 
	ECHO   3   - Create Tutorial 3 
	ECHO   4   - Create Tutorial 4 
	ECHO   5   - Create Tutorial 5 
	ECHO   6   - Create Tutorial 6
	ECHO   7   - Create Assignment 1
	ECHO.
	REM Clear previous input so pressing ENTER does not repeat the last option.
	SET "M="
	SET /P M=Type option then press ENTER:

	REM If user presses ENTER without typing anything, return to menu safely.
	IF "%M%"=="" GOTO MENU

	IF /I %M%==S GOTO CMAKE_BUILD
	IF /I %M%==D GOTO CLEAN
	IF /I %M%==B GOTO BUILD_ALL
	IF /I %M%==C GOTO CLEAN_BUILD_ALL
	IF /I %M%==R GOTO BUILDCMAKELISTS
	IF /I %M%==Z GOTO CREATE_SUBMISSION
	IF /I %M%==E GOTO :EOF
	IF %M%==1 GOTO CREATE_PROJECT_TUTORIAL_1
	IF %M%==2 GOTO CREATE_PROJECT_TUTORIAL_2
	IF %M%==3 GOTO CREATE_PROJECT_TUTORIAL_3
	IF %M%==4 GOTO CREATE_PROJECT_TUTORIAL_4
	IF %M%==5 GOTO CREATE_PROJECT_TUTORIAL_5
	IF %M%==6 GOTO CREATE_PROJECT_TUTORIAL_6
	IF %M%==7 GOTO CREATE_PROJECT_ASSIGNMENT_1
	IF %M%==100 (
		GOTO PLAYLOGS
	) ELSE (
	    ECHO Invalid choice. Please enter valid option from menu.
	    PAUSE
	    GOTO MENU
	)

:CREATE_PROJECT_TUTORIAL_0
	CALL :CHECK_OPENGL_LIBS "%lib-dir%"
	CALL :COPY_FROM_REMOTE_URL https://faculty.digipen.edu/~parminder.singh/tutorial-0.zip projects
	GOTO BUILDCMAKELISTS

:CREATE_PROJECT_TUTORIAL_1
	CALL :CHECK_OPENGL_LIBS "%lib-dir%"
	CALL :COPY_FROM_REMOTE_URL https://faculty.digipen.edu/~parminder.singh/tutorial-1.zip projects
	GOTO BUILDCMAKELISTS

:CREATE_PROJECT_TUTORIAL_2
	CALL :CHECK_OPENGL_LIBS "%lib-dir%"
	CALL :COPY_FROM_REMOTE_URL https://faculty.digipen.edu/~parminder.singh/tutorial-2.zip projects
	GOTO BUILDCMAKELISTS

:CREATE_PROJECT_TUTORIAL_3
	CALL :CHECK_OPENGL_LIBS "%lib-dir%"
	CALL :COPY_FROM_REMOTE_URL https://faculty.digipen.edu/~parminder.singh/tutorial-3.zip projects
	GOTO BUILDCMAKELISTS

:CREATE_PROJECT_TUTORIAL_4
	CALL :CHECK_OPENGL_LIBS "%lib-dir%"
	CALL :COPY_FROM_REMOTE_URL https://faculty.digipen.edu/~parminder.singh/tutorial-4.zip projects

	set "scene-src-path=%projects-dir%/tutorial-4/%scene-resource-dir%"
    CALL :COPY_RESOURCE %scene-src-path% %scene-resource-dir%

	set "mesh-src-path=%projects-dir%/tutorial-4/%meshes-resource-dir%"
    CALL :COPY_RESOURCE %mesh-src-path% %meshes-resource-dir%

	GOTO BUILDCMAKELISTS

:CREATE_PROJECT_TUTORIAL_5
	CALL :CHECK_OPENGL_LIBS "%lib-dir%"
	CALL :COPY_FROM_REMOTE_URL https://faculty.digipen.edu/~parminder.singh/tutorial-5.zip projects

	set "images-src-path=%projects-dir%/tutorial-5/%images-resource-dir%"
    CALL :COPY_RESOURCE %images-src-path% %images-resource-dir%

	GOTO BUILDCMAKELISTS

:CREATE_PROJECT_TUTORIAL_6
	CALL :CHECK_OPENGL_LIBS "%lib-dir%"
	CALL :COPY_FROM_REMOTE_URL https://faculty.digipen.edu/~parminder.singh/tutorial-6.zip projects
	GOTO BUILDCMAKELISTS

:CREATE_PROJECT_ASSIGNMENT_1
	CALL :CHECK_OPENGL_LIBS "%lib-dir%"
	CALL :COPY_FROM_REMOTE_URL https://faculty.digipen.edu/~parminder.singh/assignment-1.zip projects

	set "scene-src-path=%projects-dir%/assignment-1/%scene-resource-dir%"
    CALL :COPY_RESOURCE %scene-src-path% %scene-resource-dir%

	set "mesh-src-path=%projects-dir%/assignment-1/%meshes-resource-dir%"
    CALL :COPY_RESOURCE %mesh-src-path% %meshes-resource-dir%

	set "dpml-src-path=%projects-dir%/assignment-1/dpml"
    CALL :COPY_RESOURCE %dpml-src-path% %lib-dir%/dpml

	GOTO BUILDCMAKELISTS

:COPY_FROM_REMOTE_URL
	setlocal
		SET "remote_url=%~1"
		SET "destination_folder=%~2"

		FOR /f "delims=/ tokens=*" %%a in ("%remote_url%") do (
			SET "filename_ext=%%~nxa"
		    SET "filename=%%~na"
		)

		::curl -L -o "%filename_ext%" "%remote_url%"
        :: Download with Googlebot user-agent
        curl --ssl-no-revoke -L -o "%filename_ext%" -A "Googlebot/2.1 (+http://www.google.com/bot.html)" "%remote_url%"
		IF %errorlevel% neq 0 (ECHO Failed to download the file.)

		tar -xf "%filename_ext%" -C .
		IF %errorlevel% neq 0 (ECHO Failed to unzip the file.)

		IF EXIST "%filename_ext%" ECHO y|del "%filename_ext%"
		xcopy "%filename%" "%destination_folder%/%filename%" /s /i /-y

		IF EXIST "%filename%" ECHO y|rmdir /s "%filename%"
	endlocal
	goto :eof

:: copy resources
:COPY_RESOURCE
	call :CHECK_OPENGL_LIBS "%lib-dir%"
	set "source-path=%~1"
	set "destination-path=%~2"

	if not exist "%destination-path%" (
		mkdir "%destination-path%"
	)

	if not exist "%source-path%" (
	    echo [WARNING!] %source-path% directory does not exists.
	    pause
	    GOTO MENU
	) else (
		xcopy "%source-path%" "%destination-path%" /s /e /i /-y
		rmdir /s /q "%source-path%"
	)
	goto :eof


:: Check OpenGL dependencies
:CHECK_OPENGL_LIBS
	SET "folder=%~1"
	IF not EXIST "%folder%" (
	    ECHO [WARNING!] OpenGL 4.5 is not configured.
	    ECHO Choose the menu option: Press S to set up OpenGL 4.5.
	    REM You can add further instructions or actions here.
	    PAUSE
	    GOTO MENU
	)
	goto :eof

:CMAKE_BUILD
	SET "GitFolder=%~dp0.git"
	SET "GitModules=%~dp0.gitmodules"

	IF EXIST "%GitFolder%" ECHO y|rmdir /s "%GitFolder%"
    CALL git init
    CALL git config core.autocrlf false
	CALL git submodule add https://github.com/glfw/glfw ./lib/glfw
	CALL git submodule add https://github.com/omniavinco/glew-cmake ./lib/glew
	CALL git submodule add https://github.com/glfw/gleq ./lib/gleq
	CALL git submodule add https://github.com/giraphics/glfw-event-recorder ./lib/glfw-event-recorder
	CALL git submodule add https://github.com/lvandeve/lodepng ./lib/lodepng
	CALL git submodule add https://github.com/g-truc/glm ./lib/glm
	CALL git submodule add https://github.com/tinyobjloader/tinyobjloader ./lib/tinyobjloader

	IF EXIST "%GitFolder%" ECHO y|rmdir /s "%GitFolder%"
	IF EXIST "%GitModules%" ECHO y|del "%GitModules%"

	SET "folders=build %test-submission-dir% %projects-dir%"

	REM Loop through each folder name and create it
	FOR %%i in (%folders%) do (
	    IF NOT EXIST "%%i" mkdir "%%i"
	)

	REM Create cmake folder and all required CMake files if they do not exist
	CALL :SETUP_CMAKE_FILES

	GOTO BUILDCMAKELISTS
	GOTO MENU

:BUILD_ALL
	cmake --build build --config Release
	GOTO MENU

:CLEAN_BUILD_ALL
	cmake --build build --clean-first --config Release
	GOTO MENU

:CLEAN
	IF EXIST "%GitDir%" ECHO y|rmdir /s "%GitDir%"
	IF EXIST "%BuildDir%" ECHO y|rmdir /s "%BuildDir%"
	IF EXIST "%lib-dir%" ECHO y|rmdir /s "%lib-dir%"
	IF EXIST "%GitModules%" ECHO y|del "%GitModules%"
	IF EXIST "%CMakeFile%" ECHO y|del "%CMakeFile%"
	GOTO MENU

:PLAYLOGS
	:: Below Playback log are har
	SET curDir=%~dp0
	PUSHD %curDir%build
	Release\tutorial-0.exe -p -f ..\logs\tutorial-0-playback.log
	POPD
	GOTO MENU

:BUILDCMAKELISTS
	:: Create CMake project
	SET curDir=%~dp0
	PUSHD %curDir%build
	cmake -G "Visual Studio 17 2022" ..
	POPD
	GOTO MENU

:: ============================================================
:: SETUP_CMAKE_FILES - Creates cmake folder and all CMake files
:: if they do not already exist.
:: ============================================================
:SETUP_CMAKE_FILES
	IF NOT EXIST "cmake" mkdir "cmake"
	CALL :WRITE_CMAKE_UTILITIES
	CALL :WRITE_CMAKE_BUILDTESTS
	CALL :WRITE_PROJECTS_CMAKELISTS
	CALL :WRITE_TESTSUBMISSIONS_CMAKELISTS
	CALL :WRITE_ROOT_CMAKELISTS
	goto :eof

:: ============================================================
:: WRITE_CMAKE_UTILITIES - Writes cmake\Utilities.cmake
:: ============================================================
:WRITE_CMAKE_UTILITIES
	IF EXIST "cmake\Utilities.cmake" goto :eof
	echo # Define a macro to group source and header files by directory > cmake\Utilities.cmake
	echo macro^(GROUP_FILES_BY_FOLDER all_files^) >> cmake\Utilities.cmake
	echo     foreach^(FILE ${ALL_FILES}^)  >> cmake\Utilities.cmake
	echo         # Get the directory of the file >> cmake\Utilities.cmake
	echo         get_filename_component^(PARENT_DIR "${FILE}" PATH^) >> cmake\Utilities.cmake
	echo. >> cmake\Utilities.cmake
	echo         # Replace '/' with '\' for Visual Studio group formatting >> cmake\Utilities.cmake
	echo         string^(REPLACE "/" "\\" GROUP "${PARENT_DIR}"^) >> cmake\Utilities.cmake
	echo. >> cmake\Utilities.cmake
	echo         # Classify files into Source or Header groups based on their extension >> cmake\Utilities.cmake
	echo         if ^("${FILE}" MATCHES ".*\\.cpp"^) >> cmake\Utilities.cmake
	echo         set^(GROUP "Source Files\\${GROUP}"^) >> cmake\Utilities.cmake
	echo         elseif^("${FILE}" MATCHES ".*\\.hpp"^) >> cmake\Utilities.cmake
	echo         set^(GROUP "Header Files\\${GROUP}"^) >> cmake\Utilities.cmake
	echo         endif^(^) >> cmake\Utilities.cmake
	echo. >> cmake\Utilities.cmake
	echo         # Add the file to the corresponding source group >> cmake\Utilities.cmake
	echo         source_group^("${GROUP}" FILES "${FILE}"^) >> cmake\Utilities.cmake
	echo     endforeach^(^) >> cmake\Utilities.cmake
	echo endmacro^(^) >> cmake\Utilities.cmake
	echo. >> cmake\Utilities.cmake
	echo # Define function to copy assets with conditional copying >> cmake\Utilities.cmake
	echo function^(COPY_FOLDER_IF_DIFFERENT PROJECT_NAME source_dir dest_dir^) >> cmake\Utilities.cmake
	echo     file^(GLOB_RECURSE ASSET_FILES "${source_dir}/*"^) >> cmake\Utilities.cmake
	echo. >> cmake\Utilities.cmake
	echo     foreach^(asset ${ASSET_FILES}^) >> cmake\Utilities.cmake
	echo         get_filename_component^(relative_path ${asset} DIRECTORY^) >> cmake\Utilities.cmake
	echo         string^(REPLACE "${source_dir}" "" relative_path "${relative_path}"^) >> cmake\Utilities.cmake
	echo         set^(dest_file "${dest_dir}/${relative_path}"^) >> cmake\Utilities.cmake
	echo. >> cmake\Utilities.cmake
	echo         add_custom_command^( >> cmake\Utilities.cmake
	echo             TARGET ${PROJECT_NAME} >> cmake\Utilities.cmake
	echo             POST_BUILD >> cmake\Utilities.cmake
	echo             COMMAND ${CMAKE_COMMAND} -E make_directory ${dest_file} >> cmake\Utilities.cmake
	echo             COMMAND ${CMAKE_COMMAND} -E copy_if_different ${asset} ${dest_file} >> cmake\Utilities.cmake
	echo         ^) >> cmake\Utilities.cmake
	echo     endforeach^(^) >> cmake\Utilities.cmake
	echo endfunction^(^) >> cmake\Utilities.cmake
	goto :eof

:: ============================================================
:: WRITE_CMAKE_BUILDTESTS - Writes cmake\BuildTests.cmake
:: ============================================================
:WRITE_CMAKE_BUILDTESTS
	IF EXIST "cmake\BuildTests.cmake" goto :eof
	echo macro^(SETUP_EXAMPLE TARGET_NAME^) > cmake\BuildTests.cmake
	echo. >> cmake\BuildTests.cmake
	echo     set^(GAME_ENGINE_NAME opengl-dev^) >> cmake\BuildTests.cmake
	echo. >> cmake\BuildTests.cmake
	echo     # Project name >> cmake\BuildTests.cmake
	echo     project^(${TARGET_NAME} VERSION 1.0.0 LANGUAGES CXX^) >> cmake\BuildTests.cmake
	echo. >> cmake\BuildTests.cmake
	echo     # C++ standard >> cmake\BuildTests.cmake
	echo     set^(CMAKE_CXX_STANDARD 20^) >> cmake\BuildTests.cmake
	echo     set^(CMAKE_CXX_STANDARD_REQUIRED True^) >> cmake\BuildTests.cmake
	echo     # Fix: pin the VS debugger working dir to build root so relative shader/asset >> cmake\BuildTests.cmake
	echo     # regardless of which .sln launched the executable. >> cmake\BuildTests.cmake
	echo. >> cmake\BuildTests.cmake
	echo     # Source files >> cmake\BuildTests.cmake
	echo     file^(GLOB_RECURSE ${TARGET_NAME}_source_files >> cmake\BuildTests.cmake
	echo         CONFIGURE_DEPENDS >> cmake\BuildTests.cmake
	echo         ${CMAKE_CURRENT_LIST_DIR}/${TARGET_NAME}/src/*.[ch]pp >> cmake\BuildTests.cmake
	echo         ${CMAKE_CURRENT_LIST_DIR}/${TARGET_NAME}/include/*.h >> cmake\BuildTests.cmake
	echo     ^) >> cmake\BuildTests.cmake
	echo. >> cmake\BuildTests.cmake
	echo     add_executable^(${TARGET_NAME} >> cmake\BuildTests.cmake
	echo         ${${TARGET_NAME}_source_files} >> cmake\BuildTests.cmake
	echo     ^) >> cmake\BuildTests.cmake
	echo. >> cmake\BuildTests.cmake
	echo     set_target_properties^(${TARGET_NAME} PROPERTIES >> cmake\BuildTests.cmake
	echo         VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_BINARY_DIR}" >> cmake\BuildTests.cmake
	echo     ^) >> cmake\BuildTests.cmake
	echo. >> cmake\BuildTests.cmake
	echo     # Include dirs >> cmake\BuildTests.cmake
	echo     target_include_directories^(${TARGET_NAME} >> cmake\BuildTests.cmake
	echo         PRIVATE >> cmake\BuildTests.cmake
	echo             ${CMAKE_CURRENT_LIST_DIR}/${TARGET_NAME}/include >> cmake\BuildTests.cmake
	echo     ^) >> cmake\BuildTests.cmake
	echo. >> cmake\BuildTests.cmake
	echo     # Compiler options >> cmake\BuildTests.cmake
	echo     if^(CMAKE_CXX_COMPILER_ID MATCHES "GNU^|Clang"^) >> cmake\BuildTests.cmake
	echo         target_compile_options^(${TARGET_NAME} PRIVATE -Wall ${DisableWarnings}^) >> cmake\BuildTests.cmake
	echo     elseif^(MSVC^) >> cmake\BuildTests.cmake
	echo         target_compile_options^(${TARGET_NAME} PRIVATE /W3 /WX-^) >> cmake\BuildTests.cmake
	echo     endif^(^) >> cmake\BuildTests.cmake
	echo. >> cmake\BuildTests.cmake
	echo     set_property^(TARGET ${TARGET_NAME} PROPERTY CXX_STANDARD 20^) >> cmake\BuildTests.cmake
	echo. >> cmake\BuildTests.cmake
	echo     # ---- dpml detection ---- >> cmake\BuildTests.cmake
	echo     set^(DPML_INCLUDE_PATH ${CMAKE_SOURCE_DIR}/lib/dpml/include^) >> cmake\BuildTests.cmake
	echo     set^(DPML_DEBUG_PATH   ${CMAKE_SOURCE_DIR}/lib/dpml/lib/Debug^) >> cmake\BuildTests.cmake
	echo     set^(DPML_RELEASE_PATH ${CMAKE_SOURCE_DIR}/lib/dpml/lib/Release^) >> cmake\BuildTests.cmake
	echo. >> cmake\BuildTests.cmake
	echo     if^(EXISTS "${DPML_INCLUDE_PATH}"^) >> cmake\BuildTests.cmake
	echo         # message^(STATUS "[${TARGET_NAME}] dpml headers found, adding include path..."^) >> cmake\BuildTests.cmake
	echo         target_include_directories^(${TARGET_NAME} >> cmake\BuildTests.cmake
	echo             PRIVATE >> cmake\BuildTests.cmake
	echo                 ${DPML_INCLUDE_PATH} >> cmake\BuildTests.cmake
	echo         ^) >> cmake\BuildTests.cmake
	echo     endif^(^) >> cmake\BuildTests.cmake
	echo. >> cmake\BuildTests.cmake
	echo     if^(EXISTS "${DPML_DEBUG_PATH}/dpml.lib" OR EXISTS "${DPML_RELEASE_PATH}/dpml.lib"^) >> cmake\BuildTests.cmake
	echo         # message^(STATUS "[${TARGET_NAME}] dpml found, linking..."^) >> cmake\BuildTests.cmake
	echo. >> cmake\BuildTests.cmake
	echo         target_link_directories^(${TARGET_NAME} >> cmake\BuildTests.cmake
	echo             PRIVATE >> cmake\BuildTests.cmake
	echo                 $^<$^<CONFIG:Debug^>:${DPML_DEBUG_PATH}^> >> cmake\BuildTests.cmake
	echo                 $^<$^<CONFIG:Release^>:${DPML_RELEASE_PATH}^> >> cmake\BuildTests.cmake
	echo         ^) >> cmake\BuildTests.cmake
	echo. >> cmake\BuildTests.cmake
	echo         target_link_libraries^(${TARGET_NAME} >> cmake\BuildTests.cmake
	echo             PRIVATE >> cmake\BuildTests.cmake
	echo                 ${ALL_LIBS} >> cmake\BuildTests.cmake
	echo                 csd2101 >> cmake\BuildTests.cmake
	echo                 dpml >> cmake\BuildTests.cmake
	echo         ^) >> cmake\BuildTests.cmake
	echo     else^(^) >> cmake\BuildTests.cmake
	echo         target_link_libraries^(${TARGET_NAME} >> cmake\BuildTests.cmake
	echo             PRIVATE >> cmake\BuildTests.cmake
	echo                 ${ALL_LIBS} >> cmake\BuildTests.cmake
	echo                 csd2101 >> cmake\BuildTests.cmake
	echo         ^) >> cmake\BuildTests.cmake
	echo     endif^(^) >> cmake\BuildTests.cmake
	echo. >> cmake\BuildTests.cmake
	echo endmacro^(^) >> cmake\BuildTests.cmake
	goto :eof

:: ============================================================
:: WRITE_PROJECTS_CMAKELISTS - Writes projects\CMakeLists.txt
:: ============================================================
:WRITE_PROJECTS_CMAKELISTS
	IF EXIST "projects\CMakeLists.txt" goto :eof
	echo # Get a list of all directories in the current folder > projects\CMakeLists.txt
	echo file^(GLOB directories LIST_DIRECTORIES true *^) >> projects\CMakeLists.txt
	echo. >> projects\CMakeLists.txt
	echo # Iterate over each directory >> projects\CMakeLists.txt
	echo foreach^(dir ${directories}^) >> projects\CMakeLists.txt
	echo     # Check if the path is a directory >> projects\CMakeLists.txt
	echo     if^(IS_DIRECTORY ${dir}^) >> projects\CMakeLists.txt
	echo         # Get the directory name from the full path >> projects\CMakeLists.txt
	echo         get_filename_component^(dir_name ${dir} NAME^) >> projects\CMakeLists.txt
	echo         # Call the macro with the directory name >> projects\CMakeLists.txt
	echo         SETUP_EXAMPLE^(${dir_name}^) >> projects\CMakeLists.txt
	echo     endif^(^) >> projects\CMakeLists.txt
	echo endforeach^(^) >> projects\CMakeLists.txt
	goto :eof

:: ============================================================
:: WRITE_TESTSUBMISSIONS_CMAKELISTS - Writes test-submissions\CMakeLists.txt
:: ============================================================
:WRITE_TESTSUBMISSIONS_CMAKELISTS
	IF EXIST "test-submissions\CMakeLists.txt" goto :eof
	echo # Get a list of all directories in the current folder > test-submissions\CMakeLists.txt
	echo file^(GLOB directories LIST_DIRECTORIES true *^) >> test-submissions\CMakeLists.txt
	echo. >> test-submissions\CMakeLists.txt
	echo # Iterate over each directory >> test-submissions\CMakeLists.txt
	echo foreach^(dir ${directories}^) >> test-submissions\CMakeLists.txt
	echo     # Check if the path is a directory >> test-submissions\CMakeLists.txt
	echo     if^(IS_DIRECTORY ${dir}^) >> test-submissions\CMakeLists.txt
	echo         # Get the directory name from the full path >> test-submissions\CMakeLists.txt
	echo         get_filename_component^(dir_name ${dir} NAME^) >> test-submissions\CMakeLists.txt
	echo         # Call the macro with the directory name >> test-submissions\CMakeLists.txt
	echo         SETUP_EXAMPLE^(${dir_name}^) >> test-submissions\CMakeLists.txt
	echo     endif^(^) >> test-submissions\CMakeLists.txt
	echo endforeach^(^) >> test-submissions\CMakeLists.txt
	goto :eof

::============================================================
:: CREATE_SUBMISSION - Zips project, copies and extracts in
:: test-submissions\ for pre-upload verification.
::============================================================
:CREATE_SUBMISSION
	ECHO.
	ECHO .....................................................
	ECHO  CSD2101 ^| Create Submission Zip
	ECHO .....................................................
	ECHO.

	REM -- Get DigiPen login ID --
	SET "LOGIN_ID="
	SET /P LOGIN_ID=Enter your DigiPen login ID (e.g. foo):
	IF "%LOGIN_ID%"=="" (
		ECHO [ERROR] Login ID cannot be empty.
		PAUSE
		GOTO MENU
	)

	REM -- Show available projects and get project name --
	ECHO.
	ECHO Available projects:
	FOR /D %%D IN ("%projects-dir%\*") DO ECHO   %%~nD
	ECHO.
	SET "PROJECT_NAME="
	SET /P PROJECT_NAME=Enter project name to submit (e.g. tutorial-1):
	IF "%PROJECT_NAME%"=="" (
		ECHO [ERROR] Project name cannot be empty.
		PAUSE
		GOTO MENU
	)

	REM -- Validate project directory exists --
	IF NOT EXIST "%projects-dir%\%PROJECT_NAME%" (
		ECHO [ERROR] Project "%PROJECT_NAME%" not found in %projects-dir%\.
		ECHO         Check the available projects listed above and try again.
		PAUSE
		GOTO MENU
	)

	REM -- Build all paths --
	SET "curDir=%~dp0"
	SET "SUBMISSION_NAME=%LOGIN_ID%-%PROJECT_NAME%"
	SET "ZIP_FILE=%SUBMISSION_NAME%.zip"
	SET "SOURCE_DIR=%curDir%%projects-dir%\%PROJECT_NAME%"
	SET "STAGING_DIR=%curDir%%projects-dir%\%SUBMISSION_NAME%"
	SET "ZIP_PATH=%curDir%%projects-dir%\%ZIP_FILE%"
	SET "TEST_DIR=%curDir%%test-submission-dir%"
	SET "TEST_ZIP=%TEST_DIR%\%ZIP_FILE%"
	SET "TEST_EXTRACT=%TEST_DIR%\%SUBMISSION_NAME%"

	ECHO.
	ECHO .....................................................
	ECHO  Submission : %SUBMISSION_NAME%
	ECHO  Zip        : %projects-dir%\%ZIP_FILE%
	ECHO  Verify at  : %test-submission-dir%\%SUBMISSION_NAME%
	ECHO .....................................................
	ECHO.

	REM -- [1/5] Create renamed staging copy --
	ECHO [1/5] Preparing submission directory...
	IF EXIST "%STAGING_DIR%" ECHO y|rmdir /s "%STAGING_DIR%"
	xcopy "%SOURCE_DIR%" "%STAGING_DIR%\" /s /i /q
	IF %ERRORLEVEL% NEQ 0 (
		ECHO [ERROR] Failed to copy project directory. Aborting.
		PAUSE
		GOTO MENU
	)

	REM -- [2/5] Create zip archive --
	ECHO [2/5] Creating zip archive...
	IF EXIST "%ZIP_PATH%" del /f /q "%ZIP_PATH%"
	powershell -Command "Compress-Archive -Path '%STAGING_DIR%' -DestinationPath '%ZIP_PATH%' -Force"
	IF NOT EXIST "%ZIP_PATH%" (
		ECHO [ERROR] Failed to create zip archive. Aborting.
		IF EXIST "%STAGING_DIR%" ECHO y|rmdir /s "%STAGING_DIR%"
		PAUSE
		GOTO MENU
	)

	REM -- [3/5] Remove staging directory --
	ECHO [3/5] Removing temporary staging directory...
	IF EXIST "%STAGING_DIR%" ECHO y|rmdir /s "%STAGING_DIR%"

	REM -- [4/5] Copy zip to test-submissions\ --
	ECHO [4/5] Copying zip to test-submissions...
	IF NOT EXIST "%TEST_DIR%" mkdir "%TEST_DIR%"
	copy /y "%ZIP_PATH%" "%TEST_ZIP%" > nul
	IF %ERRORLEVEL% NEQ 0 (
		ECHO [ERROR] Failed to copy zip to test-submissions\. Aborting.
		PAUSE
		GOTO MENU
	)

	REM -- [5/5] Extract in test-submissions\ for verification --
	ECHO [5/5] Extracting in test-submissions for verification...
	IF EXIST "%TEST_EXTRACT%" ECHO y|rmdir /s "%TEST_EXTRACT%"
	powershell -Command "Expand-Archive -LiteralPath '%TEST_ZIP%' -DestinationPath '%TEST_DIR%' -Force"
	IF NOT EXIST "%TEST_EXTRACT%" (
		ECHO [ERROR] Extraction failed. Please check the zip manually.
		PAUSE
		GOTO MENU
	)

	ECHO.
	ECHO .....................................................
	ECHO  Submission created successfully!
	ECHO .....................................................
	ECHO  Zip      : %projects-dir%\%ZIP_FILE%
	ECHO  Verified : %test-submission-dir%\%SUBMISSION_NAME%
	ECHO .....................................................
	ECHO  Next steps:
	ECHO  1. Select R to refresh the Visual Studio solution
	ECHO  2. Build and run "%SUBMISSION_NAME%" to verify it works
	ECHO  3. Upload %ZIP_FILE% from %projects-dir%\ to Moodle
	ECHO .....................................................
	ECHO.
	PAUSE
	GOTO MENU

:: ============================================================
:: WRITE_ROOT_CMAKELISTS - Writes root CMakeLists.txt
:: ============================================================
:WRITE_ROOT_CMAKELISTS
	IF EXIST "CMakeLists.txt" goto :eof
	echo # Projects Settings > CMakeLists.txt
	echo. >> CMakeLists.txt
	echo cmake_minimum_required^(VERSION 2.8...3.13^) >> CMakeLists.txt
	echo. >> CMakeLists.txt
	echo project ^(opengl-dev^) >> CMakeLists.txt
	echo. >> CMakeLists.txt
	echo add_definitions^(-DUSE_CSD2101_AUTOMATION=0^) >> CMakeLists.txt
	echo add_definitions^(-DGLEW_STATIC^) >> CMakeLists.txt
	echo add_subdirectory^(lib/glfw EXCLUDE_FROM_ALL^) >> CMakeLists.txt
	echo add_subdirectory^(lib/glew EXCLUDE_FROM_ALL^) >> CMakeLists.txt
	echo. >> CMakeLists.txt
	echo set^(EXTERNAL_LIBRARY_SOURCES >> CMakeLists.txt
	echo     ${CMAKE_CURRENT_LIST_DIR}/lib/lodepng/lodepng.h >> CMakeLists.txt
	echo     ${CMAKE_CURRENT_LIST_DIR}/lib/lodepng/lodepng.cpp >> CMakeLists.txt
	echo     ${CMAKE_CURRENT_LIST_DIR}/lib/glfw-event-recorder/recorder.h >> CMakeLists.txt
	echo     ${CMAKE_CURRENT_LIST_DIR}/lib/glfw-event-recorder/recorder.cpp >> CMakeLists.txt
	echo     ${CMAKE_CURRENT_LIST_DIR}/lib/glfw-event-recorder/hook_csd2101.h >> CMakeLists.txt
	echo ^) >> CMakeLists.txt
	echo. >> CMakeLists.txt
	echo # All libraries - GLFW, glew >> CMakeLists.txt
	echo set^(ALL_LIBS >> CMakeLists.txt
	echo   PRIVATE glfw >> CMakeLists.txt
	echo   PRIVATE libglew_static >> CMakeLists.txt
	echo ^) >> CMakeLists.txt
	echo ADD_LIBRARY^(csd2101 STATIC >> CMakeLists.txt
	echo     ${EXTERNAL_LIBRARY_SOURCES} >> CMakeLists.txt
	echo ^) >> CMakeLists.txt
	echo target_link_libraries^(csd2101 >> CMakeLists.txt
	echo     ${ALL_LIBS} >> CMakeLists.txt
	echo ^) >> CMakeLists.txt
	echo include_directories^(${CMAKE_CURRENT_LIST_DIR}/lib/imgui^) >> CMakeLists.txt
	echo include_directories^(${CMAKE_CURRENT_LIST_DIR}/lib/imgui/backends^) >> CMakeLists.txt
	echo include_directories^(${CMAKE_CURRENT_LIST_DIR}/lib/gleq^) >> CMakeLists.txt
	echo include_directories^(${CMAKE_CURRENT_LIST_DIR}/lib/lodepng^) >> CMakeLists.txt
	echo include_directories^(${CMAKE_CURRENT_LIST_DIR}/lib/glfw-event-recorder^) >> CMakeLists.txt
	echo include_directories^(${CMAKE_CURRENT_LIST_DIR}/lib/glm^) >> CMakeLists.txt
	echo include_directories^(${CMAKE_CURRENT_LIST_DIR}/lib/tinyobjloader^) >> CMakeLists.txt
	echo. >> CMakeLists.txt
	echo # Set the directory where your custom modules are located >> CMakeLists.txt
	echo set^(CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake"^) >> CMakeLists.txt
	echo include^(Utilities^) >> CMakeLists.txt
	echo include^(BuildTests^) >> CMakeLists.txt
	echo. >> CMakeLists.txt
	echo ADD_SUBDIRECTORY^(projects^) >> CMakeLists.txt
	echo ADD_SUBDIRECTORY^(test-submissions^) >> CMakeLists.txt
	echo. >> CMakeLists.txt
	echo # -- Resource copying -- >> CMakeLists.txt
	echo # Single custom target so the directories are copied ONCE per build, >> CMakeLists.txt
	echo # regardless of how many ^(or which^) executables are being compiled. >> CMakeLists.txt
	echo set(COPY_RESOURCE_COMMANDS^) >> CMakeLists.txt
	echo.>> CMakeLists.txt
	echo if^(EXISTS "${CMAKE_SOURCE_DIR}/meshes"^) >> CMakeLists.txt
	echo     list^(APPEND COPY_RESOURCE_COMMANDS >> CMakeLists.txt
	echo         COMMAND ${CMAKE_COMMAND} -E copy_directory >> CMakeLists.txt
	echo             "${CMAKE_SOURCE_DIR}/meshes" >> CMakeLists.txt
	echo             "${CMAKE_BINARY_DIR}/meshes" >> CMakeLists.txt
	echo     ^) >> CMakeLists.txt
	echo endif^(^) >> CMakeLists.txt
	echo.>> CMakeLists.txt
	echo if^(EXISTS "${CMAKE_SOURCE_DIR}/scenes"^) >> CMakeLists.txt
	echo     list^(APPEND COPY_RESOURCE_COMMANDS >> CMakeLists.txt
	echo         COMMAND ${CMAKE_COMMAND} -E copy_directory >> CMakeLists.txt
	echo             "${CMAKE_SOURCE_DIR}/scenes" >> CMakeLists.txt
	echo             "${CMAKE_BINARY_DIR}/scenes" >> CMakeLists.txt
	echo     ^) >> CMakeLists.txt
	echo endif^(^) >> CMakeLists.txt
	echo.>> CMakeLists.txt
	echo if^(EXISTS "${CMAKE_SOURCE_DIR}/images"^) >> CMakeLists.txt
	echo     list^(APPEND COPY_RESOURCE_COMMANDS >> CMakeLists.txt
	echo         COMMAND ${CMAKE_COMMAND} -E copy_directory >> CMakeLists.txt
	echo             "${CMAKE_SOURCE_DIR}/images" >> CMakeLists.txt
	echo             "${CMAKE_BINARY_DIR}/images" >> CMakeLists.txt
	echo     ^) >> CMakeLists.txt
	echo endif^(^) >> CMakeLists.txt
	echo.>> CMakeLists.txt
	echo add_custom_target^(copy_resources >> CMakeLists.txt
	echo     ${COPY_RESOURCE_COMMANDS} >> CMakeLists.txt
	echo     COMMENT "Copying resource directories to build folder" >> CMakeLists.txt
	echo ^) >> CMakeLists.txt
	echo # Auto-discover all project targets by globbing subdirectory names from >> CMakeLists.txt
	echo # both projects/ and test-submissions/. Target names are assumed to match >> CMakeLists.txt
	echo # their folder names ^(e.g. projects/tutorial-0 -^> target tutorial-0^). >> CMakeLists.txt
	echo # The if^(TARGET^) guard safely skips any directory that has no CMake target. >> CMakeLists.txt
	echo foreach^(_src_dir >> CMakeLists.txt
	echo         "${CMAKE_SOURCE_DIR}/projects" >> CMakeLists.txt
	echo         "${CMAKE_SOURCE_DIR}/test-submissions"^) >> CMakeLists.txt
	echo     file^(GLOB _subdirs LIST_DIRECTORIES true "${_src_dir}/*"^) >> CMakeLists.txt
	echo     foreach^(_dir ${_subdirs}^) >> CMakeLists.txt
	echo         if^(IS_DIRECTORY "${_dir}"^) >> CMakeLists.txt
	echo             get_filename_component^(_tgt "${_dir}" NAME^) >> CMakeLists.txt
	echo             if^(TARGET ${_tgt}^) >> CMakeLists.txt
	echo                 add_dependencies^(${_tgt} copy_resources^) >> CMakeLists.txt
	echo             endif^(^) >> CMakeLists.txt
	echo         endif^(^) >> CMakeLists.txt
	echo     endforeach^(^) >> CMakeLists.txt
	echo endforeach^(^) >> CMakeLists.txt
	goto :eof
