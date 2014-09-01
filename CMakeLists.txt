cmake_minimum_required(VERSION 2.8)

project(AS)

include(lm3s_cmake.txt)

add_subdirectory(Luminary)
add_subdirectory(FreeRTOS/Source)
add_subdirectory(src)

message("Using CMAKE_TOOLCHAIN_FILE=${}")
message("Using CMAKE_SYSTEM_NAME=${CMAKE_SYSTEM_NAME}")
message("Using CMAKE_C_COMPILER=${CMAKE_C_COMPILER}")
message("Using CMAKE_CROSSCOMPILING=${CMAKE_CROSSCOMPILING}")
message("Using linker flags ${CMAKE_EXE_LINKER_FLAGS}")
message("Using shared linker flags ${CMAKE_SHARED_LIBRARY_LINK_C_FLAGS}")
