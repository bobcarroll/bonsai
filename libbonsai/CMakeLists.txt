
set(LIBBONSAI_SRC 
    log.c
    pgcommon.c
    pgctxpool.c
    session.c
    ntlmauth.c
    authz.c
    base64.c)

set_source_files_properties(
    ${Cabrillo_BINARY_DIR}/libbonsai/pgcommon.c
    PROPERTIES GENERATED 1)

add_library(bonsai SHARED ${LIBBONSAI_SRC})

add_custom_command(OUTPUT ${Cabrillo_BINARY_DIR}/libbonsai/pgcommon.c 
                   COMMAND ${ECPG}
                   ${Cabrillo_SOURCE_DIR}/libbonsai/pgcommon.pgc
                   -o ${Cabrillo_BINARY_DIR}/libbonsai/pgcommon.c
                   MAIN_DEPENDENCY pgcommon.pgc
                   COMMENT "Running ecpg on pgcommon.pgc")

target_link_libraries(bonsai ${Thread_LIBRIARIES} ecpg ${LIBNETAPI_LIBRARIES})

