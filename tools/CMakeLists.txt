
set(TFPREP_SRC tfprep.c)
add_executable(tfprep ${TFPREP_SRC})
target_link_libraries(tfprep bonsai tf)

set(PCNEW_SRC pcnew.c)
add_executable(pcnew ${PCNEW_SRC})
target_link_libraries(pcnew bonsai tf config)

