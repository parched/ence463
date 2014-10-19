add_subdirectory(driverlib)
add_subdirectory(utils)
add_subdirectory(boards/ek-lm3s1968)
add_subdirectory(IQmath)

add_library(StellarisWare INTERFACE
)

target_link_libraries(StellarisWare
	INTERFACE board
	INTERFACE driver
	INTERFACE utils
	INTERFACE IQmath
	)
