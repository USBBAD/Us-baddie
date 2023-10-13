# About

Various platform-dependent code pieces

# Build

- Libraries here are GUARANTEED to be provided with variables specified under "COMMON VARIABLES" section (see [the core CMakeLists.txt](./../../CMakeLists.txt));
- Libraries here MUST be linked as static libraries
- Libraries here MUST provide linking arguments for the target linker
	- Libraries here MUST expose the linking arguments to the object those will be linked against through use of `PUBLIC` keyword, so those will be "inherited" by the entire application code

# Code

- Libraries here MUST implement `target.h` API
- Libraries here MUST call `main()` (see `src/common/application/main.c`) as soon as possible, depending on the target platform
- It MUST be considered that `target.h` API will be used AFTER `main()` is called
