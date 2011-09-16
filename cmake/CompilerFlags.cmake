macro(set_default_compiler_flags)
  if (MSVC)

    # Newlines inside flags variables break CMake's NMake generator.   
    set(cxx_base_flags "-GS -W4 -wd4127 -wd4251 -wd4275 -nologo -J -Zi")

    set(cxx_base_flags "${cxx_base_flags} -D_UNICODE -DUNICODE -DWIN32 -D_WIN32")
    set(cxx_base_flags "${cxx_base_flags} -DWIN32_LEAN_AND_MEAN")
 
    set(cxx_exception_flags "-EHsc -D_HAS_EXCEPTIONS=1")
    set(cxx_no_exception_flags "-D_HAS_EXCEPTIONS=0")
    set(cxx_no_rtti_flags "-GR-")
  elseif (CMAKE_COMPILER_IS_GNUCXX)
    set(cxx_base_flags "-Wall -Wshadow")

    if(BUILD_WITH_COVERAGE)
      set(cxx_base_flags "${cxx_base_flags} -fprofile-arcs -ftest-coverage")
    endif()

    set(cxx_exception_flags "-fexceptions")
    set(cxx_no_exception_flags "-fno-exceptions")
    set(cxx_no_rtti_flags "-fno-rtti")
    set(cxx_strict_flags "-Wextra")
  endif()

	# Disable deprecation warnings for standard C functions.
	# really only needed for newer versions of VS, but should
	# not hurt other versions, and this will work into the 
	# future
	if (MSVC OR _INTEL_WINDOWS)
	  ADD_DEFINITIONS(-D_CRT_SECURE_NO_DEPRECATE -D_CRT_NONSTDC_NO_DEPRECATE -D_SCL_SECURE_NO_WARNINGS)
	endif()

	# Activate stdint.h Limit Macros
	ADD_DEFINITIONS(-D__STDC_LIMIT_MACROS)
	
  foreach (flag_var
         CMAKE_CXX_FLAGS CMAKE_CXX_FLAGS_DEBUG CMAKE_CXX_FLAGS_RELEASE
         CMAKE_CXX_FLAGS_MINSIZEREL CMAKE_CXX_FLAGS_RELWITHDEBINFO
    set(${flag_var} "${${flag_var}} ${cxx_base_flags} ${cxx_strict_flags} ${cxx_exception_flags}")
  endforeach()
endmacro()