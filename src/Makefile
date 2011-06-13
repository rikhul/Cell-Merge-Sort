# Variables
CFLAGS=-Wall -I./
ifdef optimize
CFLAGS=-Wall -I./ -O3
endif
ifdef debug
CFLAGS=-Wall -I./ -g
endif
CPPU=ppu-gcc
CSPU=spu-gcc
EMBED=ppu-embedspu
EFLAGS=-m32

OUT=sorter
PPU_TARGETS=ppu/main.c ppu/node_spu_map.c
SPU_TARGETS=spu/mergernode.c rootnode.c
SPU_FILES=sort_vectors.c


all: clean $(OUT)

$(OUT): embed_merger
	$(CPPU) $(CFLAGS) $(PPU_TARGETS) mergernode.o -o $(OUT) -lspe2 -m32

embed_merger: spu/mergernode
	$(EMBED) $(EFLAGS) spu_mergernode spu/mergernode mergernode.o

spu/mergernode:
	$(CSPU) $(CFLAGS) spu/mergernode.c spu/sort_vectors.c -o spu/mergernode

clean: 
	rm -f ppu/$(OUT)
	rm -f spu/*.o
	rm -f spu/rootnode
	rm -f spu/mergernode
	rm -f *.o
	rm -f $(OUT)