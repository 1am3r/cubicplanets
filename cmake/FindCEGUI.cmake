# Locate CEGUI (Made for CEGUI 7.5)
#
# This module defines
# CEGUI_FOUND, if false, do not try to link to CEGUI
# CEGUI_LIBRARIES, where to find the librarys
# CEGUI_INCLUDE_DIR, where to find the headers
#
# $CEGUI_ROOT is an environment variable that would
# correspond to the ./configure --prefix=$CEGUI_ROOT
#
# There are several COMPONENTS that can be included:
# NULL, OPENGL, DIRECT3D9, DIRECT3D10, DIRECT3D11, DIRECTFB, OGRE, IRRLICHT
# Selecting no render as COMPONENT will create a error massage!
#
# 2011-07-21 Created by Frederik vom Hofe using the findSFML.cmake versions from David Guthrie with code from Robert Osfield.

SET( CEGUI_ROOT $ENV{CEGUI_ROOT} )
IF(MSVC)
   # Convert backslashes to slashes
   STRING(REGEX REPLACE "\\\\" "/" CEGUI_ROOT "${CEGUI_ROOT}")
ENDIF()


#To always have the right case sensitive name we use this list and a helper macro:
SET(RENDER_NAME
   Null
   OpenGL
   Direct3D9
   Direct3D10
   Direct3D11
   DirectFB
   Ogre
   Irrlicht
)

MACRO(HELPER_GET_CASE_FROM_LIST SEARCHSTR LOOKUPLIST RESULTSTR)
   SET(${RESULTSTR} ${SEARCHSTR}) #default return value if nothing is found
   FOREACH(LOOP_S IN LISTS ${LOOKUPLIST})
      string(TOLOWER ${LOOP_S} LOOP_S_LOWER)
      string(TOLOWER ${SEARCHSTR} LOOP_SEARCHSTR_LOWER)
      string(COMPARE EQUAL ${LOOP_S_LOWER} ${LOOP_SEARCHSTR_LOWER} LOOP_STR_COMPARE)
      IF(LOOP_STR_COMPARE)
         SET(${RESULTSTR} ${LOOP_S})
      ENDIF()
   ENDFOREACH()
ENDMACRO()

SET( CEGUI_INCLUDE_SEARCH_DIR
   ${CEGUI_ROOT}/include
   ${CEGUI_ROOT}/cegui/include
)

find_path(CEGUI_INCLUDE_DIR NAMES CEGUI.h
  HINTS
    ${CEGUI_INCLUDE_SEARCH_DIR}
  PATH_SUFFIXES cegui
)

IF("${CEGUI_FIND_COMPONENTS}" STREQUAL "")
   MESSAGE("ERROR: No CEGUI renderer selected. \n\nSelect a renderer by including it's name in the component list:\n\ne.g. Find_Package(CEGUI REQUIRED COMPONENTS OPENGL)\n\nCEGUI renderers:")
   FOREACH(LOOP_S IN LISTS RENDER_NAME)
      MESSAGE("${LOOP_S}")
   ENDFOREACH()
   MESSAGE("\n")
   MESSAGE(SEND_ERROR "Select at last one renderer!" )
ENDIF()

SET( CEGUI_LIBRARIES_SEARCH_DIR
   ${CEGUI_ROOT}/lib
        ${CEGUI_ROOT}
        ~/Library/Frameworks
        /Library/Frameworks
        /usr/local/lib
        /usr/lib
        /sw/lib
        /opt/local/lib
        /opt/csw/lib
        /opt/lib
        [HKEY_LOCAL_MACHINE\\SYSTEM\\CurrentControlSet\\Control\\Session\ Manager\\Environment;CEGUI_ROOT]/lib
        /usr/freeware/lib64
)

macro(_FIND_CEGUI_LIBRARY _var)
  find_library(${_var}
     NAMES 
        ${ARGN}
     HINTS
        ${CEGUI_LIBRARIES_SEARCH_DIR}
     PATH_SUFFIXES "" debug release relwithdebinfo minsizerel
  )
  mark_as_advanced(${_var})
endmacro()

macro(_CEGUI_APPEND_LIBRARIES _list _release)
  set(_debug ${_release}_DEBUG)
  if(${_debug})
    set(${_list} ${${_list}} optimized ${${_release}} debug ${${_debug}})
  else()
    set(${_list} ${${_list}} ${${_release}})
  endif()
endmacro()

_FIND_CEGUI_LIBRARY(CEGUIBASE_LIBRARY CEGUIBase)
_FIND_CEGUI_LIBRARY(CEGUIBASE_LIBRARY_DEBUG CEGUIBase_d)

FOREACH(COMPONENT ${CEGUI_FIND_COMPONENTS})
  HELPER_GET_CASE_FROM_LIST( ${COMPONENT} RENDER_NAME COMPONENT_CASE)
  _FIND_CEGUI_LIBRARY(CEGUI${COMPONENT_CASE}Renderer_LIBRARY CEGUI${COMPONENT_CASE}Renderer)
  _FIND_CEGUI_LIBRARY(CEGUI${COMPONENT_CASE}Renderer_LIBRARY_DEBUG CEGUI${COMPONENT_CASE}Renderer_d)
  LIST(APPEND _CEGUI_FOUND_RENDERERS CEGUI${COMPONENT_CASE}Renderer_LIBRARY)
ENDFOREACH(COMPONENT)

INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(CEGUI DEFAULT_MSG CEGUIBASE_LIBRARY _CEGUI_FOUND_RENDERERS CEGUI_INCLUDE_DIR)

set(CEGUI_INCLUDE_DIRS ${CEGUI_INCLUDE_DIR})
if(CEGUI_FOUND)
  _CEGUI_APPEND_LIBRARIES(CEGUI_LIBRARIES CEGUIBASE_LIBRARY)
  FOREACH(COMPONENT ${CEGUI_FIND_COMPONENTS})
    HELPER_GET_CASE_FROM_LIST( ${COMPONENT} RENDER_NAME COMPONENT_CASE)
    _CEGUI_APPEND_LIBRARIES(CEGUI_LIBRARIES CEGUI${COMPONENT_CASE}Renderer_LIBRARY)
  ENDFOREACH(COMPONENT)
endif()


