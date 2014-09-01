add_subdirectory(drivers)

add_library(board INTERFACE)

target_link_libraries(board
	INTERFACE drivers
	)
