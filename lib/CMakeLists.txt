# Add all directories listed here
file(GLOB LIBRARY_DIRECTORIES ${CMAKE_CURRENT_SOURCE_DIR}/*)

if (USE_STM32CUBEF1_LIBRARY)
	add_subdirectory(stm32cubef1)
	message(STATUS "Using library stm32cubef1")
endif()

