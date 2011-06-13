TARGETS=merger_node

all: $(TARGETS)

$(TARGETS): $(TARGETS).c
	ppu-gcc -Wall -O3 $^ -o $@

clean:
	rm -f *.o
	rm -f $(TARGETS)
	rm -f *.bin