CC = gcc
CFLAGS = -Wall -pedantic
.PHONY: clean	exe build

# $@	:	nome del target
# $^	:	dependency list
# $<	:	first name of dependency list
# Passare al make solo matrixGenerator


farm	:	farm_main.c
	$(CC) $(CFLAGS) $^ -o $@

generate	:	generafile.c
	$(CC) $(CFLAGS) $^ -o $@

prova :	prova.c
	$(CC) $(CFLAGS) $^ -o $@

build :
	./generate file1 3
exe	:
	@echo "Let's produce:"
	./farm -n 2 file1 file2

clean	:
	@echo "Removing garbage"
	-rm -f *.o
	-rm -f *.txt
	-rm -f *.dat
