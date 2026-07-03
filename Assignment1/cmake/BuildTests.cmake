macro(SETUP_EXAMPLE TARGET_NAME) 
 
    set(GAME_ENGINE_NAME opengl-dev) 
 
    # Project name 
    project(${TARGET_NAME} VERSION 1.0.0 LANGUAGES CXX) 
 
    # C++ standard 
    set(CMAKE_CXX_STANDARD 20) 
    set(CMAKE_CXX_STANDARD_REQUIRED True) 
    # Fix: pin the VS debugger working dir to build root so relative shader/asset 
    # regardless of which .sln launched the executable. 
 
    # Source files 
    file(GLOB_RECURSE ${TARGET_NAME}_source_files 
        CONFIGURE_DEPENDS 
        ${CMAKE_CURRENT_LIST_DIR}/${TARGET_NAME}/src/*.[ch]pp 
        ${CMAKE_CURRENT_LIST_DIR}/${TARGET_NAME}/include/*.h 
    ) 
 
    add_executable(${TARGET_NAME} 
        ${${TARGET_NAME}_source_files} 
    ) 
 
    set_target_properties(${TARGET_NAME} PROPERTIES 
        VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_BINARY_DIR}" 
    ) 
 
    # Include dirs 
    target_include_directories(${TARGET_NAME} 
        PRIVATE 
            ${CMAKE_CURRENT_LIST_DIR}/${TARGET_NAME}/include 
    ) 
 
    # Compiler options 
    if(CMAKE_CXX_COMPILER_ID MATCHES "GNU^|Clang") 
        target_compile_options(${TARGET_NAME} PRIVATE -Wall ${DisableWarnings}) 
    elseif(MSVC) 
        target_compile_options(${TARGET_NAME} PRIVATE /W3 /WX-) 
    endif() 
 
    set_property(TARGET ${TARGET_NAME} PROPERTY CXX_STANDARD 20) 
 
    # ---- dpml detection ---- 
    set(DPML_INCLUDE_PATH ${CMAKE_SOURCE_DIR}/lib/dpml/include) 
    set(DPML_DEBUG_PATH   ${CMAKE_SOURCE_DIR}/lib/dpml/lib/Debug) 
    set(DPML_RELEASE_PATH ${CMAKE_SOURCE_DIR}/lib/dpml/lib/Release) 
 
    if(EXISTS "${DPML_INCLUDE_PATH}") 
        # message(STATUS "[${TARGET_NAME}] dpml headers found, adding include path...") 
        target_include_directories(${TARGET_NAME} 
            PRIVATE 
                ${DPML_INCLUDE_PATH} 
        ) 
    endif() 
 
    if(EXISTS "${DPML_DEBUG_PATH}/dpml.lib" OR EXISTS "${DPML_RELEASE_PATH}/dpml.lib") 
        # message(STATUS "[${TARGET_NAME}] dpml found, linking...") 
 
        target_link_directories(${TARGET_NAME} 
            PRIVATE 
                $<$<CONFIG:Debug>:${DPML_DEBUG_PATH}> 
                $<$<CONFIG:Release>:${DPML_RELEASE_PATH}> 
        ) 
 
        target_link_libraries(${TARGET_NAME} 
            PRIVATE 
                ${ALL_LIBS} 
                csd2101 
                dpml 
        ) 
    else() 
        target_link_libraries(${TARGET_NAME} 
            PRIVATE 
                ${ALL_LIBS} 
                csd2101 
        ) 
    endif() 
 
endmacro() 
