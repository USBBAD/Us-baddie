file(GLOB SOURCES "src/*.c")

add_library(stm32f103c6 STATIC ${SOURCES})
target_include_directories(stm32f103c6 PRIVATE "src")
