build:
	mkdir -p build  \
		&& cd build \
		&& cmake .. \
		&& $(MAKE) -j$(shell nproc)

.PHONY: build
