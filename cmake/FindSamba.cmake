
find_path(LIBSMBCLIENT_INCLUDE_DIR libsmbclient.h
    /usr/local/include
    /opt/samba/include)

find_library(LIBSMBCLIENT_LIBRARY smbclient
    /usr/local/lib
    /opt/samba/lib)

find_path(LIBNETAPI_INCLUDE_DIR netapi.h
    /usr/local/include
    /opt/samba/include)

find_library(LIBNETAPI_LIBRARY netapi
    /usr/local/lib
    /opt/samba/lib)

find_program(NTLMAUTH ntlm_auth
    /usr/local/bin
    /opt/samba/bin)

if(LIBSMBCLIENT_INCLUDE_DIR AND LIBSMBCLIENT_LIBRARY)
    set(LIBSMBCLIENT_FOUND true)
    set(LIBSMBCLIENT_LIBRARIES ${LIBSMBCLIENT_LIBRARY})
endif(LIBSMBCLIENT_INCLUDE_DIR AND LIBSMBCLIENT_LIBRARY)

if(LIBNETAPI_INCLUDE_DIR AND LIBNETAPI_LIBRARY)
    set(LIBNETAPI_FOUND true)
    set(LIBNETAPI_LIBRARIES ${LIBNETAPI_LIBRARY})
endif(LIBNETAPI_INCLUDE_DIR AND LIBNETAPI_LIBRARY)

if(NTLMAUTH)
    set(NTLMAUTH_FOUND true)
endif(NTLMAUTH)

if(LIBSMBCLIENT_FOUND AND LIBNETAPI_FOUND AND NTLMAUTH)
    if(NOT Samba_FIND_QUIELTY)
        message(STATUS "Found libsmbclient: ${LIBSMBCLIENT_LIBRARIES}")
        message(STATUS "Found libnetapi: ${LIBNETAPI_LIBRARIES}")
        message(STATUS "Found ntlm_auth: ${NTLMAUTH}")
    endif(NOT Samba_FIND_QUIELTY)
else(LIBSMBCLIENT_FOUND AND LIBNETAPI_FOUND AND NTLMAUTH)
    if(Samba_FIND_REQUIRED)
        message(SEND_ERROR "Could not find Samba")
    endif(Samba_FIND_REQUIRED)
endif(LIBSMBCLIENT_FOUND AND LIBNETAPI_FOUND AND NTLMAUTH)

mark_as_advanced(
    LIBSMBCLIENT_INCLUDE_DIR
    LIBSMBCLIENT_LIBRARIES
    LIBNETAPI_INCLUDE_DIR
    LIBNETAPI_LIBRARIES
    NTLMAUTH)

