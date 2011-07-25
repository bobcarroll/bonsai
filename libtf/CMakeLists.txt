
set(LIBTF_SRC 
    catalog.c
    catalogdb.c
    location.c
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

target_link_libraries(tf ${LIBXML2_LIBRARIES} ecpg)

