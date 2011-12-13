
include_directories(${Cabrillo_SOURCE_DIR}/csd)

set(CSD_SRC
    csd.c
    tfmain.c
    pcmain.c
    location.c
    catalog.c
    registration.c
    status.c)

add_executable(csd ${CSD_SRC})
target_link_libraries(csd bonsai tf ${CSOAP_LIBRARIES} ${LIBCONFIG_LIBRARY})

