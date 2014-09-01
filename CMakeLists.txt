cmake_minimum_required(VERSION 2.8)

project(AS)

# flags for C an C++
set(COMMON_FLAGS "-mcpu=cortex-m3 -mthumb")

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${COMMON_FLAGS}")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${COMMON_FLAGS}")

add_definitions(
	-Dgcc
	-DGCC_ARMCM3_LM3S102
	-Dinline=
	-Dsprintf=usprintf
	-Dsnprintf=usnprintf
	-Dprintf=uipprintf
)

add_subdirectory(StellarisWare)
add_subdirectory(FreeRTOS/Source)
add_subdirectory(src)

message("Using CMAKE_TOOLCHAIN_FILE=${CMAKE_TOOLCHAIN_FILE}")
message("Using CMAKE_SYSTEM_NAME=${CMAKE_SYSTEM_NAME}")
message("Using CMAKE_C_COMPILER=${CMAKE_C_COMPILER}")
message("Using CMAKE_CROSSCOMPILING=${CMAKE_CROSSCOMPILING}")
message("CMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}")
