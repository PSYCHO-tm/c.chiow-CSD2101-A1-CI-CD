# Define a macro to group source and header files by directory 
macro(GROUP_FILES_BY_FOLDER all_files) 
    foreach(FILE ${ALL_FILES})  
        # Get the directory of the file 
        get_filename_component(PARENT_DIR "${FILE}" PATH) 
 
        # Replace '/' with '\' for Visual Studio group formatting 
        string(REPLACE "/" "\\" GROUP "${PARENT_DIR}") 
 
        # Classify files into Source or Header groups based on their extension 
        if ("${FILE}" MATCHES ".*\\.cpp") 
        set(GROUP "Source Files\\${GROUP}") 
        elseif("${FILE}" MATCHES ".*\\.hpp") 
        set(GROUP "Header Files\\${GROUP}") 
        endif() 
 
        # Add the file to the corresponding source group 
        source_group("${GROUP}" FILES "${FILE}") 
    endforeach() 
endmacro() 
 
# Define function to copy assets with conditional copying 
function(COPY_FOLDER_IF_DIFFERENT PROJECT_NAME source_dir dest_dir) 
    file(GLOB_RECURSE ASSET_FILES "${source_dir}/*") 
 
    foreach(asset ${ASSET_FILES}) 
        get_filename_component(relative_path ${asset} DIRECTORY) 
        string(REPLACE "${source_dir}" "" relative_path "${relative_path}") 
        set(dest_file "${dest_dir}/${relative_path}") 
 
        add_custom_command( 
            TARGET ${PROJECT_NAME} 
            POST_BUILD 
            COMMAND ${CMAKE_COMMAND} -E make_directory ${dest_file} 
            COMMAND ${CMAKE_COMMAND} -E copy_if_different ${asset} ${dest_file} 
        ) 
    endforeach() 
endfunction() 
