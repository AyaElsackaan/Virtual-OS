build:
	gcc process_generator.c -o process_generator.out
	gcc clk.c -o clk.out
	gcc scheduler.c -lm -o scheduler.out
	gcc process.c -o process.out
	gcc test_generator.c -o test_generator.out
	gcc data_structures.h -o data_structures.out
	gcc memory_utilities.h -o memory_utilities.out

clean:
	rm -f *.out  processes.txt

all: clean build

run:
	./process_generator.out

