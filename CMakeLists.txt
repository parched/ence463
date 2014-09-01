cmake_minimum_required(VERSION 2.8)

project(AS)

# flags for C an C++
set(COMMON_FLAGS "-mcpu=cortex-m3 -mthumb")

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${COMMON_FLAGS}")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${COMMON_FLAGS}")

add_definitions(
	-DGCC_ARMCM3_LM3S102
	-Dinline=
	-DPACK_STRUCT_END=__attribute\(\(packed\)\)
	-DALIGN_STRUCT_END=__attribute\(\(aligned\(4\)\)\)
	-Dsprintf=usprintf
	-Dsnprintf=usnprintf
	-Dprintf=uipprintf
)

add_subdirectory(Luminary)
add_subdirectory(FreeRTOS/Source)
add_subdirectory(src)

message("Using CMAKE_TOOLCHAIN_FILE=${CMAKE_TOOLCHAIN_FILE}")
message("Using CMAKE_SYSTEM_NAME=${CMAKE_SYSTEM_NAME}")
message("Using CMAKE_C_COMPILER=${CMAKE_C_COMPILER}")
message("Using CMAKE_CROSSCOMPILING=${CMAKE_CROSSCOMPILING}")
message("Using linker flags ${CMAKE_EXE_LINKER_FLAGS}")
message("Using shared linker flags ${CMAKE_SHARED_LIBRARY_LINK_C_FLAGS}")
