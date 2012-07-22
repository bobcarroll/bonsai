
include_directories(${Cabrillo_SOURCE_DIR}/csd)

set(CSD_SRC
    csd.c
    tfmain.c
    location.c
    catalog.c)

add_executable(csd ${CSD_SRC})
target_link_libraries(csd bonsai tf ${LIBCONFIG_LIBRARY})

