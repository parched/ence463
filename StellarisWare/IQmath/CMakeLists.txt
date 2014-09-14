add_library(IQmath INTERFACE)

target_link_libraries(IQmath
	INTERFACE ${CMAKE_CURRENT_LIST_DIR}/gcc-cm3/IQmathLib-cm3.a
	)
