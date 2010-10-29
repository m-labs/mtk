help:
	@echo "Available targets"
	@echo "================="
	@echo "                  Build DOpE for..."
	@echo "milkymist       - ... Milkymist (using RTEMS). RTEMS_MAKEFILE_PATH environment variable must be set."
	@echo ""
	@echo "clean           - clean generated files"
	@echo "distclean       - clean generated files and backup files"


milkymist:
	make -C lib/milkymist

ALL_MAKEFILES = $(shell find -mindepth 2 -name Makefile)

clean:
	for i in $(dir $(ALL_MAKEFILES)); do make -C $$i $@; done

distclean: clean
	find -name "*~" | xargs rm -f
