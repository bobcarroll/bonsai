
include_directories(${Cabrillo_SOURCE_DIR}/pcd)
include_directories(${Cabrillo_SOURCE_DIR}/csd)

set(PCD_SRC
    pcd.c
    tpcmain.c
    ../csd/location.c
    registration.c
    status.c
    authz.c
    commonstr.c
    proctmpl.c)

add_executable(pcd ${PCD_SRC})
target_link_libraries(pcd bonsai tf ${LIBCONFIG_LIBRARY})

