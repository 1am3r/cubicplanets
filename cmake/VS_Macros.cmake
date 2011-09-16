
#Support for precompiled Headers. Use this macro in subfile.

MACRO(ADD_MSVC_PRECOMPILED_HEADER PrecompiledHeader PrecompiledSource SourcesVar)
  if (MSVC)
    GET_FILENAME_COMPONENT(PrecompiledBasename ${PrecompiledHeader} NAME_WE)
    SET(PrecompiledBinary "$(IntDir)/${PrecompiledBasename}.pch")
    SET(Sources ${${SourcesVar}})

    SET_SOURCE_FILES_PROPERTIES(${Sources}
      PROPERTIES COMPILE_FLAGS "/Yu\"${PrecompiledHeader}\" /FI\"${PrecompiledHeader}\" /Fp\"${PrecompiledBinary}\""
      OBJECT_DEPENDS "${PrecompiledBinary}")  
 
    SET_SOURCE_FILES_PROPERTIES(${PrecompiledSource}
      PROPERTIES COMPILE_FLAGS "/Yc\"${PrecompiledHeader}\" /Fp\"${PrecompiledBinary}\""
      OBJECT_OUTPUTS "${PrecompiledBinary}")
  endif(MSVC)
ENDMACRO(ADD_MSVC_PRECOMPILED_HEADER)

MACRO(create_source_group_name_form_absolute_path source_path root_path source_group_name)

  #MESSAGE("D: ${source_path}")
  #MESSAGE("D: ${root_path}")
  #MESSAGE("D: ${source_group_name}")
  #Get folder prefix
  string(REGEX MATCHALL "([^/]+)+" root_path_tokenized "${root_path}")
  string(REGEX MATCHALL "([^/]+)+" source_path_tokenized "${source_path}")
  
  #We only need the relative binary path from the root.
  
  foreach(dir_path_token ${root_path_tokenized})
    list(REMOVE_AT source_path_tokenized 0)
  endforeach(dir_path_token ${root_path_tokenized}) 
    
  #remove filename at last position
  list(REMOVE_AT source_path_tokenized -1)
  
  #Create folder string for path to current binary dir. Required by source_group
  set(source_group_name_temp "")
  foreach(bin_dir_path_token ${source_path_tokenized})
    set(source_group_name_temp "${source_group_name_temp}\\\\${bin_dir_path_token}")
  endforeach(bin_dir_path_token ${source_path_tokenized})
  
  #string(LENGTH ${source_group_name_temp} source_group_name_length)
  #MESSAGE("source_group_name_temp: ${source_group_name_temp}")

  if (${source_group_name_temp} MATCHES "^\\\\*")
    string(REGEX REPLACE "^\\\\" "" ${source_group_name} ${source_group_name_temp})
  endif (${source_group_name_temp} MATCHES "^\\\\*")
  #MESSAGE("source_group_name: ${source_group_name}")
  #MESSAGE("source_group_name_temp: ${source_group_name_temp}")
  
ENDMACRO(create_source_group_name_form_absolute_path source_path dir_path source_group_name)

MACRO (cmp_IDE_SOURCE_PROPERTIES SOURCES)
  if (MSVC)
    foreach(srcfile ${SOURCES})
      set(source_path_name "")
      set(srcfile "${CMAKE_CURRENT_SOURCE_DIR}/${srcfile}")
      #MESSAGE("srcfile ${srcfile}")
      create_source_group_name_form_absolute_path(${srcfile} ${CMAKE_CURRENT_SOURCE_DIR} source_path_name)      
      
      #MESSAGE("source name: ${source_path_name}")       
      set(source_path_name "Sources\\\\${source_path_name}")
      SOURCE_GROUP(${source_path_name} FILES "${srcfile}")

    endforeach(srcfile ${SOURCES})
  endif()
ENDMACRO (cmp_IDE_SOURCE_PROPERTIES NAME SOURCES INSTALL_FILES)



MACRO (cmp_IDE_HEADER_PROPERTIES SOURCES)
if (MSVC)
    foreach(srcfile ${SOURCES})
      set(source_path_name "")
      set(srcfile "${CMAKE_CURRENT_SOURCE_DIR}/${srcfile}")
      #MESSAGE("srcfile ${srcfile}")
      create_source_group_name_form_absolute_path(${srcfile} ${CMAKE_CURRENT_SOURCE_DIR} source_path_name)      
      
      #MESSAGE("source name: ${source_path_name}")       
      set(source_path_name "Headers\\\\${source_path_name}")
      #MESSAGE("srcfile ${srcfile}")
      SOURCE_GROUP(${source_path_name} FILES "${srcfile}")

    endforeach(srcfile ${SOURCES})
  endif()
ENDMACRO (cmp_IDE_HEADER_PROPERTIES NAME SOURCES INSTALL_FILES)


MACRO (cmp_IDE_QT_GENERATED_HEADER_PROPERTIES SOURCES)
  if (MSVC)
    foreach(srcfile ${SOURCES})
      set(source_path_name "")
      create_source_group_name_form_absolute_path(${srcfile} ${CMAKE_CURRENT_BINARY_DIR} source_path_name )      
      
      set(source_path_name "binary\\\\${source_path_name}")
      SOURCE_GROUP(${source_path_name} FILES "${srcfile}")

    endforeach(srcfile ${SOURCES})
  endif()
ENDMACRO (cmp_IDE_QT_GENERATED_HEADER_PROPERTIES NAME SOURCES INSTALL_FILES)