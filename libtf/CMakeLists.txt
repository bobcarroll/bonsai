
set(LIBTF_SRC 
    catalog.c
    catalogdb.c
    location.c
    locationdb.c
    xml.c
    fault.c
    dbhelp.c)

add_library(tf ${LIBTF_SRC})

add_custom_command(OUTPUT ${Cabrillo_BINARY_DIR}/libtf/catalogdb.c 
                   COMMAND ${ECPG}
                   ${Cabrillo_SOURCE_DIR}/libtf/catalogdb.pgc
                   -o ${Cabrillo_BINARY_DIR}/libtf/catalogdb.c
                   MAIN_DEPENDENCY catalogdb.pgc
                   COMMENT "Running ecpg on catalogdb.pgc")

add_custom_command(OUTPUT ${Cabrillo_BINARY_DIR}/libtf/locationdb.c 
                   COMMAND ${ECPG}
                   ${Cabrillo_SOURCE_DIR}/libtf/locationdb.pgc
                   -o ${Cabrillo_BINARY_DIR}/libtf/locationdb.c
                   MAIN_DEPENDENCY locationdb.pgc
                   COMMENT "Running ecpg on locationdb.pgc")

target_link_libraries(tf ${LIBXML2_LIBRARIES} ecpg)

