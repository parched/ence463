include_directories(
	${AS_SOURCE_DIR}/StellarisWare
	${AS_SOURCE_DIR}/FreeRTOS/Source/include
	${AS_SOURCE_DIR}/include
	)

add_library(FreeRTOS
./portable/GCC/ARM_CM3/port.c
./portable/MemMang/heap_2.c
./croutine.c
./event_groups.c
./list.c
./queue.c
./tasks.c
./timers.c
)

target_link_libraries(FreeRTOS
	PRIVATE StellarisWare
	)
