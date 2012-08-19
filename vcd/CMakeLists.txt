
include_directories(${Cabrillo_SOURCE_DIR}/vcd)

set(VCD_SRC
    vcd.c)

add_executable(vcd ${VCD_SRC})
target_link_libraries(vcd bonsai tf ${LIBCONFIG_LIBRARY})

