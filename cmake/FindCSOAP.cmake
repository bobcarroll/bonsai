include(LibFindMacros)

libfind_pkg_check_modules(CSOAP_PKGCONF libcsoap)
find_path(CSOAP_INCLUDE_DIR
    NAMES libcsoap/soap-fault.h
    PATHS ${CSOAP_PKGCONF_INCLUDE_DIRS}
    PATH_SUFFIXES libcsoap-1.1
)

libfind_library(CSOAP csoap)

set(CSOAP_PROCESS_INCLUDES CSOAP_INCLUDE_DIR)
set(CSOAP_PROCESS_LIBS CSOAP_LIBRARY)
libfind_process(CSOAP)
