build:
	mkdir -p build  \
		&& cd build \
		&& cmake .. \
		&& $(MAKE) -j$(shell nproc)

clean:
	cd build && make clean

distclean:
	rm -rf build

.PHONY: build
