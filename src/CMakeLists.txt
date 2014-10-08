# set the linker script
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Tstandalone.ld -nostartfiles")

include_directories(
	${AS_SOURCE_DIR}/include
	${AS_SOURCE_DIR}/FreeRTOS/Source/
	${AS_SOURCE_DIR}/FreeRTOS/Source/include
	${AS_SOURCE_DIR}/StellarisWare
	${AS_SOURCE_DIR}/StellarisWare/utils
	${AS_SOURCE_DIR}/StellarisWare/boards/ek-lm3s1968
	)

set_source_files_properties(startup_gcc.c PROPERTIES
	COMPILE_FLAGS "${COMPILE_FLAGS} -O1"
	)

# Add shared c files to this list
add_library(shared
	shared_adc.c
	shared_pwm.c
	shared_uart_task.c
	shared_button_task.c
	shared_guidraw_task.c
	shared_guilayout.c
	)

# Add ASC c files to this list
add_library(asc
	asc_pulse_in.c
	asc_controller.c
	asc_control_task.c
	)

# Add WUS c files to this list
add_library(wus
	wus_pulse_out.c
	wus_simulate_task.c
	)

set_target_properties(shared asc wus
	PROPERTIES
	COMPILE_FLAGS "${COMPILE_FLAGS} -Wall"
	)

add_executable(wus.axf
	startup_gcc.c
	wus.c
	)

target_link_libraries(wus.axf
	wus
	shared
	FreeRTOS
	StellarisWare
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
