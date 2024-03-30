add_library(cmsis5 INTERFACE)
target_include_directories(cmsis5 INTERFACE
	"${CMAKE_CURRENT_SOURCE_DIR}/cmsis5/CMSIS/Driver/Include"
)
