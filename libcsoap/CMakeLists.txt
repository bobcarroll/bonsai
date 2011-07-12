
set(LIBCSOAP_SRC
    soap-xml.c
    soap-fault.c
    soap-env.c
    soap-service.c
    soap-router.c
    soap-client.c
    soap-server.c
    soap-ctx.c)

add_library(csoap ${LIBCSOAP_SRC})
target_link_libraries(csoap gcs ${LIBS})

