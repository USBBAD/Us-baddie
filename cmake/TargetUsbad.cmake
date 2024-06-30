file(GLOB_RECURSE TARGET_SOURCES
		"src/*.c"
		"src/utility/*.c"
		"src/application/*.c"
		"src/application/default/*.c"
		"src/target/*.c"
		"src/target/arm/*.c"
		"src/target/arm/stm32f1/*.c"
		"src/target/usbad/*.c"
		"src/target/usbad/src/*.c"
		"src/driver/*.c"
		"src/driver/usb_microphone/*.c"
		"src/driver/usb_microphone/*.s"
		"src/hal/*.c"
)
add_executable(application
		${COMMON_SOURCES}
		${TARGET_SOURCES})
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
