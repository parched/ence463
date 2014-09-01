set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Xlinker -M -Xlinker -Map=wus.map")


link_directories(
	${AS_SOURCE_DIR}/FreeRTOS/Source
	${AS_SOURCE_DIR}/Luminary
	)

include_directories(
	${AS_SOURCE_DIR}/include
	${AS_SOURCE_DIR}/FreeRTOS/Source/include
	${AS_SOURCE_DIR}/Luminary
	)

add_executable(wus.axf
	startup.c
	wus.c
	#wus_simulate_task.c
	)

set_source_files_properties(startup.c PROPERTIES COMPILE_FLAGS "${COMPILE_FLAGS} -O1")

target_link_libraries(wus.axf
	FreeRTOS
	)

add_custom_command(
	TARGET wus.axf
	COMMAND arm-none-eabi-objcopy wus.axf -O binary wus.bin
	COMMENT "Creating bin."
	)

set_directory_properties(PROPERTIES ADDITIONAL_MAKE_CLEAN_FILES wus.bin)
