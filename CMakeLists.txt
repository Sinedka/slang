cmake_minimum_required(VERSION 3.21)
project(MyProject LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 23)

set(CMAKE_C_COMPILER "/etc/profiles/per-user/sinedka/bin/clang")
set(CMAKE_CXX_COMPILER "/etc/profiles/per-user/sinedka/bin/clang++")


set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
if(CMAKE_EXPORT_COMPILE_COMMANDS)
    add_custom_target(copy_compile_commands ALL
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${CMAKE_BINARY_DIR}/compile_commands.json
        ${CMAKE_SOURCE_DIR}/compile_commands.json
    )
endif()

set(SOURCES
    main.cpp
)

add_library(libiberty STATIC
    libiberty/xmalloc.c
)

target_include_directories(libiberty PUBLIC ${CMAKE_SOURCE_DIR}/include)

add_library(system STATIC
    system/system.cpp
)

target_include_directories(system PUBLIC ${CMAKE_SOURCE_DIR}/include)

add_library(ast STATIC
    ast/ast.cpp
)

target_include_directories(ast PUBLIC ${CMAKE_SOURCE_DIR}/include/ast ${CMAKE_SOURCE_DIR}/include)


file(GLOB SPP_SOURCES "spp/cpp_string.cpp" "spp/spp_buffer.cpp" "spp/spp_file.cpp" "spp/spp_reader.cpp" "spp/cpp_identifier.cpp")
add_library(spp ${SPP_SOURCES})

target_include_directories(spp PUBLIC ${CMAKE_SOURCE_DIR}/include/spp ${CMAKE_SOURCE_DIR}/include)


add_executable(${PROJECT_NAME} ${SOURCES})

target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_link_libraries(ast PUBLIC spp)


target_link_libraries(${PROJECT_NAME} PUBLIC ast)
