include_directories(
	${AS_SOURCE_DIR}/Luminary
	)

add_library(Luminary
./rit128x96x4.c
./ustdlib.c
)

target_link_libraries(Luminary driver gr)
