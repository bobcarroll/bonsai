
set(TFADMIN_SRC tfadmin.c)
add_executable(tfadmin ${TFADMIN_SRC})

set(TFADMIN_HELP_SRC tfadmin-help.c)
add_executable(tfadmin-help ${TFADMIN_HELP_SRC})

set(TFADMIN_SETUP_SRC tfadmin-setup.c)
add_executable(tfadmin-setup ${TFADMIN_SETUP_SRC})
target_link_libraries(tfadmin-setup bonsai tf config)

set(TFADMIN_TPC_CREATE_SRC tfadmin-tpc-create.c)
add_executable(tfadmin-tpc-create ${TFADMIN_TPC_CREATE_SRC})
target_link_libraries(tfadmin-tpc-create bonsai tf config)

set(TFADMIN_TPC_ATTACH_SRC tfadmin-tpc-attach.c)
add_executable(tfadmin-tpc-attach ${TFADMIN_TPC_ATTACH_SRC})
target_link_libraries(tfadmin-tpc-attach bonsai tf config)

