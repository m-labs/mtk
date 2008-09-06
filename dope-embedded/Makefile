help:
	@echo "--- available targets ---"
	@echo "s3a_starter - build DOpE and test program for Spartan3A starter kit"
	@echo "ml507       - build DOpE and test program for ML507"
	@echo "sdl         - build DOpE and test program for libSDL"
	@echo "clean       - clean generated files"
	@echo "distclean   - clean generated files and backup files"

s3a_starter:
	make -C lib/s3a_starter
	make -C test/s3a_starter

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
