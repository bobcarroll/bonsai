
set(LIBGCS_SRC 
    log.c
    pgcommon.c
    pgctxpool.c
    session.c
    ntlmauth.c)

add_library(gcs ${LIBGCS_SRC})

add_custom_command(OUTPUT ${Cabrillo_BINARY_DIR}/libgcs/pgcommon.c 
                   COMMAND ${ECPG}
                   ${Cabrillo_SOURCE_DIR}/libgcs/pgcommon.pgc
                   -o ${Cabrillo_BINARY_DIR}/libgcs/pgcommon.c
                   MAIN_DEPENDENCY pgcommon.pgc
                   COMMENT "Running ecpg on pgcommon.pgc")

target_link_libraries(gcs ${Thread_LIBRIARIES} ecpg)

