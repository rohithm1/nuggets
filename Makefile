# Makefile for nuggets-jyre game
#
# Rohith Mandavilli

MAKE = make
.PHONY: all valgrind clean

############## default: make all libs and programs ##########
all:
	$(MAKE) -C support
	$(MAKE) -C common
	$(MAKE) -C player
	$(MAKE) -C server

############## clean  ##########
clean:
	rm -f *~
	rm -f TAGS
	$(MAKE) -C support clean
	$(MAKE) -C common clean
	$(MAKE) -C player clean
	$(MAKE) -C server clean
