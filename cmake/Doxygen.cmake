function(enable_doxygen)
	option(ENABLE_DOXYGEN "Enable doxygen doc builder" OFF)

	if (ENABLE_DOXYGEN)
		find_package(Doxygen)
		if (NOT DOXYGEN_FOUND)
			message(FATAL_ERROR "Doxygen is not installed")
		endif()

		set(DOXYGEN_CALLER_GRAPH ON)
		set(DOXYGEN_CALL_GRAPH ON)
		set(DOXYGEN_EXTRACT_ALL ON)

		configure_file(${CMAKE_SOURCE_DIR}/documentation/Doxyfile.in ${CMAKE_BINARY_DIR}/documentation/Doxyfile @ONLY)

		add_custom_target(
			doc_doxygen ALL
			COMMAND ${DOXYGEN_EXECUTABLE} ${CMAKE_BINARY_DIR}/documentation/Doxyfile
			WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
			COMMENT "Generating documentation with doxygen"
			VERBATIM
		)
	endif()
endfunction()