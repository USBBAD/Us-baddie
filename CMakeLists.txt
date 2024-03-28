cmake_minimum_required(VERSION 3.12)
set(CMAKE_C_STANDARD 99)
project(usbad)
enable_language(ASM)
list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR} cmake)

include(config)

# Validate build configuration

# Make sure the "build type" is one of the supported types
if(NOT ("${BUILD_TYPE}" STREQUAL "Debug" OR "${BUILD_TYPE}" STREQUAL "Release"))
	message(FATAL_ERROR "Unsupported build type: \"${BUILD_TYPE}\"")
endif()

# Configure the toolchain
if (TOOLCHAIN STREQUAL "gcc")
	set(GCC_TOOLCHAIN_PREFIX "${GCC_TOOLCHAIN_PATH_PREFIX}${GCC_TOOLCHAIN_NAME_PREFIX}")  # Concat
	message(STATUS "The toolchain prefix is: `${GCC_TOOLCHAIN_PREFIX}`")
	set(CMAKE_C_COMPILER "${GCC_TOOLCHAIN_PREFIX}gcc")
	set(CMAKE_CXX_COMPILER "${GCC_TOOLCHAIN_PREFIX}g++")
	set(CMAKE_ASM_COMPILER "${GCC_TOOLCHAIN_PREFIX}as")
else()
	message(FATAL_ERROR "No toolchain has been specified")
endif()

# Build libraries
add_subdirectory(lib)

# Recurse into building the taget-dependent code
add_subdirectory(src)
