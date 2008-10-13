help:
	@echo "Available targets"
	@echo "================="
	@echo "                  Build DOpE and test program for..."
	@echo "s3a_starter     - ... Spartan3A and Spartan 3AN starter kit (standalone)"
	@echo "s3e_starter     - ... Spartan3E starter kit (standalone)"
	@echo "ml405           - ... ML405 (standalone)"
	@echo "ml507           - ... ML507 (standalone)"
	@echo "s3a_starter_xil - ... Spartan3A and Spartan 3AN starter kit (using Xilkernel)"
	@echo "sdl             - ... libSDL"
	@echo ""
	@echo "clean           - clean generated files"
	@echo "distclean       - clean generated files and backup files"

s3a_starter:
	make -C lib/s3a_starter
	make -C test/s3a_starter

s3e_starter:
	make -C lib/s3e_starter
	make -C test/s3e_starter

s3a_starter_xil:
	make -C lib/s3a_starter_xil
	make -C test_xilkernel/s3a_starter

ml405:
	make -C lib/ml405
	make -C test/ml405

ml507:
	make -C lib/ml507
	make -C test/ml507

sdl:
	make -C lib/sdl
	make -C test/sdl

ALL_MAKEFILES = $(shell find -mindepth 2 -name Makefile)

clean:
	for i in $(dir $(ALL_MAKEFILES)); do make -C $$i $@; done

distclean: clean
	find -name "*~" | xargs rm -f
