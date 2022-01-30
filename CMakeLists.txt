
cmake_minimum_required (VERSION 3.7)

project (hs C)

# include shared utilities
if(NOT EXISTS "target/cmake-modules/src/utils.cmake")
  execute_process(COMMAND git clone https://github.com/sagiegurari/cmake-modules.git)
endif()
include("target/cmake-modules/src/utils.cmake")

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_BUILD_TYPE Release)
if(NOT WIN32)
  set(X_CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Werror -Wall -Wextra -Wcast-align -Wunused -Wshadow -Wpedantic")
endif()

set(X_CMAKE_PROJECT_ROOT_DIR ${CMAKE_BINARY_DIR}/..)

set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

macro(add_external_lib)
  utils_add_external_github_lib(
    REPO_USERNAME sagiegurari
    REPO_NAME c_${ARGV0}
    TAG_NAME ${ARGV1}
    LIBRARY_NAME ${ARGV0}
    LIBRARY_PARENT_DIRECTORY target
  )
endmacro(add_external_lib)
add_external_lib("stringfn")
add_external_lib("string_buffer")
add_external_lib("vector")
add_external_lib("fsio")
add_external_lib("ini")

include_directories(include "${STRINGFN_INCLUDE}" "${STRING_BUFFER_INCLUDE}" "${VECTOR_INCLUDE}" "${FSIO_INCLUDE}" "${INI_INCLUDE}")

# define all sources
file(GLOB SOURCES "src/*.c")
file(GLOB HEADER_SOURCES "include/*.h")
file(GLOB TEST_SOURCES "tests/*")
file(GLOB COMMON_TEST_SOURCES "tests/test.*")
file(GLOB EXAMPLE_SOURCES "examples/*.c")
file(GLOB EXAMPLE_PEM_FILES "examples/*.pem")

# lint code
find_program(CMAKE_CXX_CPPCHECK NAMES cppcheck)
if (CMAKE_CXX_CPPCHECK)
  add_custom_target(cppcheck ALL
    ${CMAKE_CXX_CPPCHECK}
    --enable=all --inline-suppr --error-exitcode=1 --suppress=missingIncludeSystem -I ./include/ ./src/*.c
    WORKING_DIRECTORY "${X_CMAKE_PROJECT_ROOT_DIR}")
endif()

# format code
if(DEFINED ENV{X_CMAKE_UNCRUSTIFY_ENABLED})
  add_custom_target(uncrustify ALL
    uncrustify
    -c ${X_CMAKE_PROJECT_ROOT_DIR}/uncrustify.cfg
    --no-backup ${SOURCES} ${HEADER_SOURCES} ${TEST_SOURCES} ${EXAMPLE_SOURCES})
endif()

# create static library
add_library(${CMAKE_PROJECT_NAME} STATIC ${SOURCES} ${STRINGFN_SOURCES} ${STRING_BUFFER_SOURCES} ${VECTOR_SOURCES} ${FSIO_SOURCES} ${INI_SOURCES})
if(NOT WIN32)
  set_target_properties(${CMAKE_PROJECT_NAME} PROPERTIES COMPILE_FLAGS "${X_CMAKE_C_FLAGS} -Wconversion")
endif()

# openssl support
if("$ENV{X_CMAKE_OPEN_SSL}" STREQUAL "true")
  find_package(OpenSSL REQUIRED)

  message("OpenSSL Include: ${OPENSSL_INCLUDE_DIR}")
  message("OpenSSL Libs: ${OPENSSL_LIBRARIES}")

  add_definitions(-DHS_SSL_SUPPORTED)

  include_directories(${OPENSSL_INCLUDE_DIR})
  target_link_libraries(${CMAKE_PROJECT_NAME} ${OPENSSL_LIBRARIES})

  file(COPY ${EXAMPLE_PEM_FILES} DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
endif()

# example
add_executable(example examples/example.c)
target_link_libraries(example ${CMAKE_PROJECT_NAME})
if("$ENV{X_CMAKE_OPEN_SSL}" STREQUAL "true")
  target_link_libraries(example ${OPENSSL_LIBRARIES})
endif()
set_target_properties(example PROPERTIES COMPILE_FLAGS "${X_CMAKE_C_FLAGS}")

# tests
include(CTest)

utils_setup_c_all_tests(
  ADDITIONAL_SOURCES "${COMMON_TEST_SOURCES}"
  COMPILATION_FLAGS "${X_CMAKE_C_FLAGS}"
  BINARY_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)

