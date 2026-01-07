# FindMPFR.cmake

find_path(MPFR_INCLUDE_DIR NAMES mpfr.h)
find_library(MPFR_LIBRARIES NAMES mpfr libmpfr)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(MPFR DEFAULT_MSG MPFR_INCLUDE_DIR MPFR_LIBRARIES)

if(MPFR_FOUND)
  set(MPFR_INCLUDE_DIRS ${MPFR_INCLUDE_DIR})
  if(NOT TARGET MPFR::MPFR)
    add_library(MPFR::MPFR UNKNOWN IMPORTED)
    set_target_properties(MPFR::MPFR PROPERTIES
      IMPORTED_LOCATION "${MPFR_LIBRARIES}"
      INTERFACE_INCLUDE_DIRECTORIES "${MPFR_INCLUDE_DIRS}"
    )
  endif()
endif()

mark_as_advanced(MPFR_INCLUDE_DIR MPFR_LIBRARIES)
