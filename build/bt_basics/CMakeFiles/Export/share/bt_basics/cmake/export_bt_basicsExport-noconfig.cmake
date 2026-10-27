#----------------------------------------------------------------
# Generated CMake target import file.
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "bt_basics::bt_basics" for configuration ""
set_property(TARGET bt_basics::bt_basics APPEND PROPERTY IMPORTED_CONFIGURATIONS NOCONFIG)
set_target_properties(bt_basics::bt_basics PROPERTIES
  IMPORTED_LOCATION_NOCONFIG "${_IMPORT_PREFIX}/lib/libbt_basics.so"
  IMPORTED_SONAME_NOCONFIG "libbt_basics.so"
  )

list(APPEND _IMPORT_CHECK_TARGETS bt_basics::bt_basics )
list(APPEND _IMPORT_CHECK_FILES_FOR_bt_basics::bt_basics "${_IMPORT_PREFIX}/lib/libbt_basics.so" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
