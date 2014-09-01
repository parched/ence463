# set the linker script
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Tstandalone.ld -nostartfiles")

include_directories(
	${AS_SOURCE_DIR}/include
	${AS_SOURCE_DIR}/FreeRTOS/Source/include
	${AS_SOURCE_DIR}/StellarisWare
	)

set_source_files_properties(startup.c PROPERTIES
	COMPILE_FLAGS "${COMPILE_FLAGS} -O1"
	)

add_library(shared
	shared_adc.c
	)

add_library(asc
	asc_pulse_in.c
	)

add_library(wus
	wus_pulse_out.c
	)


add_executable(wus.axf
	startup.c
	wus.c
	#wus_simulate_task.c
	)

target_link_libraries(wus.axf
	FreeRTOS
	StellarisWare
	shared
	wus
	)

set_target_properties(wus.axf PROPERTIES
       LINK_FLAGS "${LINK_FLAGS} -Xlinker -M -Xlinker -Map=wus.map"
       )

add_custom_command(
	TARGET wus.axf
	COMMAND arm-none-eabi-objcopy wus.axf -O binary wus.bin
	COMMENT "Creating bin."
	)

set_directory_properties(PROPERTIES ADDITIONAL_MAKE_CLEAN_FILES wus.bin)
