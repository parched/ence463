add_library(IQmath INTERFACE)

link_directories(
	"."
	)

target_link_libraries(IQmath
	INTERFACE gcc-cm3/IQmathLib-cm3
	)
