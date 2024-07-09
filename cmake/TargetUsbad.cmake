# Apply configuration
# Target platform
set(US_TARGET_INCLUDE_DIRECTORIES "${CMAKE_CURRENT_SOURCE_DIR}/src")

# Target platform
set(US_TARGET_NAME "usbad" CACHE STRING "Sources required by target, in glob format")
set(US_TARGET_INCLUDE_DIRECTORIES "src/target/usbad/src" "src/application/default" "src/target")
set(US_TARGET_LINKED_LIBRARIES "cmsis5" "stm32cubef1")
set(CMAKE_C_COMPILER "arm-none-eabi-gcc")
set(CMAKE_CXX_COMPILER "arm-none-eabi-g++")
set(CMAKE_ASM_COMPILER "arm-none-eabi-as")

# fdata-sections ffunction-sections: Discard unused data/function sections, to save space, and prevent "symbol not found" errors
# nosys.specs: system calls are replaces w/ stubs returning error
set(US_TARGET_C_COMPILE_OPTIONS "-mthumb -mcpu=cortex-m3 -g3 --specs=nosys.specs -fdata-sections -ffunction-sections")
set(US_TARGET_CXX_COMPILE_OPTIONS "-mthumb -mcpu=cortex-m3 -g3 --specs=nosys.specs -fdata-sections -ffunction-sections")
set(US_TARGET_ASM_COMPILE_OPTIONS "-mthumb -mcpu=cortex-m3 -g3")

# gc-sections: remove unused sections
set(US_TARGET_LINKER_OPTIONS "-Wl,-T,${CMAKE_CURRENT_SOURCE_DIR}/src/target/${US_TARGET_NAME}/${US_TARGET_NAME}.ld,--gc-sections -mcpu=cortex-m3 -mlittle-endian -g3 --specs=nosys.specs -nostartfiles")

file(GLOB_RECURSE COMMON_SOURCES
		"src/application/default/*.c"
		"src/driver/*.c"
		"src/driver/*.s"
		"src/hal/*.c"
		"src/hal/*.cpp"
		"src/target/arm/*.c"
		"src/target/arm/stm32f1/*.c"
		"src/target/usbad/*.c"
		"src/target/usbad/src/*.c"
		"src/target/usbad/src/*.s"
		"src/utility/*.c"
		"src/utility/*.cpp"
		)
add_executable(application
		${COMMON_SOURCES})
target_include_directories(application PUBLIC
		${CMAKE_CURRENT_SOURCE_DIR}/src
		${US_TARGET_INCLUDE_DIRECTORIES})

message(STATUS "Linked libraries: ${US_TARGET_LINKED_LIBRARIES}") 
message(STATUS "Linker options: ${US_TARGET_LINKER_OPTIONS}")

target_link_libraries(application PUBLIC ${US_TARGET_LINKED_LIBRARIES}) 
target_link_options(application PUBLIC "SHELL:${US_TARGET_LINKER_OPTIONS}")
target_compile_options(application PUBLIC
	"$<$<COMPILE_LANGUAGE:ASM>:SHELL:${US_TARGET_ASM_COMPILE_OPTIONS}>"
	"$<$<COMPILE_LANGUAGE:CXX>:SHELL:${US_TARGET_CXX_COMPILE_OPTIONS}>"
	"$<$<COMPILE_LANGUAGE:C>:SHELL:${US_TARGET_C_COMPILE_OPTIONS}>"
	)
set_target_properties(application PROPERTIES LINKER_LANGUAGE C)

