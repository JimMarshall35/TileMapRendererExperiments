#----------------------------------------------------------------
# Generated CMake target import file.
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "freetype" for configuration ""
set_property(TARGET freetype APPEND PROPERTY IMPORTED_CONFIGURATIONS NOCONFIG)
set_target_properties(freetype PROPERTIES
  IMPORTED_LOCATION_NOCONFIG "${_IMPORT_PREFIX}/lib/libfreetype.so.6.20.2"
  IMPORTED_SONAME_NOCONFIG "libfreetype.so.6"
  )

list(APPEND _cmake_import_check_targets freetype )
list(APPEND _cmake_import_check_files_for_freetype "${_IMPORT_PREFIX}/lib/libfreetype.so.6.20.2" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
