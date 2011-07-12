include(LibFindMacros)

libfind_pkg_check_modules(NTLM_PKGCONF libntlm)
find_path(NTLM_INCLUDE_DIR
    NAMES ntlm.h
    PATHS ${NTLM_PKGCONF_INCLUDE_DIRS}
)

libfind_library(NTLM ntlm)

set(NTLM_PROCESS_INCLUDES NTLM_INCLUDE_DIR)
set(NTLM_PROCESS_LIBS NTLM_LIBRARY)
libfind_process(NTLM)
