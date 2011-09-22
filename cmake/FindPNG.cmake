# - Try to find PNG
# Once done, this will define
#
#  PNG_FOUND - system has PNG
#  PNG_INCLUDE_DIRS - the PNG include directories 
#  PNG_LIBRARIES - link these to use PNG

include(FindPkgMacros)
findpkg_begin(PNG)

# Get path, convert backslashes as ${ENV_${var}}
getenv_path(PNG_HOME)

# construct search paths
set(PNG_PREFIX_PATH ${PNG_HOME} ${ENV_PNG_HOME})
create_search_paths(PNG)
# redo search if prefix path changed
clear_if_changed(PNG_PREFIX_PATH
  PNG_LIBRARY_FWK
  PNG_LIBRARY_REL
  PNG_LIBRARY_DBG
  PNG_INCLUDE_DIR
)

set(PNG_LIBRARY_NAMES png png15 libpng libpng15)
get_debug_names(PNG_LIBRARY_NAMES)

findpkg_framework(PNG)

find_path(PNG_INCLUDE_DIR NAMES png.h HINTS ${PNG_INC_SEARCH_PATH})
find_library(PNG_LIBRARY_REL NAMES ${PNG_LIBRARY_NAMES} HINTS ${PNG_LIB_SEARCH_PATH} PATH_SUFFIXES "" release relwithdebinfo minsizerel)
find_library(PNG_LIBRARY_DBG NAMES ${PNG_LIBRARY_NAMES_DBG} HINTS ${PNG_LIB_SEARCH_PATH} PATH_SUFFIXES "" debug)

make_library_set(PNG_LIBRARY)

findpkg_finish(PNG)

