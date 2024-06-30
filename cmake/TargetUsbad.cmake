file(GLOB_RECURSE COMMON_SOURCES
		"src/driver/*.c"
		"src/hal/*.c"
		"src/hal/*.cpp"
		"src/utility/*.c"
		"src/utility/*.cpp")
file(GLOB_RECURSE TARGET_SOURCES
		"target/usbad/src/*.c"
		"target/usbad/src/*.s"
		"target/arm/stm32f1/*.c"
		"application/default/*.c"
		"target/usbad/src/*.cpp")
add_executable(application
		${COMMON_SOURCES}
		${TARGET_SOURCES})
set(US_TARGET_INCLUDE_DIRECTORIES "target/usbad/src" "application/default" "target")
target_include_directories(application PUBLIC 
		"src/target/usbad/src"
		"src/application/default"
		"src/target"
		"src/")
set_target_properties(application PROPERTIES LINKER_LANGUAGE C)
target_link_libraries(application PUBLIC "cmsis5" "stm32cubef1")

set(US_TARGET_LINKER_OPTIONS "-Wl,-T,${CMAKE_CURRENT_SOURCE_DIR}/src/target/usbad/usbad.ld,--gc-sections -mcpu=cortex-m3 -mlittle-endian -g3 --specs=nosys.specs -nostartfiles")
target_link_options(application PUBLIC "SHELL:${US_TARGET_LINKER_OPTIONS}")
include(TargetStm32f1)
include(TargetArmGcc)
