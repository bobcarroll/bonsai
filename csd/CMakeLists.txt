
set(CSD_SRC
    csd.c
    location.c
    catalog.c)

add_executable(csd ${CSD_SRC})
target_link_libraries(csd gcs tf ${CSOAP_LIBRARIES} ${LIBCONFIG_LIBRARY})

