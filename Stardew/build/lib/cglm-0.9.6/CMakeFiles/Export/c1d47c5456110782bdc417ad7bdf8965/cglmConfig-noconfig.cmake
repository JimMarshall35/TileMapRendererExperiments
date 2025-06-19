#----------------------------------------------------------------
# Generated CMake target import file.
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "cglm::cglm" for configuration ""
set_property(TARGET cglm::cglm APPEND PROPERTY IMPORTED_CONFIGURATIONS NOCONFIG)
set_target_properties(cglm::cglm PROPERTIES
  IMPORTED_LOCATION_NOCONFIG "${_IMPORT_PREFIX}/lib/libcglm.so.0.9.6"
  IMPORTED_SONAME_NOCONFIG "libcglm.so.0"
  )

list(APPEND _cmake_import_check_targets cglm::cglm )
list(APPEND _cmake_import_check_files_for_cglm::cglm "${_IMPORT_PREFIX}/lib/libcglm.so.0.9.6" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
