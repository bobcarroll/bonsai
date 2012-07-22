cmake_minimum_required(VERSION 2.0)
project(Cabrillo)

set(CPACK_PACKAGE_VENDOR "Bonsai")
set(CPACK_PACKAGE_VERSION "0.1.0")
set(CPACK_PACKAGE_VERSION_MAJOR "0")
set(CPACK_PACKAGE_VERSION_MINOR "1")
set(CPACK_PACKAGE_VERSION_PATCH "0")

set(CMAKE_MODULE_PATH ${Cabrillo_SOURCE_DIR}/cmake)

cmake_policy(SET CMP0003 NEW)

include(CPack)

find_package(PostgreSQL REQUIRED)
find_package(LibXml2 REQUIRED)
find_package(OpenSSL REQUIRED)
find_package(NTLM REQUIRED)
find_package(Threads REQUIRED)
find_package(LibConfig REQUIRED)
find_package(Samba REQUIRED)
find_package(Valgrind)

find_program(ECPG ecpg)

if(VALGRIND_FOUND)
    set(VALGRIND ${VALGRIND_PROGRAM})
endif()

enable_testing()
set(RUNTEST ${Cabrillo_SOURCE_DIR}/tests/runtest.sh)

if(CMAKE_SYSTEM_NAME MATCHES Linux)
    add_definitions(-DLINUX)
elseif(CMAKE_SYSTEM_NAME MATCHES Darwin)
    add_definitions(-DDARWIN)
endif()

if(OPENSSL_FOUND)
    add_definitions(-DHAVE_SSL)
endif()

if(NTLM_FOUND)
    add_definitions(-DHAVE_NTLM)
endif()

include_directories(
    ${Cabrillo_SOURCE_DIR}/include 
    ${Cabrillo_BINARY_DIR}/include
    ${LIBXML2_INCLUDE_DIR}
    ${OPENSSL_INCLUDE_DIR}
    ${NTLM_INCLUDE_DIR}
    ${LIBNETAPI_INCLUDE_DIR}
    ${Threads_INCLUDE_DIR}
    ${LIBCONFIG_INCLUDE_DIR}
    ${PostgreSQL_INCLUDE_DIR})

set(CSOAP_LIBRARIES 
    csoap
    nanohttp
    ${LIBXML2_LIBRARIES} 
    ${OPENSSL_LIBRARIES})

set(CMAKE_C_FLAGS "-g")

add_subdirectory(csd)
add_subdirectory(libbonsai)
add_subdirectory(libcsoap)
add_subdirectory(libtf)
add_subdirectory(nanohttp)
add_subdirectory(pcd)
add_subdirectory(tests)
add_subdirectory(tools)

