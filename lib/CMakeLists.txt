# Add all directories listed here
file(GLOB LIBRARY_DIRECTORIES ${CMAKE_CURRENT_SOURCE_DIR}/*)

foreach(LIBRARY_DIRECTORY ${LIBRARY_DIRECTORIES})
	get_filename_component(LIBRARY_NAME ${LIBRARY_DIRECTORY} NAME_WE)
	if (LIBRARY_NAME IN_LIST LINKED_LIBRARIES)
		message(STATUS "Adding library ${LIBRARY_NAME}")
		add_subdirectory(stm32cubef1)
	endif()
endforeach()
