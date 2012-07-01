
set(LIBTF_SRC 
    schema.c
    catalog.c
    catalogdb.c
    location.c
    locationdb.c
    servicehost.c
    servicehostdb.c
    property.c
    propertydb.c
    collection.c
    xml.c
    fault.c
    dbhelp.c)

set_source_files_properties(
    ${Cabrillo_BINARY_DIR}/libtf/schema.c
    ${Cabrillo_BINARY_DIR}/libtf/catalogdb.c
    ${Cabrillo_BINARY_DIR}/libtf/locationdb.c
    ${Cabrillo_BINARY_DIR}/libtf/servicehostdb.c
    ${Cabrillo_BINARY_DIR}/libtf/propertydb.c
    PROPERTIES GENERATED 1)

add_library(tf SHARED ${LIBTF_SRC})

add_custom_command(OUTPUT ${Cabrillo_BINARY_DIR}/libtf/schema.c 
                   COMMAND ${ECPG}
                   -t ${Cabrillo_SOURCE_DIR}/libtf/schema.pgc
                   -o ${Cabrillo_BINARY_DIR}/libtf/schema.c
                   MAIN_DEPENDENCY schema.pgc
                   COMMENT "Running ecpg on schema.pgc")

add_custom_command(OUTPUT ${Cabrillo_BINARY_DIR}/libtf/catalogdb.c 
                   COMMAND ${ECPG}
                   -t ${Cabrillo_SOURCE_DIR}/libtf/catalogdb.pgc
                   -o ${Cabrillo_BINARY_DIR}/libtf/catalogdb.c
                   MAIN_DEPENDENCY catalogdb.pgc
                   COMMENT "Running ecpg on catalogdb.pgc")

add_custom_command(OUTPUT ${Cabrillo_BINARY_DIR}/libtf/locationdb.c 
                   COMMAND ${ECPG}
                   -t ${Cabrillo_SOURCE_DIR}/libtf/locationdb.pgc
                   -o ${Cabrillo_BINARY_DIR}/libtf/locationdb.c
                   MAIN_DEPENDENCY locationdb.pgc
                   COMMENT "Running ecpg on locationdb.pgc")

add_custom_command(OUTPUT ${Cabrillo_BINARY_DIR}/libtf/servicehostdb.c 
                   COMMAND ${ECPG}
                   -t ${Cabrillo_SOURCE_DIR}/libtf/servicehostdb.pgc
                   -o ${Cabrillo_BINARY_DIR}/libtf/servicehostdb.c
                   MAIN_DEPENDENCY servicehostdb.pgc
                   COMMENT "Running ecpg on servicehostdb.pgc")

add_custom_command(OUTPUT ${Cabrillo_BINARY_DIR}/libtf/propertydb.c 
                   COMMAND ${ECPG}
                   -t ${Cabrillo_SOURCE_DIR}/libtf/propertydb.pgc
                   -o ${Cabrillo_BINARY_DIR}/libtf/propertydb.c
                   MAIN_DEPENDENCY propertydb.pgc
                   COMMENT "Running ecpg on propertydb.pgc")

target_link_libraries(tf bonsai ${LIBXML2_LIBRARIES} ecpg ${CSOAP_LIBRARIES})

