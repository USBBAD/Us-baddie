cmake_minimum_required(VERSION 3.12)
set(CMAKE_C_STANDARD 99)
project(usbad)

# BUILD CONFIGURATION (SUBJECT TO FURTHER OPTIONING)

set(TARGET_PREFIX "src/target/stm32f103c6")
message(STATUS "Target Prefix is `${TARGET_PREFIX}`")
set(TOOLCHAIN "gcc")
message(STATUS "Using toolchain: `${TOOLCHAIN}`")
set(GCC_TOOLCHAIN_PATH_PREFIX "")
set(GCC_TOOLCHAIN_NAME_PREFIX "arm-none-eabi-")
set(LINKED_LIBRARIES "stm32f103c6")
message(STATUS "The following libraries will be compiled: ${LINKED_LIBRARIES}")

# Configure the toolchain
if (TOOLCHAIN STREQUAL "gcc")
	set(GCC_TOOLCHAIN_PREFIX "${GCC_TOOLCHAIN_PATH_PREFIX}${GCC_TOOLCHAIN_NAME_PREFIX}")  # Concat
	message(STATUS "The toolchain prefix is: `${GCC_TOOLCHAIN_PREFIX}`")
	set(CMAKE_C_COMPILER "${GCC_TOOLCHAIN_PREFIX}gcc")
	set(CMAKE_CXX_COMPILER "${GCC_TOOLCHAIN_PREFIX}g++")
else()
	message(FATAL_ERROR "No toolchain has been specified")
endif()

# COMMON VARIABLES. Variables globally accessible by all subs

# Include directories that are common for all sub-s
set(COMMON_INCLUDE_DIRECTORIES
	"src/common"
)

# CREATE EXECUTABLE

set(EXECUTABLE_NAME "${PROJECT_NAME}")
file(GLOB_RECURSE SOURCES "src/common/*.c")
add_executable("${EXECUTABLE_NAME}" ${SOURCES})
target_include_directories(${EXECUTABLE_NAME} PUBLIC ${TARGET_PREFIX} ${COMMON_INCLUDE_DIRECTORIES})

# Link the executable to the libraries
target_link_libraries("${PROJECT_NAME}" PUBLIC ${LINKED_LIBRARIES})
