help:
	@echo "Available targets"
	@echo "================="
	@echo "                  Build MTK for..."
	@echo "milkymist       - ... Milkymist SoC (using RTEMS). RTEMS_MAKEFILE_PATH environment variable must be set."
	@echo ""
	@echo "clean           - clean generated files"
	@echo "distclean       - clean generated files and backup files"


milkymist:
	make -C lib/milkymist

install-milkymist: milkymist
	test -n "$(RTEMS_MAKEFILE_PATH)"
	cp lib/milkymist/libmtk.a $(RTEMS_MAKEFILE_PATH)/lib
	mkdir -p $(RTEMS_MAKEFILE_PATH)/lib/include
	cp include/* $(RTEMS_MAKEFILE_PATH)/lib/include

ALL_MAKEFILES = $(shell find -mindepth 2 -name Makefile)

clean:
	for i in $(dir $(ALL_MAKEFILES)); do make -C $$i $@; done

distclean: clean
	find -name "*~" | xargs rm -f

.PHONY: milkymist install-milkymist clean distclean
