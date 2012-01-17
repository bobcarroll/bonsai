
set(TFPREP_SRC tfprep.c)
add_executable(tfprep ${TFPREP_SRC})
target_link_libraries(tfprep bonsai tf)

