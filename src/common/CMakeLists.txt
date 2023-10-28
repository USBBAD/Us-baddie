file(GLOB_RECURSE SOURCES "*.c")
add_executable(application ${SOURCES})
target_include_directories(application PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(application PRIVATE ${TARGET_PLATFORM_LIBRARY_NAME})
