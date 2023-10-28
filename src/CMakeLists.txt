if (USE_STM32F103C6_TARGET)
	add_subdirectory(target/stm32f103c6)
	target_include_directories(stm32f103c6 PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/common")

	# Set TARGET_PLATFORM_LIBRARY_NAME for target 
	set(TARGET_PLATFORM_LIBRARY_NAME stm32f103c6)
endif()

add_subdirectory(common)  # Builds executable "application"
