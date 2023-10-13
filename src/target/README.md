# About

Various platform-dependent code pieces

# Build

- Libraries here are GUARANTEED to be provided with variables specified under "COMMON VARIABLES" section (see [the core CMakeLists.txt](./../../CMakeLists.txt));

# Code

- Libraries here MUST implement `target.h` API
- Libraries here MUST call `main()` (see `src/common/application/main.c`) as soon as possible, depending on the target platform
- It MUST be considered that `target.h` API will be used AFTER `main()` is called
