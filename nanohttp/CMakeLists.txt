
set(NANOHTTP_SRC
    nanohttp-common.c
    nanohttp-socket.c
    nanohttp-client.c
    nanohttp-server.c
    nanohttp-stream.c
    nanohttp-mime.c
    nanohttp-request.c
    nanohttp-response.c
    nanohttp-base64.c
    nanohttp-ssl.c)

add_library(nanohttp ${NANOHTTP_SRC})
target_link_libraries(nanohttp gcs ${LIBS})

