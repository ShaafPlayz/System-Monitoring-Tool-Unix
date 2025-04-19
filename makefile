CC=gcc
CFLAGS= -std=gnu99

a3.out: main.o memory.o cores.o cpu.o
	$(CC) $(CFLAGS) -o $@ $^

%.o: %.c
	$(CC) -c $< -o $@

.PHONY: clean run
run:
	./a3.out

clean:
	rm -f *.o
	rm -f *.out
