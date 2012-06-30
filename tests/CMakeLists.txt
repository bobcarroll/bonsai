
option(ENABLE_TESTS "Enable the testing suite" OFF)

if(ENABLE_TESTS)
    set(QUERY_RESOURCE_TYPES_SRC query-resource-types.c)
    add_executable(query-resource-types ${QUERY_RESOURCE_TYPES_SRC})
    target_link_libraries(query-resource-types bonsai tf)

    add_test(
        query-resource-types 
        ${RUNTEST}
        "${Cabrillo_BINARY_DIR}/tests/query-resource-types" 
        ${Cabrillo_BINARY_DIR}/tests/query-resource-types.out)

    if(VALGRIND)
        add_test(
            query-resource-types-vg 
            ${RUNTEST}
            "${VALGRIND} --leak-check=full --track-origins=yes ${Cabrillo_BINARY_DIR}/tests/query-resource-types"
            ${Cabrillo_BINARY_DIR}/tests/query-resource-types.vg-out)
    endif()
endif()

