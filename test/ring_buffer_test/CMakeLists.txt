cmake_minimum_required(VERSION 3.12)
project(ring_buffer_test)
include_directories(".")
set(SOURCES
	"main.cpp"
	"./utility/ring_buffer.c"
)
set(EXECUTABLE_NAME ring_buffer_test)
add_executable(${EXECUTABLE_NAME} ${SOURCES})
set_property(TARGET ${EXECUTABLE_NAME} PROPERTY CXX_STANDARD 11)
set_property(TARGET ${EXECUTABLE_NAME} PROPERTY C_STANDARD 99)
add_compile_options(${EXECUTABLE_NAME} PUBLIC "-g")
