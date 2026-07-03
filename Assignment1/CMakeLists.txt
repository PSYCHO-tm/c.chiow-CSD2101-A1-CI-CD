# Projects Settings 
 
cmake_minimum_required(VERSION 2.8...3.13) 
 
project (opengl-dev) 
 
add_definitions(-DUSE_CSD2101_AUTOMATION=0) 
add_definitions(-DGLEW_STATIC) 
add_subdirectory(lib/glfw EXCLUDE_FROM_ALL) 
add_subdirectory(lib/glew EXCLUDE_FROM_ALL) 
 
set(EXTERNAL_LIBRARY_SOURCES 
    ${CMAKE_CURRENT_LIST_DIR}/lib/lodepng/lodepng.h 
    ${CMAKE_CURRENT_LIST_DIR}/lib/lodepng/lodepng.cpp 
    ${CMAKE_CURRENT_LIST_DIR}/lib/glfw-event-recorder/recorder.h 
    ${CMAKE_CURRENT_LIST_DIR}/lib/glfw-event-recorder/recorder.cpp 
    ${CMAKE_CURRENT_LIST_DIR}/lib/glfw-event-recorder/hook_csd2101.h 
) 
 
# All libraries - GLFW, glew 
set(ALL_LIBS 
  PRIVATE glfw 
  PRIVATE libglew_static 
) 
ADD_LIBRARY(csd2101 STATIC 
    ${EXTERNAL_LIBRARY_SOURCES} 
) 
target_link_libraries(csd2101 
    ${ALL_LIBS} 
) 
include_directories(${CMAKE_CURRENT_LIST_DIR}/lib/imgui) 
include_directories(${CMAKE_CURRENT_LIST_DIR}/lib/imgui/backends) 
include_directories(${CMAKE_CURRENT_LIST_DIR}/lib/gleq) 
include_directories(${CMAKE_CURRENT_LIST_DIR}/lib/lodepng) 
include_directories(${CMAKE_CURRENT_LIST_DIR}/lib/glfw-event-recorder) 
include_directories(${CMAKE_CURRENT_LIST_DIR}/lib/glm) 
include_directories(${CMAKE_CURRENT_LIST_DIR}/lib/tinyobjloader) 
 
# Set the directory where your custom modules are located 
set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake") 
include(Utilities) 
include(BuildTests) 
 
ADD_SUBDIRECTORY(projects) 
ADD_SUBDIRECTORY(test-submissions) 
 
# -- Resource copying -- 
# Single custom target so the directories are copied ONCE per build, 
# regardless of how many (or which) executables are being compiled. 
set(COPY_RESOURCE_COMMANDS) 

if(EXISTS "${CMAKE_SOURCE_DIR}/meshes") 
    list(APPEND COPY_RESOURCE_COMMANDS 
        COMMAND ${CMAKE_COMMAND} -E copy_directory 
            "${CMAKE_SOURCE_DIR}/meshes" 
            "${CMAKE_BINARY_DIR}/meshes" 
    ) 
endif() 

if(EXISTS "${CMAKE_SOURCE_DIR}/scenes") 
    list(APPEND COPY_RESOURCE_COMMANDS 
        COMMAND ${CMAKE_COMMAND} -E copy_directory 
            "${CMAKE_SOURCE_DIR}/scenes" 
            "${CMAKE_BINARY_DIR}/scenes" 
    ) 
endif() 

if(EXISTS "${CMAKE_SOURCE_DIR}/images") 
    list(APPEND COPY_RESOURCE_COMMANDS 
        COMMAND ${CMAKE_COMMAND} -E copy_directory 
            "${CMAKE_SOURCE_DIR}/images" 
            "${CMAKE_BINARY_DIR}/images" 
    ) 
endif() 

add_custom_target(copy_resources 
    ${COPY_RESOURCE_COMMANDS} 
    COMMENT "Copying resource directories to build folder" 
) 
# Auto-discover all project targets by globbing subdirectory names from 
# both projects/ and test-submissions/. Target names are assumed to match 
# their folder names (e.g. projects/tutorial-0 -> target tutorial-0). 
# The if(TARGET) guard safely skips any directory that has no CMake target. 
foreach(_src_dir 
        "${CMAKE_SOURCE_DIR}/projects" 
        "${CMAKE_SOURCE_DIR}/test-submissions") 
    file(GLOB _subdirs LIST_DIRECTORIES true "${_src_dir}/*") 
    foreach(_dir ${_subdirs}) 
        if(IS_DIRECTORY "${_dir}") 
            get_filename_component(_tgt "${_dir}" NAME) 
            if(TARGET ${_tgt}) 
                add_dependencies(${_tgt} copy_resources) 
            endif() 
        endif() 
    endforeach() 
endforeach() 
