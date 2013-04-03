EXE := skein
CC := gcc
AR := ar cr
LD := gcc

CFLAGS = -O2 -std=c99
# comment this out on 32 bit machines:
CFLAGS += -m64

SKEIN_BLOCK = skein_block_x64.o
# SKEIN_BLOCK = skein_block.o

SRC    = main.c	SHA3api_ref.c skein.c
OBJ    = $(SRC:.c=.o) $(SKEIN_BLOCK)

all: $(EXE)

$(EXE): $(OBJ)
	@echo "Linking the target $@"
	$(LD) $(OBJ) -o $@
%.o: %.c
	@echo "Compiling C++ source: $<"
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	rm -f $(OBJ)
	rm -f $(EXE)
