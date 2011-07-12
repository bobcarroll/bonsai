include(LibFindMacros)

libfind_pkg_check_modules(NanoHTTP_PKGCONF nanohttp)
find_path(NanoHTTP_INCLUDE_DIR
    NAMES nanohttp/nanohttp-common.h
    PATHS ${NanoHTTP_PKGCONF_INCLUDE_DIRS}
    PATH_SUFFIXES nanohttp-1.1
)

find_library(NanoHTTP_LIBRARY
    NAMES nanohttp
    PATHS ${NanoHTTP_PKGCONF_LIBRARY_DIRS})

set(NanoHTTP_PROCESS_INCLUDES NanoHTTP_INCLUDE_DIR)
set(NanoHTTP_PROCESS_LIBS NanoHTTP_LIBRARY)
libfind_process(NanoHTTP)

