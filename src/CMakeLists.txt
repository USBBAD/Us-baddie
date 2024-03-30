file(GLOB_RECURSE COMMON_SOURCES
		"hal/*.c"
		"utility/*.c")
file(GLOB_RECURSE TARGET_SOURCES ${US_TARGET_SOURCES})
add_executable(application
		${COMMON_SOURCES}
		${TARGET_SOURCES})
target_include_directories(application PUBLIC
		${CMAKE_CURRENT_SOURCE_DIR}
		${US_TARGET_INCLUDE_DIRECTORIES})

message(STATUS "Linked libraries: ${US_TARGET_LINKED_LIBRARIES}") 
message(STATUS "Linker options: ${US_TARGET_LINKER_OPTIONS}")
message(STATUS "Compile options: ${US_TARGET_COMPILE_OPTIONS}")

target_link_libraries(application PUBLIC ${US_TARGET_LINKED_LIBRARIES}) 
target_link_options(application PUBLIC "SHELL:${US_TARGET_LINKER_OPTIONS}")
target_compile_options(application PUBLIC "SHELL:${US_TARGET_COMPILE_OPTIONS}")
set_target_properties(application PROPERTIES LINKER_LANGUAGE C)
