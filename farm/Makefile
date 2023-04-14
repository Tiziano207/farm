CC = gcc
CFLAGS = -Wall -pedantic
LDFLAGS = -L./lib
LIBDIR = lib

.PHONY: clear	exe build test clean memcheck

# $@	:	nome del target
# $^	:	dependency list
# $<	:	first name of dependency list
# Passare al make solo matrixGenerator


farm_main	:	farm_main.o lib_farm.a
	$(CC) $(CFLAGS) -o farm_main farm_main.o $(LDFLAGS) -l:lib_farm.a

lib_farm.a	:	signal_installer.o collector.o worker.o generic.o queue_lib.o master_worker.o
	ar rcs $(LIBDIR)/lib_farm.a $(LIBDIR)/signal_installer.o $(LIBDIR)/collector.o $(LIBDIR)/generic.o $(LIBDIR)/worker.o $(LIBDIR)/queue_lib.o $(LIBDIR)/master_worker.o


master_worker.o :	$(LIBDIR)/master_worker.c $(LIBDIR)/master_worker.h
	$(CC) $(CFLAGS) -c $(LIBDIR)/master_worker.c -o $(LIBDIR)/master_worker.o

worker.o :	$(LIBDIR)/worker.c $(LIBDIR)/worker.h
	$(CC) $(CFLAGS) -c $(LIBDIR)/worker.c -o $(LIBDIR)/worker.o

collector.o : 	$(LIBDIR)/collector.c $(LIBDIR)/collector.h
	$(CC) $(CFLAGS) -c $(LIBDIR)/collector.c -o $(LIBDIR)/collector.o

queue_lib.o :	$(LIBDIR)/queue_lib.c $(LIBDIR)/queue_lib.h
	$(CC) $(CFLAGS) -c $(LIBDIR)/queue_lib.c -o $(LIBDIR)/queue_lib.o

signal_installer.o : $(LIBDIR)/signal_installer.c $(LIBDIR)/signal_installer.h
	$(CC) $(CFLAGS) -c $(LIBDIR)/signal_installer.c -o $(LIBDIR)/signal_installer.o

generic.o	:	$(LIBDIR)/generic.c $(LIBDIR)/generic.h
	$(CC) $(CFLAGS) -c $(LIBDIR)/generic.c -o $(LIBDIR)/generic.o

farm_main.o	:	farm_main.c
	$(CC) $(CFLAGS) -c farm_main.c

exe	:
	./farm_main  -n 2 -q 1  -t 1000 file1.dat file2.dat -d testdir

test :
	./test.sh

memcheck :
	valgrind --leak-check=full --track-origins=yes ./farm_main  -n 2 -q 1 file1.dat file2.dat -d testdir
clean	:
	-rm -f *.sck

clear	:
	@echo "Removing garbage"
	-rm -f *.o
	-rm -f *.txt
	-rm -f *.dat
	-rm -f *.sck
